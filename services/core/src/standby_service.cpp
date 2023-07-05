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

#include "standby_service.h"

#include <functional>
#include <map>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "file_ex.h"
#include "string_ex.h"

#include "device_standby_switch.h"
#include "standby_service_impl.h"
#include "standby_service_log.h"
#include "standby_config_manager.h"

namespace OHOS {
namespace DevStandbyMgr {
namespace {
const uint32_t COMMON_EVENT_READY = 1;
const uint32_t TIMER_SERVICE_READY = 2;
const uint32_t ABILITY_SERVICE_READY = 4;
const uint32_t BUNDLE_MGR_READY = 8;
const uint32_t POWER_SERVICE_READY = 16;
const uint32_t ALL_DEPENDS_READY = 31;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    StandbyService::GetInstance().get());
}

IMPLEMENT_SINGLE_INSTANCE(StandbyService);

StandbyService::StandbyService() : SystemAbility(DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID, true) {}

StandbyService::StandbyService(const int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID, true) {}

StandbyService::~StandbyService() {}

void StandbyService::OnStart()
{
    if (!g_softwareSleep) {
        return;
    }
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("device standby service has already started.");
        return;
    }
    if (!StandbyServiceImpl::GetInstance()->Init()) {
        STANDBYSERVICE_LOGE("failed to init device standby service");
        return;
    }
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    AddSystemAbilityListener(TIME_SERVICE_ID);
    AddSystemAbilityListener(ABILITY_MGR_SERVICE_ID);
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    AddSystemAbilityListener(POWER_MANAGER_SERVICE_ID);
    if (!Publish(StandbyService::GetInstance().get())) {
        STANDBYSERVICE_LOGE("standby service start failed!");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    STANDBYSERVICE_LOGI("standby service start succeed!");
}

void StandbyService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    STANDBYSERVICE_LOGI("add system ability, systemAbilityId : %{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case COMMON_EVENT_SERVICE_ID:
            STANDBYSERVICE_LOGI("common event service is ready!");
            dependsReady_ |= COMMON_EVENT_READY;
            StandbyServiceImpl::GetInstance()->RegisterCommEventObserver();
            break;
        case TIME_SERVICE_ID:
            STANDBYSERVICE_LOGI("timer service is ready!");
            dependsReady_ |= TIMER_SERVICE_READY;
            StandbyServiceImpl::GetInstance()->RegisterTimeObserver();
            break;
        case ABILITY_MGR_SERVICE_ID:
            STANDBYSERVICE_LOGI("ability mgr service is ready!");
            dependsReady_ |= ABILITY_SERVICE_READY;
            break;
        case BUNDLE_MGR_SERVICE_SYS_ABILITY_ID:
            STANDBYSERVICE_LOGI("bundle mgr service is ready!");
            dependsReady_ |= BUNDLE_MGR_READY;
            break;
        case POWER_MANAGER_SERVICE_ID:
            STANDBYSERVICE_LOGI("power service is ready!");
            dependsReady_ |= POWER_SERVICE_READY;
            break;
        default:
            break;
    }
    STANDBYSERVICE_LOGI("after add system ability, ready state : %{public}u", dependsReady_);
    if (dependsReady_ == ALL_DEPENDS_READY) {
        STANDBYSERVICE_LOGI("all necessary system service for standby service has been satisfied!");
        StandbyServiceImpl::GetInstance()->InitReadyState();
    }
}

void StandbyService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    STANDBYSERVICE_LOGI("remove system ability, systemAbilityId : %{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case COMMON_EVENT_SERVICE_ID:
            STANDBYSERVICE_LOGI("common event service is removed!");
            dependsReady_ &= (~COMMON_EVENT_READY);
            StandbyServiceImpl::GetInstance()->UnregisterCommEventObserver();
            break;
        case TIME_SERVICE_ID:
            STANDBYSERVICE_LOGI("time service is removed!");
            dependsReady_ &= (~TIMER_SERVICE_READY);
            StandbyServiceImpl::GetInstance()->UnregisterTimeObserver();
            break;
        case ABILITY_MGR_SERVICE_ID:
            STANDBYSERVICE_LOGI("ability mgr service is removed!");
            dependsReady_ &= (~ABILITY_SERVICE_READY);
            break;
        case BUNDLE_MGR_SERVICE_SYS_ABILITY_ID:
            STANDBYSERVICE_LOGI("bundle mgr service is removed!");
            dependsReady_ &= (~BUNDLE_MGR_READY);
            break;
        case POWER_MANAGER_SERVICE_ID:
            STANDBYSERVICE_LOGI("power service is removed!");
            dependsReady_ &= (~POWER_SERVICE_READY);
            break;
        default:
            break;
    }
    if (dependsReady_ != ALL_DEPENDS_READY) {
        STANDBYSERVICE_LOGI("necessary system service for standby service has been unsatisfied");
        StandbyServiceImpl::GetInstance()->UninitReadyState();
    }
}

ErrCode StandbyService::SubscribeStandbyCallback(const sptr<IStandbyServiceSubscriber>& subscriber)
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("standby service is not running");
        return ERR_STANDBY_SYS_NOT_READY;
    }
    return StandbyServiceImpl::GetInstance()->SubscribeStandbyCallback(subscriber);
}

ErrCode StandbyService::UnsubscribeStandbyCallback(const sptr<IStandbyServiceSubscriber>& subscriber)
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("standby service is not running");
        return ERR_STANDBY_SYS_NOT_READY;
    }
    return StandbyServiceImpl::GetInstance()->UnsubscribeStandbyCallback(subscriber);
}

ErrCode StandbyService::ApplyAllowResource(const sptr<ResourceRequest>& resourceRequest)
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("standby service is not running");
        return ERR_STANDBY_SYS_NOT_READY;
    }
    return StandbyServiceImpl::GetInstance()->ApplyAllowResource(resourceRequest);
}

ErrCode StandbyService::UnapplyAllowResource(const sptr<ResourceRequest>& resourceRequest)
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("standby service is not running");
        return ERR_STANDBY_SYS_NOT_READY;
    }
    return StandbyServiceImpl::GetInstance()->UnapplyAllowResource(resourceRequest);
}

ErrCode StandbyService::GetAllowList(uint32_t allowType, std::vector<AllowInfo>& allowInfoList,
    uint32_t reasonCode)
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("standby service is not running");
        return ERR_STANDBY_SYS_NOT_READY;
    }
    return StandbyServiceImpl::GetInstance()->GetAllowList(allowType, allowInfoList, reasonCode);
}

ErrCode StandbyService::IsDeviceInStandby(bool& isStandby)
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        STANDBYSERVICE_LOGW("standby service is not running");
        return ERR_STANDBY_SYS_NOT_READY;
    }
    return StandbyServiceImpl::GetInstance()->IsDeviceInStandby(isStandby);
}

void StandbyService::OnStop()
{
    StandbyServiceImpl::GetInstance()->UnInit();
    state_ = ServiceRunningState::STATE_NOT_START;
    STANDBYSERVICE_LOGI("standby service task manager stop");
}

int32_t StandbyService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string& arg) {
        return Str16ToStr8(arg);
    });
    std::string result;
    StandbyServiceImpl::GetInstance()->ShellDump(argsInStr, result);
    if (!SaveStringToFd(fd, result)) {
        STANDBYSERVICE_LOGE("StandbyService dump save string to fd failed!");
        return ERR_STANDBY_DUMP_SAVE_DENIED;
    }
    return ERR_OK;
}
}  // namespace DevStandbyMgr
}  // namespace OHOS
