"use strict";!function(){angular.module("app").service("StatsRoutingUtil",["$rootScope","cpe","device","devinfo","funcs","navigationFilter",function($rs,cpe,device,devinfo,funcs,navigationFilter){function needTable(){return $rs.deviceMode&&"Firewall"==$rs.deviceMode?!0:config&&config.Tables.length>1}function makeHelper(){return new device.statsRouting.Helper(config)}var config=null,converter=device.statsRouting.converter,customRules=navigationFilter.rules()||{},constants={statsRouting:"Device.Statistics.Routing.",tables:"Device.Routing.Tables.",ifaces:"Device.Network.Interface.",xdsl:"Device.xDSL.",vlans:"Device.Switch.Switches.",statsRules:"Device.Statistics.RoutingRules."},paths=[constants.statsRouting,constants.tables,constants.ifaces,constants.vlans,constants.statsRules];return{pull:function(){function success(response){return config=funcs.buildTree(response[0].result.ParameterList),config.hideTRVoip=customRules.HideTRVoipStats,config=converter.dsysinitToNative(config),Promise.resolve()}return Promise.all([cpe.GetParameterValues(paths)]).then(success,function(){return Promise.reject()})},subscribeInfo:function(cb,$scope){devinfo.onceAndSubscribe(paths.join("|"),function(response){if(response&&response[constants.statsRouting]){var stats=funcs.splitTree(response[constants.statsRouting]),tables=funcs.splitTree(response[constants.tables]),ifaces=funcs.splitTree(response[constants.ifaces]),xdsl=funcs.splitTree(response[constants.xdsl]),switches=funcs.splitTree(response[constants.vlans]),rules=funcs.splitTree(response[constants.statsRules]),input=funcs.buildTree(stats.concat(tables,ifaces,xdsl,switches,rules));input.hideTRVoip=customRules.HideTRVoipStats;var data=converter.dsysinitToNative(input);config=data,cb&&cb(data)}},$scope)},needTable:needTable,makeHelper:makeHelper}}])}();