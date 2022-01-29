"use strict";!function(){angular.module("app").controllerProvider.register("UrlFilterCtrl",["$scope","$state","$location","translate","urlfilterUtil","funcs","devinfo","ngDialog",function($scope,$state,$location,translate,util,funcs,devinfo,ngDialog){function apply(){function success(){$state.reload(),$scope.overlay.circular.stop(overlayId)}function error(){$scope.overlay.circular.stop(overlayId),overlayId=null,$state.go("error",{code:"msg_push_error",message:"msg_error_desc"})}if($scope.form.$valid&&wasModified()){if("Black"==urlfilter.config.clients_mode&&0==urlfilter.clientsList.length)return void alert(translate("wan_clients_list_empty"));overlayId=$scope.overlay.circular.start();var settings=util.needPrepareSettings()?helper.prepareSettings(funcs.deepClone(urlfilter)):funcs.deepClone(urlfilter);util.push(settings).then(success)["catch"](error)}}function defaultRule(){var rule={};return rule.url="",rule}function removeRuleDns(rule,inx){updateRule("remove",null,inx)}function updateRule(action,rule,inx){switch(rule&&(rule=angular.fromJson(angular.toJson(rule))),action){case"add":helper.addRule(rule);break;case"set":helper.setRule(rule,inx);break;case"remove":helper.removeRuleDNS(inx)}urlfilter.list=helper.getList()}function wasModified(){return helper?helper.wasModified(urlfilter.config):!1}function showDialog(options){function closeDialog(result){result&&result.value&&(void 0!=result.value.id?("save"==result.value.action&&(urlfilter.clientsList[result.value.id]=result.value.rule),"remove"==result.value.action&&(delete urlfilter.clientsList[result.value.id],urlfilter.clientsList=_.compact(urlfilter.clientsList))):urlfilter.clientsList.push(result.value.rule),helper.updateClientsList(urlfilter.clientsList))}(function(options){return ngDialog.open({template:"/admin/dialogs/urlfilter_client_form/dialog.tpl.html",controller:"UrlFilterClientFormDialogCtrl",scope:$scope,data:options})})(options).closePromise.then(closeDialog)}function showTemplDialog(options){function closeDialog(result){result&&result.value&&(void 0!=result.value.id?("save"==result.value.action&&(urlfilter.list[result.value.id]=result.value.rule),"remove"==result.value.action&&removeRuleDns(result.value.rule,[result.value.id])):urlfilter.list.push(result.value.rule),helper.updateAddrsList(urlfilter.list))}(function(options){return ngDialog.open({template:"dialogs/urlfilter_addr_form/dialog.tpl.html",controller:"UrlFilterAddrFormDialogCtrl",scope:$scope,data:options})})(options).closePromise.then(closeDialog)}function onDeleteClient(item,inx){_.each(inx,function(num){delete urlfilter.clientsList[num]}),urlfilter.clientsList=_.compact(urlfilter.clientsList),helper.updateClientsList(urlfilter.clientsList)}function onDeleteTemplate(item,inx){_.each(inx,function(){helper.removeRuleDNS(inx)}),urlfilter.list=helper.getList()}$scope.urlfilter={isActivate:!1,config:null,list:null,apply:apply,addRule:function(){var rule=defaultRule();$scope.urlfilter.focus=!0,updateRule("add",rule)},setRule:function(rule,inx){updateRule("set",rule,inx)},removeRule:function(inx){updateRule("remove",null,inx)},removeRuleDns:removeRuleDns,getTypesList:function(){var result=[];return result.push({name:"urlflt_type_exlude",value:"Exclude"}),result.push({name:"urlflt_type_include",value:"Include"}),result},getClientModeList:function(){return[{name:"urlflt_dns_client_mode_black",value:"Black"},{name:"urlflt_dns_client_mode_white",value:"White"}]},validationUrl:function(url,index){return url?function(url){return/^(ht|f)tp(s?)\:/.test(url)}(url)?"urlflt_error_rule_url_contains_protocol":function(url,without){return _.every(urlfilter.list,function(elem,index){return elem.__removed?!0:without==index?!0:elem.url!=url})}(url,index)?null:"urlflt_error_rule_is_not_uniq":null},supportedParam:function(param){return _.has(urlfilter.config,param)},wasModified:wasModified,addressesList:[],clientsList:[],onAddClient:function(){showDialog({rule:{},client:urlfilter.client,clientsList:urlfilter.clientsList})},onEditClient:function(item,inx){showDialog({rule:item,inx:inx,client:urlfilter.client,clientsList:urlfilter.clientsList})},onDeleteClient:onDeleteClient,onAddTemplate:function(){showTemplDialog({rule:{},list:urlfilter.list})},onEditTemplate:function(item,inx){showTemplDialog({rule:item,inx:inx,list:urlfilter.list})},onDeleteTemplate:onDeleteTemplate,getTplName:function(tpl){switch(tpl){case"full":return translate("urlflt_dns_tpl_full");case"begin":return translate("urlflt_dns_tpl_begin");case"end":return translate("urlflt_dns_tpl_end");case"middle":return translate("urlflt_dns_tpl_middle")}},isRLX819XFilter:autoconf.BR2_PACKAGE_ANWEB_RLX_819X_DNS_FILTER||autoconf.BR2_PACKAGE_ANWEB_DNS_FILTER};var helper,urlfilter=$scope.urlfilter,overlayId=null;$scope.schedule={active:!1},function(){function success(){helper=util.makeHelper(),urlfilter.config=helper.getConfig(),urlfilter.list=helper.getList(),urlfilter.clientsList=helper.getClientsList(),urlfilter.focus=!1;var conf=urlfilter.config;$scope.schedule.isDisabled=!conf.__id,$scope.schedule.indexesLink=[conf.__id],devinfo.once("client").then(function(result){urlfilter.client=result.client,urlfilter.isActivate=!0,$scope.$emit("pageload")},error),overlayId&&($scope.overlay.circular.stop(overlayId),overlayId=null)}function error(){$state.go("error",{code:"msg_pull_error",message:"msg_error_desc"})}util.pull().then(success)["catch"](error)}()}])}();