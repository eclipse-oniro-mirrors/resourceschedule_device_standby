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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_STANDBY_SERVICE_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_STANDBY_SERVICE_H

#include <ctime>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

#include "singleton.h"
#include "system_ability.h"

#include "istandby_service.h"
#include "standby_service_client.h"
#include "standby_service_stub.h"
#include "singleton.h"

namespace OHOS {
namespace DevStandbyMgr {
enum class ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};

class StandbyService final : public SystemAbility, public StandbyServiceStub {
    DECLARE_SYSTEM_ABILITY(StandbyService);
    DECLARE_DELAYED_SINGLETON(StandbyService);
public:
    StandbyService(const int32_t systemAbilityId, bool runOnCreate);
    static std::shared_ptr<StandbyService> GetInstance();
    void OnStart() final;
    void OnStop() final;
    int32_t OnExtension(const std::string& extension, MessageParcel& data, MessageParcel& reply) override;

    ErrCode SubscribeStandbyCallback(const sptr<IStandbyServiceSubscriber>& subscriber,
        const std::string& subscriberName, const std::string& moduleName) override;
    ErrCode UnsubscribeStandbyCallback(const sptr<IStandbyServiceSubscriber>& subscriber) override;
    ErrCode ApplyAllowResource(const ResourceRequest& resourceRequest) override;
    ErrCode UnapplyAllowResource(const ResourceRequest& resourceRequest) override;
    ErrCode GetAllowList(uint32_t allowType, std::vector<AllowInfo>& allowInfoList,
        uint32_t reasonCode) override;
    ErrCode IsDeviceInStandby(bool& isStandby) override;
    ErrCode SetNatInterval(uint32_t type, bool enable, uint32_t interval) override;
    ErrCode DelayHeartBeat(int64_t timestamp) override;
    ErrCode ReportSceneInfo(uint32_t resType, int64_t value, const std::string &sceneInfo) override;
    ErrCode ReportWorkSchedulerStatus(bool started, int32_t uid, const std::string& bundleName) override;
    ErrCode GetRestrictList(uint32_t restrictType, std::vector<AllowInfo>& restrictInfoList,
        uint32_t reasonCode) override;
    ErrCode IsStrategyEnabled(const std::string& strategyName, bool& isEnabled) override;
    ErrCode ReportDeviceStateChanged(int32_t type, bool enabled) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;
    void AddPluginSysAbilityListener(int32_t systemAbilityId);
    ErrCode NotifySystemAbilityStatusChanged(bool isAdded, int32_t systemAbilityId);
    ErrCode HandleEvent(const uint32_t resType, const int64_t value, const std::string &sceneInfo) override;
    ErrCode ReportPowerOverused(const std::string &module, uint32_t level) override;
private:
    StandbyService(const StandbyService&) = delete;
    StandbyService& operator= (const StandbyService&) = delete;
    StandbyService(StandbyService&&) = delete;
    StandbyService& operator= (StandbyService&&) = delete;
    void DumpUsage(std::string& result);
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    bool CheckProcessNamePermission(const std::string& processName);

private:
    std::mutex systemAbilityLock_ {};
    std::atomic<ServiceRunningState> state_ {ServiceRunningState::STATE_NOT_START};
    std::mutex listenedSALock_ {};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_STANDBY_SERVICE_H
