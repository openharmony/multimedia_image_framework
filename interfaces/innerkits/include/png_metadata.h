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

#ifndef INTERFACES_INNERKITS_INCLUDE_PNG_METADATA_H
#define INTERFACES_INNERKITS_INCLUDE_PNG_METADATA_H

#include <string>
#include <memory>
#include "kv_metadata.h"

namespace OHOS {
namespace Media {

static const std::string PNG_METADATA_KEY_X_PIXELS_PER_METER = "PngXPixelsPerMeter";
static const std::string PNG_METADATA_KEY_Y_PIXELS_PER_METER = "PngYPixelsPerMeter";
static const std::string PNG_METADATA_KEY_GAMMA = "PngGamma";
static const std::string PNG_METADATA_KEY_INTERLACE_TYPE = "PngInterlaceType";
static const std::string PNG_METADATA_KEY_SRGB_INTENT = "PngSRGBIntent";
static const std::string PNG_METADATA_KEY_CHROMATICITIES = "PngChromaticities";
static const std::string PNG_METADATA_KEY_TITLE = "PngTitle";
static const std::string PNG_METADATA_KEY_DESCRIPTION = "PngDescription";
static const std::string PNG_METADATA_KEY_COMMENT = "PngComment";
static const std::string PNG_METADATA_KEY_DISCLAIMER = "PngDisclaimer";
static const std::string PNG_METADATA_KEY_WARNING = "PngWarning";
static const std::string PNG_METADATA_KEY_AUTHOR = "PngAuthor";
static const std::string PNG_METADATA_KEY_COPYRIGHT = "PngCopyright";
static const std::string PNG_METADATA_KEY_CREATION_TIME = "PngCreationTime";
static const std::string PNG_METADATA_KEY_MODIFICATION_TIME = "PngModificationTime";
static const std::string PNG_METADATA_KEY_SOFTWARE = "PngSoftware";

class PngMetadata : public ImageKvMetadata {
public:
    PngMetadata()
    {
        metadataType_ = MetadataType::PNG;
    }
    ~PngMetadata() = default;

    PngMetadata(const PngMetadata&) = delete;
    PngMetadata& operator=(const PngMetadata&) = delete;
};

} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PNG_METADATA_H