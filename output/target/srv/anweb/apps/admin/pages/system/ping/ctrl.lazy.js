"use strict";var _typeof="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(obj){return typeof obj}:function(obj){return obj&&"function"==typeof Symbol&&obj.constructor===Symbol&&obj!==Symbol.prototype?"symbol":typeof obj};angular.module("app").controllerProvider.register("SysPingCtrl",["$scope","$timeout","funcs","ngDialog","translate","pingUtil",function($scope,$timeout,funcs,ngDialog,translate,util){function scrollToBottom(){var console=document.getElementById("console");return function(){console.scrollTop=console.scrollHeight}}var settingsPacketsize=56,settingsTimeout=3;!function(){util.init().then(function(){$scope.loading=!1,$scope.isRequestInProgress=!0,$scope.pingFlowActive=!1,$scope.ping={host:"",count:3,is_ipv6:!1,packetsize:settingsPacketsize,timeout:settingsTimeout},$scope.console="",$scope.$emit("pageload")})}(),$scope.flowDetector={add:function(msg){$scope.console+=msg,/^---/.test(msg)&&($scope.console+="\n"),$timeout(scrollToBottom()),$scope.$apply()},error:function(){$scope.pingFlowActive=!1,$scope.$apply(),alert(translate("msg_error_pull_data_desc"))},close:function(){$scope.console+="\n\n",$scope.pingFlowActive=!1,$timeout(scrollToBottom()),$scope.$apply()}},$scope.startPing=function(){return $scope.pingSettings.$invalid?void 0:function(){return $scope.pingFlowActive=!0,util.startPing($scope.ping,$scope.flowDetector,$scope)}()},$scope.stopPing=function(){util.stopPing(),$scope.flowDetector.close()},$scope.clearConsole=function(){$scope.console=""},$scope.settingsDiffCount=function(){var count=0;return $scope.ping.packetsize!=settingsPacketsize&&count++,$scope.ping.timeout!=settingsTimeout&&count++,count},$scope.openPingSettings=function(){$scope.ping.tmp_packetsize=$scope.ping.packetsize,$scope.ping.tmp_timeout=$scope.ping.timeout,ngDialog.open({template:"/admin/dialogs/ping_settings/dialog.tpl.html",controller:"PingSettings",closeByEscape:!1,resolve:funcs.getLazyResolve("/admin/dialogs/ping_settings/ctrl.lazy.js","PingSettings"),scope:$scope})},$scope.disableButtons=function(name){switch(name){case"start":case"clear":return $scope.pingFlowActive;case"stop":return!$scope.pingFlowActive}}}]);