"use strict";angular.module("app").controllerProvider.register("storageFilebrowserCtrl",function($scope){function activate(){$scope.$emit("pageload")}$scope.$on("filebrowser:load",function(){activate()})});