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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_INTERFACES_KITS_NAPI_INCLUDE_INIT_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_INTERFACES_KITS_NAPI_INCLUDE_INIT_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DevStandbyMgr {
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor)) void RegisterModule(void);
void SetNamedPropertyByInteger(napi_env env, napi_value dstObj, int32_t objName, const char *propName);
napi_value DeviceStandbyFuncInit(napi_env env, napi_value exports);
napi_value DeviceStandbyTypeInit(napi_env env, napi_value exports);
static napi_value DeviceStandbyInit(napi_env env, napi_value exports);

#ifdef __cplusplus
}
#endif

/*
 * Module define
 */
napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = DeviceStandbyInit,
    .nm_modname = "resourceschedule.deviceStandby",
    .nm_priv = nullptr,
    .reserved = {nullptr}
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_INTERFACES_KITS_NAPI_INCLUDE_INIT_H