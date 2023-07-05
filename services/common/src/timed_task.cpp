/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "timed_task.h"

#include "standby_service_log.h"
#include "common_constant.h"
#include "time_provider.h"

namespace OHOS {
namespace DevStandbyMgr {
TimedTask::TimedTask()
{}

TimedTask::TimedTask(bool repeat, uint64_t interval, bool isExact)
{
    this->repeat = repeat;
    this->interval = interval;
    this->type = TIMER_TYPE_WAKEUP;
    if (isExact) {
        this->type = TIMER_TYPE_WAKEUP + TIMER_TYPE_EXACT;
    }
}

TimedTask::~TimedTask()
{}

void TimedTask::OnTrigger()
{
    STANDBYSERVICE_LOGD("timed task had been triggered");
    if (callBack_ != nullptr) {
        STANDBYSERVICE_LOGD("start invoke callback function of timed task");
        callBack_();
    }
    STANDBYSERVICE_LOGD("end timed task callback");
}

void TimedTask::SetType(const int &type)
{
    this->type = type;
}

void TimedTask::SetRepeat(bool repeat)
{
    this->repeat = repeat;
}

void TimedTask::SetInterval(const uint64_t& interval)
{
    this->interval = interval;
}

void TimedTask::SetWantAgent(std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> wantAgent)
{
    this->wantAgent = wantAgent;
}

void TimedTask::SetCallbackInfo(const std::function<void()>& callBack)
{
    this->callBack_ = callBack;
}

uint64_t TimedTask::CreateTimer(bool repeat, uint64_t interval, bool isExact, const std::function<void()>& callBack)
{
    auto timedTask = std::make_shared<TimedTask>(repeat, interval, isExact);
    timedTask->SetCallbackInfo(callBack);
    return MiscServices::TimeServiceClient::GetInstance()->CreateTimer(timedTask);
}

bool TimedTask::StartTimer(uint64_t& timeId)
{
    int64_t timeDiff {0};
    if (!TimeProvider::TimeDiffToDayNightSwitch(timeDiff)) {
        return false;
    }
    STANDBYSERVICE_LOGD("start next day and night switch after %{public}ld ms", timeDiff);
    auto curTimeStamp = MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
    if (!MiscServices::TimeServiceClient::GetInstance()->StartTimer(timeId, curTimeStamp + timeDiff)) {
        STANDBYSERVICE_LOGE("day and night switch observer start failed");
        return false;
    }
    return true;
}

bool TimedTask::RegisterTimer(uint64_t& timeId, bool repeat, uint64_t interval, const std::function<void()>& callBack)
{
    timeId = CreateTimer(repeat, interval, false, callBack);
    if (timeId == 0) {
        STANDBYSERVICE_LOGE("create timer failed");
        return false;
    }
    return StartTimer(timeId);
}
} // namespace DevStandbyMgr
} // namespace OHOS