"use strict";function EtherwanStepController($scope,$rootScope,manualProfile,translate){function error(msg){_.defer(function(){return alert(translate(msg))})}var profile=manualProfile.profile();$scope.etherwanStep={profile:profile,setEtherwanPort:function(port){var service=manualProfile.getService(port.name);port.management?error("dcc_port_warn_pc"):service?error("dcc_port_warn_iptv_voip"):port.isWan||(profile.$EtherWAN.Port=port.name)}}}angular.module("wizard").controller("EtherwanStepController",EtherwanStepController),EtherwanStepController.$inject=["$scope","$rootScope","manualProfile","translate"];