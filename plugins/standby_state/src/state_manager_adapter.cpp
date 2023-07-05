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

#include "state_manager_adapter.h"

#include <vector>
#include <string>
#include <memory>

#include "common_event_support.h"
#include "standby_service_impl.h"
#include "standby_state_subscriber.h"
#include "standby_config_manager.h"

#include "base_state.h"
#include "dark_state.h"
#include "maintenance_state.h"
#include "nap_state.h"
#include "sleep_state.h"
#include "working_state.h"

namespace OHOS {
namespace DevStandbyMgr {

bool StateManagerAdapter::Init()
{
    auto StandbyServiceImpl = StandbyServiceImpl::GetInstance();
    handler_ = StandbyServiceImpl->GetHandler();
    constraintManager_ = StandbyServiceImpl->GetConstraintManager();
    strategyManager_ = StandbyServiceImpl->GetStrategyManager();
    auto stateManager = StandbyServiceImpl->GetStateManager();
    BaseState::InitRunningLock();
    workingStatePtr_ = std::make_shared<WorkingState>(StandbyState::WORKING, 0, stateManager, handler_);
    darkStatePtr_ = std::make_shared<DarkState>(StandbyState::DARK, 0, stateManager, handler_);
    napStatePtr_ = std::make_shared<NapState>(StandbyState::NAP, 0, stateManager, handler_);
    maintStatePtr_ = std::make_shared<MaintenanceState>(StandbyState::MAINTENANCE, 0, stateManager, handler_);
    sleepStatePtr_ = std::make_shared<SleepState>(StandbyState::SLEEP, 0, stateManager, handler_);
    indexToState_ = {
        workingStatePtr_, darkStatePtr_, napStatePtr_, maintStatePtr_, sleepStatePtr_
    };
    auto callbackTask = [this]() { OnScreenOffHalfHour(true, false); };
    scrOffHalfHourTimerId_ = TimedTask::CreateTimer(false, 0, true, callbackTask);
    if (scrOffHalfHourTimerId_ == 0) {
        STANDBYSERVICE_LOGE("timer of screen off half hour is nullptr");
    }
    for (const auto& statePtr : indexToState_) {
        if (statePtr->Init() != ERR_OK) {
            return false;
        }
    }
    curStatePtr_ = workingStatePtr_;
    preStatePtr_ = curStatePtr_;
    if (curStatePtr_->BeginState() != ERR_OK) {
        return false;
    }
    SendNotification(preStatePtr_->GetCurState(), true);
    STANDBYSERVICE_LOGI("state manager plugin initialization succeed");
    return true;
}

bool StateManagerAdapter::UnInit()
{
    TransitToState(StandbyState::WORKING);
    curStatePtr_->EndState();
    for (auto& statePtr : indexToState_) {
        statePtr->UnInit();
        statePtr.reset();
    }
    if (scrOffHalfHourTimerId_ > 0) {
        MiscServices::TimeServiceClient::GetInstance()->StopTimer(scrOffHalfHourTimerId_);
        MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(scrOffHalfHourTimerId_);
    }
    BaseState::ReleaseStandbyRunningLock();
    return true;
}

void StateManagerAdapter::HandleEvent(const StandbyMessage& message)
{
    if (message.eventId_ == StandbyMessageType::COMMON_EVENT) {
        HandleCommonEvent(message);
    } else if (message.eventId_ == StandbyMessageType::RES_CTRL_CONDITION_CHANGED) {
        SendNotification(curStatePtr_->GetCurState(), false);
    }
}

void StateManagerAdapter::HandleCommonEvent(const StandbyMessage& message)
{
    HandleScrOffHalfHour(message);
    if (message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON ||
        message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_CHARGING ||
        message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED) {
        TransitToState(StandbyState::WORKING);
    }
    if (curStatePtr_->GetCurState() != StandbyState::WORKING) {
        return;
    }
    if (message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF ||
        message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_DISCHARGING) {
        TransitToState(StandbyState::DARK);
    }
}

void StateManagerAdapter::HandleScrOffHalfHour(const StandbyMessage& message)
{
    if (scrOffHalfHourTimerId_ == 0) {
        return;
    }
    if (message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF) {
        isScreenOn_ = false;
        screenOffTimeStamp_ = MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
        MiscServices::TimeServiceClient::GetInstance()->StartTimer(scrOffHalfHourTimerId_,
            screenOffTimeStamp_ + HALF_HOUR);
    } else if (message.action_ == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        isScreenOn_ = true;
        MiscServices::TimeServiceClient::GetInstance()->StopTimer(scrOffHalfHourTimerId_);
    }
}

uint32_t StateManagerAdapter::GetCurState()
{
    return curStatePtr_->GetCurState();
}

uint32_t StateManagerAdapter::GetPreState()
{
    return preStatePtr_->GetCurState();
}

ErrCode StateManagerAdapter::StartEvalCurrentState(const ConstraintEvalParam& params)
{
    STANDBYSERVICE_LOGD("start evalution current state, current: %{public}u, %{public}u, next:"\
        " %{public}u, %{public}u", params.curState_, params.curPhase_, params.nextState_,
        params.nextPhase_);
    isEvalution_ = true;
    constraintManager_->StartEvalution(params);
    return ERR_OK;
}

ErrCode StateManagerAdapter::EndEvalCurrentState(bool evalResult)
{
    if (!isEvalution_) {
        STANDBYSERVICE_LOGE("can not end evalution before start evalution");
        return ERR_STANDBY_STATE_TIMING_SEQ_ERROR;
    }
    STANDBYSERVICE_LOGD("end evalution current state, result is %{public}d", evalResult);
    isEvalution_ = false;
    constraintManager_->StopEvalution();
    curStatePtr_->EndEvalCurrentState(evalResult);
    return ERR_OK;
}

void StateManagerAdapter::BlockCurrentState()
{
    isBlocked_ = true;
    curStatePtr_->OnStateBlocked();
}

void StateManagerAdapter::UnblockCurrentState()
{
    isBlocked_ = false;
}

ErrCode StateManagerAdapter::TransitToState(uint32_t nextState)
{
    uint32_t curState = curStatePtr_->GetCurState();
    if (!CheckTransitionValid(curState, nextState)) {
        return ERR_STANDBY_STATE_TRANSITION_FAILED;
    }
    if (curState == nextState) {
        UnblockCurrentState();
    } else if (nextState == StandbyState::MAINTENANCE ||
        curState == StandbyState::MAINTENANCE) {
        return TransitWithMaint(nextState);
    } else if (curState < nextState) {
        return EnterStandby(nextState);
    } else {
        return ExitStandby(nextState);
    }
    return ERR_OK;
}

ErrCode StateManagerAdapter::ExitStandby(uint32_t nextState)
{
    if (isEvalution_) {
        constraintManager_->StopEvalution();
        isEvalution_ = false;
    }
    UnblockCurrentState();
    if (scrOffHalfHourCtrl_) {
        OnScreenOffHalfHour(false, false);
    }
    TransitToStateInner(nextState);
    return ERR_OK;
}

bool StateManagerAdapter::CheckTransitionValid(uint32_t curState, uint32_t nextState)
{
    bool ret = curStatePtr_->CheckTransitionValid(nextState);
    if (!ret) {
        STANDBYSERVICE_LOGE("can not transitting from now %{public}s to next %{public}s",
            STATE_NAME_LIST[curState].c_str(), STATE_NAME_LIST[nextState].c_str());
    }
    return ret;
}

ErrCode StateManagerAdapter::EnterStandby(uint32_t nextState)
{
    if (isBlocked_) {
        STANDBYSERVICE_LOGE("current standby state is blocked due to the previous failure of constraint");
        return ERR_STANDBY_STATAE_BLOCKED;
    }
    ConstraintEvalParam params{curStatePtr_->GetCurState(), curStatePtr_->GetCurInnerPhase(), nextState, 0};
    StartEvalCurrentState(params);
    return ERR_OK;
}

ErrCode StateManagerAdapter::TransitWithMaint(uint32_t nextState)
{
    return TransitToStateInner(nextState);
}

ErrCode StateManagerAdapter::TransitToStateInner(uint32_t nextState)
{
    curStatePtr_->EndState();
    preStatePtr_ = curStatePtr_;
    curStatePtr_ = indexToState_[nextState];
    curStatePtr_->BeginState();

    SendNotification(preStatePtr_->GetCurState(), true);
    BaseState::ReleaseStandbyRunningLock();
    return ERR_OK;
}

void StateManagerAdapter::StopEvalution()
{
    if (isEvalution_) {
        constraintManager_->StopEvalution();
        isEvalution_ = false;
    }
}

void StateManagerAdapter::OnScreenOffHalfHour(bool scrOffHalfHourCtrl, bool repeated)
{
    handler_->PostSyncTask([this, scrOffHalfHourCtrl, repeated]() {
        OnScreenOffHalfHourInner(scrOffHalfHourCtrl, repeated);
        }, AppExecFwk::EventQueue::Priority::HIGH);
}

void StateManagerAdapter::OnScreenOffHalfHourInner(bool scrOffHalfHourCtrl, bool repeated)
{
    uint32_t curState = curStatePtr_->GetCurState();
    uint32_t preState = preStatePtr_->GetCurState();
    STANDBYSERVICE_LOGD("screen off half hour, cur state is %{public}s, pre state is %{public}s",
        STATE_NAME_LIST[curState].c_str(), STATE_NAME_LIST[preState].c_str());
    if (scrOffHalfHourCtrl && !(curState == StandbyState::SLEEP || (preState == StandbyState::SLEEP &&
        curState == StandbyState::MAINTENANCE))) {
        return;
    }
    if (!repeated && scrOffHalfHourCtrl_ == scrOffHalfHourCtrl) {
        return;
    }
    STANDBYSERVICE_LOGD("half hour ctrl status from %{public}d to %{public}d", scrOffHalfHourCtrl_, scrOffHalfHourCtrl);
    scrOffHalfHourCtrl_ = scrOffHalfHourCtrl;
    StandbyMessage message(StandbyMessageType::SCREEN_OFF_HALF_HOUR);
    message.want_ = AAFwk::Want{};
    message.want_->SetParam(SCR_OFF_HALF_HOUR_STATUS, scrOffHalfHourCtrl_);
    StandbyServiceImpl::GetInstance()->DispatchEvent(message);
}

void StateManagerAdapter::SendNotification(uint32_t preState, bool needDispatchEvent)
{
    uint32_t curState = curStatePtr_->GetCurState();
    STANDBYSERVICE_LOGI("state transit succeed, previous is %{public}s, current is %{public}s",
        STATE_NAME_LIST[preState].c_str(), STATE_NAME_LIST[curState].c_str());
    if (needDispatchEvent) {
        StandbyMessage message(StandbyMessageType::STATE_TRANSIT);
        message.want_ = AAFwk::Want{};
        message.want_->SetParam(PREVIOUS_STATE, static_cast<int32_t>(preState));
        message.want_->SetParam(CURRENT_STATE, static_cast<int32_t>(curState));
        StandbyServiceImpl::GetInstance()->DispatchEvent(message);
    }
    StandbyStateSubscriber::GetInstance()->ReportStandbyState(curState);
}

void StateManagerAdapter::ShellDump(const std::vector<std::string>& argsInStr, std::string& result)
{
    if (argsInStr[0] == DUMP_DETAIL_INFO) {
        DumpShowDetailInfo(argsInStr, result);
    } else if (argsInStr[0] == DUMP_ENTER_STATE) {
        DumpEnterSpecifiedState(argsInStr, result);
    } else if (argsInStr[0] == DUMP_SIMULATE_SENSOR) {
        DumpActivateMotion(argsInStr, result);
    }
    curStatePtr_->ShellDump(argsInStr, result);
}

void StateManagerAdapter::DumpShowDetailInfo(const std::vector<std::string>& argsInStr, std::string& result)
{
    result += "isEvalution: " + std::to_string(isEvalution_) + ", isBlocking: " +
        std::to_string(isBlocked_) + ", current state: " + STATE_NAME_LIST[
        curStatePtr_->GetCurState()] + ", current phase: " + std::to_string(curStatePtr_->
        GetCurInnerPhase()) + ", previous state: " + STATE_NAME_LIST[preStatePtr_->GetCurState()] +
        ", scrOffHalfHourCtrl: " + std::to_string(scrOffHalfHourCtrl_) + "\n";
}

void StateManagerAdapter::DumpEnterSpecifiedState(const std::vector<std::string>& argsInStr, std::string& result)
{
    isBlocked_ = false;
    if (argsInStr[2] == "false") {
        curStatePtr_->StartTransitNextState();
    } else {
        TransitToStateInner(static_cast<uint32_t>(std::atoi(argsInStr[1].c_str())));
    }
}

void StateManagerAdapter::DumpActivateMotion(const std::vector<std::string>& argsInStr, std::string& result)
{
    if (argsInStr[1] == "--motion") {
        curStatePtr_->StartTransitNextState();
        handler_->PostTask([this]() {
            STANDBYSERVICE_LOGD("after 3000ms, stop sensor");
            this->EndEvalCurrentState(false);
            }, MOTION_DETECTION_TIMEOUT);
        result += "finished start periodly sensor\n";
    } else if (argsInStr[1] == "--blocked") {
        BlockCurrentState();
    } else if (argsInStr[1] == "--halfhour") {
        OnScreenOffHalfHourInner(true, true);
    }
}
}  // namespace DevStandbyMgr
}  // namespace OHOS