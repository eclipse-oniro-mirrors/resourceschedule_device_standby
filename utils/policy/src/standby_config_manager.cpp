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

#include "standby_config_manager.h"

#include <string>
#include <sstream>
#include <functional>
#include <unistd.h>

#ifdef STANDBY_CONFIG_POLICY_ENABLE
#include "config_policy_utils.h"
#endif

#include "json_utils.h"

namespace OHOS {
namespace DevStandbyMgr {
namespace {
    const std::string DEFAULT_CONFIG_ROOT_DIR = "/system";
    const std::string STANDBY_CONFIG_PATH = "/etc/standby_service/device_standby_config.json";
    const std::string STRATEGY_CONFIG_PATH = "/etc/standby_service/standby_strategy_config.json";
    const std::string TAG_PLUGIN_NAME = "plugin_name";
    const std::string TAG_STANDBY = "standby";
    const std::string TAG_MAINTENANCE_LIST = "maintenance_list";
    const std::string TAG_DETECT_LIST = "detect_list";
    const std::string TAG_STRATEGY_SWITCH = "strategy_switch";
    const std::string TAG_STRATEGY_LIST = "strategy_list";
    const std::string TAG_HALFHOUR_SWITCH_SETTING = "halfhour_switch_setting";

    const std::string TAG_CONDITION = "condition";
    const std::string TAG_ACTION = "action";
    const std::string TAG_ALLOW = "allow";
    const std::string TAG_PROCESSES = "processes";
    const std::string TAG_APPS = "apps";
    const std::string TAG_PROCESSES_LIMIT = "processes_limit";
    const std::string TAG_TIME_CLOCK_APPS = "time_clock_apps";
    const std::string TAG_APPS_LIMIT = "apps_limit";
    const std::string TAG_NAME = "name";
    const std::string TAG_MAX_DURATION_LIM = "duration";

    const std::string TAG_TIMER = "TIMER";
    const std::string TAG_TIMER_CLOCK = "timer_clock";
    const std::string TAG_TIMER_PERIOD = "timer_period";

