/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_BANDJPEG_PROGRESSIVE_JPEG_DECODER_H
#define PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_BANDJPEG_PROGRESSIVE_JPEG_DECODER_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <memory>

#include "abs_image_decoder.h"
#include "include/codec/SkCodec.h"
#include "include/core/SkImageInfo.h"

namespace OHOS {
namespace ImagePlugin {
class InputDataStream;

class ProgressiveJpegDecoder {
public:
    struct JpegInputData {
        uint8_t* buffer = nullptr;
        uint32_t bufferSize = 0;
        std::unique_ptr<uint8_t[]> ownedBuffer = nullptr;
    };
    using ReadJpegDataFunc = std::function<uint32_t(uint8_t*, uint32_t)>;

    struct RgbDecodeOptions {
        SkCodec* codec = nullptr;
        SkImageInfo srcInfo;
        SkImageInfo dstInfo;
        OHOS::Media::Size desiredSize = {0, 0};
        OHOS::Media::PixelFormat pixelFormat = OHOS::Media::PixelFormat::UNKNOWN;
    }
}
}
}
#endif
