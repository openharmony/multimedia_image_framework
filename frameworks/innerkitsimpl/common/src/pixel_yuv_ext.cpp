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

#include "pixel_yuv_ext.h"
#include "pixel_yuv_ext_utils.h"

#include "image_utils.h"
#include "image_trace.h"
#include "image_type_converter.h"
#include "memory_manager.h"
#include "hilog/log.h"
#include "hitrace_meter.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pubdef.h"
#include "pixel_yuv_utils.h"
#include "securec.h"
#include "image_log.h"
#include "image_mdk_common.h"
#include "image_system_properties.h"
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PixelYuvExt"

namespace OHOS {
namespace Media {
using namespace std;

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_4 = 4;
static const uint8_t RGBA_BIT_DEPTH = 4;
static const int32_t DEGREES360 = 360;
static const float ROUND_FLOAT_NUMBER = 0.5f;

static SkImageInfo ToSkImageInfo(ImageInfo &info, sk_sp<SkColorSpace> colorSpace)
{
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    IMAGE_LOGD("ToSkImageInfo w %{public}d,h %{public}d", info.size.width, info.size.height);
    IMAGE_LOGD("ToSkImageInfo pf %{public}s, at %{public}s, skpf %{public}s, "
               "skat %{public}s",
               ImageTypeConverter::ToName(info.pixelFormat).c_str(),
               ImageTypeConverter::ToName(info.alphaType).c_str(),
               ImageTypeConverter::ToName(colorType).c_str(),
               ImageTypeConverter::ToName(alphaType).c_str());
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static sk_sp<SkColorSpace> ToSkColorSpace(PixelMap *pixelmap)
{
#ifdef IMAGE_COLORSPACE_FLAG
    if (pixelmap->InnerGetGrColorSpacePtr() == nullptr) {
        return nullptr;
    }
    return pixelmap->InnerGetGrColorSpacePtr()->ToSkColorSpace();
#else
    return nullptr;
#endif
}

static bool isSameColorSpace(const OHOS::ColorManager::ColorSpace &src,
    const OHOS::ColorManager::ColorSpace &dst)
{
    auto skSrc = src.ToSkColorSpace();
    auto skDst = dst.ToSkColorSpace();
    return SkColorSpace::Equals(skSrc.get(), skDst.get());
}

bool  PixelYuvExt::resize(float xAxis, float yAxis)
{
    scale(xAxis, yAxis);
    return true;
}

bool PixelYuvExt::resize(int32_t dstW, int32_t dstH)
{
    scale(dstW, dstH);
    return true;
}

constexpr int32_t ANTIALIASING_SIZE = 350;

static bool IsSupportAntiAliasing(const ImageInfo &imageInfo, const AntiAliasingOption &option)
{
    return option != AntiAliasingOption::NONE &&
           imageInfo.size.width <= ANTIALIASING_SIZE &&
           imageInfo.size.height <= ANTIALIASING_SIZE;
}

void PixelYuvExt::scale(int32_t dstW, int32_t dstH)
{
    if (!IsYuvFormat()) {
        return;
    }
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    AntiAliasingOption operation = AntiAliasingOption::NONE;
    AntiAliasingOption option = AntiAliasingOption::NONE;
    if (ImageSystemProperties::GetAntiAliasingEnabled() && IsSupportAntiAliasing(imageInfo, option)) {
        operation = AntiAliasingOption::MEDIUM;
    } else {
        operation = option;
    }
    scale(dstW, dstH, operation);
}

PixelYuvExt::~PixelYuvExt()
{
}
void PixelYuvExt::scale(float xAxis, float yAxis)
{
    if (!IsYuvFormat()) {
        return;
    }
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    AntiAliasingOption operation = AntiAliasingOption::NONE;
    AntiAliasingOption option = AntiAliasingOption::NONE;
    if (ImageSystemProperties::GetAntiAliasingEnabled() && IsSupportAntiAliasing(imageInfo, option)) {
        operation = AntiAliasingOption::MEDIUM;
    } else {
        operation = option;
    }
    scale(xAxis, yAxis, operation);
}

void PixelYuvExt::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    ImageTrace imageTrace("PixelMap scale");
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t dstW = (imageInfo.size.width  * xAxis + ROUND_FLOAT_NUMBER);
    int32_t dstH = (imageInfo.size.height * yAxis + ROUND_FLOAT_NUMBER);
    YUVStrideInfo dstStrides;
    auto m = CreateMemory(imageInfo.pixelFormat, "Trans ImageData", dstW, dstH, dstStrides);
    CHECK_ERROR_RETURN_LOG(m == nullptr, "scale CreateMemory failed");
    int64_t dstBufferSizeOverflow = static_cast<int64_t>(dstW) * static_cast<int64_t>(dstH);
    bool cond = ImageUtils::GetPixelBytes(imageInfo.pixelFormat) == 0;
    CHECK_ERROR_RETURN_LOG(cond, "invalid pixelFormat:[%{public}d]", imageInfo.pixelFormat);
    int64_t maxDstBufferSize = static_cast<int64_t>(INT32_MAX) / ImageUtils::GetPixelBytes(imageInfo.pixelFormat);
    cond = abs(dstBufferSizeOverflow) > maxDstBufferSize;
    CHECK_ERROR_RETURN_LOG(cond, "scale target size too large, srcWidth(%{public}d) * xAxis(%{public}.2f), "
            "srcHeight(%{public}d) * yAxis(%{public}.2f)", imageInfo.size.width, xAxis, imageInfo.size.height, yAxis);

    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);
    uint32_t pixelsSize = pixelsSize_;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        SurfaceBuffer* sbBuffer = static_cast<SurfaceBuffer*>(GetFd());
        if (sbBuffer == nullptr) {
            IMAGE_LOGE("PixelYuvExt scale get SurfaceBuffer failed");
        } else {
            pixelsSize = sbBuffer->GetSize();
        }
    }
#endif
    YuvImageInfo yuvInfo = {PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat),
                            imageInfo.size.width, imageInfo.size.height,
                            imageInfo.pixelFormat, yuvDataInfo, pixelsSize};

    PixelYuvExtUtils::ScaleYuv420(xAxis, yAxis, option, yuvInfo, data_, dst, dstStrides);
    SetPixelsAddr(reinterpret_cast<void *>(dst), m->extend.data, m->data.size, m->GetType(), nullptr);
    imageInfo.size.width = dstW;
    imageInfo.size.height = dstH;
    SetImageInfo(imageInfo, true);
    UpdateYUVDataInfo(imageInfo.pixelFormat, imageInfo.size.width, imageInfo.size.height, dstStrides);
    AddVersionId();
}

