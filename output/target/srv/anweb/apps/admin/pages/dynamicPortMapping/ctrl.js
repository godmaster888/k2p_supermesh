"use strict";!function(){angular.module("app").controllerProvider.register("dynPortMappingCtrl",["$scope","$state","dynPortMappingUtil",function($scope,$state,util){$scope.dynPortMapping={Mode:null},function(){util.pull().then(function(){$scope.dynPortMapping.Mode=util.getMode(),$scope.$emit("pageload")},function(){$state.go("error",{code:"msg_pull_error",message:"msg_error_desc"})})}()}])}();