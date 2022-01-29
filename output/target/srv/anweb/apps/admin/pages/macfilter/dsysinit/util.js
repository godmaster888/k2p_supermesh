"use strict";!function(){angular.module("app").service("macFilterUtil",["cpe","devinfo","device","funcs",function(cpe,devinfo,device,funcs){function getConfig(){return nativeConfig}function getAttrs(){return nativeAttrs}function applyConfig(changed){function prepareRules(rules){var result=[];return _.each(rules,function(rule){var tempRule,tempRulev6,tempRuleWanv6,ids,key=result.length;if(rule.__ruleId)ids=funcs.deepClone(rule.__ruleId),rule.DestZone="fw",rule.__ruleId=ids[0],rule.Type="v4",result.push(rule),tempRule=funcs.deepClone(rule),tempRule.DestZone="all",tempRule.__ruleId=ids[1],result.push(tempRule),tempRulev6=funcs.deepClone(rule),tempRulev6.Type="v6",tempRulev6.__ruleId=ids[2],result.push(tempRulev6),tempRuleWanv6=funcs.deepClone(rule),tempRuleWanv6.Type="v6",tempRuleWanv6.DestZone="all",tempRuleWanv6.__ruleId=ids[3],result.push(tempRuleWanv6);else{rule.__ruleId="new_inst_"+key,rule.DestZone="fw",rule.Type="v4",result.push(rule);var tempRule=funcs.deepClone(rule);tempRule.__ruleId="new_inst_"+(key+1),tempRule.DestZone="all",result.push(tempRule);var tempRulev6=funcs.deepClone(rule);tempRulev6.Type="v6",result.push(tempRulev6);var tempRuleWanv6=funcs.deepClone(rule);tempRuleWanv6.Type="v6",tempRuleWanv6.DestZone="all",tempRuleWanv6.__ruleId="new_inst_"+(key+1),result.push(tempRuleWanv6)}}),result}changed.Rules=prepareRules(changed.Rules);var config=converter.nativeToDsysinit.config(changed),init=function(init){return init.BaseRule.__ruleIdv4||delete init.BaseRule,init.Rules=prepareRules(init.Rules),converter.nativeToDsysinit.config(init)}(__initNativeConfig),diff=funcs.newConfig.makeDiff(init,config,dsysinitAttrs);return cpe.ApplyDifference(diff)}var nativeConfig=null,dsysinitConfig=null,nativeAttrs=null,dsysinitAttrs=null,__initNativeConfig=null,firewallPath="Device.Firewall.",Helper=device.macfilter.Helper,converter=device.macfilter.converter;return{getConfig:getConfig,getAttrs:getAttrs,pull:function(){function success(response){dsysinitConfig=funcs.buildTree(response[0].result.ParameterList),dsysinitAttrs=funcs.buildTreeAttributes(response[1].result.ParameterList);var client=response[2]?response[2].client:null;return nativeConfig=converter.dsysinitToNative.config({config:dsysinitConfig,client:client}),nativeAttrs=converter.dsysinitToNative.attrs({attrs:dsysinitAttrs}),__initNativeConfig=funcs.deepClone(nativeConfig),Promise.resolve()}return Promise.all([cpe.GetParameterValues([firewallPath]),cpe.GetParameterAttributes([firewallPath]),devinfo.once("client")]).then(success)},getClient:function(){return devinfo.once("client").then(function(response){return response&&response.client?converter.dsysinitToNative.config({client:response.client}):Promise.resolve()})},applyConfig:applyConfig,makeHelper:function(){return new Helper}}}])}();