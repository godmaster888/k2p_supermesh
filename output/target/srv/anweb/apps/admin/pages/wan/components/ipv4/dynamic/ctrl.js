"use strict";angular.module("app").controller("WanIpv4DynamicCtrl",["$scope",function($scope){$scope.validateHostname=function(hostname){return hostname&&(hostname.length<3||hostname.length>30)?"msg_invalid_name_length":null},$scope.validateVendorID=function(vendorID){var re=/[а-яА-Я]/;return re.test(vendorID)?"msg_invalid_not_cyrillic_char":vendorID.length>128?"msg_input_lenght_more_max":null},$scope.isTatTRConn=(autoconf.BR2_PACKAGE_ANWEB_CUSTOM_TATTELEKOM_36956||autoconf.BR2_PACKAGE_ANWEB_CUSTOM_TATTELEKOM_39681)&&!$scope.customRules.tatSuperAdmin&&"DSL.ATM"==$scope.conn.MediaType&&"0/69"==$scope.conn.Media.DSL.ATM.DestinationAddress}]);