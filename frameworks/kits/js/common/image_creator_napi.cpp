/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "image_creator_napi.h"
#include <uv.h>
#include "media_errors.h"
#include "image_log.h"
#include "image_napi_utils.h"
#include "image_creator_context.h"
#include "image_napi.h"
#include "image_creator_manager.h"

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::make_shared;
using std::make_unique;

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ImageCreatorNapi"

namespace {
    constexpr int32_t TEST_WIDTH = 8192;
    constexpr int32_t TEST_HEIGHT = 8;
    constexpr int32_t TEST_FORMAT = 4;
    constexpr int32_t TEST_CAPACITY = 8;
}

namespace OHOS {
namespace Media {
static const std::string CLASS_NAME = "ImageCreator";
shared_ptr<ImageCreator> ImageCreatorNapi::staticInstance_ = nullptr;
thread_local napi_ref ImageCreatorNapi::sConstructor_ = nullptr;
static bool g_isCreatorTest = false;
static std::shared_ptr<ImageCreatorReleaseListener> g_listener = nullptr;

const int ARGS0 = 0;
const int ARGS1 = 1;
const int ARGS2 = 2;
const int ARGS3 = 3;
const int ARGS4 = 4;
const int PARAM0 = 0;
const int PARAM1 = 1;
const int PARAM2 = 2;
const int PARAM3 = 3;

ImageCreatorNapi::ImageCreatorNapi():env_(nullptr)
{}

ImageCreatorNapi::~ImageCreatorNapi()
{
    release();
}

static void CommonCallbackRoutine(napi_env env, Contextc &context, const napi_value &valueParam, bool isRelease = true)
{
    IMAGE_FUNCTION_IN();
    napi_value result[2] = {0};
    napi_value retVal;
    napi_value callback = nullptr;

    napi_get_undefined(env, &result[0]);
    napi_get_undefined(env, &result[1]);

    if (context->status == SUCCESS) {
        result[1] = valueParam;
    }

    if (context->deferred) {
        if (context->status == SUCCESS) {
            napi_resolve_deferred(env, context->deferred, result[1]);
        } else {
            napi_reject_deferred(env, context->deferred, result[0]);
        }
    } else {
        napi_create_uint32(env, context->status, &result[0]);
        napi_get_reference_value(env, context->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, PARAM2, result, &retVal);
    }

    if (isRelease) {
        if (context->callbackRef != nullptr) {
            napi_delete_reference(env, context->callbackRef);
            context->callbackRef = nullptr;
        }

        napi_delete_async_work(env, context->work);

        delete context;
        context = nullptr;
    }
    IMAGE_FUNCTION_OUT();
}

void ImageCreatorNapi::NativeRelease()
{
    if (imageCreator_ != nullptr) {
        imageCreator_->~ImageCreator();
        imageCreator_ = nullptr;
    }
}

napi_value ImageCreatorNapi::Init(napi_env env, napi_value exports)
{
    IMAGE_FUNCTION_IN();
    napi_property_descriptor props[] = {
        DECLARE_NAPI_FUNCTION("dequeueImage", JsDequeueImage),
        DECLARE_NAPI_FUNCTION("queueImage", JsQueueImage),
        DECLARE_NAPI_FUNCTION("on", JsOn),
        DECLARE_NAPI_FUNCTION("off", JsOff),
        DECLARE_NAPI_FUNCTION("release", JsRelease),

#ifdef IMAGE_DEBUG_FLAG
        DECLARE_NAPI_GETTER("test", JsTest),
#endif
        DECLARE_NAPI_GETTER("capacity", JsGetCapacity),
        DECLARE_NAPI_GETTER("format", JsGetFormat),
        DECLARE_NAPI_GETTER("size", JsGetSize),
    };
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createImageCreator", JSCreateImageCreator),
    };

