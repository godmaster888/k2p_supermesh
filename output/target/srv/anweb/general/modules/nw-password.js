"use strict";!function(){var nw=angular.module(regdep("nw-password"),[]);nw.directive("nwPassword",["translate","nwPasswordService",function(translate,passwordService){return{restrict:"A",replace:!0,template:"<div><input type='password' name='autocomplete_off_trick' style='display: none;' ng-if='!password.autocomplete' /><div nw-field nw-label='{{password.label}}' nw-required='password.required'><input type='password' tab-index='{{password.tabindex}}' ng-model='password.value' ng-change=passwordChange(password.value) nw-custom-valid='passwordValidation(value, password, password.model)' nw-eye='nwPasswordHideEye'name='login_password'nw-set-focus='{{password.focus}}' nw-hide-value /></div><div class='note description' ng-if='password.description'>{{ password.description | translate }}</div></div>",scope:{nwPasswordValue:"=",nwPasswordValid:"&",nwPasswordChange:"&",nwPasswordHideEye:"="},link:function($scope,$element,$attrs){$scope.password={label:"",value:"",required:!1,autocomplete:_.has($attrs,"nwPasswordAutocomplete"),novalidation:_.has($attrs,"nwPasswordNovalidation"),focus:_.has($attrs,"nwPasswordFocus")?"true":"false",tabindex:_.has($attrs,"nwPasswordTabIndex")?$attrs.nwPasswordTabIndex:"0"},$attrs.$observe("nwPasswordDescription",function(value){$scope.password.description=value||""}),$attrs.$observe("nwPasswordRequired",function(value){$scope.password.required=value||""===value}),$attrs.$observe("nwPasswordLabel",function(value){$scope.password.label=value||"password"}),$scope.$watch("nwPasswordValue",function(value){_.isUndefined(value)||($scope.password.value=value)}),$scope.passwordChange=function(value){_.isFunction($scope.nwPasswordChange)&&$scope.nwPasswordChange({value:value}),$scope.nwPasswordValue=value},$scope.passwordValidation=function(value,password,model){var customValid=$attrs.nwPasswordValid&&_.isFunction($scope.nwPasswordValid);return $scope.password.novalidation?null:customValid?$scope.nwPasswordValid({value:value}):passwordService.validation(value,password,model)}}}}]),nw.directive("nwPasswordConfirm",["translate","nwPasswordService",function(translate,passwordService){return{restrict:"A",replace:!0,template:"<div><input type='password' name='autocomplete_off_trick' style='display: none;' /><div nw-field nw-label='{{password.label}}' nw-required><input type='password' ng-model='password.value' ng-change=passwordChange(password.value) nw-custom-valid='passwordValidation(value, password, password.model)' /></div><div class='note' ng-if='password.description'>{{ password.description | translate }}</div><input type='password' name='autocomplete_off_trick' style='display: none;' /><div nw-field nw-label='{{password.confirm.label}}' nw-required><input type='password' ng-model='password.confirm.value' nw-custom-valid='passwordValidation(value, password, password.confirm.model)' /></div></div>",scope:{nwPasswordConfirmValue:"=",nwPasswordConfirmCheck:"&",nwPasswordConfirmReset:"=",nwPasswordConfirmValid:"&"},link:function($scope,$element,$attrs){function getModel(input){return input?angular.element(input).data("$ngModelController"):null}$scope.password={label:"",value:"",model:getModel($element.find("input")[1]),confirm:{label:"",value:"",model:getModel($element.find("input")[3])}};var password=$scope.password;$attrs.$observe("nwPasswordDescription",function(value){password.description=value||""}),$attrs.$observe("nwPasswordConfirmLabel",function(value){password.label=value||"password"}),$attrs.$observe("nwPasswordConfirmLabel2",function(value){password.confirm.label=value||"confirm_password"}),$scope.$watch("nwPasswordConfirmValue",function(value){_.isUndefined(value)||(password.value=value)}),$scope.$watch(function(){return""!=password.confirm.value?password.value==password.confirm.value:!1},function(value){$scope.nwPasswordConfirmCheck({value:value})}),$scope.$watch("nwPasswordConfirmReset",function(value){value&&(password.value="",password.confirm.value="",password.model.$setPristine(),password.confirm.model.$setPristine(),$scope.nwPasswordConfirmReset=!1)}),$scope.passwordValidation=function(value,password,model){var customValid=$attrs.nwPasswordConfirmValid&&_.isFunction($scope.nwPasswordConfirmValid);return customValid?$scope.nwPasswordConfirmValid({value:value}):passwordService.validation(value,password,model)},$scope.passwordChange=function(value){$scope.nwPasswordConfirmValue=value}}}}]),nw.service("nwPasswordService",[function(){return{validation:function(value,password,model){var reCyrill=/[А-яЁё]+/g,reLegal=/^[\x20-\x7E]*$/g,passwordsNotEqual=password&&password.confirm&&""!==password.confirm.value&&password.value!==password.confirm.value&&model.$dirty;return value.length>31?"msg_invalid_password_length":reCyrill.test(value)?"msg_password_contains_cirill":reLegal.test(value)?passwordsNotEqual?"msg_passwords_not_equal":null:"msg_password_contains_illegal"}}}])}();