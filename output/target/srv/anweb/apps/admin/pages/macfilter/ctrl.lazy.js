"use strict";!function(){angular.module("app").controllerProvider.register("MacFilterCtrl",["$scope","$state","funcs","translate","ngDialog","snackbars","macFilterUtil",function($scope,$state,funcs,translate,ngDialog,snackbars,util){function activate(overlayId){util.pull().then(function(){helper=util.makeHelper(),macfilter.config=util.getConfig(),macfilter.attrs=util.getAttrs(),macfilter.isActivate=!0})["catch"](function(){$state.go("error",{code:"msg_pull_error",message:"msg_error_desc"})})["finally"](function(){overlayId&&overlay.stop(overlayId),$scope.$emit("pageload")})}function remove(items,indexes){function checkRemove(changedConfig){return helper.accessWillLost(changedConfig)?(alert(translate("macflt_warn_access")),Promise.reject()):confirm(translate("macflt_warn_remove_rules"))?Promise.resolve(changedConfig):Promise.reject()}var changedConfig=helper.changeConfig(macfilter.config,{removeIndexes:indexes}),overlayId=overlay.start();return util.getClient().then(function(clientConfig){return clientConfig&&clientConfig.Client&&(changedConfig.Client=clientConfig.Client),Promise.resolve(changedConfig)}).then(checkRemove).then(function(changedConfig){return applyConfig(changedConfig,overlayId)})["catch"](overlay.stop.bind(overlay,overlayId))}function changeBaseAction(){function checkChange(changedConfig){return helper.accessWillLost(changedConfig)?(alert(translate("macflt_warn_access")),macfilter.config.BaseRule.Action="DROP"==macfilter.config.BaseRule.Action?"ACCEPT":"DROP",Promise.reject()):Promise.resolve(changedConfig)}var changedConfig=helper.changeConfig(macfilter.config),overlayId=overlay.start();return util.getClient().then(function(clientConfig){return clientConfig&&clientConfig.Client&&(changedConfig.Client=clientConfig.Client),Promise.resolve(changedConfig)}).then(checkChange).then(function(changedConfig){return applyConfig(changedConfig,overlayId)})["catch"](overlay.stop.bind(overlay,overlayId))}function showDialog(options){(function(options){return ngDialog.open({template:"/admin/dialogs/macfilter_form/dialog.tpl.html",controller:"MacFilterFormDialogCtrl",scope:$scope,data:options})})(options).closePromise.then(function(result){result&&result.value&&applyConfig(result.value.changed,result.value.overlayId)})}function applyConfig(config,overlayId){function success(){snackbars.add("msg_apply_success"),activate(overlayId)}function error(){overlay.stop(overlayId),$state.go("error",{code:"msg_push_error",message:"msg_error_desc"})}var overlayId;overlayId||(overlayId=overlay.start()),util.applyConfig(config).then(success)["catch"](error)}function getState(enable,nowrap){var result="";return _.isUndefined(enable)?renable:(enable?(result+=translate("st_enabled"),nowrap||(result="<span class='status_enable'>"+result+"</span>")):(result+=translate("st_disabled"),nowrap||(result="<span class='status_disable'>"+result+"</span>")),result)}function getActionName(value){var result=_.find(getActionList(),function(e){return e.value==value});return result?translate(result.name):""}function getActionList(){return[{name:"act_allow",value:"ACCEPT"},{name:"act_deny",value:"DROP"}]}$scope.macfilter={config:null,attrs:null,isActivate:!1,add:function(){showDialog({overRules:macfilter.config.Rules})},edit:function(item,index){showDialog({rule:item,index:index,overRules:_.omit(macfilter.config.Rules,index)})},remove:remove,changeBaseAction:changeBaseAction,getState:getState,getMiniCaption:function(item){return item.MACAddress.toUpperCase()},getMiniInfo:function(item){var result=[];return item.Enable&&result.push(translate("state")+": "+getState(item.Enable,!0)),item.Action&&result.push(translate("action")+": "+getActionName(item.Action)),result},getActionList:getActionList,getActionName:getActionName,isSupported:function(param){var rule=macfilter.config.Rules?macfilter.config.Rules[0]:{};return _.has(rule,param)}};var helper,macfilter=$scope.macfilter,overlay=$scope.overlay.circular;$scope.schedule={indexesLink:"[['IPv4', 'IPv6'], ['__ruleIdv4', '__ruleIdv6']]"},activate()}])}();