    napi_value constructor = nullptr;
    size_t props_count = IMG_ARRAY_SIZE(props);
    size_t static_props_count = IMG_ARRAY_SIZE(static_prop);

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor,
        nullptr, props_count, props, &constructor)),
        nullptr,
        IMAGE_ERR("define class fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_create_reference(env, constructor, 1, &sConstructor_)),
        nullptr,
        IMAGE_ERR("create reference fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor)),
        nullptr,
        IMAGE_ERR("set named property fail")
    );
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_define_properties(env, exports, static_props_count, static_prop)),
        nullptr,
        IMAGE_ERR("define properties fail")
    );

    IMAGE_DEBUG("Init success");

    IMAGE_FUNCTION_OUT();
    return exports;
}

napi_value ImageCreatorNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value undefineVar = nullptr;
    napi_get_undefined(env, &undefineVar);

    napi_status status;
    napi_value thisVar = nullptr;

    IMAGE_FUNCTION_IN();
    status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status == napi_ok && thisVar != nullptr) {
        std::unique_ptr<ImageCreatorNapi> reference = std::make_unique<ImageCreatorNapi>();
        if (reference != nullptr) {
            reference->env_ = env;
            if (!g_isCreatorTest) {
                reference->imageCreator_ = staticInstance_;
            }
            status = napi_wrap(env, thisVar, reinterpret_cast<void *>(reference.get()),
                               ImageCreatorNapi::Destructor, nullptr, nullptr);
            if (status == napi_ok) {
                IMAGE_FUNCTION_OUT();
                reference.release();
                return thisVar;
            } else {
                IMAGE_ERR("Failure wrapping js to native napi");
            }
        }
    }

    return undefineVar;
}

void ImageCreatorNapi::Destructor(napi_env env, void *nativeObject, void *finalize)
{
}

static bool isTest(const int32_t* args, const int32_t len)
{
    if ((args[PARAM0] ==  TEST_WIDTH) &&
        (args[PARAM1] ==  TEST_HEIGHT) &&
        (args[PARAM2] ==  TEST_FORMAT) &&
        (args[PARAM3] ==  TEST_CAPACITY) &&
        (len == ARGS4)) {
        return true;
    }
    return false;
}
napi_value ImageCreatorNapi::JSCreateImageCreator(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = ARGS4;
    napi_value argv[ARGS4] = {0};
    int32_t args[ARGS4] = {0};

    IMAGE_FUNCTION_IN();
    napi_get_undefined(env, &result);
    status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok || ((argc != ARGS3) && (argc != ARGS4))) {
        std::string errMsg = "Invailed arg counts ";
        return ImageNapiUtils::ThrowExceptionError(env, COMMON_ERR_INVALID_PARAMETER,
            errMsg.append(std::to_string(argc)));
    }
    std::string errMsg;
    if (!ImageNapiUtils::ParseImageCreatorReceiverArgs(env, argc, argv, args, errMsg)) {
        return ImageNapiUtils::ThrowExceptionError(env, COMMON_ERR_INVALID_PARAMETER, errMsg);
    }
    int32_t len = sizeof(args) / sizeof(args[PARAM0]);
    if (isTest(args, len)) {
        g_isCreatorTest = true;
    }
    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (IMG_IS_OK(status)) {
        if (!g_isCreatorTest) {
            staticInstance_ = ImageCreator::CreateImageCreator(args[PARAM0],
                args[PARAM1], args[PARAM2], args[PARAM3]);
        }
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            IMAGE_FUNCTION_OUT();
            return result;
        } else {
            IMAGE_ERR("New instance could not be obtained");
        }
    }
    return result;
}

static bool CheckArgs(const ImageCreatorCommonArgs &args)
{
    if (args.async != CreatorCallType::GETTER) {
        if (args.queryArgs == nullptr) {
            IMAGE_ERR("No query args function");
            return false;
        }
    }

    if (args.async != CreatorCallType::ASYNC || args.asyncLater) {
        if (args.nonAsyncBack == nullptr) {
            IMAGE_ERR("No non async back function");
            return false;
        }
    }
    return true;
}

static bool PrepareOneArg(ImageCreatorCommonArgs &args, struct ImageCreatorInnerContext &ic)
{
    if (ic.argc == ARGS1) {
        auto argType = ImageNapiUtils::getType(args.env, ic.argv[PARAM0]);
        if (argType == napi_function) {
            napi_create_reference(args.env, ic.argv[PARAM0], ic.refCount, &(ic.context->callbackRef));
        } else {
            IMAGE_ERR("Unsupport arg 0 type: %{public}d", argType);
            return false;
        }
    }

    if (ic.context->callbackRef == nullptr) {
        napi_create_promise(args.env, &(ic.context->deferred), &(ic.result));
    } else {
        napi_get_undefined(args.env, &ic.result);
    }
    return true;
}

