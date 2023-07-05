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

#include "motion_sensor_monitor.h"
#include <vector>
#include <string>
#include "standby_config_manager.h"
#include "standby_service_impl.h"
#include "istate_manager_adapter.h"

namespace OHOS {
namespace DevStandbyMgr {
namespace {
    const std::vector<int32_t> SENSOR_TYPE_CONFIG = {SENSOR_TYPE_ID_ACCELEROMETER, SENSOR_TYPE_ID_SIGNIFICANT_MOTION};
    const int32_t COUNT_TIMES = 15;
}

double MotionSensorMonitor::energy_ = 0;
bool MotionSensorMonitor::isPrevDataEmpty = false;
AccelData MotionSensorMonitor::previousAccelData {0, 0, 0};
AccelData MotionSensorMonitor::currentAccelData {0, 0, 0};

MotionSensorMonitor::MotionSensorMonitor(int32_t detectionTimeOut, int32_t restTimeOut, int32_t totalTimeOut,
    const ConstraintEvalParam& params): detectionTimeOut_(detectionTimeOut), restTimeOut_(restTimeOut),
    totalTimeOut_(totalTimeOut), params_(params)
{
    handler_ = StandbyServiceImpl::GetInstance()->GetHandler();
}

bool MotionSensorMonitor::CheckSersorConfig(SensorInfo *sensorInfo, int32_t count, int32_t sensorTypeId)
{
    SensorInfo *pt = sensorInfo + count;
    for (SensorInfo *ps = sensorInfo; ps < pt; ++ps) {
        if (sensorInfo->sensorTypeId == sensorTypeId) {
            return true;
        }
    }
    return true;
}

void MotionSensorMonitor::AcceleromterCallback(SensorEvent *event)
{
    if (event == NULL) {
        return;
    }
    AccelData* sensorData = (AccelData*)event->data;
    MotionSensorMonitor::AddEnergy(sensorData);
    STANDBYSERVICE_LOGD("sensor motion: %{public}lf, threshold: %{public}d", MotionSensorMonitor::GetEnergy(),
        StandbyConfigManager::GetInstance()->GetStandbyParam(MOTION_THREADSHOLD));
    if (MotionSensorMonitor::GetEnergy() > StandbyConfigManager::GetInstance()->
            GetStandbyParam(MOTION_THREADSHOLD)) {
        StandbyServiceImpl::GetInstance()->GetHandler()->PostTask([]() {
            StandbyServiceImpl::GetInstance()->GetStateManager()->EndEvalCurrentState(false);
            }, MOTION_DECTION_TASK);
    }
}

void MotionSensorMonitor::RepeatAcceleromterCallback(SensorEvent *event)
{
    if (event == NULL) {
        return;
    }
    STANDBYSERVICE_LOGD("periodly receive Acceleromter motion sensor callback");
    AccelData* sensorData = (AccelData*)event->data;
    MotionSensorMonitor::AddEnergy(sensorData);
    STANDBYSERVICE_LOGD("sensor motion: %{public}lf, threshold: %{public}lf", MotionSensorMonitor::GetEnergy(),
        StandbyConfigManager::GetInstance()->GetStandbyParam(MOTION_THREADSHOLD) * 1.0 / COUNT_TIMES);
    if (MotionSensorMonitor::GetEnergy() > StandbyConfigManager::GetInstance()->
            GetStandbyParam(MOTION_THREADSHOLD) * 1.0 / COUNT_TIMES) {
        StandbyServiceImpl::GetInstance()->GetHandler()->PostTask([]() {
            StandbyServiceImpl::GetInstance()->GetStateManager()->EndEvalCurrentState(false);
            }, MOTION_DECTION_TASK);
    }
}

void MotionSensorMonitor::MotionSensorCallback(SensorEvent *event)
{
    StandbyServiceImpl::GetInstance()->GetHandler()->PostTask([]() {
        StandbyServiceImpl::GetInstance()->GetStateManager()->EndEvalCurrentState(false);
        }, MOTION_DECTION_TASK);
}

double MotionSensorMonitor::GetEnergy()
{
    return energy_;
}

void MotionSensorMonitor::SetEnergy(double energy)
{
    energy_ = energy;
}

void MotionSensorMonitor::AddEnergy(AccelData *accelData)
{
    currentAccelData = *accelData;
    if (!isPrevDataEmpty) {
        isPrevDataEmpty = true;
        previousAccelData = currentAccelData;
    }
    AccelData diff {currentAccelData.x - previousAccelData.x, currentAccelData.y - previousAccelData.y,
        currentAccelData.z - previousAccelData.z};
    energy_ += (diff.x *  diff.x) + (diff.y *  diff.y) + (diff.z *  diff.z);
}

bool MotionSensorMonitor::Init()
{
    int32_t count = -1;
    SensorInfo* sensorInfo = nullptr;
    int32_t ret = GetAllSensors(&sensorInfo, &count);
    if (ret != 0) {
        STANDBYSERVICE_LOGE("get all sensors failed, sensors are not available");
        return false;
    }
    for (const auto iter : SENSOR_TYPE_CONFIG) {
        if (!CheckSersorConfig(sensorInfo, count, iter)) {
            return false;
        }
    }
    if (params_.isRepeatedDetection_) {
        acceSensorUser_.callback = &RepeatAcceleromterCallback;
    } else {
        acceSensorUser_.callback = &AcceleromterCallback;
    }
    motionSensorUser_.callback = &MotionSensorCallback;
    auto &constraintManager = StandbyServiceImpl::GetInstance()->GetConstraintManager();
    constraintManager->RegisterConstraintCallback(params_, shared_from_this());
    return true;
}

void MotionSensorMonitor::StartMonitoring()
{
    STANDBYSERVICE_LOGD("start motion sensor monitoring");
    handler_->PostTask([this]() {
        STANDBYSERVICE_LOGI("stop motion sensor monitoring");
        StandbyServiceImpl::GetInstance()->GetStateManager()->EndEvalCurrentState(true);
        }, MOTION_DECTION_TASK, totalTimeOut_);
    PeriodlyStartMotionDetection();
}

void MotionSensorMonitor::StopMotionDetection()
{
    handler_->PostTask([this]() {
        StopMonitoringInner();
        }, MOTION_DECTION_TASK, detectionTimeOut_);
}

void MotionSensorMonitor::PeriodlyStartMotionDetection()
{
    if (StartMonitoringInner() != ERR_OK) {
        StandbyServiceImpl::GetInstance()->GetStateManager()->EndEvalCurrentState(false);
        return;
    }
    StopMotionDetection();
    handler_->PostTask([this]() {
        PeriodlyStartMotionDetection();
        }, MOTION_DECTION_TASK, detectionTimeOut_ + restTimeOut_);
}

ErrCode MotionSensorMonitor::StartMonitoringInner()
{
    energy_ = 0;
    isMonitoring = true;
    if (StartSensor(SENSOR_TYPE_ID_ACCELEROMETER, &acceSensorUser_) == ERR_OK &&
        StartSensor(SENSOR_TYPE_ID_SIGNIFICANT_MOTION, &motionSensorUser_) == ERR_OK) {
        return ERR_OK;
    }
    return ERR_STANDBY_START_SENSOR_FAILED;
}

void MotionSensorMonitor::StopMonitoring()
{
    handler_->RemoveTask(MOTION_DECTION_TASK);
    StopMonitoringInner();
}

void MotionSensorMonitor::StopMonitoringInner()
{
    StopSensor(SENSOR_TYPE_ID_ACCELEROMETER, &acceSensorUser_);
    StopSensor(SENSOR_TYPE_ID_SIGNIFICANT_MOTION, &motionSensorUser_);
    isMonitoring = false;
}

ErrCode MotionSensorMonitor::StartSensor(int32_t sensorTypeId, SensorUser* sensorUser)
{
    if (SubscribeSensor(sensorTypeId, sensorUser) != 0) {
        STANDBYSERVICE_LOGE("subscribe sensor failed for sensor ID %{public}d", sensorTypeId);
        return ERR_STANDBY_START_SENSOR_FAILED;
    }
    STANDBYSERVICE_LOGD("subscribe sensor succeed for sensor ID %{public}d", sensorTypeId);
    SetBatch(sensorTypeId, sensorUser, SENSOR_SAMPLING_RATE, SENSOR_REPORTING_RATE);
    if (ActivateSensor(sensorTypeId, sensorUser) != 0) {
        STANDBYSERVICE_LOGE("activate sensor failed for sensor ID %{public}d", sensorTypeId);
        return ERR_STANDBY_START_SENSOR_FAILED;
    }
    return ERR_OK;
}

void MotionSensorMonitor::StopSensor(int32_t sensorTypeId, SensorUser* sensorUser)
{
    if (!isMonitoring) {
        return;
    }
    if (DeactivateSensor(sensorTypeId, sensorUser) != 0) {
        STANDBYSERVICE_LOGE("deactivate sensor failed for sensor ID %{public}d", sensorTypeId);
    }
    if (UnsubscribeSensor(sensorTypeId, sensorUser) != 0) {
        STANDBYSERVICE_LOGE("unsubscribe sensor failed for sensor ID %{public}d", sensorTypeId);
    }
}
} // DevStandbyMgr
} // OHOS