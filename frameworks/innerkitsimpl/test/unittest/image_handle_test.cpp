/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "image_handle.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {

class ImageHandleTest : public testing::Test {
public:
    ImageHandleTest() {}
    ~ImageHandleTest() {}
};

/**
 * @tc.name: StartHandle001
 * @tc.desc: test HandleSubSample
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle001 start";
    DecodeOptions opts;
    ImageInfo info;
    info.size.width = 1080;
    info.size.height = 1920;
    ImageHandle::GetInstance().HandleSubSample(opts, info);
    ASSERT_EQ(opts.desiredSize.width, 810);
    ASSERT_EQ(opts.desiredSize.height, 1440);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle001 end";
}

/**
 * @tc.name: StartHandle002
 * @tc.desc: test HandleSubSample
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle002 start";
    DecodeOptions opts;
    ImageInfo info;
    info.size.width = 8;
    info.size.height = 4;
    ImageHandle::GetInstance().HandleSubSample(opts, info);
    ASSERT_EQ(opts.desiredSize.width, 6);
    ASSERT_EQ(opts.desiredSize.height, 3);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle002 end";
}

/**
 * @tc.name: StartHandle003
 * @tc.desc: test CheckOptsNeedOptimize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle003 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 0;
    opts.preference = MemoryUsagePreference::DEFAULT;
    opts.resolutionQuality = ResolutionQuality::UNKNOWN;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle003 end";
}

/**
 * @tc.name: StartHandle004
 * @tc.desc: test CheckOptsNeedOptimize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle004 start";
    DecodeOptions opts;
    opts.desiredSize.width = 100;
    opts.desiredSize.height = 100;
    opts.resolutionQuality = ResolutionQuality::UNKNOWN;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle004 end";
}

/**
 * @tc.name: StartHandle005
 * @tc.desc: test CheckOptsNeedOptimize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle005 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 0;
    opts.resolutionQuality = ResolutionQuality::UNKNOWN;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle005 end";
}

/**
 * @tc.name: StartHandle006
 * @tc.desc: test CheckOptsNeedOptimize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle006 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 0;
    opts.resolutionQuality = ResolutionQuality::LOW;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle006 end";
}

/**
 * @tc.name: StartHandle007
 * @tc.desc: test CheckOptsNeedOptimize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle007 start";
    DecodeOptions opts;
    opts.desiredSize.width = 100;
    opts.desiredSize.height = 0;
    opts.resolutionQuality = ResolutionQuality::UNKNOWN;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle007 end";
}

/**
 * @tc.name: StartHandle008
 * @tc.desc: test CheckOptsNeedOptimize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle008 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 100;
    opts.resolutionQuality = ResolutionQuality::UNKNOWN;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle008 end";
}

/**
 * @tc.name: StartHandle009
 * @tc.desc: test CheckImageNeedResize
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle009 start";
    ImageInfo info;
    info.size.width = 2900;
    info.size.height = 2920;
    bool isValid = ImageHandle::GetInstance().CheckImageNeedResize(info);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle009 end";
}

/**
 * @tc.name: StartHandle010
 * @tc.desc: test CheckOptsSetDesiredRegion when desiredRegion.left is not zero
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle010 start";
    DecodeOptions opts;
    opts.desiredRegion.left = 10;
    opts.desiredRegion.top = 0;
    opts.desiredRegion.width = 0;
    opts.desiredRegion.height = 0;
    bool isValid = ImageHandle::GetInstance().CheckOptsSetDesiredRegion(opts);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle010 end";
}

/**
 * @tc.name: StartHandle011
 * @tc.desc: test CheckOptsSetDesiredRegion when desiredRegion.top is not zero
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle011 start";
    DecodeOptions opts;
    opts.desiredRegion.left = 0;
    opts.desiredRegion.top = 20;
    opts.desiredRegion.width = 0;
    opts.desiredRegion.height = 0;
    bool isValid = ImageHandle::GetInstance().CheckOptsSetDesiredRegion(opts);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle011 end";
}

/**
 * @tc.name: StartHandle012
 * @tc.desc: test CheckOptsSetDesiredRegion when desiredRegion.width is not zero
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle012 start";
    DecodeOptions opts;
    opts.desiredRegion.left = 0;
    opts.desiredRegion.top = 0;
    opts.desiredRegion.width = 100;
    opts.desiredRegion.height = 0;
    bool isValid = ImageHandle::GetInstance().CheckOptsSetDesiredRegion(opts);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle012 end";
}

/**
 * @tc.name: StartHandle013
 * @tc.desc: test CheckOptsSetDesiredRegion when desiredRegion.height is not zero
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle013 start";
    DecodeOptions opts;
    opts.desiredRegion.left = 0;
    opts.desiredRegion.top = 0;
    opts.desiredRegion.width = 0;
    opts.desiredRegion.height = 200;
    bool isValid = ImageHandle::GetInstance().CheckOptsSetDesiredRegion(opts);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle013 end";
}

/**
 * @tc.name: StartHandle014
 * @tc.desc: test CheckOptsSetDesiredRegion when all desiredRegion values are zero
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle014 start";
    DecodeOptions opts;
    opts.desiredRegion.left = 0;
    opts.desiredRegion.top = 0;
    opts.desiredRegion.width = 0;
    opts.desiredRegion.height = 0;
    bool isValid = ImageHandle::GetInstance().CheckOptsSetDesiredRegion(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle014 end";
}

/**
 * @tc.name: StartHandle015
 * @tc.desc: test CheckOptsNeedOptimize when desiredRegion is set
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle015, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle015 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 0;
    opts.resolutionQuality = ResolutionQuality::UNKNOWN;
    opts.desiredRegion.left = 10;
    opts.desiredRegion.top = 10;
    opts.desiredRegion.width = 100;
    opts.desiredRegion.height = 100;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle015 end";
}

/**
 * @tc.name: StartHandle016
 * @tc.desc: test CheckOptsNeedOptimize when resolutionQuality is MEDIUM
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle016, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle016 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 0;
    opts.resolutionQuality = ResolutionQuality::MEDIUM;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle016 end";
}

/**
 * @tc.name: StartHandle017
 * @tc.desc: test CheckOptsNeedOptimize when resolutionQuality is HIGH
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle017, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle017 start";
    DecodeOptions opts;
    opts.desiredSize.width = 0;
    opts.desiredSize.height = 0;
    opts.resolutionQuality = ResolutionQuality::HIGH;
    bool isValid = ImageHandle::GetInstance().CheckOptsNeedOptimize(opts);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle017 end";
}

/**
 * @tc.name: StartHandle018
 * @tc.desc: test CheckImageNeedResize when image size is smaller than threshold
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle018, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle018 start";
    ImageInfo info;
    info.size.width = 800;
    info.size.height = 600;
    bool isValid = ImageHandle::GetInstance().CheckImageNeedResize(info);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle018 end";
}

/**
 * @tc.name: StartHandle019
 * @tc.desc: test CheckImageNeedResize when width is smaller than threshold
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle019, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle019 start";
    ImageInfo info;
    info.size.width = 1000;
    info.size.height = 2000;
    bool isValid = ImageHandle::GetInstance().CheckImageNeedResize(info);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle019 end";
}

/**
 * @tc.name: StartHandle020
 * @tc.desc: test CheckImageNeedResize when height is smaller than threshold
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle020, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle020 start";
    ImageInfo info;
    info.size.width = 2000;
    info.size.height = 1000;
    bool isValid = ImageHandle::GetInstance().CheckImageNeedResize(info);
    ASSERT_EQ(isValid, false);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle020 end";
}

/**
 * @tc.name: StartHandle021
 * @tc.desc: test CheckImageNeedResize when image size equals threshold
 * @tc.type: FUNC
 */
HWTEST_F(ImageHandleTest, StartHandle021, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle021 start";
    ImageInfo info;
    info.size.width = 1080;
    info.size.height = 1920;
    bool isValid = ImageHandle::GetInstance().CheckImageNeedResize(info);
    ASSERT_EQ(isValid, true);
    GTEST_LOG_(INFO) << "ImageHandle: StartHandle021 end";
}
} // namespace Media
} // namespace OHOS