/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "image_receiver.h"
#include "image_packer.h"
#include "image_source.h"
#include "image_utils.h"
#include "hilog/log.h"

namespace OHOS {
namespace Media {
        constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "imageReceiver"};
        using namespace OHOS::HiviewDFX;
        enum class mode_ {
            MODE_PREVIEW = 0,
            MODE_PHOTO
        };
        int64_t PackImage(const std::string &filePath, std::unique_ptr<PixelMap> pixelMap)
        {
            HiLog::Error(LABEL, "PackImage");
            ImagePacker imagePacker;
            PackOption option;
            option.format = ImageReceiver::OPTION_FORMAT;
            option.quality = ImageReceiver::OPTION_QUALITY;
            option.numberHint = ImageReceiver::OPTION_NUMBERHINT;
            std::set<std::string> formats;
            uint32_t ret = imagePacker.GetSupportedFormats(formats);
            if (ret != SUCCESS) {
                HiLog::Error(LABEL, "image packer get supported format failed, ret=%{public}u.", ret);
                return 0;
            } else {
                HiLog::Error(LABEL, "SUCCESS");
            }
            imagePacker.StartPacking(filePath, option);
            imagePacker.AddImage(*pixelMap);
            int64_t packedSize = 0;
            imagePacker.FinalizePacking(packedSize);
            HiLog::Debug(LABEL, "packedSize=%{public}lld.", static_cast<long long>(packedSize));
            return packedSize;
        }
        std::unique_ptr<PixelMap> ImageReceiver::getSurfacePixelMap(InitializationOptions initializationOpts)
        {
            uint32_t *addr = (uint32_t *)iraContext_->currentBuffer_->GetVirAddr();
            int32_t size = iraContext_->currentBuffer_->GetSize();
            return PixelMap::Create(addr, (uint32_t)size, initializationOpts);
        }
        static int32_t SaveSTP(uint32_t *buffer,
                               uint32_t bufferSize,
                               std::string path,
                               InitializationOptions initializationOpts)
        {
            int64_t errorCode = -1;
            std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(buffer, bufferSize, initializationOpts);
            if (pixelMap.get() != nullptr) {
                ImageInfo imageInfo;
                pixelMap->GetImageInfo(imageInfo);
                HiLog::Debug(LABEL, "create pixel map imageInfo.size.width=%{public}u.", imageInfo.size.width);
            } else {
                HiLog::Error(LABEL, "pixelMap.get() == nullptr");
                return ERR_MEDIA_INVALID_VALUE;
            }
            ImagePacker imagePacker;
            errorCode = PackImage(path, std::move(pixelMap));
            if (errorCode > 0) {
                errorCode = SUCCESS;
            } else {
                errorCode = ERR_MEDIA_INVALID_VALUE;
            }
            return errorCode;
        }
        int32_t ImageReceiver::SaveBufferAsImage(std::string path,
                                                 OHOS::sptr<OHOS::SurfaceBuffer> buffer,
                                                 InitializationOptions initializationOpts)
        {
            int32_t errorcode = 0;
            if (buffer != nullptr) {
                uint32_t *addr = (uint32_t *)buffer->GetVirAddr();
                int32_t size = buffer->GetSize();
                errorcode = SaveSTP(addr, (uint32_t)size, path, initializationOpts);
                (iraContext_->GetReceiverBufferConsumer())->ReleaseBuffer(buffer, -1);
            } else {
                HiLog::Debug(LABEL, "SaveBufferAsImage buffer == nullptr");
            }
            return errorcode;
        }
        int32_t ImageReceiver::SaveBufferAsImage(std::string path,
                                                 InitializationOptions initializationOpts)
        {
            if (iraContext_->currentBuffer_ != nullptr) {
                return SaveBufferAsImage(path, iraContext_->currentBuffer_, initializationOpts);
            } else {
                HiLog::Debug(LABEL, "iraContext_->GetCurrentBuffer() == nullptr");
                return 0;
            }
            return 0;
        }
        void ImageReceiverSurfaceListener ::OnBufferAvailable()
        {
            int32_t flushFence = 0;
            int64_t timestamp = 0;
            OHOS::Rect damage = {};
            OHOS::sptr<OHOS::SurfaceBuffer> buffer;
            sptr<Surface> listenerConsumerSerface = irContext_->GetReceiverBufferConsumer();
            listenerConsumerSerface->AcquireBuffer(buffer, flushFence, timestamp, damage);
            if (buffer != nullptr) {
                irContext_->currentBuffer_ = buffer;
            }
        }
        std::shared_ptr<ImageReceiverContext> ImageReceiverContext ::CreateImageReceiverContext()
        {
            std::shared_ptr<ImageReceiverContext> irc = std::make_shared<ImageReceiverContext>();
            return irc;
        }

        std::shared_ptr<ImageReceiver> ImageReceiver::CreateImageReceiver(int32_t width,
                                                                          int32_t height)
        {
            std::shared_ptr<ImageReceiver> iva = std::make_shared<ImageReceiver>();
            iva->iraContext_ = ImageReceiverContext::CreateImageReceiverContext();
            iva->receiverConsumerSurface_ = Surface::CreateSurfaceAsConsumer();
            if (iva->receiverConsumerSurface_ == nullptr) {
                HiLog::Debug(LABEL, "SurfaceAsConsumer == nullptr");
            }
            iva->receiverConsumerSurface_->SetDefaultWidthAndHeight(width, height);
            auto p = iva->receiverConsumerSurface_->GetProducer();
            iva->receiverProducerSurface_ = Surface::CreateSurfaceAsProducer(p);
            if (iva->receiverProducerSurface_ == nullptr) {
                HiLog::Debug(LABEL, "SurfaceAsProducer == nullptr");
            }
            iva->iraContext_->SetReceiverBufferConsumer(iva->receiverConsumerSurface_);
            iva->iraContext_->SetReceiverBufferProducer(iva->receiverProducerSurface_);
            iva->iraContext_->SetWidth(width);
            iva->iraContext_->SetHeight(height);
            sptr<ImageReceiverSurfaceListener> listener = new ImageReceiverSurfaceListener();
            listener->irContext_ = iva->iraContext_;
            iva->receiverConsumerSurface_->
            RegisterConsumerListener((sptr<IBufferConsumerListener> &)listener);
            return iva;
        }
        OHOS::sptr<OHOS::SurfaceBuffer> ImageReceiver::ReadNextImage()
        {
            return iraContext_->GetCurrentBuffer();
        }
        sptr<Surface> ImageReceiver::GetReceiverSurface()
        {
            return iraContext_->GetReceiverBufferProducer();
        }
        void ImageReceiver::ReleaseReceiver()
        {
            ImageReceiver::~ImageReceiver();
        }
    } // namespace Media
} // namespace OHOS
