"use strict";function SafeDnsSettingsCtrl($scope,$state,funcs,snackbars,translate,device){function activate(){var overlayId=overlay.start();util.pull().then(function(){updateModel(),$scope.model=$scope.workModel,$scope.initModel=funcs.deepClone($scope.workModel),$scope.isActivate=!0,$scope.$emit("pageload")},function(){$state.go("error",{code:"msg_pull_error",message:"msg_error_desc"})})["finally"](overlay.stop.bind(overlay,overlayId))}function updateModel(){$scope.workModel.params=util.getParams(),$scope.workModel.providerList=util.getProviderList(),$scope.workModel.accountList=util.account.getList({__pid:$scope.workModel.params.default_provider}),$scope.workModel.profileList=util.getProfileList({__pid:$scope.workModel.params.default_provider,__aid:$scope.workModel.params.default_account}),$scope.workModel.macList=util.mac.getList(),$scope.workModel.profileList.length>0&&""==$scope.workModel.params.default_profile&&($scope.workModel.params.default_profile=$scope.workModel.profileList[0].__id,save())}function changeDefault(target){var params={};params.enabled=$scope.workModel.params.enabled,params["default_"+target]=$scope.workModel.params["default_"+target],util.setParams(params),updateModel()}function providerAvailable(){return!!_.find($scope.workModel.providerList,function(item){return $scope.workModel.params&&item.__id==$scope.workModel.params.default_provider&&item.available})}function save(){if(isFormValid()){var overlayId=overlay.start();util.pushParams($scope.workModel.params).then(function(){activate(),snackbars.add("msg_rpc_write_success")},function(){snackbars.add("msg_rpc_write_error")})["finally"](overlay.stop.bind(overlay,overlayId))}}function isFormValid(){return $scope.safedns_form.$valid}var util=device.safeDns,overlay=$scope.overlay.circular;$scope.workModel={},$scope.initModel={},$scope.modelFuncs={wasModified:function(){return $scope.isActivate&&!_.isEqual($scope.workModel.params,$scope.initModel.params)},changeDefault:changeDefault,getOptions:function(target){var name="account"==target?"email":"name",noDef=!0,options=_.map($scope.workModel[target+"List"],function(item){return noDef&=item.__id!=$scope.workModel.params["default_"+target],{name:item[name],value:item.__id}});return(!_.size(options)||noDef)&&(options=_.union([{name:translate("st_not_selected"),value:""}],options),$scope.workModel.params["default_"+target]=""),options},providerAvailable:providerAvailable,providerCabinet:function(){var provider=_.find($scope.workModel.providerList,function(item){return $scope.workModel.params&&item.__id==$scope.workModel.params.default_provider});return provider?provider.cabinet:""},saveParams:save,needHide:function(field){switch(field){case"default_profile":return!_.size($scope.workModel.profileList)||!$scope.initModel.params.default_provider||$scope.initModel.params.default_provider!=$scope.model.params.default_provider;case"default_provider_desc":case"sync":return!$scope.workModel.params.default_provider;case"auth_status":return _.isUndefined($scope.workModel.params.__isAuth)||!providerAvailable();case"tarif":return!$scope.workModel.params.__plan||!providerAvailable();case"account":return!($scope.model.params.enabled&&$scope.initModel.params.enabled&&$scope.initModel.params.default_provider&&$scope.initModel.params.default_provider==$scope.model.params.default_provider&&providerAvailable());default:return!1}},isEmptyAccounts:function(){return!_.size($scope.workModel.accountList)},isProviderSelected:function(){return!!$scope.initModel.params&&!!$scope.initModel.params.default_provider&&!!$scope.workModel.params.default_provider},updateModel:updateModel,syncAccount:function(){confirm(translate("safedns_sync_alert"))&&util.account.sync().then(function(){activate()})},sortProvByLang:function(list){var lang=translate.getLang(),sortedList=[];return"rus"==lang||""!=list[0].value?sortedList=list:(sortedList.push(list[0]),sortedList.push(list[2]),sortedList.push(list[1])),sortedList},onEnable:function(){var defProvValue,defProvValue="1";!$scope.model.params.enabled||$scope.model.params.default_provider&&$scope.model.params.default_provider==defProvValue||($scope.model.params.default_provider=defProvValue,changeDefault("provider")),save()}},$scope.modelFuncs.dnsWarn="safedns_yadns_warning_sky",$scope.modelFuncs.safeDNSIcon="skydns_logo",$scope.modelFuncs.safeDNSCaption="safedns_sky_nav",util.yaDNSEnabled().then(function(enabled){enabled?($scope.isYaDNSWarning=!0,$scope.$emit("pageload")):activate()})["catch"](activate)}angular.module("app").controllerProvider.register("SafeDnsSettingsCtrl",SafeDnsSettingsCtrl),SafeDnsSettingsCtrl.$inject=["$scope","$state","funcs","snackbars","translate","device"];