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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_STANDBY_SERVICE_IMPL_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_STANDBY_SERVICE_IMPL_H

#include <memory>
#include <list>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "event_runner.h"
#include "event_handler.h"

#include "allow_info.h"
#include "allow_record.h"
#include "resourcce_request.h"
#include "standby_state_subscriber.h"
#include "istandby_service_subscriber.h"
#include "iconstraint_manager_adapter.h"
#include "ilistener_manager_adapter.h"
#include "istrategy_manager_adapter.h"
#include "istate_manager_adapter.h"
#include "standby_service_log.h"
#include "single_instance.h"

#include "app_mgr_client.h"
#include "app_mgr_helper.h"
#include "common_event_observer.h"

namespace OHOS {
namespace DevStandbyMgr {
class StandbyServiceImpl : public std::enable_shared_from_this<StandbyServiceImpl> {
DECLARE_SINGLE_INSTANCE(StandbyServiceImpl);
public:
    bool Init();
    void InitReadyState();
    ErrCode RegisterCommEventObserver();
    ErrCode RegisterAppStateObserver();
    ErrCode RegisterTimeObserver();
    ErrCode UnregisterCommEventObserver();
    ErrCode UnregisterAppStateObserver();
    ErrCode UnregisterTimeObserver();
    ErrCode ResetTimeObserver();
    void DayNightSwitchCallback();
    ErrCode RegisterPlugin(const std::string& pluginName);
    void UninitReadyState();
    void UnInit();

    ErrCode SubscribeStandbyCallback(const sptr<IStandbyServiceSubscriber>& subscriber);
    ErrCode UnsubscribeStandbyCallback(const sptr<IStandbyServiceSubscriber>& subscriber);
    ErrCode ApplyAllowResource(const sptr<ResourceRequest>& resourceRequest);
    ErrCode UnapplyAllowResource(const sptr<ResourceRequest>& resourceRequest);
    ErrCode GetAllowList(uint32_t allowType, std::vector<AllowInfo>& allowInfoList,
        uint32_t reasonCode);
    ErrCode IsDeviceInStandby(bool& isStandby);

    void RegisterPluginInner(IConstraintManagerAdapter* constraintManager,
        IListenerManagerAdapter* listenerManager,
        IStrategyManagerAdapter* strategyManager,
        IStateManagerAdapter* stateManager);

    std::shared_ptr<AppExecFwk::EventHandler>& GetHandler();
    std::shared_ptr<IConstraintManagerAdapter>& GetConstraintManager();
    std::shared_ptr<IListenerManagerAdapter>& GetListenerManager();
    std::shared_ptr<IStrategyManagerAdapter>& GetStrategyManager();
    std::shared_ptr<IStateManagerAdapter>& GetStateManager();

    ErrCode RemoveAppAllowRecord(int32_t uid, const std::string &bundleName, bool resetAll);

    void ShellDump(const std::vector<std::string>& argsInStr, std::string& result);
    void GetAllowListInner(uint32_t allowType, std::vector<AllowInfo>& allowInfoList,
        uint32_t reasonCode);

    void DispatchEvent(const StandbyMessage& message);
private:
    void ApplyAllowResInner(const sptr<ResourceRequest>& resourceRequest, int32_t pid);
    void UpdateRecord(std::shared_ptr<AllowRecord>& allowRecord, const sptr<ResourceRequest>& resourceRequest);
    void UnapplyAllowResInner(int32_t uid, const std::string& name, uint32_t allowType,  bool removeAll);
    void GetTemporaryAllowList(uint32_t allowTypeIndex, std::vector<AllowInfo>& allowInfoList,
        uint32_t reasonCode);
    void GetPersistAllowList(uint32_t allowTypeIndex, std::vector<AllowInfo>& allowInfoList, bool isApp);
    void NotifyAllowListChanged(int32_t uid, const std::string& name, uint32_t allowType, bool added);

    void RecoverTimeLimitedTask();
    bool ParsePersistentData();
    void GetPidAndProcName(std::unordered_map<int32_t, std::string>& pidNameMap);
    void DumpPersistantData();

    ErrCode CheckCallerPermission(uint32_t reasonCode);
    ErrCode IsSystemAppWithPermission(int32_t uid, Security::AccessToken::AccessTokenID tokenId, uint32_t reasonCode);
    ErrCode CheckNativePermission(Security::AccessToken::AccessTokenID tokenId);
    bool CheckAllowTypeInfo(uint32_t allowType);
    ErrCode CheckRunningResourcesApply(const int32_t uid, const std::string& bundleName);
    int32_t GetUserIdByUid(int32_t uid);

    void ShellDumpInner(const std::vector<std::string>& argsInStr, std::string& result);
    void DumpUsage(std::string& result);
    void DumpShowDetailInfo(const std::vector<std::string>& argsInStr, std::string& result);
    void DumpAllowListInfo(std::string& result);
    void DumpChangeModeSwitch(const std::vector<std::string>& argsInStr, std::string& result);
    void DumpEnterSpecifiedState(const std::vector<std::string>& argsInStr, std::string& result);
    void DumpModifyAllowList(const std::vector<std::string>& argsInStr, std::string& result);
    void DumpActivateMotion(const std::vector<std::string>& argsInStr, std::string& result);

private:
    std::atomic<bool> isServiceReady_ {false};

    std::shared_ptr<AppExecFwk::EventHandler> handler_ {nullptr};
    std::mutex observerMutex_ {};
    std::unique_ptr<AppExecFwk::AppMgrClient> appMgrClient_ {nullptr};
    std::shared_ptr<CommonEventObserver> commonEventObserver_ {nullptr};
    uint64_t dayNightSwitchTimerId_ {0};
    std::unordered_map<std::string, std::shared_ptr<AllowRecord>> allowInfoMap_ {};
    std::shared_ptr<StandbyStateSubscriber> standbySubscriber_ {nullptr};

    void* registerPlugin_ {nullptr};
    std::shared_ptr<IConstraintManagerAdapter> constraintManager_ {nullptr};
    std::shared_ptr<IListenerManagerAdapter> listenerManager_ {nullptr};
    std::shared_ptr<IStrategyManagerAdapter> strategyManager_ {nullptr};
    std::shared_ptr<IStateManagerAdapter> standbyStateManager_ {nullptr};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_STANDBY_SERVICE_IMPL_H