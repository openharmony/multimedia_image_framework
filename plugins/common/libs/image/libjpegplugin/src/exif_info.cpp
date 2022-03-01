/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "exif_info.h"
#include <algorithm>
#include <cstdio>
#include "string_ex.h"

namespace OHOS {
namespace ImagePlugin {
namespace {
    using namespace OHOS::HiviewDFX;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "Exif" };
    static const int PARSE_EXIF_SUCCESS = 0;
    static const int PARSE_EXIF_DATA_ERROR = 10001;
    static const int PARSE_EXIF_IFD_ERROR = 10002;
    static const int BUFFER_POSITION_6 = 6;
    static const int BUFFER_POSITION_7 = 7;
    static const int BUFFER_POSITION_8 = 8;
    static const int BUFFER_POSITION_9 = 9;
    static const int LENGTH_OFFSET_2 = 2;
    static const int MOVE_OFFSET_8 = 8;
    static const unsigned int IMAGE_DATA_OFFSET = 20; // start of JPEG image data section
    /* raw EXIF header data */
    static const unsigned char exifHeader[] = {
        0xff, 0xd8, 0xff, 0xe1
    };
    /*
    static const std::string KEY_BITS_PER_SAMPLE = "Bits per Sample";
    static const std::string KEY_ORIENTATION = "Orientation";
    static const std::string KEY_IMAGE_LENGTH = "RelatedImageLength";
    static const std::string KEY_IMAGE_WIDTH = "RelatedImageWidth";
    static const std::string KEY_GPS_LATITUDE = "Latitude";
    static const std::string KEY_GPS_LONGITUDE = "Longitude";
    static const std::string KEY_GPS_LATITUDE_REF = "North or South Latitude";
    static const std::string KEY_GPS_LONGITUDE_REF = "East or West Longitude";
    static const std::string KEY_DATE_TIME_ORIGINAL = "DateTimeOriginal";*/
}

EXIFInfo::EXIFInfo() : imageFileDirectory_(EXIF_IFD_COUNT), exifData_(nullptr)
{
}

EXIFInfo::~EXIFInfo()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
    }
}

int EXIFInfo::ParseExifData(const unsigned char *buf, unsigned len)
{
    HiLog::Debug(LABEL, "ParseExifData ENTER");
    exifData_ = exif_data_new_from_data(buf, len);
    if (!exifData_) {
        return PARSE_EXIF_DATA_ERROR;
    }
    exif_data_foreach_content(exifData_,
        [](ExifContent *ec, void *userData) {
            ExifIfd ifd = exif_content_get_ifd(ec);
            ((EXIFInfo*)userData)->imageFileDirectory_ = ifd;
            if (ifd == EXIF_IFD_COUNT) {
                HiLog::Debug(LABEL, "GetIfd ERROR");
                return;
            }
            exif_content_foreach_entry(ec,
                [](ExifEntry *ee, void* userData) {
                    //std::string tagName = exif_tag_get_name_in_ifd(ee->tag,
                    //    ((EXIFInfo*)userData)->imageFileDirectory_);
                    char tagValueChar[1024];
                    exif_entry_get_value(ee, tagValueChar, sizeof(tagValueChar));
                    std::string tagValueStr(&tagValueChar[0], &tagValueChar[strlen(tagValueChar)]);
                    // ((EXIFInfo*)userData)->SetExifTagValues(tagName, tagValueStr);
                    ((EXIFInfo*)userData)->SetExifTagValues(ee->tag, tagValueStr);
                }, userData);
        }, this);

    if (imageFileDirectory_ == EXIF_IFD_COUNT) {
        return PARSE_EXIF_IFD_ERROR;
    }
    return PARSE_EXIF_SUCCESS;
}

int EXIFInfo::ParseExifData(const std::string &data)
{
    return ParseExifData((const unsigned char *)data.data(), data.length());
}

