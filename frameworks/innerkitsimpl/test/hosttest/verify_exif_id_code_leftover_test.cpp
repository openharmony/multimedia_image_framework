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
 * Host leftover test for VerifyExifIdCode off-by-one search (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/verify_exif_id_code_leftover_test.cpp \
 *   -o /tmp/verify_exif_id_code_leftover_test && /tmp/verify_exif_id_code_leftover_test
 */

#include "verify_exif_id_code_leftover.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>

using OHOS::Media::VerifyExifIdCodeLeftover::EXIF_ID_CODE;
using OHOS::Media::VerifyExifIdCodeLeftover::EXIF_ID_CODE_SIZE;
using OHOS::Media::VerifyExifIdCodeLeftover::VerifyExifIdCode;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static size_t LeftoverVerifyExifIdCode(const uint8_t *data, size_t length)
{
    size_t exifIdPos = std::numeric_limits<size_t>::max();
    /* leftover: i < length - size never checks the last valid start */
    for (size_t i = 0; i < length - EXIF_ID_CODE_SIZE; ++i) {
        if (std::memcmp(data + i, EXIF_ID_CODE, EXIF_ID_CODE_SIZE) == 0) {
            return i;
        }
    }
    return exifIdPos;
}

static void TestLeftoverMissesExact6AndEndMinusOne()
{
    const uint8_t exact6[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
    EXPECT_TRUE(LeftoverVerifyExifIdCode(exact6, sizeof(exact6)) == std::numeric_limits<size_t>::max());

    const uint8_t xExact6[] = { 'X', 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
    EXPECT_TRUE(LeftoverVerifyExifIdCode(xExact6, sizeof(xExact6)) == std::numeric_limits<size_t>::max());
}

static void TestExact6Hits()
{
    const uint8_t exact6[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
    EXPECT_TRUE(VerifyExifIdCode(exact6, sizeof(exact6)) == 0);
}

static void TestXPlusExact6Hits()
{
    const uint8_t xExact6[] = { 'X', 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
    EXPECT_TRUE(VerifyExifIdCode(xExact6, sizeof(xExact6)) == 1);
}

static void TestMidBufferStillHits()
{
    const uint8_t mid[] = { 0x00, 0x01, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 0x02 };
    EXPECT_TRUE(VerifyExifIdCode(mid, sizeof(mid)) == 2);
}

static void TestShortLengthReturnsMax()
{
    const uint8_t shortBuf[] = { 0x45, 0x78, 0x69, 0x66, 0x00 };
    EXPECT_TRUE(VerifyExifIdCode(shortBuf, sizeof(shortBuf)) == std::numeric_limits<size_t>::max());
    EXPECT_TRUE(VerifyExifIdCode(nullptr, EXIF_ID_CODE_SIZE) == std::numeric_limits<size_t>::max());
}

int main()
{
    TestLeftoverMissesExact6AndEndMinusOne();
    TestExact6Hits();
    TestXPlusExact6Hits();
    TestMidBufferStillHits();
    TestShortLengthReturnsMax();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("VerifyExifIdCode leftover host tests passed\n");
    return 0;
}
