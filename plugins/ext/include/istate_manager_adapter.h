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
#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_ISTATE_MANAGER_ADAPTER_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_ISTATE_MANAGER_ADAPTER_H

#include <vector>
#include <memory>

#include "event_handler.h"
#include "event_runner.h"

#include "standby_service_errors.h"
#include "base_state.h"
#include "istrategy_manager_adapter.h"

namespace OHOS {
namespace DevStandbyMgr {
class IConstraintManagerAdapter;
class BaseState;
struct ConstraintEvalParam;
class IStateManagerAdapter {
public:
    virtual ~IStateManagerAdapter() = default;
    virtual bool Init() = 0;
    virtual bool UnInit() = 0;
    virtual void HandleEvent(const StandbyMessage& message) = 0;

    virtual ErrCode StartEvalCurrentState(const ConstraintEvalParam& params) = 0;
    virtual ErrCode EndEvalCurrentState(bool evalResult) = 0;

    virtual ErrCode TransitToState(uint32_t nextState) = 0;
    virtual ErrCode NextStateImpl(uint32_t nextState) = 0;
    virtual void ShellDump(const std::vector<std::string>& argsInStr, std::string& result) = 0;

    virtual uint32_t GetCurState() = 0;
    virtual uint32_t GetPreState() = 0;
    virtual void BlockCurrentState() = 0;
    virtual void UnblockCurrentState() = 0;
    virtual void SetEvalution(bool isEvalution);
    virtual bool IsEvalution();
protected:
    bool isEvalution_ {false};
    bool isBlocked_ {false};
    std::shared_ptr<BaseState> preStatePtr_ {nullptr};
    std::shared_ptr<BaseState> curStatePtr_ {nullptr};
    std::shared_ptr<AppExecFwk::EventHandler> handler_ {nullptr};
    std::shared_ptr<IConstraintManagerAdapter> constraintManager_ {nullptr};
    std::shared_ptr<IStrategyManagerAdapter> strategyManager_ {nullptr};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_ISTATE_MANAGER_ADAPTER_H
