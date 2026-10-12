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
 * Host leftover test for GetExifInfoLen newline skip (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/get_exif_info_len_leftover_test.cpp \
 *   -o /tmp/get_exif_info_len_leftover_test && /tmp/get_exif_info_len_leftover_test
 */

#include "get_exif_info_len_leftover.h"

#include <cstdio>
#include <cstring>
#include <string>

using OHOS::Media::GetExifInfoLenLeftover::GetExifInfoLen;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static const char *Parse(const std::string &text, size_t &lengthOut)
{
    if (text.size() < 2) {
        return nullptr;
    }
    const char *sourcePtr = text.data();
    const char *endPtr = text.data() + text.size() - 1;
    return GetExifInfoLen(sourcePtr, &lengthOut, endPtr);
}

static void TestAcceptNewLineAfterDigits()
{
    size_t lengthOut = 0;
    const char *hexStart = Parse("6\n457869", lengthOut);
    EXPECT_TRUE(hexStart != nullptr);
    EXPECT_TRUE(lengthOut == 6);
    EXPECT_TRUE(std::strncmp(hexStart, "457869", 6) == 0);
}

static void TestRejectNonNewLineAfterDigits()
{
    size_t lengthOut = 42;
    const char *hexStart = Parse("6X457869", lengthOut);
    /* leftover skip ate 'X' and continued hex parse on "457869" */
    EXPECT_TRUE(hexStart == nullptr);
}

int main()
{
    TestAcceptNewLineAfterDigits();
    TestRejectNonNewLineAfterDigits();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("GetExifInfoLen leftover host tests passed\n");
    return 0;
}