static void JSCommonProcessSendEvent(ImageCreatorCommonArgs &args, napi_status status,
                                     ImageCreatorAsyncContext* context, napi_event_priority prio)
{
    auto task = [args, status, context]() {
        (void)args.callBack(args.env, status, context);
    };
    if (napi_status::napi_ok != napi_send_event(args.env, task, prio)) {
        IMAGE_LOGE("JSCommonProcessSendEvent: failed to SendEvent!");
    }
}

napi_value ImageCreatorNapi::JSCommonProcess(ImageCreatorCommonArgs &args)
{
    IMAGE_FUNCTION_IN();
    struct ImageCreatorInnerContext ic;
    ic.argc = args.argc;
    ic.argv.resize(ic.argc);
    napi_get_undefined(args.env, &ic.result);
    IMG_NAPI_CHECK_RET(CheckArgs(args), ic.result);

    IMG_JS_ARGS(args.env, args.info, ic.status, ic.argc, &(ic.argv[0]), ic.thisVar);

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(ic.status), ic.result, IMAGE_ERR("fail to napi_get_cb_info"));

    if (args.async != CreatorCallType::STATIC) {
        ic.context = std::make_unique<ImageCreatorAsyncContext>();
        if (ic.context == nullptr) {
            return ic.result;
        }
        ic.status = napi_unwrap(args.env, ic.thisVar, reinterpret_cast<void**>(&(ic.context->constructor_)));

        IMG_NAPI_CHECK_RET_D(IMG_IS_READY(ic.status, ic.context->constructor_),
            ic.result, IMAGE_ERR("fail to unwrap context"));
        if (ic.context->constructor_ == nullptr) {
            return ic.result;
        }
        if (!g_isCreatorTest) {
            ic.context->creator_ = ic.context->constructor_->imageCreator_;

            IMG_NAPI_CHECK_RET_D(IMG_IS_READY(ic.status, ic.context->creator_),
                ic.result, IMAGE_ERR("empty native creator"));
        }
    }
    if (args.async != CreatorCallType::GETTER && !args.queryArgs(args, ic)) {
        return ic.result;
    }
    if (args.async == CreatorCallType::ASYNC) {
        if (args.asyncLater) {
            args.nonAsyncBack(args, ic);
        } else {
            JSCommonProcessSendEvent(args, ic.status, ic.context.get(), napi_eprio_high);
            ic.context.release();
        }
    } else {
        args.nonAsyncBack(args, ic);
    }
    IMAGE_FUNCTION_OUT();
    return ic.result;
}

static napi_value BuildJsSize(napi_env env, int32_t width, int32_t height)
{
    napi_value result = nullptr;
    napi_value sizeWith = nullptr;
    napi_value sizeHeight = nullptr;

    napi_create_object(env, &result);

    napi_create_int32(env, width, &sizeWith);
    napi_set_named_property(env, result, "width", sizeWith);

    napi_create_int32(env, height, &sizeHeight);
    napi_set_named_property(env, result, "height", sizeHeight);
    return result;
}

napi_value ImageCreatorNapi::JsGetSize(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::GETTER,
    };
    args.argc = ARGS0;

    args.nonAsyncBack = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        napi_get_undefined(args.env, &(ic.result));
        if (g_isCreatorTest) {
            ic.result = BuildJsSize(args.env, TEST_WIDTH, TEST_HEIGHT);
            return true;
        }

        auto native = ic.context->constructor_->imageCreator_;
        if (native == nullptr) {
            IMAGE_ERR("Native instance is nullptr");
            return false;
        }

        if (native->iraContext_ == nullptr) {
            IMAGE_ERR("Image creator context is nullptr");
            return false;
        }
        ic.result = BuildJsSize(args.env,
                                native->iraContext_->GetWidth(),
                                native->iraContext_->GetHeight());
        return true;
    };

    return JSCommonProcess(args);
}

