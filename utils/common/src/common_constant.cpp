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
#include "common_constant.h"

namespace OHOS {
namespace DevStandbyMgr {

const std::string TRANSIT_NEXT_STATE_TIMED_TASK = "TransitNextStateTimedTask";
const std::string TRANSIT_NEXT_STATE_CONDITION_TASK = "TransitNextStateConditionTask";
const std::string TRANSIT_NEXT_PHASE_INSTANT_TASK = "TransitNextPhaseInstantTask";
const std::string REPEATED_MOTION_DETECTION_TASK = "MotionDetectionPeriodlyTask";

const std::string DEFAULT_PLUGIN_NAME = "libstandby_plugin.z.so";
const int32_t REPEATED_MOTION_DETECTION_INTERVAL = 10 * 60 * 1000;
const std::string DARK_TIMEOUT = "dark_timeout";
const std::string NAP_TIMEOUT = "nap_timeout";
const std::string NAP_MAINTENANCE_TIMEOUT = "nap_maintenance_timeout";
const std::string SLEEP_MAINT_TIMEOUT = "sleep_maintenance_timeout";

const std::string NAP_MAINT_DURATION = "nap_interval";
const std::string SLEEP_MAINT_DURATOIN = "sleep_interval";
const std::vector<std::string> STATE_NAME_LIST = {"working", "dark", "nap", "maintenance", "sleep"};

const std::string DETECT_MOTION_CONFIG = "detect_motion";
const std::string MOTION_THREADSHOLD = "motion_threshold";
const std::string MOTION_DECTION_TASK = "motion_dection_task";

const std::string NAP_SWITCH = "nap_switch";
const std::string SLEEP_SWITCH = "sleep_switch";

const int32_t MOTION_DETECTION_TIMEOUT = 3000;
const int32_t REST_TIMEOUT = 5000;
const int32_t TOTAL_TIMEOUT = 30000;

const int32_t PERIODLY_TASK_DECTION_TIMEOUT = 210;
const int32_t PERIODLY_TASK_REST_TIMEOUT = 10;
const int32_t PERIODLY_TASK_TOTAL_TIMEOUT = 220;

const int64_t SENSOR_SAMPLING_RATE = 200000000LL; // 200ms
const int64_t SENSOR_REPORTING_RATE = 0LL;
const int64_t HALF_HOUR = 1800 * 1000;

const std::string PREVIOUS_STATE = "previous_state";
const std::string CURRENT_STATE = "current_state";
const std::string PREVIOUS_PHASE = "previous_phase";
const std::string CURRENT_PHASE = "current_phase";
const std::string SENSOR_DETECTION = "sensor_detection";

const std::string RES_CTRL_CONDITION = "res_ctrl_condition";
const std::string SCR_OFF_HALF_HOUR_STATUS = "scr_off_half_hour_status";
const uint32_t NO_DUMP_PARAM_NUMS = 0;
const uint32_t DUMP_DETAILED_INFO_MAX_NUMS = 2;
const uint32_t DUMP_SLEEP_SWITCH_PARAM_NUMS = 3;
const uint32_t DUMP_SLEEP_ENTER_STATE_NUMS = 3;
const uint32_t DUMP_SLEEP_ALLOW_LIST_NUMS = 4;
const uint32_t DUMP_SLEEP_APPLY_ALLOW_LIST_NUMS = 7;
const uint32_t DUMP_SWITCH_PARAM_NUMS = 3;
const uint32_t DUMP_STATE_TIMEOUT_PARAM_NUMS = 3;

const std::string DUMP_ON = "on";
const std::string DUMP_OFF = "off";
const std::string DUMP_SLEEP_MODE = "sleep";
const std::string DUMP_DAYTIME_SLEEP_MODE = "daytimesleep";
const std::string DUMP_NIGHTTIME_SLEEP_MODE = "nighttimesleep";
const std::string DUMP_DEBUG_SWITCH = "debug";
const std::string DUMP_RESET_STATE = "--reset_state";
const std::string DUMP_DETAIL_CONFIG = "--config";

const std::string DUMP_DETAIL_INFO = "-D";
const std::string DUMP_ENTER_STATE = "-E";
const std::string DUMP_APPLY_ALLOW_RECORD = "-A";
const std::string DUMP_SIMULATE_SENSOR = "-S";
const std::string DUMP_SUBSCRIBER_OBSERVER = "-O";
const std::string DUMP_TURN_ON_OFF_SWITCH  = "-T";
const std::string DUMP_CHANGE_STATE_TIMEOUT  = "-C";
}  // namespace DevStandbyMgr
}  // namespace OHOS