    const char TAG_CONDITION_DELIM = '&';
    const std::string TAG_DAY_STANDBY = "day_standby";
    const std::string TAG_NIGHT_STANDBY = "night_standby";
    const std::string TAG_SCREENOFF = "screenoff";
    const std::string TAG_SCREENOFF_HALFHOUR = "screenoff_halfhour";
    const std::unordered_map<std::string, ConditionType::Type> conditionMap = {
        {TAG_DAY_STANDBY, ConditionType::DAY_STANDBY},
        {TAG_NIGHT_STANDBY, ConditionType::NIGHT_STANDBY},
    };
}

IMPLEMENT_SINGLE_INSTANCE(StandbyConfigManager);

StandbyConfigManager::StandbyConfigManager() {}

StandbyConfigManager::~StandbyConfigManager() {}

ErrCode StandbyConfigManager::Init()
{
    STANDBYSERVICE_LOGI("start to read config");

    std::vector<std::string> configFileList = GetConfigFileList(STANDBY_CONFIG_PATH);
    for (const auto& configFile : configFileList) {
        nlohmann::json devStandbyConfigRoot;
        if (!JsonUtils::LoadJsonValueFromFile(devStandbyConfigRoot, configFile)) {
            STANDBYSERVICE_LOGE("load config file %{public}s failed", configFile.c_str());
            return ERR_STANDBY_CONFIG_FILE_LOAD_FAILED;
        }
        if (!ParseDeviceStanbyConfig(devStandbyConfigRoot)) {
            STANDBYSERVICE_LOGE("parse config file %{public}s failed", configFile.c_str());
            return ERR_STANDBY_CONFIG_FILE_LOAD_FAILED;
        }
    }

    configFileList = GetConfigFileList(STRATEGY_CONFIG_PATH);
    for (const auto& configFile : configFileList) {
        nlohmann::json resCtrlConfigRoot;
        if (!JsonUtils::LoadJsonValueFromFile(resCtrlConfigRoot, configFile)) {
            STANDBYSERVICE_LOGE("load config file %{public}s failed", configFile.c_str());
            return ERR_STANDBY_CONFIG_FILE_PARSE_FAILED;
        }
        if (!ParseResCtrlConfig(resCtrlConfigRoot)) {
            STANDBYSERVICE_LOGE("parse config file %{public}s failed", configFile.c_str());
            return ERR_STANDBY_CONFIG_FILE_PARSE_FAILED;
        }
    }
    return ERR_OK;
}

std::vector<std::string> StandbyConfigManager::GetConfigFileList(const std::string& relativeConfigPath)
{
    std::list<std::string> rootDirList;
    #ifdef STANDBY_CONFIG_POLICY_ENABLE
        auto cfgDirList = GetCfgDirList();
        if (cfgDirList != nullptr) {
            for (const auto &cfgDir : cfgDirList->paths) {
                if (cfgDir == nullptr) {
                    continue;
                }
                STANDBYSERVICE_LOGD("cfgDir: %{public}s ", cfgDir);
                rootDirList.emplace_back(cfgDir);
            }
            FreeCfgDirList(cfgDirList);
        }
    #endif
    if (std::find(rootDirList.begin(), rootDirList.end(), DEFAULT_CONFIG_ROOT_DIR)
        == rootDirList.end()) {
        rootDirList.emplace_front(DEFAULT_CONFIG_ROOT_DIR);
    }
    std::string baseRealPath;
    std::vector<std::string> configFilesList;
    for (auto configDir : rootDirList) {
        if (JsonUtils::GetRealPath(configDir + relativeConfigPath, baseRealPath)
            && access(baseRealPath.c_str(), F_OK) == ERR_OK) {
            STANDBYSERVICE_LOGD("Get valid base config file: %{public}s", baseRealPath.c_str());
            configFilesList.emplace_back(baseRealPath);
        }
    }
    return configFilesList;
}

const std::string& StandbyConfigManager::GetPluginName()
{
    return pluginName_;
}

bool StandbyConfigManager::GetStandbySwitch(const std::string& switchName)
{
    return GetConfigWithName(switchName, standbySwitchMap_);
}

int32_t StandbyConfigManager::GetStandbyParam(const std::string& paramName)
{
    return GetConfigWithName(paramName, standbyParaMap_);
}

bool StandbyConfigManager::GetStrategySwitch(const std::string& switchName)
{
    return GetConfigWithName(switchName, strategySwitchMap_);
}

bool StandbyConfigManager::GetHalfHourSwitch(const std::string& switchName)
{
    return GetConfigWithName(switchName, halfhourSwitchMap_);
}

std::shared_ptr<std::vector<DefaultResourceConfig>>
    StandbyConfigManager::GetResCtrlConfig(const std::string& switchName)
{
    return GetConfigWithName(switchName, defaultResourceConfigMap_);
}

template<typename T>
T StandbyConfigManager::GetConfigWithName(const std::string& switchName,
    std::unordered_map<std::string, T>& configMap)
{
    std::lock_guard<std::mutex> lock(configMutex_);
    auto iter = configMap.find(switchName);
    if (iter == configMap.end()) {
        STANDBYSERVICE_LOGW("failed to find config %{public}s", switchName.c_str());
        return T{};
    }
    return iter->second;
}

const std::vector<TimerResourceConfig>& StandbyConfigManager::GetTimerResConfig()
{
    return timerResConfigList_;
}

const std::vector<std::string>& StandbyConfigManager::GetStrategyConfigList()
{
    return strategyList_;
}

std::vector<int32_t> StandbyConfigManager::GetStandbyDurationList(const std::string& switchName)
{
    return GetConfigWithName(switchName, intervalListMap_);
}

int32_t StandbyConfigManager::GetMaxDuration(const std::string& name, const std::string& paramName,
    uint32_t condition, bool isApp)
{
    auto eligibleAllowTimeList = GetEligibleAllowTimeConfig(paramName, condition, true, isApp);
    auto findConfigTask = [&name](const auto& it) { return it.name_ == name; };
    auto it = std::find_if(eligibleAllowTimeList.begin(), eligibleAllowTimeList.end(), findConfigTask);
    if (it == eligibleAllowTimeList.end()) {
        return 0;
    } else {
        return it->maxDurationLim_;
    }
}

template<typename T> std::vector<T> StandbyConfigManager::GetEligibleAllowConfig(const std::string& paramName,
    uint32_t condition, bool isAllow, bool isApp, const std::function<void(bool, std::vector<T>&,
    const DefaultResourceConfig&)>& func)
{
    if (defaultResourceConfigMap_.find(paramName) == defaultResourceConfigMap_.end()) {
        return {};
    }
    std::vector<T> eligibleResCtrlConfig;
    const auto& resCtrlConfig = *(defaultResourceConfigMap_.find(paramName)->second);
    STANDBYSERVICE_LOGD("find duration from %{public}s, size is %{public}lu", paramName.c_str(), resCtrlConfig.size());
    for (const auto& config : resCtrlConfig) {
        if (config.isAllow_ != isAllow) {
            continue;
        }
        bool isEligiable {false};
        for (const auto configCondition : config.conditions_) {
            if ((condition & configCondition) == configCondition) {
                isEligiable = true;
                break;
            }
        }
        if (!isEligiable) {
            continue;
        }
        func(isApp, eligibleResCtrlConfig, config);
    }
    STANDBYSERVICE_LOGD("eligibleResCtrlConfig size is %{public}lu", eligibleResCtrlConfig.size());
    return eligibleResCtrlConfig;
}

std::vector<TimeLtdProcess> StandbyConfigManager::GetEligibleAllowTimeConfig(const std::string& paramName,
    uint32_t condition, bool isAllow, bool isApp)
{
    auto func = [](bool isApp, std::vector<TimeLtdProcess>& eligibleResCtrlConfig,
        const DefaultResourceConfig& config) {
        if (isApp) {
            eligibleResCtrlConfig.insert(eligibleResCtrlConfig.end(),
                config.timeLtdApps_.begin(), config.timeLtdApps_.end());
        } else {
            eligibleResCtrlConfig.insert(eligibleResCtrlConfig.end(),
                config.timeLtdProcesses_.begin(), config.timeLtdProcesses_.end());
        }
        STANDBYSERVICE_LOGD("after calculate, eligible size is %{public}lu", eligibleResCtrlConfig.size());
    };
    return GetEligibleAllowConfig<TimeLtdProcess>(paramName, condition, isAllow, isApp, func);
}

std::vector<std::string> StandbyConfigManager::GetEligiblePersistAllowConfig(const std::string& paramName,
    uint32_t condition, bool isAllow, bool isApp)
{
    auto func = [](bool isApp, std::vector<std::string>& eligibleResCtrlConfig,
        const DefaultResourceConfig& config) {
        if (isApp) {
            eligibleResCtrlConfig.insert(eligibleResCtrlConfig.end(),
                config.apps_.begin(), config.apps_.end());
        } else {
            eligibleResCtrlConfig.insert(eligibleResCtrlConfig.end(),
                config.processes_.begin(), config.processes_.end());
        }
    };
    return GetEligibleAllowConfig<std::string>(paramName, condition, isAllow, isApp, func);
}

bool StandbyConfigManager::ParseDeviceStanbyConfig(const nlohmann::json& devStandbyConfigRoot)
{
    nlohmann::json standbyConfig;
    nlohmann::json detectlist;
    nlohmann::json standbySwitchConfig;
    nlohmann::json standbyListConfig;
    nlohmann::json standbyIntervalList;

    JsonUtils::GetStringFromJsonValue(devStandbyConfigRoot, TAG_PLUGIN_NAME, pluginName_);
    if (JsonUtils::GetObjFromJsonValue(devStandbyConfigRoot, TAG_STANDBY, standbyConfig) &&
        !ParseStandbyConfig(standbyConfig)) {
        STANDBYSERVICE_LOGW("failed to parse standby config in %{public}s", STANDBY_CONFIG_PATH.c_str());
        return false;
    }
    if (JsonUtils::GetObjFromJsonValue(devStandbyConfigRoot, TAG_DETECT_LIST, detectlist) &&
        !ParseStandbyConfig(detectlist)) {
        STANDBYSERVICE_LOGW("failed to parse detect list in %{public}s", STANDBY_CONFIG_PATH.c_str());
        return false;
    }
    if (JsonUtils::GetObjFromJsonValue(devStandbyConfigRoot, TAG_MAINTENANCE_LIST, standbyIntervalList) &&
        !ParseIntervalList(standbyIntervalList)) {
        STANDBYSERVICE_LOGW("failed to parse standby interval list in %{public}s", STANDBY_CONFIG_PATH.c_str());
        return false;
    }
    if (JsonUtils::GetObjFromJsonValue(devStandbyConfigRoot, TAG_STRATEGY_SWITCH, standbySwitchConfig) &&
        !ParseStandbySwitchConfig(standbySwitchConfig)) {
        STANDBYSERVICE_LOGW("failed to parse standby switch config in %{public}s", STANDBY_CONFIG_PATH.c_str());
        return false;
    }
    if (JsonUtils::GetArrayFromJsonValue(devStandbyConfigRoot, TAG_STRATEGY_LIST, standbyListConfig) &&
        !ParseStrategyListConfig(standbyListConfig)) {
        STANDBYSERVICE_LOGW("failed to parse strategy list config in %{public}s", STANDBY_CONFIG_PATH.c_str());
        return false;
    }

    if (JsonUtils::GetObjFromJsonValue(devStandbyConfigRoot, TAG_HALFHOUR_SWITCH_SETTING, standbyConfig)) {
        if (!ParseHalfHourSwitchConfig(standbyConfig)) {
            STANDBYSERVICE_LOGW("failed to parse halfhour config");
            return false;
        }
    }
    return true;
}

bool StandbyConfigManager::ParseStandbyConfig(const nlohmann::json& standbyConfig)
{
    for (const auto& element : standbyConfig.items()) {
        if (!element.value().is_primitive()) {
            STANDBYSERVICE_LOGE("there is unexpected type of key in standby config");
            return false;
        }
        if (element.value().is_boolean()) {
            standbySwitchMap_.erase(element.key());
            standbySwitchMap_.emplace(element.key(), element.value().get<bool>());
        } else if (element.value().is_number_integer()) {
            standbyParaMap_.erase(element.key());
            if (element.value().get<int32_t>() < 0) {
                STANDBYSERVICE_LOGE("there is negative value in standby config");
                return false;
            }
            standbyParaMap_.emplace(element.key(), element.value().get<int32_t>());
        }
    }
    return true;
}

bool StandbyConfigManager::ParseIntervalList(const nlohmann::json& standbyIntervalList)
{
    for (const auto& element : standbyIntervalList.items()) {
        if (!element.value().is_array()) {
            STANDBYSERVICE_LOGE("there is unexpected value of %{public}s in standby interval list",
                element.key().c_str());
            return false;
        }
        std::vector<int32_t> intervalList;
        for (const int32_t interval : element.value()) {
            intervalList.emplace_back(interval);
        }
        intervalListMap_.emplace(element.key(), std::move(intervalList));
    }
    return true;
}

bool StandbyConfigManager::ParseStandbySwitchConfig(const nlohmann::json& standbySwitchConfig)
{
    for (const auto& element : standbySwitchConfig.items()) {
        if (!element.value()) {
            STANDBYSERVICE_LOGE("there is unexpected type of value in standby switch config");
            return false;
        }
        strategySwitchMap_.erase(element.key());
        strategySwitchMap_.emplace(element.key(), element.value().get<bool>());
    }
    return true;
}

bool StandbyConfigManager::ParseStrategyListConfig(const nlohmann::json& standbyListConfig)
{
    if (!standbyListConfig.is_array()) {
        STANDBYSERVICE_LOGE("there is error in strategy list config");
        return false;
    }
    strategyList_.clear();
    for (const auto& element : standbyListConfig) {
        strategyList_.emplace_back(element.get<std::string>());
    }
    return true;
}

bool StandbyConfigManager::ParseHalfHourSwitchConfig(const nlohmann::json& halfHourSwitchConfig)
{
    for (const auto& element : halfHourSwitchConfig.items()) {
        if (!element.value().is_boolean()) {
            STANDBYSERVICE_LOGE("there is unexpected type of value in half hour standby switch config");
            return false;
        }
        halfhourSwitchMap_.erase(element.key());
        halfhourSwitchMap_.emplace(element.key(), element.value().get<bool>());
    }
    return true;
}

bool StandbyConfigManager::ParseResCtrlConfig(const nlohmann::json& resCtrlConfigRoot)
{
    for (const auto& element : resCtrlConfigRoot.items()) {
        if (!element.value().is_array()) {
            STANDBYSERVICE_LOGE("there is unexpected type of value in resource control config");
            return false;
        }
        std::string resCtrlKey = element.key();
        if (!ParseDefaultResCtrlConfig(resCtrlKey, element.value())) {
            STANDBYSERVICE_LOGE("there is error in config of %{public}s", resCtrlKey.c_str());
            return false;
        }
        if (resCtrlKey == TAG_TIMER && !ParseTimerResCtrlConfig(element.value())) {
            STANDBYSERVICE_LOGE("there is error in config of %{public}s", resCtrlKey.c_str());
            return false;
        }
    }
    return true;
}

bool StandbyConfigManager::ParseTimerResCtrlConfig(const nlohmann::json& resConfigArray)
{
    if (!resConfigArray.is_array()) {
        STANDBYSERVICE_LOGE("the value of timer config should be an array");
        return false;
    }
    timerResConfigList_.clear();
    for (const auto &singleConfigItem : resConfigArray) {
        TimerResourceConfig timerResourceConfig;
        if (!singleConfigItem.contains(TAG_TIME_CLOCK_APPS)) {
            timerResConfigList_.emplace_back(std::move(timerResourceConfig));
            continue;
        }
        const nlohmann::json& limitedAppItems = singleConfigItem.at(TAG_TIME_CLOCK_APPS);
        for (const auto &singleLtdAppItem : limitedAppItems) {
            TimerClockApp timerClockApp;
            if (!JsonUtils::GetStringFromJsonValue(singleLtdAppItem, TAG_NAME, timerClockApp.name_) ||
                (!JsonUtils::GetBoolFromJsonValue(singleLtdAppItem, TAG_TIMER_CLOCK, timerClockApp.isTimerClock_) &&
                !JsonUtils::GetInt32FromJsonValue(singleLtdAppItem, TAG_TIMER_PERIOD, timerClockApp.timerPeriod_))) {
                STANDBYSERVICE_LOGE("there is error in timer clock config");
                return false;
            }
            timerResourceConfig.timerClockApps_.emplace_back(std::move(timerClockApp));
        }
        timerResConfigList_.emplace_back(std::move(timerResourceConfig));
    }
    STANDBYSERVICE_LOGI("succeed to parse the config of TIMER");
    return true;
}

bool StandbyConfigManager::ParseDefaultResCtrlConfig(const std::string& resCtrlKey,
    const nlohmann::json& resConfigArray)
{
    if (!resConfigArray.is_array()) {
        STANDBYSERVICE_LOGE("the value of %{public}s should be an array", resCtrlKey.c_str());
        return false;
    }
    defaultResourceConfigMap_.erase(resCtrlKey);
    auto defaultResConfigPtr = std::make_shared<std::vector<DefaultResourceConfig>>();
    for (const auto &singleConfigItem : resConfigArray) {
        DefaultResourceConfig defaultResourceConfig;
        if (!ParseCommonResCtrlConfig(singleConfigItem, defaultResourceConfig)) {
            STANDBYSERVICE_LOGE("the value of %{public}s can not be parsed", resCtrlKey.c_str());
            return false;
        }
        if (!singleConfigItem.contains(TAG_APPS_LIMIT)) {
            defaultResConfigPtr->emplace_back(std::move(defaultResourceConfig));
            continue;
        }
        const nlohmann::json& limitedAppItems = singleConfigItem.at(TAG_APPS_LIMIT);
        for (const auto &singleLtdAppItem : limitedAppItems) {
            TimeLtdProcess timeLtdApps;
            if (!JsonUtils::GetStringFromJsonValue(singleLtdAppItem, TAG_NAME, timeLtdApps.name_) ||
                !JsonUtils::GetInt32FromJsonValue(singleLtdAppItem, TAG_MAX_DURATION_LIM,
                timeLtdApps.maxDurationLim_)) {
                STANDBYSERVICE_LOGE("there is error in %{public}s config", resCtrlKey.c_str());
                return false;
            }
            defaultResourceConfig.timeLtdApps_.emplace_back(std::move(timeLtdApps));
        }
        defaultResConfigPtr->emplace_back(std::move(defaultResourceConfig));
    }
    defaultResourceConfigMap_.emplace(resCtrlKey, defaultResConfigPtr);
    STANDBYSERVICE_LOGI("succeed to parse the config of %{public}s", resCtrlKey.c_str());
    return true;
}

template<typename T>
bool StandbyConfigManager::ParseCommonResCtrlConfig(const nlohmann::json& singleConfigItem, T& resCtrlConfig)
{
    if (!singleConfigItem.contains(TAG_ACTION) || !singleConfigItem.contains(TAG_CONDITION)) {
        STANDBYSERVICE_LOGE("there is no necessary field %{public}s or %{public}s",
            TAG_ACTION.c_str(), TAG_CONDITION.c_str());
        return false;
    }
    resCtrlConfig.isAllow_ = false;
    std::string resCtrlAction;
    if (!JsonUtils::GetStringFromJsonValue(singleConfigItem, TAG_ACTION, resCtrlAction)) {
        STANDBYSERVICE_LOGE("get action config failed");
    } else if (resCtrlAction == TAG_ALLOW) {
        resCtrlConfig.isAllow_ = true;
    }
    const nlohmann::json& conditionItems = singleConfigItem.at(TAG_CONDITION);
    for (const auto &singleConditionItem : conditionItems) {
        uint32_t conditionValue = ParseCondition(singleConditionItem.get<std::string>());
        if (conditionValue > 0) {
            resCtrlConfig.conditions_.emplace_back(conditionValue);
        }
    }
    if (singleConfigItem.contains(TAG_PROCESSES)) {
        const nlohmann::json& precessItems = singleConfigItem.at(TAG_PROCESSES);
        for (const auto &singleProcessItem : precessItems) {
            resCtrlConfig.processes_.emplace_back(singleProcessItem.get<std::string>());
        }
    }
    if (singleConfigItem.contains(TAG_APPS)) {
        const nlohmann::json& appItems = singleConfigItem.at(TAG_APPS);
        for (const auto &singleAppItem : appItems) {
            resCtrlConfig.apps_.emplace_back(singleAppItem.get<std::string>());
        }
    }

    if (singleConfigItem.contains(TAG_PROCESSES_LIMIT)) {
        const nlohmann::json& limitedProcessItems = singleConfigItem.at(TAG_PROCESSES_LIMIT);
        for (const auto &singleLtdProcessItem : limitedProcessItems) {
            std::string name {};
            int32_t duration {0};
            if (!JsonUtils::GetStringFromJsonValue(singleLtdProcessItem, TAG_NAME, name) ||
                !JsonUtils::GetInt32FromJsonValue(singleLtdProcessItem, TAG_MAX_DURATION_LIM, duration)) {
                STANDBYSERVICE_LOGE("there is error in duration config");
                continue;
            }
            resCtrlConfig.timeLtdProcesses_.emplace_back(TimeLtdProcess{name, duration});
        }
    }
    return true;
}

uint32_t StandbyConfigManager::ParseCondition(const std::string& conditionStr)
{
    uint32_t conditionValue = 0;
    std::stringstream ss(conditionStr);
    std::string conditionSubstr;
    while (std::getline(ss, conditionSubstr, TAG_CONDITION_DELIM)) {
        auto iter = conditionMap.find(conditionSubstr);
        if (iter == conditionMap.end()) {
            continue;
        }
        conditionValue |= iter->second;
    }
    return conditionValue;
}
}  // namespace DevStandbyMgr
}  // namespace OHOS