napi_value ImageCreatorNapi::JsGetCapacity(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::GETTER,
    };
    args.argc = ARGS0;

    args.nonAsyncBack = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        napi_get_undefined(args.env, &(ic.result));
        if (g_isCreatorTest) {
            napi_create_int32(args.env, TEST_CAPACITY, &(ic.result));
            return true;
        }
        auto native = ic.context->constructor_->imageCreator_;
        if (native == nullptr) {
            IMAGE_ERR("Native instance is nullptr");
            return false;
        }

        if (native->iraContext_ == nullptr) {
            IMAGE_ERR("Image creator context is nullptr");
            return false;
        }
        napi_create_int32(args.env, native->iraContext_->GetCapicity(), &(ic.result));
        return true;
    };

    return JSCommonProcess(args);
}

napi_value ImageCreatorNapi::JsGetFormat(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::GETTER,
    };
    args.argc = ARGS0;

    args.nonAsyncBack = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        napi_get_undefined(args.env, &(ic.result));
        if (g_isCreatorTest) {
            napi_create_int32(args.env, TEST_FORMAT, &(ic.result));
            return true;
        }
        auto native = ic.context->constructor_->imageCreator_;
        if (native == nullptr) {
            IMAGE_ERR("Native instance is nullptr");
            return false;
        }

        if (native->iraContext_ == nullptr) {
            IMAGE_ERR("Image creator context is nullptr");
            return false;
        }
        napi_create_int32(args.env, native->iraContext_->GetFormat(), &(ic.result));
        return true;
    };

    return JSCommonProcess(args);
}

#ifdef IMAGE_DEBUG_FLAG
static void TestAcquireBuffer(OHOS::sptr<OHOS::IConsumerSurface> &creatorSurface, int32_t &fence,
    int64_t &timestamp, OHOS::Rect &damage, std::shared_ptr<ImageCreator> imageCreator)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    if (creatorSurface == nullptr) {
        IMAGE_ERR("Creator Surface is nullptr");
        return;
    }
    creatorSurface->AcquireBuffer(buffer, fence, timestamp, damage);
    if (buffer == nullptr) {
        IMAGE_ERR("Creator Surface is nullptr");
        return;
    }
    IMAGE_ERR("...AcquireBuffer...");
    InitializationOptions opts;
    opts.size.width = creatorSurface->GetDefaultWidth();
    opts.size.height = creatorSurface->GetDefaultHeight();
    opts.pixelFormat = OHOS::Media::PixelFormat::BGRA_8888;
    opts.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    opts.scaleMode = OHOS::Media::ScaleMode::CENTER_CROP;
    opts.editable = true;
    imageCreator->SaveSenderBufferAsImage(buffer, opts);
}

static void DoTest(std::shared_ptr<ImageCreator> imageCreator)
{
    if (imageCreator == nullptr || imageCreator->iraContext_ == nullptr) {
        IMAGE_ERR("image creator is nullptr");
        return;
    }
    std::string creatorKey = imageCreator->iraContext_->GetCreatorKey();
    IMAGE_ERR("CreatorKey = %{public}s", creatorKey.c_str());
    OHOS::sptr<OHOS::IConsumerSurface> creatorSurface = ImageCreator::getSurfaceById(creatorKey);
    IMAGE_ERR("getDefaultWidth = %{public}d", creatorSurface->GetDefaultWidth());
    IMAGE_ERR("getDefaultHeight = %{public}d", creatorSurface->GetDefaultHeight());
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage = {};
    IMAGE_ERR("TestAcquireBuffer 1...");
    TestAcquireBuffer(creatorSurface, flushFence, timestamp, damage, imageCreator);
}
static void DoCallBackAfterWork(uv_work_t *work, int status);
napi_value ImageCreatorNapi::JsTest(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::GETTER,
    };
    args.argc = ARGS0;

    args.nonAsyncBack = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        DoTest(ic.context->creator_);
        if (g_isCreatorTest && g_listener != nullptr) {
            unique_ptr<uv_work_t> work = make_unique<uv_work_t>();
            work->data = reinterpret_cast<void *>(g_listener->context.get());
            DoCallBackAfterWork(work.release(), ARGS0);
            g_listener = nullptr;
        }
        return true;
    };

    return JSCommonProcess(args);
}
#endif

