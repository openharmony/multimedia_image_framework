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

#include <gtest/gtest.h>

#include "pixel_map.h"
#include "pixel_map_parcel.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {
class PixelMapParcelTest : public testing::Test {
public:
    PixelMapParcelTest() {}
    ~PixelMapParcelTest() {}
};

/**
 * @tc.name: PixelMapParcelTest001
 * @tc.desc: CreateFromParcel
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapParcelTest, PixelMapParcelTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapParcelTest: PixelMapParcelTest001 start";

    MessageParcel data1;
    data1.WriteInt32(100);
    data1.WriteInt32(100);
    data1.WriteInt32((int32_t)PixelFormat::UNKNOWN);
    data1.WriteInt32((int32_t)ColorSpace::SRGB);
    data1.WriteInt32((int32_t)AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN);
    data1.WriteInt32(0);
    data1.WriteInt32(0);
    data1.WriteInt32((int32_t)AllocatorType::SHARE_MEM_ALLOC);
    data1.WriteFileDescriptor(-1);
    auto pixelMap1 = PixelMapParcel::CreateFromParcel(data1);
    EXPECT_TRUE(pixelMap1 == nullptr);

    MessageParcel data2;
    data2.WriteInt32(100);
    data2.WriteInt32(100);
    data1.WriteInt32((int32_t)PixelFormat::UNKNOWN);
    data1.WriteInt32((int32_t)ColorSpace::SRGB);
    data1.WriteInt32((int32_t)AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN);
    data2.WriteInt32(0);
    data2.WriteInt32(0);
    data2.WriteInt32((int32_t)AllocatorType::SHARE_MEM_ALLOC);
    data2.WriteFileDescriptor(100);
    auto pixelMap2 = PixelMapParcel::CreateFromParcel(data2);
    EXPECT_TRUE(pixelMap2 == nullptr);

    MessageParcel data3;
    data3.WriteInt32(100);
    data3.WriteInt32(100);
    data1.WriteInt32((int32_t)PixelFormat::UNKNOWN);
    data1.WriteInt32((int32_t)ColorSpace::SRGB);
    data1.WriteInt32((int32_t)AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN);
    data3.WriteInt32(0);
    data3.WriteInt32(4);
    data3.WriteInt32((int32_t)AllocatorType::HEAP_ALLOC);
    auto pixelMap3 = PixelMapParcel::CreateFromParcel(data3);
    EXPECT_TRUE(pixelMap3 == nullptr);

    GTEST_LOG_(INFO) << "PixelMapParcelTest: PixelMapParcelTest001 end";
}
}
}