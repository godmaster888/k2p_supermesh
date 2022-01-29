"use strict";!function(){angular.module("app").factory("summaryYandexDns",["utilSummaryYandexDns","devinfo",function(util){function get(){function saveYandexSettings(){return saveParams=!0,util.setSettings(yandexInfo.settings),util.pushSettings(yandexInfo.settings).then(function(){saveParams=!1})}function turnUnsafeToDefault(){if(yandexInfo.settings.enabled){saveParams=!0;var rules=util.getRules();_.each(rules,function(rule){"off"==rule.mode&&(rule.mode=this.settings["default"])},yandexInfo),util.setRules(rules),util.pushRules(rules).then(function(){updateModesInfo(),saveParams=!1})}}function yandexHandler(data){if(data&&!saveParams){var input=util.convert(data);updateModesInfo(),yandexInfo.settings=input.settings}}function updateModesInfo(){var settings=util.getSettings(),clients=util.getClients(),inModeDeviceCount=function(mode){var inMode=_.filter(clients,function(client){return!!client.rule&&client.rule.mode==mode||!client.rule&&settings["default"]==mode}),macArr=[];return _.each(inMode,function(elem){macArr.push(elem.mac.toUpperCase())}),_.uniq(macArr).length};modesInfo.safe.devicesCount=inModeDeviceCount("safe"),modesInfo.safe.devicesString=getDeviceCountString(modesInfo.safe.devicesCount),modesInfo.child.devicesCount=inModeDeviceCount("child"),modesInfo.child.devicesString=getDeviceCountString(modesInfo.child.devicesCount),modesInfo.off.devicesCount=inModeDeviceCount("off"),modesInfo.off.devicesString=getDeviceCountString(modesInfo.off.devicesCount)}function getDeviceCountString(count){var residual=count%10,label="";return(0==residual||residual>=5&&9>=residual)&&(label="yadns_from_five_to_nine_devices"),1==residual&&(label="yadns_one_device"),residual>=2&&4>=residual&&(label="yadns_from_two_to_four_devices"),label}if(yandexInfo)return yandexInfo;var availabelModes=[{value:"off",label:"yadns_protection_off_mode"},{value:"safe",label:"yadns_safe_mode"},{value:"child",label:"yadns_child_mode"}],modesInfo={safe:{label:_.findWhere(availabelModes,{value:"safe"}).label},child:{label:_.findWhere(availabelModes,{value:"child"}).label},off:{label:_.findWhere(availabelModes,{value:"off"}).label}};return yandexInfo={toggleYandexDns:function(){return saveYandexSettings()},updateSettings:function(){return saveYandexSettings().then(updateModesInfo)},saveYandexSettings:saveYandexSettings,turnUnsafeToDefault:turnUnsafeToDefault,getDeviceCountString:getDeviceCountString,area:util.getAreas(),devinfoHandler:yandexHandler,settings:util.getSettings(),modesInfo:modesInfo,modesList:_.keys(modesInfo)}}var yandexInfo=null,saveParams=!1;return{get:get}}])}();