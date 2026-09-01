/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Host leftover test for PNG raw-profile hex + TIFF slice (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/png_raw_profile_leftover_test.cpp \
 *   -o /tmp/png_raw_profile_leftover_test && /tmp/png_raw_profile_leftover_test
 */

#include "png_raw_profile_leftover.h"

#include <cstdio>
#include <cstring>
#include <vector>

using OHOS::Media::PngRawProfileLeftover::DecodeHexAscii;
using OHOS::Media::PngRawProfileLeftover::EXIF_HEADER_SIZE;
using OHOS::Media::PngRawProfileLeftover::FindExifHeadPos;
using OHOS::Media::PngRawProfileLeftover::GetTiffSlice;
using OHOS::Media::PngRawProfileLeftover::HEX_ASCII_TO_INT;
using OHOS::Media::PngRawProfileLeftover::IsHexAscii;
using OHOS::Media::PngRawProfileLeftover::SliceTiffAfterExifHeader;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static void TestIsHexAsciiAcceptsUppercase()
{
    EXPECT_TRUE(IsHexAscii('0') && IsHexAscii('9'));
    EXPECT_TRUE(IsHexAscii('a') && IsHexAscii('f'));
    EXPECT_TRUE(IsHexAscii('A') && IsHexAscii('F'));
    EXPECT_TRUE(!IsHexAscii('G') && !IsHexAscii('g') && !IsHexAscii(' '));
}

static void TestHexTableUppercaseSlots()
{
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('A')] == 10);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('B')] == 11);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('C')] == 12);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('D')] == 13);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('E')] == 14);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('F')] == 15);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('a')] == 10);
    EXPECT_TRUE(HEX_ASCII_TO_INT[static_cast<unsigned char>('f')] == 15);
}

static void TestLeftoverLowercaseExifHex()
{
    std::vector<uint8_t> dest;
    EXPECT_TRUE(DecodeHexAscii("45786966", 8, dest));
    const uint8_t expected[] = { 0x45, 0x78, 0x69, 0x66 };
    EXPECT_TRUE(dest.size() == 4);
    EXPECT_TRUE(std::memcmp(dest.data(), expected, sizeof(expected)) == 0);
}

static void TestLeftoverUppercaseHex()
{
    std::vector<uint8_t> dest;
    EXPECT_TRUE(DecodeHexAscii("ABCDEF", 6, dest));
    const uint8_t expected[] = { 0xAB, 0xCD, 0xEF };
    EXPECT_TRUE(dest.size() == 3);
    EXPECT_TRUE(std::memcmp(dest.data(), expected, sizeof(expected)) == 0);
}

static void TestLeftoverTiffSliceUsesExifHeadPos()
{
    /* 2 pad bytes, then Exif\0\0, then dummy TIFF "II*\0". */
    const uint8_t payload[] = {
        0x00, 0x00,
        0x45, 0x78, 0x69, 0x66, 0x00, 0x00,
        0x49, 0x49, 0x2A, 0x00,
    };
    const size_t payloadSize = sizeof(payload);

    size_t exifHeadPos = 0;
    EXPECT_TRUE(FindExifHeadPos(payload, payloadSize, exifHeadPos));
    EXPECT_TRUE(exifHeadPos == 2);

    size_t leftoverOffset = EXIF_HEADER_SIZE;
    EXPECT_TRUE(leftoverOffset == 6);
    EXPECT_TRUE(payload[leftoverOffset] == 0x00);

    size_t tiffOffset = 0;
    size_t tiffLength = 0;
    EXPECT_TRUE(GetTiffSlice(exifHeadPos, payloadSize, tiffOffset, tiffLength));
    EXPECT_TRUE(tiffOffset == exifHeadPos + EXIF_HEADER_SIZE);
    EXPECT_TRUE(tiffOffset == 8);
    EXPECT_TRUE(tiffLength == 4);
    EXPECT_TRUE(payload[tiffOffset] == 0x49);

    size_t slicedOffset = 0;
    size_t slicedLength = 0;
    EXPECT_TRUE(SliceTiffAfterExifHeader(payload, payloadSize, slicedOffset, slicedLength));
    EXPECT_TRUE(slicedOffset == 8 && slicedLength == 4);
    EXPECT_TRUE(slicedOffset != leftoverOffset);
}

int main()
{
    TestIsHexAsciiAcceptsUppercase();
    TestHexTableUppercaseSlots();
    TestLeftoverLowercaseExifHex();
    TestLeftoverUppercaseHex();
    TestLeftoverTiffSliceUsesExifHeadPos();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("png raw-profile leftover host tests passed\n");
    return 0;
}
