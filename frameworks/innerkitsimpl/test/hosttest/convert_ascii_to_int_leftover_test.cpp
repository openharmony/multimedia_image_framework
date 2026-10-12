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
 * Host leftover test for ConvertAsciiToInt ImageMagick wrap budget (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/convert_ascii_to_int_leftover_test.cpp \
 *   -o /tmp/convert_ascii_to_int_leftover_test && \
 *   /tmp/convert_ascii_to_int_leftover_test
 */

#include "convert_ascii_to_int_leftover.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using OHOS::Media::ConvertAsciiToIntLeftover::ConvertAsciiToInt;
using OHOS::Media::ConvertAsciiToIntLeftover::ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
using OHOS::Media::ConvertAsciiToIntLeftover::SUCCESS;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static std::string MakeHex(size_t byteCount)
{
    std::string hex;
    hex.reserve(byteCount * 2);
    for (size_t i = 0; i < byteCount; ++i) {
        char buf[3];
        std::snprintf(buf, sizeof(buf), "%02x", static_cast<unsigned>((i + 1) & 0xff));
        hex += buf;
    }
    return hex;
}

/* ImageMagick WriteRawProfile inserts a newline every 36 hex pairs (72 chars). */
static std::string MagickWrap(const std::string &hex)
{
    std::string out;
    out.reserve(hex.size() + hex.size() / 72 + 1);
    for (size_t i = 0; i < hex.size(); ++i) {
        if (i != 0 && (i % 72) == 0) {
            out.push_back('\n');
        }
        out.push_back(hex[i]);
    }
    return out;
}

/*
 * Exact leftover loop from png_image_chunk_utils.cpp ConvertAsciiToInt.
 * sourceLength counts skipped newlines, so wrapped 40-byte succeeds with dest[39]=0x20.
 */
static int ConvertAsciiToIntLegacy(const char *sourcePtr, size_t exifInfoLength, unsigned char *destPtr)
{
    if (sourcePtr == nullptr || destPtr == nullptr) {
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    auto isHex = [](char value) {
        return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f');
    };
    auto hexVal = [](char value) -> unsigned char {
        if (value >= '0' && value <= '9') {
            return static_cast<unsigned char>(value - '0');
        }
        return static_cast<unsigned char>(value - 'a' + 10);
    };
    const size_t sourceLength = exifInfoLength * 2;
    size_t sourcePtrCount = 0;
    for (size_t i = 0; i < sourceLength && sourcePtrCount < sourceLength; i++) {
        while (sourcePtrCount < sourceLength && !isHex(*sourcePtr)) {
            if (*sourcePtr == '\0') {
                return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
            }
            sourcePtr++;
            sourcePtrCount++;
        }
        if (sourcePtrCount < sourceLength) {
            if ((i % 2) == 0) {
                *destPtr = static_cast<unsigned char>(16 * hexVal(*sourcePtr++));
            } else {
                (*destPtr++) += hexVal(*sourcePtr++);
            }
            sourcePtrCount++;
        } else {
            return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
        }
    }
    return SUCCESS;
}

static void TestLegacyWrappedDropsLastNibble()
{
    const std::string hex = MakeHex(40);
    const std::string wrapped = MagickWrap(hex);
    EXPECT_TRUE(wrapped.size() == 81);
    EXPECT_TRUE(wrapped[72] == '\n');
    std::vector<unsigned char> dest(40, 0);
    EXPECT_TRUE(ConvertAsciiToIntLegacy(wrapped.c_str(), 40, dest.data()) == SUCCESS);
    EXPECT_TRUE(dest[0] == 0x01);
    EXPECT_TRUE(dest[35] == 0x24);
    EXPECT_TRUE(dest[39] == 0x20); /* leftover: last nibble dropped, not 0x28 */
}

static void TestLeftoverWrappedKeepsLastByte()
{
    const std::string hex = MakeHex(40);
    const std::string wrapped = MagickWrap(hex);
    std::vector<unsigned char> dest(40, 0);
    const char *endPtr = wrapped.data() + wrapped.size() - 1;
    EXPECT_TRUE(ConvertAsciiToInt(wrapped.c_str(), 40, dest.data(), endPtr) == SUCCESS);
    EXPECT_TRUE(dest[0] == 0x01);
    EXPECT_TRUE(dest[35] == 0x24);
    EXPECT_TRUE(dest[39] == 0x28);
}

static void TestUnwrappedStillSucceeds()
{
    const std::string hex = MakeHex(40);
    std::vector<unsigned char> dest(40, 0);
    EXPECT_TRUE(ConvertAsciiToInt(hex.c_str(), 40, dest.data(), nullptr) == SUCCESS);
    EXPECT_TRUE(dest[0] == 0x01);
    EXPECT_TRUE(dest[39] == 0x28);
}

static void TestNullArgs()
{
    unsigned char dest = 0;
    const char hex[] = "4a";
    EXPECT_TRUE(ConvertAsciiToInt(nullptr, 1, &dest, nullptr) == ERR_IMAGE_SOURCE_DATA_INCOMPLETE);
    EXPECT_TRUE(ConvertAsciiToInt(hex, 1, nullptr, nullptr) == ERR_IMAGE_SOURCE_DATA_INCOMPLETE);
}

int main()
{
    TestLegacyWrappedDropsLastNibble();
    TestLeftoverWrappedKeepsLastByte();
    TestUnwrappedStillSucceeds();
    TestNullArgs();
    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover host checks failed\n", gFails);
        return 1;
    }
    std::puts("convert_ascii_to_int leftover host test OK");
    return 0;
}
