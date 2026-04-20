/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "image_func_timer.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {

class ImageFuncTimerTest : public testing::Test {
public:
    ImageFuncTimerTest() {}
    ~ImageFuncTimerTest() {}
};

/**
 * @tc.name: ImageFuncTimerTest001
 * @tc.desc: test ImageFuncTimer constructor with string parameter
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest001 start";
    std::string desc = "TestTimer";
    {
        ImageFuncTimer timer(desc);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest001 end";
}

/**
 * @tc.name: ImageFuncTimerTest002
 * @tc.desc: test ImageFuncTimer constructor with empty string
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest002 start";
    std::string desc = "";
    {
        ImageFuncTimer timer(desc);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest002 end";
}

/**
 * @tc.name: ImageFuncTimerTest003
 * @tc.desc: test ImageFuncTimer constructor with format string
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest003 start";
    {
        ImageFuncTimer timer("DecodeImage_%s", "JPEG");
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest003 end";
}

/**
 * @tc.name: ImageFuncTimerTest004
 * @tc.desc: test ImageFuncTimer constructor with format string and integer
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest004 start";
    {
        ImageFuncTimer timer("ProcessFrame_%d", 100);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest004 end";
}

/**
 * @tc.name: ImageFuncTimerTest005
 * @tc.desc: test ImageFuncTimer constructor with nullptr format
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest005 start";
    {
        ImageFuncTimer timer(nullptr);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest005 end";
}

/**
 * @tc.name: ImageFuncTimerTest006
 * @tc.desc: test ImageFuncTimer constructor with multiple format parameters
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest006 start";
    {
        ImageFuncTimer timer("Image_%s_Width_%d_Height_%d", "PNG", 1920, 1080);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest006 end";
}

/**
 * @tc.name: ImageFuncTimerTest007
 * @tc.desc: test ImageFuncTimer constructor with long description string
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest007 start";
    std::string longDesc(200, 'a');
    {
        ImageFuncTimer timer(longDesc);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest007 end";
}

/**
 * @tc.name: ImageFuncTimerTest008
 * @tc.desc: test ImageFuncTimer constructor with format string exceeding buffer size
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest008 start";
    {
        std::string longStr(300, 'x');
        ImageFuncTimer timer("%s", longStr.c_str());
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest008 end";
}

/**
 * @tc.name: ImageFuncTimerTest009
 * @tc.desc: test ImageFuncTimer destructor called immediately after construction
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest009 start";
    {
        ImageFuncTimer timer1("Timer1");
        ImageFuncTimer timer2("Timer2");
        ImageFuncTimer timer3("Timer3");
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest009 end";
}

/**
 * @tc.name: ImageFuncTimerTest010
 * @tc.desc: test ImageFuncTimer with special characters in description
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest010 start";
    {
        ImageFuncTimer timer("Test@#$%^&*()");
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest010 end";
}

/**
 * @tc.name: ImageFuncTimerTest011
 * @tc.desc: test ImageFuncTimer constructor with format string containing percentage
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest011 start";
    {
        ImageFuncTimer timer("Progress_%d%%", 50);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest011 end";
}

/**
 * @tc.name: ImageFuncTimerTest012
 * @tc.desc: test ImageFuncTimer constructor with chinese characters
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest012 start";
    {
        ImageFuncTimer timer("DecodeImage");
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest012 end";
}

/**
 * @tc.name: ImageFuncTimerTest013
 * @tc.desc: test ImageFuncTimer constructor with empty format string
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest013 start";
    {
        ImageFuncTimer timer("");
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest013 end";
}

/**
 * @tc.name: ImageFuncTimerTest014
 * @tc.desc: test ImageFuncTimer in nested scope
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest014 start";
    {
        ImageFuncTimer outerTimer("OuterTimer");
        {
            ImageFuncTimer innerTimer("InnerTimer");
            {
                ImageFuncTimer nestedTimer("NestedTimer");
            }
        }
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest014 end";
}

/**
 * @tc.name: ImageFuncTimerTest015
 * @tc.desc: test ImageFuncTimer constructor with float format parameter
 * @tc.type: FUNC
 */
HWTEST_F(ImageFuncTimerTest, ImageFuncTimerTest015, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest015 start";
    {
        ImageFuncTimer timer("Scale_%.2f", 1.5);
    }
    GTEST_LOG_(INFO) << "ImageFuncTimerTest: ImageFuncTimerTest015 end";
}

} // namespace Media
} // namespace OHOS
