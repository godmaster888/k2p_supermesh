"use strict";function DHCPStaticAddressDialogCtrl($scope,funcs){var options=$scope.ngDialogData,iface=$scope.device.lan["interface"].getCurrent(),version=$scope.ngDialogData.version,dnsHostsHostnames=function(version){var hosts=iface[version].dns.list(),currentHosts=[],isCurrentIP="ipv4"==version?funcs.is.ipv4:funcs.is.ipv6;return _.each(hosts,function(host){isCurrentIP(host.IPAddress)&&currentHosts.push(host.Hostname)}),currentHosts}(version);$scope.version=version,$scope.staticAddress={data:{IPAddress:"",MACAddress:"",Hostname:""},initData:null,instance:null,init:function(util,data,instance){this.util=util,data&&_.extend(this.data,data),instance&&(this.instance=instance),this.initData=angular.copy(this.data),this.usedValues=this.util.usedValues(this.instance),this.network=$scope.$parent[version].data.StaticIP[1],"ipv4"==version&&(this.subnet=funcs[version].subnet.getNetworkAddress(this.network.Address,this.network.SubnetMask),this.mask=funcs[version].mask["short"](this.network.SubnetMask)),"ipv6"==version&&this.network&&(this.subnet=funcs[version].subnet.getNetworkAddress(this.network.Address,this.network.SubnetMask),this.prefix=this.network.Prefix)},validation:function(value,param){var isIncludedToNetwork,errors=this.util.validation(this.data),usedValues=this.usedValues[param],gatewayAddress=this.network?this.network.Address:void 0;return"IPAddress"==param&&gatewayAddress==value&&errors[param].push("lan_error_ip_address_is_used_as_lan_ip"),_.contains(usedValues,value)&&("IPAddress"==param&&errors[param].push("msg_ip_address_is_used"),"MACAddress"==param&&errors[param].push("msg_mac_address_is_used"),"Hostname"==param&&""!=value&&errors[param].push("msg_error_value_is_not_uniq")),"Hostname"==param&&_.contains(dnsHostsHostnames,value)&&errors[param].push("msg_error_value_is_not_uniq"),"IPAddress"==param&&this.network&&(isIncludedToNetwork=this.util.checkForEntryInSubnet(this.data.IPAddress,this.network),isIncludedToNetwork||errors[param].push("msg_ip_address_is_not_included_to_network")),errors[param]&&errors[param].length?errors[param][0]:null},warning:function(){var isIncludedToNetwork;return this.network&&(isIncludedToNetwork=this.util.checkForEntryInSubnet(this.data.IPAddress,this.network),!isIncludedToNetwork)?"msg_ip_address_is_not_included_to_network":null},isChange:function(){return!_.isEqual(this.initData,this.data)}},$scope.staticAddress.dialog={action:options.action,onApply:function(){$scope.form.$valid&&$scope.closeThisDialog({action:this.action,data:$scope.staticAddress.data,instance:$scope.staticAddress.instance})},onRemove:function(){$scope.closeThisDialog({action:"remove",instance:$scope.staticAddress.instance})},onCancel:function(){$scope.closeThisDialog(null)}},$scope.staticAddress.init(iface[version].dhcp.staticAddress,options.data,options.instance)}