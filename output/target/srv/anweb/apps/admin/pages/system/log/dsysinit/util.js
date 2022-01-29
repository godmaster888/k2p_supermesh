"use strict";!function(){angular.module("app").service("sysLogUtil",["funcs","device","cpe","devinfo",function(funcs,device,cpe,devinfo){function apply(config){var config=converterSyslog.nativeToDsysinit(config),changes=funcs.getChangesExpanded(__initConfig,config);if(_.isEmpty(changes))return Promise.resolve();var output=funcs.splitTree(changes);return cpe.SetParameterValues(output).then(function(){return cpe.GetParameterValues([constants.syslog]).then(function(response){var enableLog,validPath,data=funcs.buildTree(response.result.ParameterList);return data.Device.Services.Syslog.Log2USB?(enableLog=data.Device.Services.Syslog.Log2USB.Enable,validPath=_.isEmpty(data.Device.Services.Syslog.Log2USB.Directory)?!0:data.Device.Services.Syslog.Log2USB.ValidPath,!validPath&&enableLog?Promise.reject({name:"invalid_usb_path"}):Promise.resolve()):Promise.resolve()})})}function getLevels(){return attrs.Levels}function getDestinationList(){return attrs.Destination}function getSettings(){return config?config:{}}var config=null,attrs=null,constants={syslog:"Device.Services.Syslog.",usb_info:"Device.USB.Storage."},converterSyslog=device.systemLog.converterSyslog,converterUsb=device.systemLog.converterUsbInfo,__initConfig=null;return{pullSettings:function(){function success(response){var data=funcs.buildTree(response[0].result.ParameterList);return attrs=funcs.buildTreeAttributes(response[1].result.ParameterList),__initConfig=funcs.deepClone(data),config=converterSyslog.dsysinitToNative(data),attrs=converterSyslog.attrsToNative(attrs),data.Device.Services.Syslog.Log2USB&&(config.Log2USB.InvalidPath=_.isEmpty(config.Log2USB.Directory)?!1:!data.Device.Services.Syslog.Log2USB.ValidPath),Promise.resolve()}return Promise.all([cpe.GetParameterValues([constants.syslog]),cpe.GetParameterAttributes([constants.syslog])]).then(success)},apply:apply,updateUsbStorage:function(subscribeFunc,$scope){function prepareData(res){var data;res&&res[constants.usb_info]&&(data=converterUsb.dsysinitToNative(res[constants.usb_info]),subscribeFunc(data))}devinfo.once(constants.usb_info).then(function(res){prepareData(res),devinfo.subscribe(constants.usb_info,prepareData,$scope),$scope.$emit("pageload")})},unmountUsb:function(device){var path=constants.usb_info+device.__id+".Umount";return cpe.Execute(path,void 0)},updateLog:function(){return cpe.Upload("Log").then(function(res){var log=funcs.fetchBranch(res,"result.Result."),output=log&&log.length?log.join("\n"):"";return Promise.resolve(output)})},needExt:function(){return!0},getLevels:getLevels,getDestinationList:getDestinationList,getSettings:getSettings}}])}();