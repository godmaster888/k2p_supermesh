"use strict";angular.module("trouble").controller("errorUndefinedCtrl",["$scope","troubleCheck","troubleConstants",function($scope,troubleCheck,consts){var data=troubleCheck.getData(),conn=data.ipv4gw?data.ipv4gw:data.ipv6gw;$scope.errText=consts.BR2_PACKAGE_ANWEB_ETYPE_APP&&"pptp"==conn.contype?"apps_trouble_pptp_undefined":"dcc_trouble_neterr"}]);