napi_value ImageCreatorNapi::JsDequeueImage(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::ASYNC,
        .name = "JsDequeueImage",
        .callBack = nullptr,
        .argc = ARGS1,
        .queryArgs = PrepareOneArg,
    };

    args.callBack = [](napi_env env, napi_status status, Contextc context) {
        IMAGE_LINE_IN();
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        if (g_isCreatorTest) {
            result = ImageNapi::Create(env);
            context->status = SUCCESS;
            CommonCallbackRoutine(env, context, result);
            return;
        }

        auto native = context->constructor_->imageCreator_;
        if (native != nullptr) {
        result = ImageNapi::Create(env, native->DequeueNativeImage());
        if (result == nullptr) {
            IMAGE_ERR("ImageNapi Create failed");
        }
        } else {
            IMAGE_ERR("Native instance is nullptr");
        }

        if (result == nullptr) {
            napi_get_undefined(env, &result);
            context->status = ERR_IMAGE_INIT_ABNORMAL;
        } else {
            context->status = SUCCESS;
        }
        
        IMAGE_LINE_OUT();
        CommonCallbackRoutine(env, context, result);
    };

    return JSCommonProcess(args);
}

static bool IsTestImageArgs(napi_env env, napi_value value)
{
    if (g_isCreatorTest) {
        ImageNapi* image = nullptr;
        napi_status status = napi_unwrap(env, value, reinterpret_cast<void**>(&image));
        return (status == napi_ok && image != nullptr);
    }
    return false;
}

static bool JsQueueArgs(napi_env env, size_t argc, napi_value* argv,
                        std::shared_ptr<NativeImage> &imageNapi_, napi_ref* callbackRef)
{
    if (argc == ARGS1 || argc == ARGS2) {
        auto argType0 = ImageNapiUtils::getType(env, argv[PARAM0]);
        if (argType0 == napi_object) {
            imageNapi_ = ImageNapi::GetNativeImage(env, argv[PARAM0]);
            if (imageNapi_ == nullptr && !IsTestImageArgs(env, argv[PARAM0])) {
                ImageNapiUtils::ThrowExceptionError(env, static_cast<int32_t>(napi_invalid_arg),
                    "Could not get queue type object");
                return false;
            }
        } else {
            std::string errMsg = "Unsupport args0 type: ";
            ImageNapiUtils::ThrowExceptionError(env, static_cast<int32_t>(napi_invalid_arg),
                errMsg.append(std::to_string(argType0)));
            return false;
        }
        if (argc == ARGS2) {
        auto argType1 = ImageNapiUtils::getType(env, argv[PARAM1]);
        if (argType1 == napi_function) {
            int32_t refCount = 1;
            napi_create_reference(env, argv[PARAM1], refCount, callbackRef);
        } else {
            std::string errMsg = "Unsupport args1 type: ";
            ImageNapiUtils::ThrowExceptionError(env, static_cast<int32_t>(napi_invalid_arg),
                errMsg.append(std::to_string(argType1)));
            return false;
        }
    }
    } else {
        std::string errMsg = "Invailed argc: ";
        ImageNapiUtils::ThrowExceptionError(env, static_cast<int32_t>(napi_invalid_arg),
            errMsg.append(std::to_string(argc)));
        return false;
    }
    return true;
}

