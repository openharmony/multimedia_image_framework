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

#if defined(SUPPORT_LIBTIFF)
#include "tiff_exif_metadata.h"

#include "exif_metadata.h"
#include "image_log.h"
#include "media_errors.h"
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "TiffExifMetadata"

namespace OHOS {
namespace Media {

using TiffExifGetType = std::function<uint32_t(TIFF* tif, uint32_t tag, MetadataValue& value)>;
constexpr size_t TIFF_WHITE_POINT_VALUE_COUNT = 2;
constexpr size_t TIFF_PRIMARY_CHROMATICITIES_VALUE_COUNT = 6;
constexpr uint8_t TIFF_TRANSFER_TABLE_MAX_BIT_DEPTH = 16;

static uint32_t GetTiffAscii(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    char* str = nullptr;
    if (TIFFGetField(tif, tag, &str) == 1 && str != nullptr) {
        value.stringValue = std::string(str);
        return SUCCESS;
    }
    return ERR_IMAGE_PROPERTY_NOT_EXIST;
}

static uint32_t GetTiffUint16(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    uint16_t v = 0;
    if (TIFFGetField(tif, tag, &v) == 1) {
        value.intArrayValue.assign(1, static_cast<int64_t>(v));
        return SUCCESS;
    }
    return ERR_IMAGE_PROPERTY_NOT_EXIST;
}

static uint32_t GetTiffUint32(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    uint32_t v = 0;
    if (TIFFGetField(tif, tag, &v) == 1) {
        value.intArrayValue.assign(1, static_cast<int64_t>(v));
        return SUCCESS;
    }
    return ERR_IMAGE_PROPERTY_NOT_EXIST;
}

static uint32_t GetTiffFloat(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    float v = 0.0f;
    if (TIFFGetField(tif, tag, &v) == 1) {
        value.doubleArrayValue.assign(1, static_cast<double>(v));
        return SUCCESS;
    }
    return ERR_IMAGE_PROPERTY_NOT_EXIST;
}

static uint32_t GetTiffWhitePoint(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    const float* arr = nullptr;
    CHECK_ERROR_RETURN_RET(TIFFGetField(tif, tag, &arr) != 1 || arr == nullptr, ERR_IMAGE_PROPERTY_NOT_EXIST);
    value.doubleArrayValue.assign(arr, arr + TIFF_WHITE_POINT_VALUE_COUNT);
    return SUCCESS;
}

static uint32_t GetTiffPrimaryChromaticities(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    const float* arr = nullptr;
    CHECK_ERROR_RETURN_RET(TIFFGetField(tif, tag, &arr) != 1 || arr == nullptr, ERR_IMAGE_PROPERTY_NOT_EXIST);
    value.doubleArrayValue.assign(arr, arr + TIFF_PRIMARY_CHROMATICITIES_VALUE_COUNT);
    return SUCCESS;
}

static void AppendTransferTable(uint16_t* arr, size_t tableSize, std::ostringstream& oss, bool& first)
{
    CHECK_ERROR_RETURN(arr == nullptr);
    for (size_t i = 0; i < tableSize; i++) {
        if (!first) {
            oss << ",";
        }
        oss << arr[i];
        first = false;
    }
}

static uint32_t GetTiffTransferFunction(TIFF* tif, uint32_t tag, MetadataValue& value)
{
    uint16_t* r = nullptr;
    uint16_t* g = nullptr;
    uint16_t* b = nullptr;
    CHECK_ERROR_RETURN_RET(TIFFGetField(tif, tag, &r, &g, &b) != 1, ERR_IMAGE_PROPERTY_NOT_EXIST);
    uint16_t bitsPerSample = 0;
    CHECK_ERROR_RETURN_RET(!TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample) || bitsPerSample == 0 ||
        bitsPerSample > TIFF_TRANSFER_TABLE_MAX_BIT_DEPTH, ERR_IMAGE_SOURCE_DATA);
    const size_t tableSize = static_cast<size_t>(1) << bitsPerSample;
    std::ostringstream oss;
    bool first = true;

