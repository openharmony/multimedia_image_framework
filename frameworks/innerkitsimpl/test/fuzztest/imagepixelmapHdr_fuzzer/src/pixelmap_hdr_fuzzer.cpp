/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "image_pixelmap_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <chrono>
#include <thread>
#include "pixel_map.h"
#include "image_log.h"
#include "image_utils.h"

constexpr uint32_t MAX_LENGTH_MODULO = 1024;
constexpr uint32_t PIXELFORMAT_MODULO = 8;
constexpr uint32_t ALPHATYPE_MODULO = 4;
constexpr uint32_t SCALEMODE_MODULO = 2;
constexpr uint32_t HDR_PIXELFORMAT_COUNT = 3;
constexpr uint32_t MIN_DMA_SIZE = 512;
constexpr uint32_t SUCCESS = 0;

namespace OHOS {
namespace Media {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

std::unique_ptr<Media::PixelMap> GetDmaHdrPixelMapFromOpts()
{
    InitializationOptions opts;
    opts.size.width = GetData<int32_t>() % MAX_LENGTH_MODULO + MIN_DMA_SIZE;
    opts.size.height = GetData<int32_t>() % MAX_LENGTH_MODULO + MIN_DMA_SIZE;
    opts.srcPixelFormat = static_cast<PixelFormat>(GetData<int32_t>() % PIXELFORMAT_MODULO);
    PixelFormat hdrFormats[] = {PixelFormat::RGBA_1010102, PixelFormat::YCBCR_P010, PixelFormat::YCRCB_P010};
    opts.pixelFormat = hdrFormats[GetData<int32_t>() % HDR_PIXELFORMAT_COUNT];
    opts.alphaType = static_cast<AlphaType>(GetData<int32_t>() % ALPHATYPE_MODULO);
    opts.scaleMode = static_cast<ScaleMode>(GetData<int32_t>() % SCALEMODE_MODULO);
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    opts.useDMA = true;
    return PixelMap::Create(opts);
}

bool PixelMapHdrToSdrFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // Initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto hdrPixelMap = GetDmaHdrPixelMapFromOpts();
    if (!hdrPixelMap) {
        return false;
    }
    OHOS::ColorManager::ColorSpace colorSpace(OHOS::ColorManager::ColorSpaceName::BT2020);
    hdrPixelMap->InnerSetColorSpace(colorSpace, GetData<bool>());
    PixelFormat dstPixelFormat = static_cast<PixelFormat>(GetData<int32_t>() % PIXELFORMAT_MODULO);
    bool toSRGB = GetData<bool>();
    uint32_t ret = hdrPixelMap->ToSdr(dstPixelFormat, toSRGB);
    if (ret != SUCCESS) {
        return false;
    }

    return true;
}
} // namespace Media
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Media::PixelMapHdrToSdrFuzzTest(data, size);
    return 0;
}