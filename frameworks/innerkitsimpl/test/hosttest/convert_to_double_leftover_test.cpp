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

/*
 * Host leftover test for ConvertToDouble ERANGE / trailing-junk polarity
 * (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/convert_to_double_leftover_test.cpp \
 *   -o /tmp/convert_to_double_leftover_test && /tmp/convert_to_double_leftover_test
 */

#include "convert_to_double_leftover.h"

#include <cstdio>
#include <string>

using OHOS::Media::ConvertToDoubleLeftover::ConvertToDouble;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static void TestAcceptCleanDecimal()
{
    double value = 0.0;
    EXPECT_TRUE(ConvertToDouble("2.5", value));
    EXPECT_TRUE(value == 2.5);

    value = 0.0;
    EXPECT_TRUE(ConvertToDouble(".5", value));
    EXPECT_TRUE(value == 0.5);
}

static void TestRejectErange()
{
    double value = 0.0;
    /* leftover polarity accepted 1e99999 as success + inf */
    EXPECT_TRUE(!ConvertToDouble("1e99999", value));

    value = 0.0;
    EXPECT_TRUE(!ConvertToDouble("1e-99999", value));
}

static void TestRejectTrailingJunk()
{
    double value = 0.0;
    /* leftover polarity accepted 1.5abc as 1.5 */
    EXPECT_TRUE(!ConvertToDouble("1.5abc", value));
}

static void TestRejectEmpty()
{
    double value = 42.0;
    /* leftover polarity accepted empty as success + 0 */
    EXPECT_TRUE(!ConvertToDouble("", value));
}

int main()
{
    TestAcceptCleanDecimal();
    TestRejectErange();
    TestRejectTrailingJunk();
    TestRejectEmpty();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("ConvertToDouble leftover host tests passed\n");
    return 0;
}
