"use strict";!function(){var slacker=angular.module(regdep("slacker"));slacker.service("slacker-helper",["$rootScope",function($rootScope){return{logout:function(){$rootScope.auth&&$rootScope.auth.logout()},isNeedLogout:function(){return!localStorage.getItem("auth-dsysinit.staySignedIn")}}}])}();