"use strict";function DynamicIpWarningCtrl($scope){function closeDialog(){$scope.closeThisDialog($scope.data)}$scope.data={},$scope.cancel=function(){$scope.data["continue"]=!1,closeDialog()},$scope["continue"]=function(){$scope.data["continue"]=!0,closeDialog()},$scope.goToDomainName=function(){$scope.data.goToDomainName=!0,closeDialog()}}