void PixelYuvExt::scale(int32_t dstW, int32_t dstH, const AntiAliasingOption &option)
{
    ImageTrace imageTrace("PixelMap scale");
    IMAGE_LOGI("%{public}s (%{public}d, %{public}d)", __func__, dstW, dstH);
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);

    YUVStrideInfo dstStrides;
    auto m = CreateMemory(imageInfo.pixelFormat, "Trans ImageData", dstW, dstH, dstStrides);
    CHECK_ERROR_RETURN_LOG(m == nullptr, "scale CreateMemory failed");
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);
    uint32_t pixelsSize = pixelsSize_;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        SurfaceBuffer* sbBuffer = static_cast<SurfaceBuffer*>(GetFd());
        if (sbBuffer == nullptr) {
            IMAGE_LOGE("PixelYuvExt scale get SurfaceBuffer failed");
        } else {
            pixelsSize = sbBuffer->GetSize();
        }
    }
#endif
    YuvImageInfo yuvInfo = {PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat),
                            imageInfo.size.width, imageInfo.size.height,
                            imageInfo.pixelFormat, yuvDataInfo, pixelsSize};
    PixelYuvExtUtils::ScaleYuv420(dstW, dstH, option, yuvInfo, data_, dst, dstStrides);
    SetPixelsAddr(reinterpret_cast<void *>(dst), m->extend.data, m->data.size, m->GetType(), nullptr);
    imageInfo.size.width = dstW;
    imageInfo.size.height = dstH;
    SetImageInfo(imageInfo, true);
    UpdateYUVDataInfo(imageInfo.pixelFormat, imageInfo.size.width, imageInfo.size.height, dstStrides);
    AddVersionId();
}

