"use strict";!function(){angular.module("app").controllerProvider.register("UDPXYCtrl",["$scope","$state","$location","translate","funcs","udpxyUtil",function($scope,$state,$location,translate,funcs,util){function activate(){function success(){helper=util.makeHelper(),udpxy.data=helper.getData(),udpxy.port=angular.copy(udpxy.data.Port),__backupData=angular.copy(udpxy.data),udpxy.isActivate=!0}util.pull().then(success)["catch"](function(){$state.go("error",{code:"msg_pull_error",message:"msg_error_desc"})})["finally"]($scope.$emit.bind($scope,"pageload"))}function showLink(){return __backupData&&__backupData.Enable}function wasModified(){return __backupData&&!_.isEqual(__backupData,udpxy.data)}function getIface(ifaceIdentifer){return[{name:translate("st_not_selected"),value:""}].concat(helper.getIfaces(ifaceIdentifer))}$scope.udpxy={isActivate:!1,data:null,port:null,getLink:function(){return"http://"+$location.host()+":"+udpxy.port+"/status"},showLink:showLink,apply:function(){if(!$scope.form.$invalid){var overlay=$scope.overlay.circular,overlayId=overlay.start();util.apply(udpxy.data).then(function(){activate()})["catch"](function(){$state.go("error",{code:"msg_push_error",message:"msg_error_desc"})})["finally"](overlay.stop.bind(overlay,overlayId))}},wasModified:wasModified,getIface:getIface,getModeList:function(){return[{name:translate("st_not_selected"),value:""},{name:translate("wan_to_lan"),value:"lan"},{name:translate("wan_to_wan"),value:"wan"}]}};var helper,udpxy=$scope.udpxy,__backupData=null;activate()}])}();