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
#define private public
#define protected public
#include <gtest/gtest.h>
#include "webp_encoder.h"
#include "image_source.h"
#include "buffer_packer_stream.h"
#include "media_errors.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace ImagePlugin {
class WebpEncoderTest : public testing::Test {
public:
    WebpEncoderTest() {}
    ~WebpEncoderTest() {}
};

/**
 * @tc.name: WebpEncoderTest001
 * @tc.desc: Test of WebpEncoder
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, WebpEncoderTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: WebpEncoderTest001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: WebpEncoderTest001 end";
}

/**
 * @tc.name: StartEncode001
 * @tc.desc: Test of StartEncode
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, StartEncode001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: StartEncode001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: StartEncode001 end";
}

/**
 * @tc.name: AddImage001
 * @tc.desc: Test of AddImage
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, AddImage001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: AddImage001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::InitializationOptions opts;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: AddImage001 end";
}

/**
 * @tc.name: AddImage002
 * @tc.desc: Test of AddImage
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, AddImage002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: AddImage002 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::InitializationOptions opts;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap1 = Media::PixelMap::Create(opts);
    auto pixelMap2 = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap1.get());
    webpEncoder->AddImage(*pixelMap2.get());
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: AddImage002 end";
}

/**
 * @tc.name: FinalizeEncode001
 * @tc.desc: pixelMaps_ is empty
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode001 end";
}

/**
 * @tc.name: FinalizeEncode002
 * @tc.desc: The first branch of SetEncodeConfig
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode002 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::InitializationOptions opts;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    pixelMap->SetPixelsAddr(nullptr, nullptr, 10, AllocatorType::HEAP_ALLOC, nullptr);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode002 end";
}

/**
 * @tc.name: FinalizeEncode003
 * @tc.desc: The first case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode003 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    opts.pixelFormat = PixelFormat::RGBA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode003 end";
}

/**
 * @tc.name: FinalizeEncode004
 * @tc.desc: The first case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode004 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.pixelFormat = PixelFormat::RGBA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode004 end";
}

/**
 * @tc.name: FinalizeEncode005
 * @tc.desc: The first case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode005 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    opts.pixelFormat = PixelFormat::RGBA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    pixelMap->SetPixelsAddr(nullptr, nullptr, 10, AllocatorType::HEAP_ALLOC, nullptr);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode005 end";
}


/**
 * @tc.name: FinalizeEncode006
 * @tc.desc: The first case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode006 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.pixelFormat = PixelFormat::RGBA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    pixelMap->SetPixelsAddr(nullptr, nullptr, 10, AllocatorType::HEAP_ALLOC, nullptr);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode006 end";
}

/**
 * @tc.name: FinalizeEncode007
 * @tc.desc: The second case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode007 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.pixelFormat = PixelFormat::BGRA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode007 end";
}

/**
 * @tc.name: FinalizeEncode008
 * @tc.desc: The second case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode008 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.pixelFormat = PixelFormat::BGRA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode008 end";
}

/**
 * @tc.name: FinalizeEncode009
 * @tc.desc: The second case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode009 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.pixelFormat = PixelFormat::BGRA_8888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode009 end";
}

/**
 * @tc.name: FinalizeEncode0010
 * @tc.desc: The third case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode0010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode0010 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::RGBA_F16;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode0010 end";
}

/**
 * @tc.name: FinalizeEncode011
 * @tc.desc: The third case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode011 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::RGBA_F16;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode011 end";
}

/**
 * @tc.name: FinalizeEncode012
 * @tc.desc: The third case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode012 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::RGBA_F16;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode012 end";
}

/**
 * @tc.name: FinalizeEncode013
 * @tc.desc: The fouth case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode013 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::ARGB_8888;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode013 end";
}

/**
 * @tc.name: FinalizeEncode014
 * @tc.desc: The fouth case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode014 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::ARGB_8888;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode014 end";
}

/**
 * @tc.name: FinalizeEncode015
 * @tc.desc: The fouth case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode015, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode015 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::ARGB_8888;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode015 end";
}

/**
 * @tc.name: FinalizeEncode016
 * @tc.desc: The fifth case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode016, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode016 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::RGB_888;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode016 end";
}

/**
 * @tc.name: FinalizeEncode017
 * @tc.desc: The sixth case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode017, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode017 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::RGB_565;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode017 end";
}

/**
 * @tc.name: FinalizeEncode018
 * @tc.desc: The sixth case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode018, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode018 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::RGB_565;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode018 end";
}

/**
 * @tc.name: FinalizeEncode019
 * @tc.desc: The seventh case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode019, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode019 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::ALPHA_8;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode019 end";
}

/**
 * @tc.name: FinalizeEncode0020
 * @tc.desc: The defaut case of CheckEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode0020, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode0020 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::CMYK;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode0020 end";
}

/**
 * @tc.name: FinalizeEncode0021
 * @tc.desc: DoTransform in DoEncode methods
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, FinalizeEncode0021, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode0021 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    Media::InitializationOptions opts;
    opts.pixelFormat = PixelFormat::NV12;
    opts.size.width = 10.f;
    opts.size.height = 10.f;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    webpEncoder->AddImage(*pixelMap.get());
    webpEncoder->FinalizeEncode();
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: FinalizeEncode0021 end";
}

/**
 * @tc.name: Write001
 * @tc.desc: Test of Write
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, Write001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: Write001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    PlEncodeOptions plOpts;
    auto outputData = std::make_unique<uint8_t[]>(1000);
    auto maxSize = 10;
    auto stream = std::make_shared<BufferPackerStream>(outputData.get(), maxSize);
    webpEncoder->StartEncode(*stream.get(), plOpts);
    std::unique_ptr<uint8_t[]> rgb = std::make_unique<uint8_t[]>(10);
    webpEncoder->Write(rgb.get(), 10);
    bool result = (webpEncoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "WebpEncoderTest: Write001 end";
}

/**
 * @tc.name: MakeImageInfoTest002
 * @tc.desc: Test of Write
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, MakeImageInfoTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: MakeImageInfoTest002 start";
    ImagePlugin::WebpEncoder encoder;
    int width = 1;
    int height = 2;
    Media::PixelFormat pf = PixelFormat::RGB_565;
    Media::AlphaType at = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    Media::ColorSpace cs = ColorSpace::EXTENDED_SRGB;
    ImageInfo ret = encoder.MakeImageInfo(width, height, pf, at, cs);
    ASSERT_EQ(ret.size.width, 1);
    ASSERT_EQ(ret.size.height, 2);
    GTEST_LOG_(INFO) << "WebpEncoderTest: MakeImageInfoTest002 end";
}

/**
 * @tc.name: DoTransform001
 * @tc.desc: Test of DoTransform
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransform001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransform001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    pixelMap.imageInfo_.pixelFormat = PixelFormat::RGBA_8888;
    pixelMap.imageInfo_.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransform(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    pixelMap.imageInfo_.alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    result = webpEncoder->DoTransform(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    pixelMap.imageInfo_.pixelFormat = PixelFormat::NV21;
    result = webpEncoder->DoTransform(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransform001 end";
}

/**
 * @tc.name: DoEncodeForICC001
 * @tc.desc: Test of DoEncodeForICC
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoEncodeForICC001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoEncodeForICC001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    uint32_t result = webpEncoder->DoEncodeForICC(pixelMap);
    ASSERT_EQ(result, ERROR);
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoEncodeForICC001 end";
}

/**
 * @tc.name: DoTransformMemcpy001
 * @tc.desc: Test of DoTransformMemcpy
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformMemcpy001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformMemcpy001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformMemcpy(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformMemcpy(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, true);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformMemcpy001 end";
}

/**
 * @tc.name: DoTransformRGBX001
 * @tc.desc: Test of DoTransformRGBX
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformRGBX001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformRGBX001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformRGBX(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformRGBX(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformRGBX001 end";
}

/**
 * @tc.name: DoTransformRgbA001
 * @tc.desc: Test of DoTransformRgbA
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformRgbA001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformRgbA001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformRgbA(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformRgbA(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformRgbA001 end";
}

/**
 * @tc.name: DoTransformBGRX001
 * @tc.desc: Test of DoTransformBGRX
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformBGRX001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformBGRX001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformBGRX(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformBGRX(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformBGRX001 end";
}

/**
 * @tc.name: DoTransformBGRA001
 * @tc.desc: Test of DoTransformBGRA
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformBGRA001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformBGRA001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformBGRA(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformBGRA(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformBGRA001 end";
}

/**
 * @tc.name: DoTransformF16To8888Test001
 * @tc.desc: Test of DoTransformF16To8888
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformF16To8888Test001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformF16To8888Test001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformF16To8888(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformF16To8888(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformF16To8888Test001 end";
}

/**
 * @tc.name: DoTransformF16pTo8888Test001
 * @tc.desc: Test of DoTransformF16pTo8888
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformF16pTo8888Test001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformF16pTo8888Test001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformF16pTo8888(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformF16pTo8888(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformF16pTo8888Test001 end";
}

/**
 * @tc.name: DoTransformArgbToRgbTest001
 * @tc.desc: Test of DoTransformArgbToRgb
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformArgbToRgbTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformArgbToRgbTest001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformArgbToRgb(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformArgbToRgb(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformArgbToRgbTest001 end";
}

/**
 * @tc.name: DoTransformArgbToRgbaTest001
 * @tc.desc: Test of DoTransformArgbToRgba
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformArgbToRgbaTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformArgbToRgbaTest001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformArgbToRgba(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformArgbToRgba(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformArgbToRgbaTest001 end";
}

/**
 * @tc.name: DoTransformRGB565Test001
 * @tc.desc: Test of DoTransformRGB565
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformRGB565Test001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformRGB565Test001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformRGB565(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformRGB565(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformRGB565Test001 end";
}

/**
 * @tc.name: DoTransformGrayTest001
 * @tc.desc: Test of DoTransformGray
 * @tc.type: FUNC
 */
HWTEST_F(WebpEncoderTest, DoTransformGrayTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformGrayTest001 start";
    auto webpEncoder = std::make_shared<WebpEncoder>();
    Media::PixelMap pixelMap;
    char* dst = nullptr;
    int componentsNum = 0;
    bool result = webpEncoder->DoTransformGray(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    dst = new char;
    pixelMap.data_ = new uint8_t;
    result = webpEncoder->DoTransformGray(pixelMap, dst, componentsNum);
    ASSERT_EQ(result, false);
    delete dst;
    delete pixelMap.data_;
    GTEST_LOG_(INFO) << "WebpEncoderTest: DoTransformGrayTest001 end";
}
}
}