    AppendTransferTable(r, tableSize, oss, first);
    AppendTransferTable(g, tableSize, oss, first);
    AppendTransferTable(b, tableSize, oss, first);
    value.stringValue = oss.str();
    return SUCCESS;
}

static const std::map<std::string, std::pair<uint32_t, TiffExifGetType>> KEY_TO_TAG_MAP = {
    {"TiffCompression", {TIFFTAG_COMPRESSION, GetTiffUint16}},
    {"TiffPhotometricInterpretation", {TIFFTAG_PHOTOMETRIC, GetTiffUint16}},
    {"TiffImageDescription", {TIFFTAG_IMAGEDESCRIPTION, GetTiffAscii}},
    {"TiffMake", {TIFFTAG_MAKE, GetTiffAscii}},
    {"TiffModel", {TIFFTAG_MODEL, GetTiffAscii}},
    {"TiffOrientation", {TIFFTAG_ORIENTATION, GetTiffUint16}},
    {"TiffXResolution", {TIFFTAG_XRESOLUTION, GetTiffFloat}},
    {"TiffYResolution", {TIFFTAG_YRESOLUTION, GetTiffFloat}},
    {"TiffResolutionUnit", {TIFFTAG_RESOLUTIONUNIT, GetTiffUint16}},
    {"TiffTransferFunction", {TIFFTAG_TRANSFERFUNCTION, GetTiffTransferFunction}},
    {"TiffSoftware", {TIFFTAG_SOFTWARE, GetTiffAscii}},
    {"TiffDateTime", {TIFFTAG_DATETIME, GetTiffAscii}},
    {"TiffArtist", {TIFFTAG_ARTIST, GetTiffAscii}},
    {"TiffWhitePoint", {TIFFTAG_WHITEPOINT, GetTiffWhitePoint}},
    {"TiffPrimaryChromaticities", {TIFFTAG_PRIMARYCHROMATICITIES, GetTiffPrimaryChromaticities}},
    {"TiffCopyright", {TIFFTAG_COPYRIGHT, GetTiffAscii}},
    {"TiffTileLength", {TIFFTAG_TILELENGTH, GetTiffUint32}},
    {"TiffTileWidth", {TIFFTAG_TILEWIDTH, GetTiffUint32}},
    {"TiffDocumentName", {TIFFTAG_DOCUMENTNAME, GetTiffAscii}},
    {"TiffHostComputer", {TIFFTAG_HOSTCOMPUTER, GetTiffAscii}},
};

bool GetTiffTagFromKey(const std::string& key, uint32_t& tagCode,
    TiffExifGetType* getter = nullptr)
{
    auto it = KEY_TO_TAG_MAP.find(key);
    if (it == KEY_TO_TAG_MAP.end()) {
        tagCode = 0;
        return false;
    }
    tagCode = it->second.first;
    if (getter != nullptr) {
        *getter = it->second.second;
    }
    return true;
}

uint32_t EnsureIFD0(TIFF* tif)
{
    return TIFFSetDirectory(tif, 0) ? SUCCESS : ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
}

void RestoreDirectory(TIFF* tif, const tdir_t savedDir, const uint64_t savedDirOffset)
{
    if (savedDirOffset != 0) {
        TIFFSetSubDirectory(tif, savedDirOffset);
    } else {
        TIFFSetDirectory(tif, savedDir);
    }
}

uint32_t SearchInDirectory(TIFF* tif, bool setOk, uint32_t tag, const TiffExifGetType& getter, MetadataValue& value)
{
    if (!setOk) {
        return EnsureIFD0(tif) != SUCCESS ? ERR_IMAGE_DECODE_EXIF_UNSUPPORT : ERR_IMAGE_PROPERTY_NOT_EXIST;
    }

    if (getter(tif, tag, value) == SUCCESS) {
        return SUCCESS;
    }
    return EnsureIFD0(tif) == SUCCESS ? ERR_IMAGE_PROPERTY_NOT_EXIST : ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
}

uint32_t SearchInExifIfd(TIFF* tif, uint32_t tag, const TiffExifGetType& getter, MetadataValue& value)
{
    CHECK_ERROR_RETURN_RET(!TIFFSetDirectory(tif, 0), ERR_IMAGE_DECODE_EXIF_UNSUPPORT);
    uint64_t exifIfdOffset = 0;
    if (TIFFGetField(tif, TIFFTAG_EXIFIFD, &exifIfdOffset) && exifIfdOffset != 0 &&
        TIFFReadEXIFDirectory(tif, exifIfdOffset)) {
        return getter(tif, tag, value);
    }
    return ERR_IMAGE_PROPERTY_NOT_EXIST;
}

TiffExifMetadata::TiffExifMetadata() : ExifMetadata(), tiffHandle_(nullptr)
{
}

TiffExifMetadata::TiffExifMetadata(ExifData* exifData, TIFF* tiffHandle,
                                   std::shared_ptr<BufferSourceStream> bufferStream)
    : ExifMetadata(exifData), tiffHandle_(tiffHandle), bufferStream_(bufferStream)
{
}

TiffExifMetadata::~TiffExifMetadata()
{
    if (tiffHandle_ != nullptr) {
        TIFFClose(tiffHandle_);
        tiffHandle_ = nullptr;
    }
}

int TiffExifMetadata::GetValue(const std::string& key, std::string& value) const
{
    return ExifMetadata::GetValue(key, value);
}

bool TiffExifMetadata::SetValue(const std::string& key, const std::string& value)
{
    return ExifMetadata::SetValue(key, value);
}

bool TiffExifMetadata::RemoveEntry(const std::string& key)
{
    return ExifMetadata::RemoveEntry(key);
}

std::shared_ptr<ImageMetadata> TiffExifMetadata::CloneMetadata()
{
    return ExifMetadata::CloneMetadata();
}

std::shared_ptr<TiffExifMetadata> TiffExifMetadata::Clone()
{
    return nullptr;
}

bool TiffExifMetadata::Marshalling(Parcel& parcel) const
{
    return ExifMetadata::Marshalling(parcel);
}

uint32_t TiffExifMetadata::GetExifProperty(MetadataValue& value)
{
    CHECK_ERROR_RETURN_RET_LOG(tiffHandle_ == nullptr, ERR_IMAGE_DECODE_EXIF_UNSUPPORT,
        "[%{public}s] tiffHandle_ is nullptr", __func__);
    std::string key = value.key;
    uint32_t tag = 0;
    TiffExifGetType getter = nullptr;
    if (!GetTiffTagFromKey(key, tag, &getter) || getter == nullptr) {
        if (ExifMetadata::GetValueByType(key, value) == SUCCESS) {
            value.type = ExifMetadata::GetPropertyValueType(key);
            return SUCCESS;
        }
        return ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    CHECK_ERROR_RETURN_RET_LOG(EnsureIFD0(tiffHandle_) != SUCCESS, ERR_IMAGE_DECODE_EXIF_UNSUPPORT,
        "[%{public}s] EnsureIFD0 failed", __func__);

    const tdir_t savedDir = TIFFCurrentDirectory(tiffHandle_);
    const uint64_t savedDirOffset = TIFFCurrentDirOffset(tiffHandle_);
    uint32_t ret = SearchInDirectory(tiffHandle_, TIFFSetDirectory(tiffHandle_, 0), tag, getter, value);
    CHECK_ERROR_RETURN_RET(ret != ERR_IMAGE_PROPERTY_NOT_EXIST, ret);

    ret = SearchInExifIfd(tiffHandle_, tag, getter, value);
    RestoreDirectory(tiffHandle_, savedDir, savedDirOffset);
    return ret == SUCCESS ? ret : ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
}

void TiffExifMetadata::GetAllTiffProperties(std::vector<MetadataValue>& result)
{
    CHECK_ERROR_RETURN_LOG(tiffHandle_ == nullptr, "[%{public}s] tiffHandle_ is nullptr", __func__);

    const auto& tiffMap = ExifMetadata::GetTiffMetadataMap();
    result.reserve(result.size() + tiffMap.size());
    for (const auto& [key, type] : tiffMap) {
        MetadataValue entry;
        entry.key = key;
        entry.type = type;
        entry.intArrayValue.clear();

        if (GetExifProperty(entry) == SUCCESS) {
            result.push_back(std::move(entry));
        }
    }
}

} // namespace Media
} // namespace OHOS
#endif // defined(SUPPORT_LIBTIFF)
