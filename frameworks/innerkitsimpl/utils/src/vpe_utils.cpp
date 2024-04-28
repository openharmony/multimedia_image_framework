/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "vpe_utils.h"

#include <dlfcn.h>

#include "hilog/log.h"
#include "log_tags.h"
#include "image_log.h"
#include "v1_0/buffer_handle_meta_key_type.h"
#include "metadata_convertor.h"
#include "external_window.h"
#include "native_window.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "VpeUtils"

namespace OHOS {
namespace Media {
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;
static constexpr uint32_t TRANSFUNC_OFFSET = 8;
static constexpr uint32_t MATRIX_OFFSET = 16;
static constexpr uint32_t RANGE_OFFSET = 21;
constexpr uint8_t INDEX_ZERO = 0;
constexpr uint8_t INDEX_ONE = 1;
constexpr uint8_t INDEX_TWO = 2;
const static char* VPE_SO_NAME = "libvideoprocessingengine.z.so";
using CreateT = int32_t (*)(int32_t*);
using ComposeImageT =
    int32_t (*)(int32_t, OHNativeWindowBuffer*, OHNativeWindowBuffer*, OHNativeWindowBuffer*, bool);
using DecomposeImageT =
    int32_t (*)(int32_t, OHNativeWindowBuffer*, OHNativeWindowBuffer*, OHNativeWindowBuffer*);
using DestoryT = int32_t (*)(int32_t*);

VpeUtils::VpeUtils()
{
}

VpeUtils::~VpeUtils()
{
}

int32_t VpeUtils::ColorSpaceConverterCreate(void* handle, int32_t* instanceId)
{
    if (handle == nullptr) {
        return VPE_ERROR_FAILED;
    }
    CreateT create = (CreateT)dlsym(handle, "ColorSpaceConverterCreate");
    if (!create) {
        return VPE_ERROR_FAILED;
    }
    return create(instanceId);
}

int32_t VpeUtils::ColorSpaceConverterDestory(void* handle, int32_t* instanceId)
{
    if (*instanceId == VPE_ERROR_FAILED || handle == nullptr) {
        return VPE_ERROR_FAILED;
    }
    DestoryT destory = (DestoryT)dlsym(handle, "ColorSpaceConverterDestroy");
    if (!destory) {
        return VPE_ERROR_FAILED;
    }
    return destory(instanceId);
}

int32_t VpeUtils::ColorSpaceConverterComposeImage(VpeSurfaceBuffers& sb, bool legacy)
{
    std::lock_guard<std::mutex> lock(vpeMtx_);
    void* vpeHandle = dlopen(VPE_SO_NAME, RTLD_LAZY);
    if (vpeHandle == nullptr) {
        return VPE_ERROR_FAILED;
    }
    
    int32_t res;
    int32_t instanceId;
    res = ColorSpaceConverterCreate(vpeHandle, &instanceId);
    if (instanceId == VPE_ERROR_FAILED || res != VPE_ERROR_OK) {
        return VPE_ERROR_FAILED;
    }

    ComposeImageT composeImage = (ComposeImageT)dlsym(vpeHandle, "ColorSpaceConverterComposeImage");
    if (!composeImage) {
        return VPE_ERROR_FAILED;
    }
    if (sb.sdr == nullptr || sb.gainmap == nullptr || sb.hdr == nullptr) {
        return VPE_ERROR_FAILED;
    }
    OHNativeWindowBuffer* sdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sb.sdr);
    OHNativeWindowBuffer* gainmap = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sb.gainmap);
    OHNativeWindowBuffer* hdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sb.hdr);
    res = composeImage(instanceId, sdr, gainmap, hdr, legacy);
    OH_NativeWindow_DestroyNativeWindowBuffer(sdr);
    OH_NativeWindow_DestroyNativeWindowBuffer(gainmap);
    OH_NativeWindow_DestroyNativeWindowBuffer(hdr);
    ColorSpaceConverterDestory(vpeHandle, &instanceId);
    dlclose(vpeHandle);
    return res;
}

