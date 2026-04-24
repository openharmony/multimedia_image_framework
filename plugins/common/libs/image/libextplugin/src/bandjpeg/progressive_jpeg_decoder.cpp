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

#include "bandjpeg/progressive_jpeg_decoder.h"

#include <algorithm>

#include "bandjpeg/fast_manager.h"
#include "image_log.h"
#include "input_data_stream.h"
#include "jpeg_yuv_decoder/jpeg_yuv_decoder.h"
#include "media_errors.h"
#include "securec.h"
#include "src/codec/SkJpegCodec.h"
#include "src/codec/SKJpegDecoderMgr.h"
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "ProgressiveJpegDecoder"

namespace {
    constexpr static int32_t NUM_1 = 1;
    constexpr static int32_t NUM_2 = 2;
    constexpr static int32_t NUM_3 = 3;
    constexpr static int32_t NUM_4 = 4;
    constexpr static uint32_t DEFAULT_SAMPLE_SIZE = 1;
    constexpr static int32_t BANDJPEG_V1_MIN_LONG_SIZE = 1080;
    constexpr static uint32_t BANDJPEG_V1_MIN_PIXEL_COUNT = 2ULL * 1024ULL * 1024ULL;
}

namespace OHOS {
namespace ImagePlugin {
using namespace Media;
}
}