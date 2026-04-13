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

#ifndef INTERFACES_INNERKITS_INCLUDE_PNG_METADATA_PARSER_H
#define INTERFACES_INNERKITS_INCLUDE_PNG_METADATA_PARSER_H

#include <memory>
#include "png.h"
#include "png_metadata.h"
#include "input_data_stream.h"

namespace OHOS {
namespace Media {

class PngMetadataParser {
public:
    PngMetadataParser();
    ~PngMetadataParser();
    PngMetadataParser(const PngMetadataParser&) = delete;
    PngMetadataParser& operator=(const PngMetadataParser&) = delete;

    bool SetupPngReading(ImagePlugin::InputDataStream *stream);
    bool GetPropertyInt(const std::string &key, int32_t &value);
    bool GetPropertyDouble(const std::string &key, double &value);
    bool GetPropertyString(const std::string &key, std::string &value);

private:
    bool ReadPngInfo(ImagePlugin::InputDataStream *stream);
    bool GetPhysProperty(const std::string &key, int32_t &value);
    bool GetGammaPropertyDouble(double &value);
    bool GetInterlaceProperty(int32_t &value);
    bool GetSrgbProperty(int32_t &value);
    bool GetChromaticitiesProperty(std::string &value);
    std::string GetTextKeyword(const std::string &key);
    bool FindTextProperty(const std::string &targetKeyword, std::string &value);
    bool GetTextProperty(const std::string &key, std::string &value);

    png_structp pngStructPtr_ = nullptr;
    png_infop pngInfoPtr_ = nullptr;
};

} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PNG_METADATA_PARSER_H