int32_t VpeUtils::ColorSpaceConverterDecomposeImage(VpeSurfaceBuffers& sb)
{
    std::lock_guard<std::mutex> lock(vpeMtx_);
    void* vpeHandle = dlopen(VPE_SO_NAME, RTLD_LAZY);
    if (vpeHandle == nullptr) {
        return VPE_ERROR_FAILED;
    }
 
    int32_t res;
    int32_t instanceId;
    res = ColorSpaceConverterCreate(vpeHandle, &instanceId);
    if (instanceId == VPE_ERROR_FAILED || res != VPE_ERROR_OK) {
        return VPE_ERROR_FAILED;
    }
    
    DecomposeImageT decomposeImage = (DecomposeImageT)dlsym(vpeHandle, "ColorSpaceConverterDecomposeImage");
    if (!decomposeImage) {
        return VPE_ERROR_FAILED;
    }
    if (sb.sdr == nullptr || sb.gainmap == nullptr || sb.hdr == nullptr) {
        return VPE_ERROR_FAILED;
    }
    OHNativeWindowBuffer* sdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sb.sdr);
    OHNativeWindowBuffer* gainmap = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sb.gainmap);
    OHNativeWindowBuffer* hdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sb.hdr);
    res = decomposeImage(instanceId, hdr, sdr, gainmap);
    OH_NativeWindow_DestroyNativeWindowBuffer(sdr);
    OH_NativeWindow_DestroyNativeWindowBuffer(gainmap);
    OH_NativeWindow_DestroyNativeWindowBuffer(hdr);
    ColorSpaceConverterDestory(vpeHandle, &instanceId);
    dlclose(vpeHandle);
    return res;
}

// surfacebuffer metadata
static GSError SetColorSpaceInfo(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceInfo& colorSpaceInfo)
{
    std::vector<uint8_t> colorSpaceInfoVec;
    auto ret = MetadataManager::ConvertMetadataToVec(colorSpaceInfo, colorSpaceInfoVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceInfoVec);
}

static bool GetColorSpaceInfo(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceInfo& colorSpaceInfo)
{
    std::vector<uint8_t> colorSpaceInfoVec;
    auto ret = buffer->GetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceInfoVec);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("GetColorSpaceInfo GetMetadata failed, return value is %{public}d", ret);
        return false;
    }
    return MetadataManager::ConvertVecToMetadata(colorSpaceInfoVec, colorSpaceInfo) == GSERROR_OK;
}

bool VpeUtils::SetSbColorSpaceType(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceType& colorSpaceType)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    uint32_t colorSpace = static_cast<uint32_t>(colorSpaceType);
    colorSpaceInfo.primaries = static_cast<CM_ColorPrimaries>(colorSpace & CM_PRIMARIES_MASK);
    colorSpaceInfo.transfunc = static_cast<CM_TransFunc>((colorSpace & CM_TRANSFUNC_MASK) >> TRANSFUNC_OFFSET);
    colorSpaceInfo.matrix = static_cast<CM_Matrix>((colorSpace & CM_MATRIX_MASK) >> MATRIX_OFFSET);
    colorSpaceInfo.range = static_cast<CM_Range>((colorSpace & CM_RANGE_MASK) >> RANGE_OFFSET);
    auto ret = SetColorSpaceInfo(buffer, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("SetColorSpaceInfo GetMetadata failed, return value is %{public}d", ret);
        return false;
    }
    return true;
}

bool VpeUtils::GetSbColorSpaceType(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceType& colorSpaceType)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    if (!GetColorSpaceInfo(buffer, colorSpaceInfo)) {
        return false;
    }
    uint32_t primaries = static_cast<uint32_t>(colorSpaceInfo.primaries);
    uint32_t transfunc = static_cast<uint32_t>(colorSpaceInfo.transfunc);
    uint32_t matrix = static_cast<uint32_t>(colorSpaceInfo.matrix);
    uint32_t range = static_cast<uint32_t>(colorSpaceInfo.range);
    colorSpaceType = static_cast<CM_ColorSpaceType>(primaries | (transfunc << TRANSFUNC_OFFSET) |
        (matrix << MATRIX_OFFSET) | (range << RANGE_OFFSET));
    return true;
}

bool VpeUtils::SetSbMetadataType(sptr<SurfaceBuffer>& buffer, const CM_HDR_Metadata_Type& metadataType)
{
    std::vector<uint8_t> hdrMetadataTypeVec;
    auto ret = MetadataManager::ConvertMetadataToVec(metadataType, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        return false;
    }
    ret = buffer->SetMetadata(ATTRKEY_HDR_METADATA_TYPE, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        return false;
    }
    return true;
}

bool VpeUtils::GetSbMetadataType(const sptr<SurfaceBuffer>& buffer, CM_HDR_Metadata_Type& metadataType)
{
    std::vector<uint8_t> hdrMetadataTypeVec;
    auto ret = buffer->GetMetadata(ATTRKEY_HDR_METADATA_TYPE, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("HdrUtils::GetHDRMetadataType GetMetadata failed, return value is %{public}d", ret);
        return false;
    }
    return MetadataManager::ConvertVecToMetadata(hdrMetadataTypeVec, metadataType) == GSERROR_OK;
}

bool VpeUtils::SetSbDynamicMetadata(sptr<SurfaceBuffer>& buffer, const std::vector<uint8_t>& dynamicMetadata)
{
    return buffer->SetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, dynamicMetadata) == GSERROR_OK;
}

bool VpeUtils::GetSbDynamicMetadata(const sptr<SurfaceBuffer>& buffer, std::vector<uint8_t>& dynamicMetadata)
{
    return buffer->GetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, dynamicMetadata) == GSERROR_OK;
}

