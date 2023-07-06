# 设备待机部件
## 简介
为提高设备续航，降低设备功耗，在设备进入待机空闲状态时，系统会限制后台应用使用资源。开发者可以根据自身情况，为自己的应用申请纳入待机资源管控，或者暂时不被待机资源管控。

## 系统架构

**图 1**  设备待机部件整体架构图

![](figures/zh-cn_image.png)

**图 2**  设备待机部件模块图

![](figures/zh-cn_module_image.png)

## 部件说明
本部件主要处理在系统正常工作（Active）与系统待机工作（Active->Standby）的状态转换。

**图 3**  整机状态变化过程

![](figures/zh-cn_state_image.png)

**图 4**  待机状态变化过程

![](figures/zh-cn_standby_state_image.png)

三种典型场景说明：

**图 5**  手机开关机

![](figures/zh-cn_phone_image.png)

**图 6**  PC开关机

![](figures/zh-cn_pc_image.png)

**图 7**  车机开关机

![](figures/zh-cn_car_image.png)

## 开发说明

### 1.目录

```
/foundation/resourceschedule/device_standby
├── frameworks       # 接口实现
├── interfaces
│   ├── innerkits    # 对内接口目录
│   └── kits         # 对外接口目录
├── sa_profile       # 组件服务配置
├── services         # 组件服务实现
└── utils            # 组件工具实现
└── plugins          # 插件（状态监控、决策、转换、执行）
└── bundle.json      # 部件描述及编译文件
```

### 2.部件内子模块职责说明

|子模块名称   |功能职责描述    |
|------------|---------------|
|interface   |1.对外提供inner级别dump维测、豁免、通知接口。 |
|            |2.提供约束接口。 |
|sa_profile  |在服务管理中配置standby服务。|
|services    |1.内部核心服务功能实现。 |
|            |2.通知、查询功能。 |
|plugins     |1.状态监控。 |
|            |2.决策是否限制设备应用资源。| 
|            |3.转换设备状态。  |
|            |4.执行策略。|
|utils       |1.通用工具、日志实现。|
|            |2.相关配置读写。 |
|frameworks  |配置对外接口框架能力。|