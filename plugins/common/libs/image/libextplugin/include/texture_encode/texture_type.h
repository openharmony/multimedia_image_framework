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

#ifndef PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_TEXTURE_TYPE_H
#define PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_TEXTURE_TYPE_H

#include "astcenc.h"
#ifndef QUALITY_CONTROL
#define QUALITY_CONTROL (1)
#endif
namespace OHOS {
namespace ImagePlugin {
typedef struct TextureEncodeOptionsType {
    int32_t width_;
    int32_t height_;
    uint8_t blockX_;
    uint8_t blockY_;
    int32_t stride_;
} TextureEncodeOptions;

typedef struct AstcEncoderInfo {
    astcenc_config config;
    astcenc_profile profile;
    astcenc_context* codec_context;
    astcenc_image image_;
    astcenc_swizzle swizzle_;
    uint8_t* data_out_;
    astcenc_error error_;
#if QUALITY_CONTROL
    bool calQualityEnable;
    int32_t *mse[RGBA_COM + 1];
#endif
} AstcEncoder;
} // namespace ImagePlugin
} // namespace OHOS

#endif // PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_TEXTURE_TYPE_H