void ImageCreatorNapi::JsQueueImageSendEvent(napi_env env, ImageCreatorAsyncContext* context,
                                             napi_event_priority prio)
{
    auto task = [env, context]() {
        IMAGE_FUNCTION_IN();
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        if (g_isCreatorTest) {
            context->status = SUCCESS;
            CommonCallbackRoutine(env, const_cast<ImageCreatorAsyncContext *&>(context), result);
            return;
        }
        auto native = context->constructor_->imageCreator_;
        if (native == nullptr || context->imageNapi_ == nullptr) {
            IMAGE_ERR("Native instance is nullptr");
            context->status = ERR_IMAGE_INIT_ABNORMAL;
        } else {
            if (SUCCESS != context->imageNapi_->CombineYUVComponents()) {
                IMAGE_ERR("JsQueueImageCallBack: try to combine componests");
            }
            native->QueueNativeImage(context->imageNapi_);
            context->status = SUCCESS;
        }
        IMAGE_LINE_OUT();
        CommonCallbackRoutine(env, const_cast<ImageCreatorAsyncContext *&>(context), result);
    };
    if (napi_status::napi_ok != napi_send_event(env, task, prio)) {
        IMAGE_LOGE("JsQueueImageSendEvent: failed to SendEvent!");
    }
}

napi_value ImageCreatorNapi::JsQueueImage(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = ARGS2;
    napi_value argv[ARGS2] = {0};

    napi_get_undefined(env, &result);

    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        IMAGE_ERR("fail to napi_get_cb_info %{public}d", status);
        return result;
    }

    unique_ptr<ImageCreatorAsyncContext> context = make_unique<ImageCreatorAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&context->constructor_));
    if (status != napi_ok || context->constructor_ == nullptr) {
        IMAGE_ERR("fail to unwrap constructor_ %{public}d", status);
        return result;
    }

    if (!JsQueueArgs(env, argc, argv, context->imageNapi_, &(context->callbackRef))) {
        return result;
    }
    if (context->callbackRef == nullptr) {
        napi_create_promise(env, &(context->deferred), &result);
    }

    JsQueueImageSendEvent(env, context.get(), napi_eprio_high);
    context.release();

    IMAGE_FUNCTION_OUT();
    return result;
}

static bool CheckOnParam0(napi_env env, napi_value value, const std::string& refStr)
{
    bool ret = true;
    size_t bufLength = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &bufLength);
    if (status != napi_ok || bufLength > PATH_MAX) {
        return false;
    }

    char *buffer = static_cast<char *>(malloc((bufLength + 1) * sizeof(char)));
    if (buffer == nullptr) {
        return false;
    }

    status = napi_get_value_string_utf8(env, value, buffer, bufLength + 1, &bufLength);
    if (status != napi_ok) {
        free(buffer);
        return false;
    }

    std::string strValue = buffer;
    if (strValue.compare(refStr) != 0) {
        IMAGE_ERR("strValue is %{public}s", strValue.c_str());
        ret = false;
    }

    strValue = "";
    free(buffer);
    buffer = nullptr;
    return ret;
}

static bool JsOnQueryArgs(ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic)
{
    if (ic.argc == ARGS2) {
        auto argType0 = ImageNapiUtils::getType(args.env, ic.argv[PARAM0]);
        auto argType1 = ImageNapiUtils::getType(args.env, ic.argv[PARAM1]);
        if (argType0 == napi_string && argType1 == napi_function) {
            if (!ImageNapiUtils::GetUtf8String(args.env, ic.argv[PARAM0], ic.onType)) {
                ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                    "Could not get On type string");
                return false;
            }

            if (!CheckOnParam0(args.env, ic.argv[PARAM0], "imageRelease")) {
                ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                    "Unsupport PARAM0");
                return false;
            }

            napi_create_reference(args.env, ic.argv[PARAM1], ic.refCount, &(ic.context->callbackRef));
        } else {
            std::string errMsg = "Unsupport args type: ";
            ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                errMsg.append(std::to_string(argType0)).append(std::to_string(argType1)));
            return false;
        }
    } else {
        std::string errMsg = "Invailed argc: ";
        ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
            errMsg.append(std::to_string(ic.argc)));
        return false;
    }

    napi_get_undefined(args.env, &ic.result);
    return true;
}
static void DoCallBackAfterWork(uv_work_t *work, int status)
{
    IMAGE_LINE_IN();
    Contextc context = reinterpret_cast<Contextc>(work->data);
    if (context == nullptr) {
        IMAGE_ERR("context is empty");
    } else {
        if (context->env != nullptr && context->callbackRef != nullptr) {
            napi_handle_scope scope = nullptr;
            napi_open_handle_scope(context->env, &scope);
            if (scope == nullptr) {
                delete work;
                return;
            }
            napi_value result[PARAM2] = {0};
            napi_value retVal = nullptr;
            napi_value callback = nullptr;
            napi_create_uint32(context->env, SUCCESS, &result[0]);
            napi_get_undefined(context->env, &result[1]);
            napi_get_reference_value(context->env, context->callbackRef, &callback);
            if (callback != nullptr) {
                napi_call_function(context->env, nullptr, callback, PARAM2, result, &retVal);
            } else {
                IMAGE_ERR("napi_get_reference_value callback is empty");
            }
            napi_close_handle_scope(context->env, scope);
        } else {
            IMAGE_ERR("env or callbackRef is empty");
        }
    }
    delete work;
    IMAGE_LINE_OUT();
}

