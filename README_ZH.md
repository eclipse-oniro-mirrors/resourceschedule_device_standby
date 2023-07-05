# 设备待机部件
## 简介
为提高设备续航，降低设备功耗，在设备进入待机空闲状态时，限制后台应用使用资源。开发者可以根据自身情况，为自己的应用申请纳入待机资源管控，或者暂时不被待机资源管控。

## 系统架构

**图 1**  设备待机部件架构图


![](figures/zh-cn_image.png)

## 目录

```
/foundation/resourceschedule/device_standby
├── frameworks       # 接口实现
├── interfaces
│   ├── innerkits    # 对内接口目录
│   └── kits         # 对外接口目录
├── sa_profile       # 组件服务配置
├── services         # 组件服务实现
└── utils            # 组件工具实现
└── plugins          # 插件
└── bundle.json      # 部件描述及编译文件
```

## 说明
[设备待机模块接口api开发文档](https://gitee.com/openharmony/docs/blob/4b89700885537543d26e2dfc96b4dee15a544229/zh-cn/application-dev/reference/apis/js-apis-resourceschedule-deviceStandby.md)

主要接口

| 接口                       | 说明|
|--------------------------|---- |
| isDeviceInStandby        |  查询应用是否处于待机空闲状态，如果处于空闲状态，该应用可能被纳入资源管控。  |
| getExemptedApps          | 获取不会被纳入待机资源管控的应用列表   |
| requestExemptionResource |  申请豁免待机状态解除指定类型资源限制  |
| releaseExemptionResource |  申请去除豁免待机状态解除指定类型资源限制  |

## 开发步骤
需要检查是否已经配置请求相应的权限: ohos.permission.DEVICE_STANDBY_EXEMPTION
<br>
导入依赖

	js import deviceStandby from '@ohos.resourceschedule.deviceStandby';

1.查询应用是否处于待机空闲状态

	// 异步方法promise方式
	try{
    	deviceStandby.isDeviceInStandby().then( res => {
        	console.log('DEVICE_STANDBY isDeviceInStandby promise succeeded, result: ' + JSON.stringify(res));
    	}).catch( err => {
        	console.log('DEVICE_STANDBY isDeviceInStandby promise failed. code is: ' + err.code + ',message is: ' + err.message);
   	    });
	} catch (error) {
    	console.log('DEVICE_STANDBY isDeviceInStandby throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

	// 异步方法callback方式
	try{
    	deviceStandby.isDeviceInStandby((err, res) => {
        	if (err) {
            	console.log('DEVICE_STANDBY isDeviceInStandby callback failed. code is: ' + err.code + ',message is: ' + err.message);
        	} else {
           	 console.log('DEVICE_STANDBY isDeviceInStandby callback succeeded, result: ' + JSON.stringify(res));
        	}
   	 	});
	} catch(error) {
    	console.log('DEVICE_STANDBY isDeviceInStandby throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

2.查询待机场景豁免应用列表(获取不会被纳入待机资源管控的应用列表)

	let resourceTypes = 1
	// 异步方法promise方式
	try{
    	deviceStandby.getExemptedApps(resourceTypes).then( res => {
        	console.log('DEVICE_STANDBY getExemptedApps promise success.');
        	for (let i = 0; i < res.length; i++) {
            	console.log('DEVICE_STANDBY getExemptedApps promise result ' + JSON.stringify(res[i]));
        	}
    	}).catch( err => {
        	console.log('DEVICE_STANDBY getExemptedApps promise failed. code is: ' + err.code + ',message is: ' + err.message);
    	});
	} catch (error) {
    	console.log('DEVICE_STANDBY getExemptedApps throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

	// 异步方法callback方式
	try{
    	deviceStandby.getExemptedApps(resourceTypes, (err, res) => {
       	 	if (err) {
           	 	console.log('DEVICE_STANDBY getExemptedApps callback failed. code is: ' + err.code + ',message is: ' + err.message);
        	} else {
            	console.log('DEVICE_STANDBY getExemptedApps callback success.');
        		for (let i = 0; i < res.length; i++) {
            		console.log('DEVICE_STANDBY getExemptedApps callback result ' + JSON.stringify(res[i]));
        		}
        	}
    	});
	} catch (error) {
    	console.log('DEVICE_STANDBY getExemptedApps throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

3.申请豁免待机状态解除指定类型资源限制

	let resRequest = {
		resourceTypes: 1,
		uid:10003,
		name:"com.example.app",
		duration:10,
		reason:"apply",
	};
	// 异步方法promise方式
	try{
    	deviceStandby.requestExemptionResource(resRequest).then( () => {
        	console.log('DEVICE_STANDBY requestExemptionResource promise succeeded.');
    	}).catch( err => {
        	console.log('DEVICE_STANDBY requestExemptionResource promise failed. code is: ' + err.code + ',message is: ' + err.message);
    	});
	} catch (error) {
    	console.log('DEVICE_STANDBY requestExemptionResource throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

	// 异步方法callback方式
	try{
    	deviceStandby.requestExemptionResource(resRequest, (err) => {
       	 	if (err) {
           	 	console.log('DEVICE_STANDBY requestExemptionResource callback failed. code is: ' + err.code + ',message is: ' + err.message);
        	} else {
            	console.log('DEVICE_STANDBY requestExemptionResource callback succeeded.');
        	}
    	});
	} catch (error) {
    	console.log('DEVICE_STANDBY requestExemptionResource throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

4.申请去除豁免待机状态解除指定类型资源限制

	let resRequest = {
		resourceTypes: 1,
		uid:10003,
		name:"com.demo.app",
		duration:10,
		reason:"unapply",
	};
	// 异步方法promise方式
	try{
    	deviceStandby.releaseExemptionResource(resRequest).then( () => {
        	console.log('DEVICE_STANDBY releaseExemptionResource promise succeeded.');
    	}).catch( err => {
        	console.log('DEVICE_STANDBY releaseExemptionResource promise failed. code is: ' + err.code + ',message is: ' + err.message);
    	});
	} catch (error) {
    	console.log('DEVICE_STANDBY releaseExemptionResource throw error, code is: ' + error.code + ',message is: ' + error.message);
	}

	// 异步方法callback方式
	try{
    	deviceStandby.releaseExemptionResource(resRequest, (err) => {
       	 	if (err) {
           	 	console.log('DEVICE_STANDBY releaseExemptionResource callback failed. code is: ' + err.code + ',message is: ' + err.message);
        	} else {
            	console.log('DEVICE_STANDBY releaseExemptionResource callback succeeded.');
        	}
    	});
	} catch (error) {
    	console.log('DEVICE_STANDBY releaseExemptionResource throw error, code is: ' + error.code + ',message is: ' + error.message);
	}


