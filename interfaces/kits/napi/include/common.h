/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_STANDBY_INTERFACES_KITS_NAPI_INCLUDE_COMMON_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_STANDBY_INTERFACES_KITS_NAPI_INCLUDE_COMMON_H

#include <string_ex.h>
#include <memory>

#include "standby_service_errors.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DevStandbyMgr {
struct AsyncWorkData {
    explicit AsyncWorkData(napi_env napiEnv);
    virtual ~AsyncWorkData();
    napi_env env;
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
    bool isCallback = false;
    int32_t errCode = 0;
};

class Common {
public:
    static napi_value NapiGetNull(napi_env env);

    static void PaddingAsyncWorkData(
        const napi_env& env, const napi_ref& callback, AsyncWorkData& info, napi_value& promise);

    static void ReturnCallbackPromise(const napi_env& env, const AsyncWorkData& info,
        const napi_value& result);

    static void SetCallback(const napi_env& env, const napi_ref& callbackIn,
        const int32_t& errCode, const napi_value& result);

    static napi_value GetCallbackErrorValue(napi_env env, const int32_t errCode, const std::string errMsg);

    static napi_value SetPromise(const napi_env& env, const AsyncWorkData& info, const napi_value& result);

    static void HandleErrCode(const napi_env &env, int32_t errCode);

    static std::string FindErrMsg(const napi_env& env, const int32_t errCode);

    static int32_t FindErrCode(const napi_env& env, const int32_t errCodeIn);

    static bool HandleParamErr(const napi_env& env, int32_t errCode);

    static void SetInt32Value(const napi_env& env, const std::string& fieldStr,
        const int32_t intValue, napi_value& result);

    static void SetUint32Value(const napi_env& env, const std::string& fieldStr,
        const uint32_t uintValue, napi_value& result);

    static void SetStringValue(const napi_env& env, const std::string& fieldStr,
        const std::string& stringValue, napi_value& result);

    static napi_value GetStringValue(const napi_env &env, const napi_value &value, std::string &result);

    static napi_value GetUint32Value(const napi_env& env, const napi_value& value, uint32_t& result);

    static napi_value GetInt32Value(const napi_env& env, const napi_value& value, int32_t& result);

    static napi_value GetNamedInt32Value(const napi_env &env, napi_value &object, const char* utf8name,
        int32_t& result);

    static napi_value GetNamedStringValue(const napi_env &env, napi_value &object, const char* utf8name,
        std::string& result);
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_STANDBY_INTERFACES_KITS_NAPI_INCLUDE_COMMON_H