void EXIFInfo::SetExifTagValues(const ExifTag &tag, const std::string &value)
{
    /*
    if (IsSameTextStr(name, KEY_BITS_PER_SAMPLE)) {
        bitsPerSample_ = value;
    } else if (IsSameTextStr(name, KEY_ORIENTATION)) {
        orientation_ = value;
    } else if (IsSameTextStr(name, KEY_IMAGE_LENGTH)) {
        imageLength_ = value;
    } else if (IsSameTextStr(name, KEY_IMAGE_WIDTH)) {
        imageWidth_ = value;
    } else if (IsSameTextStr(name, KEY_GPS_LATITUDE)) {
        gpsLatitude_ = value;
    } else if (IsSameTextStr(name, KEY_GPS_LONGITUDE)) {
        gpsLongitude_ = value;
    } else if (IsSameTextStr(name, KEY_GPS_LATITUDE_REF)) {
        gpsLatitudeRef_ = value;
    } else if (IsSameTextStr(name, KEY_GPS_LONGITUDE_REF)) {
        gpsLongitudeRef_ = value;
    } else if (IsSameTextStr(name, KEY_DATE_TIME_ORIGINAL)) {
        dateTimeOriginal_ = value;
    } else {
        HiLog::Error(LABEL, "No match tag name!");
    } */

    if (tag == EXIF_TAG_BITS_PER_SAMPLE) {
        bitsPerSample_ = value;
    } else if (tag == EXIF_TAG_ORIENTATION) {
        orientation_ = value;
    } else if (tag == EXIF_TAG_IMAGE_LENGTH) {
        imageLength_ = value;
    } else if (tag == EXIF_TAG_IMAGE_WIDTH) {
        imageWidth_ = value;
    } else if (tag == EXIF_TAG_GPS_LATITUDE) {
        gpsLatitude_ = value;
    } else if (tag == EXIF_TAG_GPS_LONGITUDE) {
        gpsLongitude_ = value;
    } else if (tag == EXIF_TAG_GPS_LATITUDE_REF) {
        gpsLatitudeRef_ = value;
    } else if (tag == EXIF_TAG_GPS_LONGITUDE_REF) {
        gpsLongitudeRef_ = value;
    } else if (tag == EXIF_TAG_DATE_TIME_ORIGINAL) {
        dateTimeOriginal_ = value;
    } else {
        HiLog::Error(LABEL, "No match tag name!");
    }
}

