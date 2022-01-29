"use strict";!function(){angular.module("app").service("wifiClientMgmUtil",["funcs","device","devinfo","cpe","translate",function(funcs,device,devinfo,cpe,translate){function getClients(){return config?formClients(config.Clients):{}}function isSupported5G(){return config.Support5G}function formClients(clients){function formBand(value){return"2.4GHz"==value?"24ghz":"5GHz"==value?"5ghz":void 0}function formSize(value){return _.isUndefined(value)?"-":funcs.lookSize(value).toString(sizeTranslater)}function sizeTranslater(value){return translate("units_"+value)}function formSpeed(value){return value?funcs.lookSize(1024*value*1024/8,"bit").toString(sizeTranslater)+"/"+translate("units_dt_S"):"-"}return _.map(clients,function(client){return _.isUndefined(client)?void 0:(_.isUndefined(client.Band)||(client.BandName=formBand(client.Band)),_.isUndefined(client.RxBytes)||(client.RxBytes=0==client.RxBytes?void 0:formSize(client.RxBytes)),_.isUndefined(client.TxBytes)||(client.TxBytes=0==client.TxBytes?void 0:formSize(client.TxBytes)),_.isUndefined(client.RateUpLink)||(client.RateUpLink=formSpeed(client.RateUpLink)),_.isUndefined(client.RateDownLink)||(client.RateDownLink=0==client.RateDownLink?void 0:formSpeed(client.RateDownLink)),client)})}var config={},constants={wifiInfo:"Device.WiFi.",wifiClientsMgm24:"Device.Statistics.WiFi.Radio.1.AccessPoint.",wifiClientsMgm5:"Device.Statistics.WiFi.Radio.2.AccessPoint.",servers:"Device.Network.Server.",hostnames:"Device.Hostnames."},converter=device.wifiClientMgm.converter,parameterArr=[constants.wifiInfo,constants.wifiClientsMgm24,constants.servers,constants.hostnames];return parameterArr.push(constants.wifiClientsMgm5),{pull:function(){function success(response){return config=funcs.buildTree(response[0].result.ParameterList),config=converter.dsysinitToNative(config),Promise.resolve()}return Promise.all([cpe.GetParameterValues(parameterArr)]).then(success)},subsribeClientInfo:function(cb,$scope,id){var subscribeString=parameterArr.join("|");devinfo.subscribe(subscribeString,function(response){var wifiClientsMgm24,wifiInfo,servers,wifiClientsMgm5,hostnames,input,data;response&&(wifiClientsMgm24=funcs.splitTree(response[constants.wifiClientsMgm24]),wifiInfo=funcs.splitTree(response[constants.wifiInfo]),servers=funcs.splitTree(response[constants.servers]),wifiClientsMgm5=funcs.splitTree(response[constants.wifiClientsMgm5]),hostnames=funcs.splitTree(response[constants.hostnames]),input=funcs.buildTree(wifiClientsMgm24.concat(wifiClientsMgm5,wifiInfo,servers,hostnames)),data=converter.dsysinitToNative(input),cb&&cb(formClients(data.Clients),id))},$scope)},updateClients:function(){function success(response){return config=funcs.buildTree(response[0].result.ParameterList),config=converter.dsysinitToNative(config),Promise.resolve()}return Promise.all([cpe.GetParameterValues(parameterArr)]).then(success)},disconnect:function(items){function getMacs(group){return group.map(function(item){return item.MAC}).join(" ")}var groupItems=_.groupBy(items,"Link"),disconnetParams=_.map(groupItems,function(group){return{Name:group[0].Link+"DisconnectStation",Value:getMacs(group)}});return cpe.SetParameterValues(disconnetParams)},getClients:getClients,isSupported5G:isSupported5G}}])}();