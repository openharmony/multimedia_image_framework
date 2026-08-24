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
 * Host leftover test for GPSVersionID unescaped '.' (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/gps_version_id_leftover_test.cpp \
 *   -o /tmp/gps_version_id_leftover_test && /tmp/gps_version_id_leftover_test
 */

#include "gps_version_id_leftover.h"

#include <cstdio>
#include <regex>
#include <string>

using OHOS::Media::GpsVersionIdLeftover::MatchesGpsVersionId;
using OHOS::Media::GpsVersionIdLeftover::TRIBLE_INT_WITH_DOT_REGEX;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static void TestLeftoverUnescapedDotMatchesAnyChar()
{
    /* leftover: R"(^[0-9]+.[0-9]+.[0-9]+.[0-9]+$)" '.' is any char */
    const auto leftoverRegex = R"(^[0-9]+.[0-9]+.[0-9]+.[0-9]+$)";
    EXPECT_TRUE(std::regex_match(std::string("9x9x9x9"), std::regex(leftoverRegex)));
    EXPECT_TRUE(std::regex_match(std::string("9.9.9.9"), std::regex(leftoverRegex)));
}

static void TestFixedRegexRejectsAnyCharDot()
{
    EXPECT_TRUE(!std::regex_match(std::string("9x9x9x9"), std::regex(TRIBLE_INT_WITH_DOT_REGEX)));
    EXPECT_TRUE(std::regex_match(std::string("9.9.9.9"), std::regex(TRIBLE_INT_WITH_DOT_REGEX)));

    EXPECT_TRUE(!MatchesGpsVersionId("9x9x9x9"));
    EXPECT_TRUE(MatchesGpsVersionId("9.9.9.9"));
    EXPECT_TRUE(MatchesGpsVersionId("2.2.0.0"));
}

int main()
{
    TestLeftoverUnescapedDotMatchesAnyChar();
    TestFixedRegexRejectsAnyCharDot();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("GPSVersionID leftover host tests passed\n");
    return 0;
}