static void DoCallBackNoUvWork(napi_env env, ImageCreatorAsyncContext* context)
{
    IMAGE_LINE_IN();
    if (context == nullptr) {
        IMAGE_ERR("context is empty");
    } else {
        if (context->env != nullptr && context->callbackRef != nullptr) {
            napi_handle_scope scope = nullptr;
            napi_open_handle_scope(context->env, &scope);
            if (scope == nullptr) {
                return;
            }
            napi_value result[PARAM2] = {0};
            napi_value retVal = nullptr;
            napi_value callback = nullptr;
            napi_create_uint32(context->env, SUCCESS, &result[0]);
            napi_get_undefined(context->env, &result[1]);
            napi_get_reference_value(context->env, context->callbackRef, &callback);
            if (callback != nullptr) {
                napi_call_function(context->env, nullptr, callback, PARAM2, result, &retVal);
            } else {
                IMAGE_ERR("napi_get_reference_value callback is empty");
            }
            napi_close_handle_scope(context->env, scope);
        } else {
            IMAGE_ERR("env or callbackRef is empty");
        }
    }
    IMAGE_LINE_OUT();
}

void ImageCreatorNapi::DoCallBack(shared_ptr<ImageCreatorAsyncContext> context,
    string name, CompleteCreatorCallback callBack)
{
    IMAGE_FUNCTION_IN();
    if (context == nullptr || context->env == nullptr) {
        IMAGE_ERR("gContext or env is empty");
        return;
    }

    auto task = [context]() {
        (void)DoCallBackNoUvWork(context->env, context.get());
    };
    if (napi_status::napi_ok != napi_send_event(context->env, task, napi_eprio_high)) {
        IMAGE_LOGE("DoCallBackSendEvent: failed to SendEvent!");
    }
    IMAGE_FUNCTION_OUT();
}

napi_value ImageCreatorNapi::JsOn(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::ASYNC,
        .name = "JsOn",
    };
    args.argc = ARGS2;
    args.asyncLater = true;
    args.queryArgs = JsOnQueryArgs;
    args.nonAsyncBack = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        IMAGE_LINE_IN();
        if (g_isCreatorTest) {
            g_listener = make_shared<ImageCreatorReleaseListener>();
            g_listener->context = std::move(ic.context);
            g_listener->context->env = args.env;
            g_listener->name = args.name;
            return true;
        }
        napi_get_undefined(args.env, &(ic.result));

        auto native = ic.context->constructor_->imageCreator_;
        if (native == nullptr) {
            IMAGE_ERR("Native instance is nullptr");
            ic.context->status = ERR_IMAGE_INIT_ABNORMAL;
            return false;
        }
        shared_ptr<ImageCreatorReleaseListener> listener = make_shared<ImageCreatorReleaseListener>();
        listener->context = std::move(ic.context);
        listener->context->env = args.env;
        listener->name = args.name;

        native->RegisterBufferReleaseListener((std::shared_ptr<SurfaceBufferReleaseListener> &)listener);

        IMAGE_LINE_OUT();
        return true;
    };

    return JSCommonProcess(args);
}

