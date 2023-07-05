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


#include "listener_manager_adapter.h"

#include "common_event_manager.h"
#include "common_event_support.h"

#include "standby_service_log.h"
#include "common_event_listener.h"
#include "device_standby_switch.h"
#include "standby_service_impl.h"


namespace OHOS {
namespace DevStandbyMgr {

bool ListenerManagerAdapter::Init()
{
    EventFwk::MatchingSkills matchingSkills;
    STANDBYSERVICE_LOGD("device type ro.build.characteristics is: %{public}s",
        system::GetParameter("ro.build.characteristics", "unknown").c_str());
    switch (DEVICE_TYPE) {
        case DeviceType::PHONE:
        case DeviceType::UNKNOWN:
            matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
            matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
            matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_CHARGING);
            matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED);
            matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_DISCHARGING);
            matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED);
            break;
        default:
            STANDBYSERVICE_LOGD("listener manager plugin initialization failed");
            return false;
    }
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    messageListenerList_.emplace_back(std::make_shared<CommonEventListener>(subscriberInfo));
    STANDBYSERVICE_LOGI("listener manager plugin initialization succeed");
    return true;
}

bool ListenerManagerAdapter::UnInit()
{
    StopListener();
    for (auto& listener : messageListenerList_) {
        listener->StopListener();
    }
    return true;
}

ErrCode ListenerManagerAdapter::StartListener()
{
    for (auto& listener : messageListenerList_) {
        ErrCode ret = listener->StartListener();
        if (ret != ERR_OK) {
            return ret;
        }
    }
    return ERR_OK;
}

ErrCode ListenerManagerAdapter::StopListener()
{
    for (auto& listener : messageListenerList_) {
        ErrCode ret = listener->StopListener();
        if (ret != ERR_OK) {
            return ret;
        }
    }
    return ERR_OK;
}

void ListenerManagerAdapter::ShellDump(const std::vector<std::string>& argsInStr, std::string& result)
{}
}  // namespace DevStandbyMgr
}  // namespace OHOS
