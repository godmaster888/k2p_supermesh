"use strict";angular.module("wizard").controller("wizardLangCtrl",["$scope","$rootScope","$state","translate","profiles","detectLang","stepManager",function($scope,$rootScope,$state,translate,profiles,detectLang,stepManager){$scope.device,$scope.isActivated=!1,$scope.detect=detectLang(),$scope.avail=[],translate.getLangNames().then(function(result){return $scope.detectLangName=result.data[$scope.detect],_.each(autoconf.__lang_arr,function(lang){$scope.avail.push({"short":lang,full:result.data[lang]})}),1==_.size($scope.avail)?(translate.changeLanguage($scope.avail[0]),void stepManager.action("next")):(_.contains(_.pluck($scope.avail,"short"),$scope.detect)||($scope.undetectedLang=!0,$scope.showlist=!0),void($scope.isActivated=!0))}),$scope.showlist=!1,$scope.yes=function(){translate.changeLanguage($scope.detect),stepManager.action("next")},$scope.no=function(){$scope.showlist=!0},$scope.back=function(){$scope.showlist=!1},$scope.change=function(lang){translate.changeLanguage(lang),stepManager.action("next")}}]);