void PixelYuvExt::rotate(float degrees)
{
    bool cond = !IsYuvFormat() || degrees == 0;
    CHECK_ERROR_RETURN(cond);
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);

    if (degrees < 0) {
        int n = abs(degrees / DEGREES360);
        degrees += DEGREES360 * (n + 1);
    }
    OpenSourceLibyuv::RotationMode rotateNum = OpenSourceLibyuv::RotationMode::kRotate0;
    int32_t dstWidth = imageInfo_.size.width;
    int32_t dstHeight = imageInfo_.size.height;
    if (!YuvRotateConvert(imageInfo_.size, degrees, dstWidth, dstHeight, rotateNum)) {
        IMAGE_LOGI("Rotate degress is invalid, don't need rotate");
        return ;
    }

    IMAGE_LOGD("PixelYuvExt::rotate dstWidth=%{public}d dstHeight=%{public}d", dstWidth, dstHeight);
    YUVStrideInfo dstStrides;
    auto m = CreateMemory(imageInfo_.pixelFormat, "rotate ImageData", dstWidth, dstHeight, dstStrides);
    CHECK_ERROR_RETURN_LOG(m == nullptr, "rotate CreateMemory failed");
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);

    yuvDataInfo.imageSize = imageInfo_.size;
    Size dstSize = {dstWidth, dstHeight};
    if (!PixelYuvExtUtils::YuvRotate(data_, imageInfo_.pixelFormat, yuvDataInfo, dstSize, dst, dstStrides,
                                     rotateNum)) {
        m->Release();
        return;
    }
    imageInfo_.size = dstSize;
    SetImageInfo(imageInfo_, true);
    SetPixelsAddr(dst, m->extend.data, m->data.size, m->GetType(), nullptr);
    UpdateYUVDataInfo(imageInfo_.pixelFormat, dstWidth, dstHeight, dstStrides)
    AddVersionId();
    return;
}

void PixelYuvExt::flip(bool xAxis, bool yAxis)
{
    bool cond = !IsYuvFormat() || (xAxis == false && yAxis == false);
    CHECK_ERROR_RETURN(cond);
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);

    if (imageInfo.pixelFormat == PixelFormat::YCBCR_P010 ||
        imageInfo.pixelFormat == PixelFormat::YCRCB_P010) {
        IMAGE_LOGD("P010 use PixelYuv flip");
        PixelYuv::flip(xAxis, yAxis);
        return;
    }
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);

    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;

    uint8_t *dst = nullptr;
    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    YUVStrideInfo dstStrides;
    auto m = CreateMemory(imageInfo.pixelFormat, "flip ImageData", width, height, dstStrides);
    CHECK_ERROR_RETURN_LOG(m == nullptr, "flip CreateMemory failed");
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    bool bRet = false;
    if (xAxis && yAxis) {
        bRet = PixelYuvExtUtils::Mirror(data_, dst, imageInfo.size, imageInfo.pixelFormat,
                                        yuvDataInfo, dstStrides, true);
    } else if (yAxis) {
        bRet = PixelYuvExtUtils::Mirror(data_, dst, imageInfo.size, imageInfo.pixelFormat,
                                        yuvDataInfo, dstStrides, false);
    } else if (xAxis) {
        bRet = PixelYuvExtUtils::FlipXaxis(data_, dst, imageInfo.size, imageInfo.pixelFormat, yuvDataInfo,
                                           dstStrides);
    }
    if (!bRet) {
        IMAGE_LOGE("flip failed xAxis=%{public}d, yAxis=%{public}d", xAxis, yAxis);
        m->Release();
        return;
    }
    SetPixelsAddr(dst, m->extend.data, m->data.size, m->GetType(), nullptr);
    UpdateYUVDataInfo(imageInfo.pixelFormat, imageInfo.size.width, imageInfo.size.height, dstStrides);
    AddVersionId();
}

int32_t PixelYuvExt::GetByteCount()
{
    return PixelYuv::GetByteCount();
}

void PixelYuvExt::SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type, CustomFreePixelMap func)
{
    PixelYuv::SetPixelsAddr(addr, context, size, type, func);
}

#ifdef IMAGE_COLORSPACE_FLAG
bool PixelYuvExt::CheckColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    auto grName = grColorSpace.GetColorSpaceName();
    if (grColorSpace_ != nullptr &&
        isSameColorSpace(*grColorSpace_, grColorSpace)) {
        if (grColorSpace_->GetColorSpaceName() != grName) {
            InnerSetColorSpace(grColorSpace);
            IMAGE_LOGE("applyColorSpace inner set");
        }
        return true;
    }
    return false;
}

