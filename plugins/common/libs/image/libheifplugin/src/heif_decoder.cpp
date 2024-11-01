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

#include "heif_decoder.h"

#include "image_log.h"
#include "media_errors.h"
#include "securec.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "HeifDecoder"

namespace OHOS {
namespace ImagePlugin {
using namespace MultimediaPlugin;
using namespace Media;

constexpr uint32_t HEIF_IMAGE_NUM = 1;

void HeifDecoder::SetSource(InputDataStream &sourceStream)
{
    heifDecoderInterface_ = HeifDecoderWrapper::CreateHeifDecoderInterface(sourceStream);
}

void HeifDecoder::Reset()
{
    heifDecoderInterface_ = nullptr;
    heifSize_.width = 0;
    heifSize_.height = 0;
    bytesPerPixel_ = 0;
}

uint32_t HeifDecoder::SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info)
{
    uint32_t ret = GetImageSize(index, info.size);
    if (ret != SUCCESS) {
        IMAGE_LOGE("get image size failed, ret=%{public}u", ret);
        return ret;
    }
    heifSize_ = info.size;

    if (heifDecoderInterface_->ConversionSupported(opts.desiredPixelFormat, bytesPerPixel_)) {
        info.pixelFormat = opts.desiredPixelFormat;
        if (info.pixelFormat == PixelFormat::UNKNOWN) {
            info.pixelFormat = PixelFormat::BGRA_8888;
        }
    } else {
        return ERR_IMAGE_COLOR_CONVERT;
    }
    heifDecoderInterface_->SetAllowPartial(opts.allowPartialImage);
    bool hasAlpha = (info.pixelFormat == PixelFormat::RGB_565 || info.pixelFormat == PixelFormat::RGB_888 ||
                     info.pixelFormat == PixelFormat::ALPHA_8);
    if (hasAlpha) {
        info.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    } else {
        info.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    }
    return SUCCESS;
}

uint32_t HeifDecoder::Decode(uint32_t index, DecodeContext &context)
{
    if (heifDecoderInterface_ == nullptr) {
        IMAGE_LOGE("create heif interface object failed!");
        return ERR_IMAGE_INIT_ABNORMAL;
    }

    if (index >= HEIF_IMAGE_NUM) {
        IMAGE_LOGE("decode image out of range, index:%{public}u, range:%{public}d.", index, HEIF_IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (!AllocHeapBuffer(context)) {
        IMAGE_LOGE("get pixels memory fail.");
        return ERR_IMAGE_MALLOC_ABNORMAL;
    }
    return heifDecoderInterface_->OnGetPixels(heifSize_, heifSize_.width * bytesPerPixel_, context);
}

uint32_t HeifDecoder::GetImageSize(uint32_t index, Size &size)
{
    if (index >= HEIF_IMAGE_NUM) {
        IMAGE_LOGE("decode image out of range, index:%{public}u, range:%{public}d.", index, HEIF_IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (heifDecoderInterface_ == nullptr) {
        IMAGE_LOGE("create heif interface object failed!");
        return ERR_IMAGE_INIT_ABNORMAL;
    }

    heifDecoderInterface_->GetHeifSize(size);
    if (size.width == 0 || size.height == 0) {
        IMAGE_LOGE("get width and height fail, height:%{public}u, width:%{public}u.", size.height,
            size.height);
        return ERR_IMAGE_GET_DATA_ABNORMAL;
    }
    return SUCCESS;
}

uint32_t HeifDecoder::PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context)
{
    // currently not support increment decode
    return ERR_IMAGE_DATA_UNSUPPORT;
}
uint32_t HeifDecoder::GetTopLevelImageNum(uint32_t &num)
{
    // currently only supports single frame
    num = HEIF_IMAGE_NUM;
    return SUCCESS;
}

bool HeifDecoder::AllocHeapBuffer(DecodeContext &context)
{
    if (context.pixelsBuffer.buffer == nullptr) {
        if (!IsHeifImageParaValid(heifSize_, bytesPerPixel_)) {
            IMAGE_LOGE("check heif image para fail");
            return false;
        }
        uint64_t byteCount = static_cast<uint64_t>(heifSize_.width) * heifSize_.height * bytesPerPixel_;
        if (context.allocatorType == Media::AllocatorType::SHARE_MEM_ALLOC) {
            return AllocShareMem(context, byteCount);
        } else {
            void *outputBuffer = malloc(byteCount);
            if (outputBuffer == nullptr) {
                IMAGE_LOGE("alloc output buffer size:[%{public}llu] error.",
                    static_cast<unsigned long long>(byteCount));
                return false;
            }
            if (memset_s(outputBuffer, byteCount, 0, byteCount) != EOK) {
                IMAGE_LOGE("memset buffer failed.");
                free(outputBuffer);
                outputBuffer = nullptr;
                return false;
            }
            context.pixelsBuffer.buffer = outputBuffer;
            context.pixelsBuffer.bufferSize = byteCount;
            context.pixelsBuffer.context = nullptr;
            context.allocatorType = AllocatorType::HEAP_ALLOC;
            context.freeFunc = nullptr;
        }
    }
    return true;
}

bool HeifDecoder::AllocShareMem(DecodeContext &context, uint64_t byteCount)
{
    uint32_t id = context.pixelmapUniqueId_;
    std::string name = "HEIF RawData, uniqueId: " + std::to_string(getpid()) + '_' + std::to_string(id);
    int fd = AshmemCreate(name.c_str(), byteCount);
    if (fd < 0) {
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, byteCount, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ::close(fd);
        return false;
    }
    context.pixelsBuffer.buffer = ptr;
    void *fdBuffer = new int32_t();
    if (fdBuffer == nullptr) {
        IMAGE_LOGE("new fdBuffer fail");
        ::munmap(ptr, byteCount);
        ::close(fd);
        context.pixelsBuffer.buffer = nullptr;
        return false;
    }
    *static_cast<int32_t *>(fdBuffer) = fd;
    context.pixelsBuffer.context = fdBuffer;
    context.pixelsBuffer.bufferSize = byteCount;
    context.allocatorType = AllocatorType::SHARE_MEM_ALLOC;
    context.freeFunc = nullptr;
    return true;
}

bool HeifDecoder::IsHeifImageParaValid(Size heifSize, uint32_t bytesPerPixel)
{
    if (heifSize.width == 0 || heifSize.height == 0 || bytesPerPixel == 0) {
        IMAGE_LOGE("heif image para is 0");
        return false;
    }
    uint64_t area = static_cast<uint64_t>(heifSize.width) * heifSize.height;
    if ((area / heifSize.width) != heifSize.height) {
        IMAGE_LOGE("compute width*height overflow!");
        return false;
    }
    uint64_t size = area * bytesPerPixel;
    if ((size / bytesPerPixel) != area) {
        IMAGE_LOGE("compute area*bytesPerPixel overflow!");
        return false;
    }
    if (size > UINT32_MAX) {
        IMAGE_LOGE("size is too large!");
        return false;
    }
    return true;
}
} // namespace ImagePlugin
} // namespace OHOS
