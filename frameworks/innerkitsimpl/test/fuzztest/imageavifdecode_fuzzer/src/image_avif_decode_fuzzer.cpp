/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "image_avif_decode_fuzzer.h"

#include <cstdint>
#include <memory>
#include <string>
#include <fuzzer/FuzzedDataProvider.h>

#include "box/item_info_box.h"
#include "box/heif_box.h"
#include "buffer_source_stream.h"
#include "common_fuzztest_function.h"
#include "ext_stream.h"
#include "ext_decoder.h"
#include "AvifDecoderImpl.h"
#include "heif_parser.h"
#include "heif_image.h"
#include "heif_stream.h"
#include "image_source.h"

namespace OHOS {
namespace Media {
using namespace OHOS::ImagePlugin;
FuzzedDataProvider* FDP;

namespace {
    static constexpr uint32_t NUM_0 = 0;
    static constexpr uint32_t NUM_2 = 2;
    static constexpr uint32_t NUM_10 = 10;
    static constexpr uint32_t ACTION_0 = 0;
    static constexpr uint32_t ACTION_1 = 1;
    static constexpr uint32_t ACTION_2 = 2;
    static constexpr uint32_t ACTION_3 = 3;
    static constexpr uint32_t ACTION_4 = 4;
    static constexpr uint32_t ACTION_5 = 5;
    static constexpr uint32_t ACTION_6 = 6;
    static constexpr uint32_t ACTION_NUM = 7;
    static constexpr uint32_t PIXELFORMAT_MODULO = 105;
    static constexpr uint32_t SOURCEOPTIONS_MIMETYPE_MODULO = 2;
    static constexpr uint32_t OPT_SIZE = 80;
    static constexpr uint32_t ALLOCATOR_TYPE_MODULO = 5;
    static constexpr uint32_t MAX_DECODE_COUNT = 100;
    static constexpr uint32_t MAX_FRAMECOUNT = 500;
}

std::unique_ptr<ImageSource> ConstructImageSourceByBuffer(const uint8_t *data, size_t size)
{
    SourceOptions opts;
    std::string mimeType[] = {"image/avif", "image/avis"};
    opts.formatHint = mimeType[FDP->ConsumeIntegral<uint8_t>() % SOURCEOPTIONS_MIMETYPE_MODULO];
    opts.pixelFormat = static_cast<PixelFormat>(FDP->ConsumeIntegral<uint8_t>() % PIXELFORMAT_MODULO);
    size_t power = FDP->ConsumeIntegralInRange<size_t>(NUM_0, NUM_10);
    int32_t height = std::pow(NUM_2, power);
    if (static_cast<size_t>(height) > size) {
        return nullptr;
    }
    int32_t width = static_cast<int32_t>(size / static_cast<size_t>(height));
    if (static_cast<size_t>(width * height) != size) {
        return nullptr;
    }
    opts.size.height = height;
    opts.size.width = width;
    uint32_t errorCode { NUM_0 };
    return ImageSource::CreateImageSource(data, size, opts, errorCode);
}

void CreatePixelMapExFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    auto imageSource = ConstructImageSourceByBuffer(data, size);
    if (!imageSource) {
        return;
    }

    uint32_t index = FDP->ConsumeIntegral<uint32_t>();
    uint32_t errorCode { NUM_0 };
    DecodeOptions decodeOpts;
    SetFdpDecodeOptions(FDP, decodeOpts);
    decodeOpts.desiredSize.width = FDP->ConsumeIntegralInRange<int16_t>(-0xfff, 0xfff);
    decodeOpts.desiredSize.height = FDP->ConsumeIntegralInRange<int16_t>(-0xfff, 0xfff);
    imageSource->CreatePixelMapEx(index, decodeOpts, errorCode);
}

void CreatePixelMapListFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    auto imageSource = ConstructImageSourceByBuffer(data, size);
    if (!imageSource) {
        return;
    }

    uint32_t errorCode { NUM_0 };
    DecodeOptions decodeOpts;
    SetFdpDecodeOptions(FDP, decodeOpts);
    decodeOpts.desiredSize.width = FDP->ConsumeIntegralInRange<int16_t>(-0xfff, 0xfff);
    decodeOpts.desiredSize.height = FDP->ConsumeIntegralInRange<int16_t>(-0xfff, 0xfff);
    decodeOpts.isAnimationDecode = FDP->ConsumeBool();
    imageSource->CreatePixelMapList(decodeOpts, errorCode);
}

