"use strict";angular.module("wizard").controller("wizardRebootStatusCtrl",["$scope","$rootScope","$state","$stateParams","$location","$http","devinfo","$interval","funcs","profileInspector","queryString",function($scope,$rootScope,$state,$stateParams,$location,$http,devinfo,$interval,funcs,profile,queryString){function shouldUseLocalDomain(){return"ap"===profile.byPath("Config.DeviceMode.Mode")&&"Static"!==profile.byPath("Config.LAN.1.IPv4.AddressingMode")&&profile.byPath("Config.LAN.1.IPv4.StaticIP.1.AddnHostname")}function isDeviceAddressChanged(){if(shouldUseLocalDomain())return!0;var ip=profile.byPath("Config.LAN.1.IPv4.StaticIP.1.Address");return ip&&$rootScope.rootDeviceIP!=ip}function devinfoHandler(data){var url;if(!$scope.rootAuthDialogIsOpen){if(data?availCount++:availCount=0,"wait"==$scope.step&&("reboot"==$scope.action&&isDeviceAddressChanged()&&(url=location.protocol+"//"+nonDefaultHostname+"/devinfo&area=notice",$http.get(url,{"X-CORS-Header":!0}).then(function(result){result.data&&$scope.nextForNonDefaultIp()})),"update"==$scope.action&&data&&data.lastUpdateError&&$state.go("status",{step:"updateFailed"}),$scope.checkCount>=$scope.checkCountMax&&"reset"===$stateParams.action&&($scope.rootIsWIFIClient?$scope.switchStep("info"):reloadPageIfNeeded())),"check"==$scope.step){if(queryString.nocheck&&!$scope.rootIsWIFIClient)return void $scope.next();$scope.checkCount>=$scope.checkCountMax&&(devinfo.unsubscribe("notice",devinfoHandler),$scope.switchStep("error")),data&&$rootScope.gDeviceAvail&&availCount>=availCountMax&&$scope.next(),mobileApp&&$rootScope.dlinkMobileApp.isPhoneConnectedToRouter().then(function(connected){connected&&$scope.nextForNonDefaultIp()})}data&&$rootScope.gDeviceAvail&&"wait"==$scope.step&&availCount>=availCountMax&&!isDomain()&&(devinfo.unsubscribe("notice",devinfoHandler),$scope.next())}}function reloadPageIfNeeded(){return isNeedSetDefaultIP(defaultIP)?void goToNewIP(defaultIP):void(isHTTPS()&&(window.onbeforeunload=null,location.reload()))}function goToNewIP(ip){var url,url=$location.protocol()+"://"+ip;goAway(url)}function isHTTPS(){return"https"==$location.protocol()}function isLocalhost(){var host=$location.host();return"localhost"==host||"127.0.0.1"==host}function isDomain(){var host=$location.host();return!funcs.is.ipv4(host)}function isNeedSetDefaultIP(ip){return!isLocalhost()&&$location.host()!=ip}function goToNewWizardURL(ip){var port=location.port,path=location.pathname,proto=location.protocol,query="?afterReload=1"+_.reduce(queryString,function(m,v,k){return m+"&"+k+"="+encodeURIComponent(v)},""),url=proto+"//"+ip+String(port?":"+port:port)+path+query;window.onbeforeunload=null,location.href=url}profile.set($scope.nativeData),$scope.setNeedAuth(!1),$scope.action=$stateParams.action,$scope.pause=!0,$scope.step="wait",$scope.checkCount=0,$scope.checkCountMax=5,$scope.rebootDuration=parseInt(autoconf.BR2_PACKAGE_ANWEB_DEVICE_REBOOT_TIME),$scope.checkTimeout=1e4;var defaultIP,circlesInterval=$interval(function(){$scope.checkCount++},2e3),availCount=0,availCountMax=5,nonDefaultHostname=null;defaultIP="192.168.0.1",$rootScope.gAutoAuth=!0;var mobileApp=$rootScope.dlinkMobileApp?$rootScope.dlinkMobileApp.isUserInMobileApp():null;$stateParams.duration&&($scope.rebootDuration=$stateParams.duration),"reset"==$stateParams.action&&($scope.checkCountMax=45,mobileApp&&($rootScope.dlinkMobileApp.deviceResetStarted(),isNeedSetDefaultIP(defaultIP)&&$rootScope.dlinkMobileApp.ipAddressChanged(defaultIP))),"update"==$stateParams.action&&($rootScope.gNeedAuth=!1,$scope.rebootDuration=25e4,$scope.checkTimeout=23e4),isDeviceAddressChanged()&&(nonDefaultHostname=profile.byPath(shouldUseLocalDomain()?"Config.LAN.1.IPv4.StaticIP.1.AddnHostname":"Config.LAN.1.IPv4.StaticIP.1.Address")),$scope.showAvailOverlay(!1),$scope.$watch("step",function(step){switch(step){case"check":$scope.checkCount=0,devinfo.subscribe("notice",devinfoHandler)}}),$scope.$watch("rootAuthDialogIsOpen",function(opened){$scope.pause=opened}),$scope.$on("$destroy",function(){devinfo.unsubscribe("notice",devinfoHandler),$interval.cancel(circlesInterval),devinfo.init({timeout:1e4})}),$scope.switchStep=function(step){$scope.step=step},$scope.next=function(){var next=$stateParams.next?decodeURIComponent($stateParams.next):null;$scope.setNeedAuth(!0),next&&/^(http|https|app):/.test(next)?goAway(next):"reset"==$stateParams.action&&$location.host()==defaultIP?(window.onbeforeunload=null,location.reload()):$stateParams.nocheck&&"update"==$stateParams.action?(window.onbeforeunload=null,location.reload()):next?$state.go(next,{prev:"reboot_status",prev_action:$scope.action}):goAway("/")},$scope.nextForNonDefaultIp=function(){function continueAfterReboot(){return $stateParams.nocheck?void $scope.next():void(isDeviceAddressChanged()?goToNewWizardURL(nonDefaultHostname):$scope.next())}"reboot"===$stateParams.action&&continueAfterReboot(),"reset"===$stateParams.action&&(reloadPageIfNeeded(),$scope.next())},$scope.callback=function(status){"finished"==status&&"wait"==$scope.step&&(devinfo.unsubscribe("notice",devinfoHandler),"update"==$stateParams.action&&$scope.next(),$scope.rootIsWIFIClient?$scope.switchStep("info"):$scope.nextForNonDefaultIp())},$scope.isWiFiEnabled=function(){return"reset"!==$stateParams.action&&profile.isWiFiBroadcasting("2.4GHz")},$scope.isWiFi5GEnabled=function(){return"reset"!==$stateParams.action&&profile.isWiFiBroadcasting("5GHz")},$scope.isWiFiEncrypted=function(){return"reset"!==$stateParams.action&&profile.isWiFiEncrypted("2.4GHz")},$scope.isWiFi5GEncrypted=function(){return"reset"!==$stateParams.action&&profile.isWiFiEncrypted("5GHz")},$scope.isStaticLanIp=function(){return"reboot"===$stateParams.action&&profile.hasStaticIp()},$scope.isSecondIpRangeAvailable=function(){return $scope.getIpRanges().length>1},$scope.getIpRanges=function(){if($scope.ipRanges)return $scope.ipRanges;var ip=$scope.nativeData.Config.LAN[1].IPv4.StaticIP[1].Address,mask=$scope.nativeData.Config.LAN[1].IPv4.StaticIP[1].SubnetMask,range=$scope.device.funcs.ipv4.subnet.getNetworkRange(ip,mask);return $scope.ipRanges=$scope.device.funcs.ipv4.subnet.splitNetworkRange(range,ip),$scope.ipRanges},$scope.isUserInMobileApp=function(){return $rootScope.dlinkMobileApp&&$rootScope.dlinkMobileApp.isUserInMobileApp()},$scope.closeWebInterface=function(){$rootScope.dlinkMobileApp.closeWebInterface()},$scope.isConnectionPossible=function(){return"reboot"!==$stateParams.action||!$scope.rootIsWIFIClient||$scope.isWiFiEnabled()||$scope.isWiFi5GEnabled()?!0:!1},$scope.isCheckPossible=function(){if("reset"===$stateParams.action&&isNeedSetDefaultIP(defaultIP)&&!$scope.isUserInMobileApp())return!1;if("reboot"===$stateParams.action){if(!$scope.isConnectionPossible())return!1;if(isDeviceAddressChanged()&&!$scope.isUserInMobileApp())return!1}return!0};var startInterval=$interval(function(){$scope.pause||($interval.cancel(startInterval),devinfo.subscribe("notice",devinfoHandler))},$scope.checkTimeout);devinfo.init({timeout:5e3})}]);