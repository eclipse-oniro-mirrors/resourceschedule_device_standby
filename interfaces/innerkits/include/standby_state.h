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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_INTERFACES_INNERKITS_INCLUDE_STANDBY_STATE_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_INTERFACES_INNERKITS_INCLUDE_STANDBY_STATE_H

namespace OHOS {
namespace DevStandbyMgr {
struct StandbyState {
    enum : uint32_t {
        WORKING = 0,
        DARK = 1,
        NAP,
        MAINTENANCE,
        SLEEP,
    };
};

struct NapStatePhase {
    enum : uint32_t {
        CONNECTION = 0,
        SYS_RES_LIGHT,
        APP_RES_LIGHT,
        END,
    };
};

struct SleepStatePhase {
    enum : uint32_t {
        SYS_RES_DEEP = 0,
        APP_RES_DEEP,
        APP_RES_HARDWARE,
        END,
    };
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_INTERFACES_INNERKITS_INCLUDE_STANDBY_STATE_H