void GetDelayTimeFuzzTest(const uint8_t *data, size_t size)
{
    if (!data) {
        return;
    }
    auto imageSource = ConstructImageSourceByBuffer(data, size);
    if (!imageSource) {
        return;
    }
    if (imageSource->InitMainDecoder() != SUCCESS) {
        return;
    }
    uint32_t errorCode = 0;
    imageSource->GetDelayTime(errorCode);
}

void CreatePictureAtIndexSequenceDecodeFuzzTest(const uint8_t *data, size_t size)
{
    if (!data) {
        return;
    }
    auto imageSource = ConstructImageSourceByBuffer(data, size);
    if (!imageSource) {
        return;
    }
    uint32_t errorCode = 0;
    uint32_t frameCount = imageSource->GetFrameCount(errorCode);
    if (errorCode != 0) {
        return;
    }
    for (uint32_t i = 0; i < frameCount && i < MAX_FRAMECOUNT; i++) {
        imageSource->CreatePictureAtIndex(i, errorCode);
    }
}

void CreatePictureAtIndexRandomFrameFuzzTest(const uint8_t *data, size_t size)
{
    if (!data) {
        return;
    }
    auto imageSource = ConstructImageSourceByBuffer(data, size);
    if (!imageSource) {
        return;
    }
    uint32_t errorCode = 0;
    uint32_t frameCount = imageSource->GetFrameCount(errorCode);
    if (errorCode != 0) {
        return;
    }
    uint32_t decodeCount = FDP->ConsumeIntegralInRange<uint32_t>(0, MAX_DECODE_COUNT);
    for (uint32_t i = 0; i < decodeCount; i++) {
        uint32_t index = FDP->ConsumeIntegralInRange<uint32_t>(0, frameCount - 1);
        imageSource->CreatePictureAtIndex(index, errorCode);
    }
}

void CreatePictureFuzzTest(const uint8_t *data, size_t size)
{
    if (!data) {
        return;
    }
    auto imageSource = ConstructImageSourceByBuffer(data, size);
    if (!imageSource) {
        return;
    }
    DecodingOptionsForPicture opts;
    opts.allocatorType = static_cast<AllocatorType>(FDP->ConsumeIntegral<uint32_t>() % ALLOCATOR_TYPE_MODULO);
    opts.desiredPixelFormat = static_cast<PixelFormat>(FDP->ConsumeIntegral<uint32_t>() % PIXELFORMAT_MODULO);
    uint32_t errorCode = 0;
    imageSource->CreatePicture(opts, errorCode);
}

void HeifAv1CBoxParseContentFuzzTest(const uint8_t *data, size_t size)
{
    if (!data) {
        return;
    }
    std::shared_ptr<HeifInputStream> stream = std::make_shared<HeifBufferInputStream>(data, size, false);
    if (!stream) {
        return;
    }
    HeifStreamReader reader(stream, 0, size);
    HeifAv1CBox av1C;
    av1C.ParseContent(reader);
}
} // namespace Media
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size <  OHOS::Media::OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OHOS::Media::OPT_SIZE, OHOS::Media::OPT_SIZE);
    OHOS::Media::FDP = &fdp;
    uint8_t action = fdp.ConsumeIntegral<uint8_t>() % OHOS::Media::ACTION_NUM;
    switch (action) {
        case OHOS::Media::ACTION_0:
            OHOS::Media::CreatePixelMapExFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        case OHOS::Media::ACTION_1:
            OHOS::Media::CreatePixelMapListFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        case OHOS::Media::ACTION_2:
            OHOS::Media::GetDelayTimeFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        case OHOS::Media::ACTION_3:
            OHOS::Media::CreatePictureAtIndexSequenceDecodeFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        case OHOS::Media::ACTION_4:
            OHOS::Media::CreatePictureAtIndexRandomFrameFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        case OHOS::Media::ACTION_5:
            OHOS::Media::CreatePictureFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        case OHOS::Media::ACTION_6:
            OHOS::Media::HeifAv1CBoxParseContentFuzzTest(data, size - OHOS::Media::OPT_SIZE);
            break;
        default:
            break;
    }
    return 0;
}