int32_t PixelYuvExt::ColorSpaceBGRAToYuv(
    uint8_t *bgraData, SkTransYuvInfo &dst, ImageInfo &imageInfo,
    PixelFormat &format, const OHOS ::ColorManager::ColorSpace &grColorSpace)
{
    int32_t dstWidth = dst.info.width();
    int32_t dstHeight = dst.info.height();
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);
    uint32_t pictureSize = GetImageSize(dstWidth, dstHeight, format);
    std::unique_ptr<uint8_t[]> yuvData = std::make_unique<uint8_t[]>(pictureSize);
    bool cond = !PixelYuvExtUtils::BGRAToYuv420(bgraData, data_, dstWidth, dstHeight, format, yuvDataInfo);
    CHECK_ERROR_RETURN_RET_LOG(cond, ERR_IMAGE_COLOR_CONVERT, "BGRAToYuv420 failed");
    auto grName = grColorSpace.GetColorSpaceName();
    grColorSpace_ = std::make_shared<OHOS ::ColorManager::ColorSpace>(
        dst.info.refColorSpace(), grName);
    AddVersionId();
    return SUCCESS;
}

uint32_t PixelYuvExt::ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    bool cond = !IsYuvFormat();
    CHECK_ERROR_RETURN_RET(cond, ERR_IMAGE_COLOR_CONVERT);
    cond = CheckColorSpace(grColorSpace);
    CHECK_ERROR_RETURN_RET(cond, SUCCESS);
    /*convert yuV420 to·BRGA */
    PixelFormat format = imageInfo_.pixelFormat;
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);

    int32_t width = imageInfo_.size.width;
    int32_t height = imageInfo_.size.height;
    cond = !PixelYuvUtils::CheckWidthAndHeightMult(width, height, RGBA_BIT_DEPTH);
    CHECK_ERROR_RETURN_RET_LOG(cond, ERR_IMAGE_COLOR_CONVERT,
        "ApplyColorSpace size overflow width(%{public}d), height(%{public}d)", width, height);
    uint8_t *srcData = data_;
    std::unique_ptr<uint8_t[]> RGBAdata =
        std::make_unique<uint8_t[]>(width * height * NUM_4);
    if (!PixelYuvExtUtils::Yuv420ToBGRA(srcData, RGBAdata.get(), imageInfo_.size, format, yuvDataInfo)) {
        IMAGE_LOGE("Yuv420ToBGRA failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    IMAGE_LOGI("applyColorSpace Yuv420ToBGRA sucess");
    ImageInfo bgraImageInfo = imageInfo_;
    bgraImageInfo.pixelFormat = PixelFormat::BGRA_8888;

    SkTransYuvInfo src;
    src.info = ToSkImageInfo(bgraImageInfo, ToSkColorSpace(this));
    uint64_t rowStride = src.info.minRowBytes();
    IMAGE_LOGI("applyColorSpace rowStride:%{public}ld sucess", rowStride);

    auto bret = src.bitmap.installPixels(src.info, RGBAdata.get(), rowStride);
    CHECK_ERROR_RETURN_RET_LOG(bret == false, -1, "src.bitmap.installPixels failed");
    // Build sk target infomation
    SkTransYuvInfo dst;

    dst.info = ToSkImageInfo(bgraImageInfo, grColorSpace.ToSkColorSpace());
    std::unique_ptr<uint8_t[]> RGBAdataC =
        std::make_unique<uint8_t[]>(width * height * NUM_4);
    // Transfor pixels*by readPixels
    cond = !src.bitmap.readPixels(dst.info, RGBAdataC.get(), rowStride, 0, 0);
    CHECK_ERROR_RETURN_RET_LOG(cond, ERR_IMAGE_COLOR_CONVERT, "ReadPixels failed");
    // convert bgra back to·yuv
    cond = ColorSpaceBGRAToYuv(RGBAdataC.get(), dst, imageInfo_, format, grColorSpace) != SUCCESS;
    CHECK_ERROR_RETURN_RET_LOG(cond, ERR_IMAGE_COLOR_CONVERT, "ColorSpaceBGRAToYuv failed");
    return SUCCESS;
}
#endif
} // namespace Media
} // namespace OHOS