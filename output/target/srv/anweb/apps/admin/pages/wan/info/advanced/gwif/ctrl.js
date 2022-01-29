"use strict";angular.module("app").controller("WanGwifCtrl",["$scope","$state","wanPageCommon","wanPage",function($scope,$state,wanPageCommon,wanPage){function preapreItemsGwif(item){return{label:item.Name,value:item.__Path,disabled:item.DisabledGateway}}function sortItemsGwif(a,b){return a.label<b.label?-1:a.label>b.label?1:0}$scope.data=$state.params.data;var flatConnList=$scope.data.__flatConnList,ipv4List=wanPage.getGwifList(flatConnList,"ipv4").map(preapreItemsGwif).sort(sortItemsGwif),ipv4Gwif=wanPageCommon.delWanBasePath($scope.data.IPv4DefaultGateway);$scope.v4={list:ipv4List,value:ipv4Gwif,hasReserve:_.some(flatConnList,function(conn){return conn.IsFailover}),hasLoadBalancing:_.some(flatConnList,function(conn){return conn.IsLoadBalancing})};var ipv6List=wanPage.getGwifList(flatConnList,"ipv6").map(preapreItemsGwif).sort(sortItemsGwif),ipv6Gwif=wanPageCommon.delWanBasePath($scope.data.IPv6DefaultGateway);$scope.v6={list:ipv6List,value:ipv6Gwif},$scope.update=function(){var overlay=$scope.overlay.circular.start();wanPage.setGwif($scope.v4.value,$scope.data).then(function(){$scope.overlay.circular.stop(overlay),$scope.$emit("pageNeedUpdate")})},$scope.updateIpv6=function(){var overlay=$scope.overlay.circular.start();wanPage.setGwif($scope.v6.value,$scope.data,!0).then(function(){$scope.overlay.circular.stop(overlay),$scope.$emit("pageNeedUpdate")})}}]);