bool EXIFInfo::ModifyExifData(const ExifTag &tag, const std::string &value, const std::string &path)
{
    FILE *file = fopen(path.c_str(), "wb+");
    if (file == nullptr) {
        HiLog::Error(LABEL, "Error creating file %{public}s", path.c_str());
        return false;
    }

    // read jpeg file to buff
    unsigned long fileLength = GetFileSize(file);
    if (fileLength == 0) {
        HiLog::Error(LABEL, "Get file size failed.");
        fclose(file);
        return false;
    }
    unsigned char *fileBuf = (unsigned char *)malloc(fileLength);
    if (fileBuf == nullptr) {
        HiLog::Error(LABEL, "Allocate buf for %{public}s failed.", path.c_str());
        fclose(file);
        return false;
    }

    if (fread(fileBuf, fileLength, 1, file) != 1) {
        HiLog::Error(LABEL, "Read %{public}s failed.", path.c_str());
        ReleaseSource(fileBuf, file);
        return false;
    }

    if (!(fileBuf[0] == 0xFF && fileBuf[1] == 0xD8)) {
        HiLog::Error(LABEL, "%{public}s is not jpeg file.", path.c_str());
        ReleaseSource(fileBuf, file);
        return false;
    }

    ExifData *ptrExifData = nullptr;
    if ((fileBuf[BUFFER_POSITION_6] == 'E' && fileBuf[BUFFER_POSITION_7] == 'x' &&
        fileBuf[BUFFER_POSITION_8] == 'i' && fileBuf[BUFFER_POSITION_9] == 'f')) {
        ptrExifData = exif_data_new_from_file(path.c_str());
        if (!ptrExifData) {
            HiLog::Error(LABEL, "Create exif data from file failed.");
            ReleaseSource(fileBuf, file);
            return false;
        }
    } else {
        ptrExifData = exif_data_new();
        if (!ptrExifData) {
            HiLog::Error(LABEL, "Create exif data failed.");
            ReleaseSource(fileBuf, file);
            return false;
        }
        /* Set the image options */
        exif_data_set_option(ptrExifData, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
        exif_data_set_data_type(ptrExifData, EXIF_DATA_TYPE_COMPRESSED);
        exif_data_set_byte_order(ptrExifData, EXIF_BYTE_ORDER_INTEL);

        /* Create the mandatory EXIF fields with default data */
        exif_data_fix(ptrExifData);
    }

    ExifIfd ifd = GetImageFileDirectory(tag);
    if (ifd == EXIF_IFD_COUNT) {
        HiLog::Debug(LABEL, "GetIfd ERROR");
        ReleaseSource(fileBuf, file);
        return false;
    }

    ExifEntry *entry = nullptr;
    switch (tag) {
        case EXIF_TAG_BITS_PER_SAMPLE: {
            entry = InitExifTag(ptrExifData, EXIF_IFD_0, EXIF_TAG_BITS_PER_SAMPLE);
            exif_set_short(entry->data, EXIF_BYTE_ORDER_INTEL, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_ORIENTATION: {
            entry = InitExifTag(ptrExifData, EXIF_IFD_0, EXIF_TAG_ORIENTATION);
            exif_set_short(entry->data, EXIF_BYTE_ORDER_INTEL, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_IMAGE_LENGTH: {
            entry = InitExifTag(ptrExifData, EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH);
            exif_set_short(entry->data, EXIF_BYTE_ORDER_INTEL, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_IMAGE_WIDTH: {
            entry = InitExifTag(ptrExifData, EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH);
            exif_set_short(entry->data, EXIF_BYTE_ORDER_INTEL, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_DATE_TIME_ORIGINAL: {
            entry = CreateExifTag(ptrExifData, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL,
                value.length() + 1, EXIF_FORMAT_ASCII);
            memcpy(entry->data, value.c_str(), value.length() + 1);
            break;
        }
        case EXIF_TAG_GPS_LATITUDE: {
            entry = CreateExifTag(ptrExifData, EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE,
                value.length() + 1, EXIF_FORMAT_ASCII);
            memcpy(entry->data, value.c_str(), value.length() + 1);
            break;
        }
        case EXIF_TAG_GPS_LONGITUDE: {
            entry = CreateExifTag(ptrExifData, EXIF_IFD_GPS, EXIF_TAG_GPS_LONGITUDE,
                value.length() + 1, EXIF_FORMAT_ASCII);
            memcpy(entry->data, value.c_str(), value.length() + 1);
            break;
        }
        case EXIF_TAG_GPS_LATITUDE_REF: {
            entry = CreateExifTag(ptrExifData, EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE_REF,
                value.length() + 1, EXIF_FORMAT_ASCII);
            memcpy(entry->data, value.c_str(), value.length() + 1);
            break;
        }
        case EXIF_TAG_GPS_LONGITUDE_REF: {
            entry = CreateExifTag(ptrExifData, EXIF_IFD_GPS, EXIF_TAG_GPS_LONGITUDE_REF,
                value.length() + 1, EXIF_FORMAT_ASCII);
            memcpy(entry->data, value.c_str(), value.length() + 1);
            break;
        }
        default:
            break;
    }

    unsigned char* exifDataBuf = nullptr;
    unsigned int exifDataBufLength = 0;
    exif_data_save_data(exifData_, &exifDataBuf, &exifDataBufLength);
    if (exifDataBuf == nullptr) {
        HiLog::Error(LABEL, "Get Exif Data Buf failed!");
        return false;
    }

    /* Write EXIF header */
    if (fwrite(exifHeader, sizeof(exifHeader), 1, file) != 1) {
        HiLog::Error(LABEL, "Error writing EXIF header to file!");
        ReleaseSource(fileBuf, file);
        return false;
    }

    /* Write EXIF block length in big-endian order */
    if (fputc((exifDataBufLength + LENGTH_OFFSET_2) >> MOVE_OFFSET_8, file) < 0) {
        HiLog::Error(LABEL, "Error writing EXIF block length to file!");
        ReleaseSource(fileBuf, file);
        return false;
    }
    if (fputc((exifDataBufLength + LENGTH_OFFSET_2) & 0xff, file) < 0) {
        HiLog::Error(LABEL, "Error writing EXIF block length to file!");
        ReleaseSource(fileBuf, file);
        return false;
    }

    /* Write EXIF data block */
    if (fwrite(exifDataBuf, exifDataBufLength, 1, file) != 1) {
        HiLog::Error(LABEL, "Error writing EXIF data block to file!");
        ReleaseSource(fileBuf, file);
        return false;
    }
    /* Write JPEG image data, skipping the non-EXIF header */
    if (fwrite(fileBuf + IMAGE_DATA_OFFSET, fileLength - IMAGE_DATA_OFFSET, 1, file) != 1) {
        HiLog::Error(LABEL, "Error writing JPEG image data to file!");
        ReleaseSource(fileBuf, file);
        return false;
    }

    ReleaseSource(fileBuf, file);
    return true;
}

ExifIfd EXIFInfo::GetImageFileDirectory(const ExifTag &tag)
{
    switch (tag) {
        case EXIF_TAG_BITS_PER_SAMPLE:
        case EXIF_TAG_ORIENTATION:
        case EXIF_TAG_IMAGE_LENGTH:
        case EXIF_TAG_IMAGE_WIDTH: {
            return EXIF_IFD_0;
        }
        case EXIF_TAG_DATE_TIME_ORIGINAL: {
            return EXIF_IFD_EXIF;
        }
        case EXIF_TAG_GPS_LATITUDE:
        case EXIF_TAG_GPS_LONGITUDE:
        case EXIF_TAG_GPS_LATITUDE_REF:
        case EXIF_TAG_GPS_LONGITUDE_REF: {
            return EXIF_IFD_GPS;
        }
        default:
            break;
    }
    return EXIF_IFD_COUNT;
}

ExifEntry* EXIFInfo::InitExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag)
{
    ExifEntry *entry;
    /* Return an existing tag if one exists */
    if (!((entry = exif_content_get_entry(exif->ifd[ifd], tag)))) {
        /* Allocate a new entry */
        entry = exif_entry_new();
        if (entry == nullptr) {
            HiLog::Error(LABEL, "Create new entry failed!");
            return nullptr;
        }
        entry->tag = tag; // tag must be set before calling exif_content_add_entry
        /* Attach the ExifEntry to an IFD */
        exif_content_add_entry (exif->ifd[ifd], entry);

        /* Allocate memory for the entry and fill with default data */
        exif_entry_initialize (entry, tag);

        /* Ownership of the ExifEntry has now been passed to the IFD.
         * One must be very careful in accessing a structure after
         * unref'ing it; in this case, we know "entry" won't be freed
         * because the reference count was bumped when it was added to
         * the IFD.
         */
        exif_entry_unref(entry);
    }
    return entry;
}

ExifEntry* EXIFInfo::CreateExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag,
    size_t len, ExifFormat format)
{
    void *buf;
    ExifEntry *entry;
    
    /* Create a memory allocator to manage this ExifEntry */
    ExifMem *mem = exif_mem_new_default();
    if (mem == nullptr) {
        HiLog::Error(LABEL, "Create mem failed!");
        return nullptr;
    }

    /* Create a new ExifEntry using our allocator */
    entry = exif_entry_new_mem (mem);
    if (entry == nullptr) {
        HiLog::Error(LABEL, "Create entry by mem failed!");
        return nullptr;
    }

    /* Allocate memory to use for holding the tag data */
    buf = exif_mem_alloc(mem, len);
    if (buf == nullptr) {
        HiLog::Error(LABEL, "Allocate memory failed!");
        return nullptr;
    }

    /* Fill in the entry */
    entry->data = static_cast<unsigned char*>(buf);
    entry->size = len;
    entry->tag = tag;
    entry->components = len;
    entry->format = format;

    /* Attach the ExifEntry to an IFD */
    exif_content_add_entry (exif->ifd[ifd], entry);

    /* The ExifMem and ExifEntry are now owned elsewhere */
    exif_mem_unref(mem);
    exif_entry_unref(entry);

    return entry;
}

long EXIFInfo::GetFileSize(FILE *fp)
{
    long int position;
    long size;

    /* Save the current position. */
    position = ftell(fp);
    
    /* Jump to the end of the file. */
    fseek(fp, 0L, SEEK_END);
    
    /* Get the end position. */
    size = ftell(fp);
    
    /* Jump back to the original position. */
    fseek(fp, position, SEEK_SET);

    return size;
}

void EXIFInfo::ReleaseSource(unsigned char *buf, FILE *file)
{
    if (buf) {
        free(buf);
    }
    buf = nullptr;
    fclose(file);
    file = nullptr;
}
} // namespace ImagePlugin
} // namespace OHOS
