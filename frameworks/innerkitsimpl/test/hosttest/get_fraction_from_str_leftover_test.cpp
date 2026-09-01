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
 * Host leftover test for GetFractionFromStr float-to-int truncation
 * (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/get_fraction_from_str_leftover_test.cpp \
 *   -o /tmp/get_fraction_from_str_leftover_test && /tmp/get_fraction_from_str_leftover_test
 */

#include "get_fraction_from_str_leftover.h"

#include <cmath>
#include <cstdio>
#include <string>

using OHOS::Media::GetFractionFromStrLeftover::GetFractionFromStr;
using OHOS::Media::GetFractionFromStrLeftover::ScaleDecimalDigits;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static void TestLeftoverZeroPointTwoNine()
{
    bool isOutRange = false;
    /* leftover: (int)(0.29 * 100) == 28, so "0.29" became 28/100 */
    const std::string result = GetFractionFromStr("0.29", isOutRange);
    EXPECT_TRUE(result == "29/100");
    EXPECT_TRUE(result != "28/100");
    EXPECT_TRUE(!isOutRange);

    int numerator = 0;
    int denominator = 0;
    EXPECT_TRUE(ScaleDecimalDigits("29", numerator, denominator));
    EXPECT_TRUE(numerator == 29);
    EXPECT_TRUE(denominator == 100);
}

static void TestLeftoverZeroPointFiveSeven()
{
    bool isOutRange = false;
    /* leftover: (int)(0.57 * 100) == 56, so "0.57" became 56/100 */
    const std::string result = GetFractionFromStr("0.57", isOutRange);
    EXPECT_TRUE(result == "57/100");
    EXPECT_TRUE(result != "56/100");
    EXPECT_TRUE(!isOutRange);

    int numerator = 0;
    int denominator = 0;
    EXPECT_TRUE(ScaleDecimalDigits("57", numerator, denominator));
    EXPECT_TRUE(numerator == 57);
    EXPECT_TRUE(denominator == 100);
}

static void TestExistingReducedCases()
{
    bool isOutRange = false;
    EXPECT_TRUE(GetFractionFromStr("2.5", isOutRange) == "5/2");
    EXPECT_TRUE(!isOutRange);
    EXPECT_TRUE(GetFractionFromStr("10.25", isOutRange) == "41/4");
    EXPECT_TRUE(!isOutRange);
    EXPECT_TRUE(GetFractionFromStr("7.0", isOutRange) == "7/1");
    EXPECT_TRUE(!isOutRange);
    EXPECT_TRUE(GetFractionFromStr("0.0", isOutRange) == "0/1");
    EXPECT_TRUE(!isOutRange);
    EXPECT_TRUE(GetFractionFromStr("1.17976abc", isOutRange) == "14747/12500");
    EXPECT_TRUE(!isOutRange);
}

static void TestLeftoverDoublePowTruncates()
{
    /* document the leftover cast: 0.29*100 and 0.57*100 truncate below the digit value */
    EXPECT_TRUE(static_cast<int>(0.29 * std::pow(10, 2)) == 28);
    EXPECT_TRUE(static_cast<int>(0.57 * std::pow(10, 2)) == 56);
}

int main()
{
    TestLeftoverZeroPointTwoNine();
    TestLeftoverZeroPointFiveSeven();
    TestExistingReducedCases();
    TestLeftoverDoublePowTruncates();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("GetFractionFromStr leftover host tests passed\n");
    return 0;
}
