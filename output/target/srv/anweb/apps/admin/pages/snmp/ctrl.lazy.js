"use strict";function SnmpCtrl($scope,$state,funcs,snackbars,translate,util,ngDialog){function init(){function success(){helper=util.makeHelper(),workModel.params=helper.getBasicParams(),workModel.users=helper.getUsersList(),workModel.communities=helper.getCommunitiesList(),initModel=funcs.deepClone(workModel),$scope.device_mode=helper.getDeviceMode(),$scope.isActivate=!0,$scope.$emit("pageload")}$scope.isActivate=!1,util.pull().then(success,function(){$state.go("error",{code:"msg_pull_error",message:"msg_error_desc"})})}function save(){if(!$scope.snmp_params_form.$invalid){var overlay=$scope.overlay.circular,overlayId=overlay.start();helper.setParams(workModel);var data=helper.prepareParams();util.push(data).then(function(){initModel=funcs.deepClone(workModel),snackbars.add("msg_rpc_write_success"),init()},function(){snackbars.add("msg_rpc_write_error")})["finally"](overlay.stop.bind(overlay,overlayId))}}function openDialogUser(user,key){ngDialog.open({template:"dialogs/snmp/users/dialog.tpl.html",controller:"SnmpUserDialogCtrl",resolve:funcs.getLazyResolve("dialogs/snmp/users/ctrl.lazy.js","SnmpUserDialogCtrl"),data:{id:key,helper:helper,utilNoNeedPull:!0},scope:$scope}).closePromise.then(function(data){data.value&&"saved"==data.value.action&&(helper.setUser(data.value.user,key),workModel.users=helper.getUsersList())})}function removeUsers(items){var ids=_.pluck(items,"__id");ids&&ids.length&&(workModel.users=helper.removeUsers(ids))}function addCommunity(){$scope.model.communities.push(helper.getDefCommunity())}function wasModified(){return!(!$scope.isActivate||_.isEqual(workModel.params,initModel.params)&&_.isEqual(workModel.communities,initModel.communities)&&_.isEqual(workModel.users,initModel.users))}var helper,initModel=null,workModel={params:null,users:null,communities:null};$scope.model=workModel,$scope.funcs={save:save,getValue:function(item,param,undef_value){var undef_value=undef_value||"-";if(!item||!param)return undef_value;var result="";switch(param){case"auth_proto":result=item[param]?"snmp_user_auth_proto_"+item[param].toLowerCase():undef_value;break;case"priv_proto":result=item[param]?"snmp_user_crypto_proto_"+item[param].toLowerCase():undef_value;break;case"can_write":result=item[param]?"yes":"no";break;default:result=item[param]||undef_value}return result},isEmptyUsers:function(){return workModel&&workModel.users&&0==workModel.users.length},addUser:function(){openDialogUser()},removeUsers:removeUsers,isEmptyCommunities:function(){return workModel&&workModel.communities&&0==workModel.communities.length},addCommunity:addCommunity,removeCommunities:function(inx){$scope.model.communities[inx]=void 0,$scope.model.communities=_.compact($scope.model.communities)},validCommunityName:function(name,index){if(!name)return null;var communities=$scope.model.communities;return function(name,without){return _.every(communities,function(elem,index){return elem?without==index?!0:elem.name!=name:!0})}(name,index)?null:"msg_error_value_is_not_uniq"},editUser:function(item,key){openDialogUser(item,key)},wasModified:wasModified,checkRemoteSubnet:function(value){if(_.isEmpty(value))return null;if(!funcs.is.ipv4Network(value))return"msg_invalid_ipv4_subnet";var addr=value.split("/")[0],msk=funcs.ipv4.mask["long"](value.split("/")[1]);return addr&&funcs.is.mask(msk)&&addr!=funcs.ipv4.subnet.getNetworkAddress(addr,msk)?"msg_invalid_ipv4_subnet":null},validateLength:function(value,length){return value&&value.length>length?"msg_invalid_name_length":null}},init()}angular.module("app").controllerProvider.register("SnmpCtrl",["$scope","$state","funcs","snackbars","translate","snmpUtil","ngDialog",SnmpCtrl]);