napi_value ImageCreatorNapi::JsOffOneArg(napi_env env, napi_callback_info info)
{
    ImageCreatorCommonArgs args = {
        .env = env, .info = info, .async = CreatorCallType::ASYNC,
        .name = "JsOff", .argc = ARGS1, .asyncLater = true,
    };

    args.queryArgs = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        if (ic.argc != ARGS1) {
            std::string errMsg = "Invalid argc: ";
            ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                errMsg.append(std::to_string(ic.argc)));
            return false;
        }
        auto argType = ImageNapiUtils::getType(args.env, ic.argv[PARAM0]);
        if (argType != napi_string) {
            std::string errMsg = "Unsupport args type: ";
            ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                errMsg.append(std::to_string(argType)));
            return false;
        }
        if (!ImageNapiUtils::GetUtf8String(args.env, ic.argv[PARAM0], ic.onType)) {
            ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                "Could not get On type string");
            return false;
        }
        if (!CheckOnParam0(args.env, ic.argv[PARAM0], "imageRelease")) {
            ImageNapiUtils::ThrowExceptionError(args.env, static_cast<int32_t>(napi_invalid_arg),
                "Unsupport PARAM0");
            return false;
        }
        napi_get_undefined(args.env, &ic.result);
        return true;
    };

    args.nonAsyncBack = [](ImageCreatorCommonArgs &args, ImageCreatorInnerContext &ic) -> bool {
        IMAGE_LINE_IN();
        napi_get_undefined(args.env, &ic.result);

        if (g_isCreatorTest && g_listener != nullptr) {
            g_listener.reset();
        } else {
            if (ic.context != nullptr && ic.context->constructor_ != nullptr
                && ic.context->constructor_->imageCreator_ != nullptr) {
                ic.context->constructor_->imageCreator_->UnRegisterBufferReleaseListener();
            }
        }
        ic.context->status = SUCCESS;
        napi_create_uint32(args.env, ic.context->status, &ic.result);
        IMAGE_LINE_OUT();
        return true;
    };

    return JSCommonProcess(args);
}

napi_value ImageCreatorNapi::JsOffTwoArgs(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    ImageCreatorCommonArgs args = {
        .env = env, .info = info, .async = CreatorCallType::ASYNC,
        .name = "JsOff", .argc = ARGS2, .queryArgs = JsOnQueryArgs,
    };

    args.callBack = [](napi_env env, napi_status status, Contextc context) {
        IMAGE_LINE_IN();
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        context->constructor_->imageCreator_->UnRegisterBufferReleaseListener();
        context->status = SUCCESS;
        CommonCallbackRoutine(env, context, result);
        IMAGE_LINE_OUT();
    };

    JSCommonProcess(args);
    napi_create_uint32(args.env, SUCCESS, &result);
    return result;
}

napi_value ImageCreatorNapi::JsOff(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    struct ImageCreatorInnerContext ic;
    ic.argc = ARGS2;
    ic.argv.resize(ic.argc);
    napi_get_undefined(env, &ic.result);
    IMG_JS_ARGS(env, info, ic.status, ic.argc, &(ic.argv[0]), ic.thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(ic.status), ic.result, IMAGE_ERR("fail to napi_get_cb_info"));

    if (ic.argc == ARGS1) {
        return JsOffOneArg(env, info);
    } else if (ic.argc == ARGS2) {
        return JsOffTwoArgs(env, info);
    } else {
        IMAGE_ERR("invalid argument count");
        return ic.result;
    }
}

napi_value ImageCreatorNapi::JsRelease(napi_env env, napi_callback_info info)
{
    IMAGE_FUNCTION_IN();
    ImageCreatorCommonArgs args = {
        .env = env, .info = info,
        .async = CreatorCallType::ASYNC,
        .name = "JsRelease",
        .callBack = nullptr,
        .argc = ARGS1,
        .queryArgs = PrepareOneArg,
    };

    args.callBack = [](napi_env env, napi_status status, Contextc context) {
        IMAGE_LINE_IN();
        napi_value result = nullptr;
        napi_get_undefined(env, &result);

        context->constructor_->NativeRelease();
        context->status = SUCCESS;

        IMAGE_LINE_OUT();
        CommonCallbackRoutine(env, context, result);
    };

    return JSCommonProcess(args);
}

void ImageCreatorNapi::release()
{
    if (!isRelease) {
        NativeRelease();
        isRelease = true;
    }
}
}  // namespace Media
}  // namespace OHOS
