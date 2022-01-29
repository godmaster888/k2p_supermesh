"use strict";angular.module("app").controller("WanGeneralCtrl",["$scope","$state","translate",function($scope,$state,translate){var names=function(){var conns=$scope.common.existsConns;return $scope.conn.__New||(conns=conns.filter(function(conn){return conn.__Path!=$state.params.path})),conns.map(function(conn){return conn.Name})}();if($scope.isTatTRConn=(autoconf.BR2_PACKAGE_ANWEB_CUSTOM_TATTELEKOM_36956||autoconf.BR2_PACKAGE_ANWEB_CUSTOM_TATTELEKOM_39681)&&!$scope.customRules.tatSuperAdmin&&"DSL.ATM"==$scope.conn.MediaType&&"0/69"==$scope.conn.Media.DSL.ATM.DestinationAddress,$scope.curIface=$scope.conn.MediaPath,$scope.pageDetails.etherwan){var etherwanNav=$scope.pageDetails.etherwan[0];$scope.noEtherwanPortDesc=translate("wan_no_wan_ports",{link:"<a href='"+etherwanNav.url+"'>"+translate(etherwanNav.menu.name)+"</a>",trustAsHtml:!0})}$scope.contypeChange=function(){$scope.$emit("generalContypeChange",$scope.common.contype,$scope.common.role)},$scope.ifaceChange=function(){$scope.$emit("generalIfaceChange",$scope.conn.MediaPath,$scope.curIface),$scope.curIface=$scope.conn.MediaPath},$scope.validateName=function(value){return _.contains(names,value)?translate("name_exists"):null},$scope.changeRealConnState=function(){$scope.conn.InterfaceOnly?($scope.conn.Flags.NAT=!1,$scope.conn.Flags.NATHide=!0,$scope.conn.Flags.Ping=!1,$scope.conn.Flags.PingHide=!0):("IPv6"!=$scope.conn.Version&&($scope.conn.Flags.NATHide&&($scope.conn.Flags.NAT=!0),$scope.conn.Flags.NATHide=!1),$scope.conn.Flags.PingHide=!1)},$scope.getUSBIfaceNote=function(){var href=$scope.pageDetails.vlanList[0].url;return translate("wan_usb_interface_only_note",{link:"<a href='"+href+"'>VLAN</a>",trustAsHtml:!0})},$scope.isBase=function(){return"base"==$state.params.role},$scope.changeProxyARP=function(){if(!$scope.common.isXxtp&&"mobileinet"!=$scope.common.contype&&"bridge"!=$scope.common.contype){var media=$scope.conn.Media,mediaType=$scope.conn.MediaType;mediaType&&-1!=mediaType.indexOf("DSL")&&(media=media.DSL,mediaType=mediaType.split(".")[1]),mediaType&&(media[mediaType].ProxyARP=$scope.conn.Flags.ProxyARP)}},$scope.disabledFirewallFlag=function(){return $scope.customRules.DisabledWANFirewallv6?/v6|dual/.test($scope.common.contype)&&$scope.conn.Flags.Firewall?!0:void 0:!1},$scope.showNoWanPortsNote=function(){return $scope.common.isNoWanPorts&&"mobileinet"!=$scope.common.contype}}]);