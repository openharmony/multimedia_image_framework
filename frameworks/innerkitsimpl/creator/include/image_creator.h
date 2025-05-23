/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef IMAGE_CREATOR_H
#define IMAGE_CREATOR_H

#include <surface.h>
#include <cstdint>
#include <mutex>
#include <string>
#include <list>
#include <securec.h>
#include "image_format.h"
#include "image_type.h"
#include "media_errors.h"
#include "pixel_map.h"
#include "image_creator_context.h"
#include "image_receiver.h"
#include "native_image.h"

namespace OHOS {
namespace Media {
class IBufferProcessor;
class NativeImage;
class SurfaceBufferReleaseListener {
public:
    SurfaceBufferReleaseListener()= default;
    virtual ~SurfaceBufferReleaseListener()= default;
    virtual void OnSurfaceBufferRelease() = 0;
};
class ImageCreator {
public:
    sptr<IConsumerSurface> creatorConsumerSurface_ = nullptr;
    sptr<Surface> creatorProducerSurface_ = nullptr;
    std::shared_ptr<ImageCreatorContext> iraContext_ = nullptr;
    std::shared_ptr<SurfaceBufferReleaseListener> surfaceBufferReleaseListener_ = nullptr;
    std::shared_ptr<SurfaceBufferAvaliableListener> surfaceBufferAvaliableListener_ = nullptr;
    ImageCreator() {};
    ~ImageCreator();
    void RegisterBufferAvaliableListener(
        std::shared_ptr<SurfaceBufferAvaliableListener> surfaceBufferAvaliableListener)
    {
        surfaceBufferAvaliableListener_ = surfaceBufferAvaliableListener;
    }
    static std::shared_ptr<ImageCreator> CreateImageCreator(int32_t width,
                                                            int32_t height,
                                                            int32_t format,
                                                            int32_t capicity);
    static int32_t SaveSTP(uint32_t *buffer,
                           uint8_t *tempBuffer,
                           uint32_t bufferSize,
                           InitializationOptions initializationOpts);

    int32_t SaveSenderBufferAsImage(
                              OHOS::sptr<OHOS::SurfaceBuffer> buffer,
                              InitializationOptions initializationOpts);
    int32_t SaveSenderBufferAsImage(InitializationOptions initializationOpts);
    OHOS::sptr<OHOS::SurfaceBuffer> DequeueImage();
    void QueueImage(OHOS::sptr<OHOS::SurfaceBuffer> &buffer);
    void RegisterBufferReleaseListener(std::shared_ptr<SurfaceBufferReleaseListener> release)
    {
        surfaceBufferReleaseListener_ = release;
    };
    void UnRegisterBufferReleaseListener()
    {
        surfaceBufferReleaseListener_.reset();
    };
    sptr<IConsumerSurface> GetCreatorSurface();
    static sptr<IConsumerSurface> getSurfaceById(std::string id);
    void ReleaseCreator();
    static GSError OnBufferRelease(sptr<SurfaceBuffer> &buffer);
    static std::map<uint8_t*, ImageCreator*> bufferCreatorMap_;
    static std::mutex creatorMutex_;

    std::shared_ptr<IBufferProcessor> GetBufferProcessor();
    std::shared_ptr<NativeImage> DequeueNativeImage();
    void QueueNativeImage(std::shared_ptr<NativeImage> image);
private:
    std::shared_ptr<IBufferProcessor> bufferProcessor_;
};
class ImageCreatorSurfaceListener : public IBufferConsumerListener {
public:
    std::shared_ptr<ImageCreator> ic_;
    void OnBufferAvailable() override;
};
} // namespace Media
} // namespace OHOS
#endif