bool VpeUtils::SetSbStaticMetadata(sptr<SurfaceBuffer>& buffer, const std::vector<uint8_t>& staticMetadata)
{
    return buffer->SetMetadata(ATTRKEY_HDR_STATIC_METADATA, staticMetadata) == GSERROR_OK;
}

bool VpeUtils::GetSbStaticMetadata(const sptr<SurfaceBuffer>& buffer, std::vector<uint8_t>& staticMetadata)
{
    return buffer->GetMetadata(ATTRKEY_HDR_STATIC_METADATA, staticMetadata) == GSERROR_OK;
}

static HDRVividGainmapMetadata GetDefaultGainmapMetadata()
{
    const float gainmapMax = 1.0f;
    const float gainmapMin = 0.0f;
    const float gamma = 1.0f;
    const float offsetDenominator = 64.0;
    const float baseOffset = 1.0 / offsetDenominator;
    const float alternateOffset = 1.0 / offsetDenominator;
    HDRVividGainmapMetadata gainmapMetadata;
    gainmapMetadata.enhanceClippedThreholdMaxGainmap[INDEX_ZERO] = gainmapMax;
    gainmapMetadata.enhanceClippedThreholdMaxGainmap[INDEX_ONE] = gainmapMax;
    gainmapMetadata.enhanceClippedThreholdMaxGainmap[INDEX_TWO] = gainmapMax;
    gainmapMetadata.enhanceClippedThreholdMinGainmap[INDEX_ZERO] = gainmapMin;
    gainmapMetadata.enhanceClippedThreholdMinGainmap[INDEX_ONE] = gainmapMin;
    gainmapMetadata.enhanceClippedThreholdMinGainmap[INDEX_TWO] = gainmapMin;
    gainmapMetadata.enhanceMappingGamma[INDEX_ZERO] = gamma;
    gainmapMetadata.enhanceMappingGamma[INDEX_ONE] = gamma;
    gainmapMetadata.enhanceMappingGamma[INDEX_TWO] = gamma;
    gainmapMetadata.enhanceMappingBaselineOffset[INDEX_ZERO] = baseOffset;
    gainmapMetadata.enhanceMappingBaselineOffset[INDEX_ONE] = baseOffset;
    gainmapMetadata.enhanceMappingBaselineOffset[INDEX_TWO] = baseOffset;
    gainmapMetadata.enhanceMappingAlternateOffset[INDEX_ZERO] = alternateOffset;
    gainmapMetadata.enhanceMappingAlternateOffset[INDEX_ONE] = alternateOffset;
    gainmapMetadata.enhanceMappingAlternateOffset[INDEX_TWO] = alternateOffset;
    return gainmapMetadata;
}

static CM_HDR_Metadata_Type ConvertHdrType(ImageHdrType hdrType)
{
    switch (hdrType) {
        case ImageHdrType::HDR_VIVID_DUAL :
        case ImageHdrType::HDR_CUVA :
            return CM_IMAGE_HDR_VIVID_DUAL;
        case ImageHdrType::HDR_ISO_DUAL :
            return CM_IMAGE_HDR_ISO_DUAL;
        default:
            return CM_METADATA_NONE;
    }
    return CM_METADATA_NONE;
}

void VpeUtils::SetSurfaceBufferInfo(sptr<SurfaceBuffer>& buffer, bool isGainmap, ImageHdrType type,
    CM_ColorSpaceType color, HdrMetadata& metadata)
{
    CM_HDR_Metadata_Type cmHdrType = ConvertHdrType(type);
    VpeUtils::SetSbMetadataType(buffer, cmHdrType);
    VpeUtils::SetSbColorSpaceType(buffer, color);
    if (type == ImageHdrType::HDR_CUVA) {
        return;
    }
    if (!isGainmap) {
        VpeUtils::SetSbDynamicMetadata(buffer, metadata.dynamicMetadata);
        VpeUtils::SetSbStaticMetadata(buffer, metadata.staticMetadata);
        return;
    }
    std::vector<uint8_t> gainmapMetadataVec(sizeof(HDRVividGainmapMetadata));
    if (metadata.gainmapMetadataFlag) {
        memcpy_s(gainmapMetadataVec.data(), gainmapMetadataVec.size(),
            &metadata.gainmapMetadata, sizeof(HDRVividGainmapMetadata));
    } else {
        HDRVividGainmapMetadata defaultGainmapMetadata = GetDefaultGainmapMetadata();
        memcpy_s(gainmapMetadataVec.data(), gainmapMetadataVec.size(),
            &defaultGainmapMetadata, sizeof(HDRVividGainmapMetadata));
    }
    VpeUtils::SetSbDynamicMetadata(buffer, gainmapMetadataVec);
}
}
}