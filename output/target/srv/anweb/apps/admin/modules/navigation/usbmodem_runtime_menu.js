"use strict";!function(){angular.module(regdep("usbmodem-runtime-menu"),["device"]).service("usbmodemRuntimeData",["cpe","funcs","translate",function(cpe,funcs,translate){function pull(){return promise=cpe.GetParameterValues([path+"*.Internal",path+"*.Model",path+"*.Vendor",path+"*.State"]).then(function(response){var obj=funcs.buildTree(response.result.ParameterList);if(!obj)return[];var modems=[];return _.each(funcs.newConfig.normalize(funcs.fetchBranch(obj,path)),function(elem,inst){"Unknown"!=elem.State&&modems.push({inst:inst,name:elem.Internal?translate("wan_modem")+" "+inst:elem.Vendor+" "+elem.Model})}),modems})["catch"](function(){return[]})["finally"](function(){promise=null})}var promise=null,path="Device.USB.Modem.";return{pull:pull}}]).directive("nwUsbmodemRuntimeMenu",function(){return{restrict:"E",scope:!0,template:'\n                <ul class="sl_menu_block">\n                    <li ng-repeat="submenu in usbmodem.menu.submenu" ng-if="usbmodem.isSingleModem()" ng-hide="submenu.hide">\n                        <a class="menu_item sl_menu_item" ui-sref-active="active" ui-sref="{{ submenu.sref }}" ng-click="selectSecondLevelMenu(usbmodem)">{{ submenu.name | translate }}</a>\n                    </li>\n                    <li ng-repeat="modem in usbmodem.modem" ng-class="{ \'sl_menu_opened\': (usbmodem.selected == modem.inst) } " ng-if="!usbmodem.isSingleModem()" ng-hide="submenu.hide">\n                        <a class="menu_item sl_menu_item of_ellipsis" ng-click="usbmodem.selectModem(modem.inst)">{{ modem.name }}</a>\n                        <ul class="tl_menu_block" ng-show="(usbmodem.selected == modem.inst)">\n                            <li ng-repeat="submenu in usbmodem.menu.submenu" ng-if="!usbmodem.isSingleMenu(submenu)">\n                                <a href="#" class="menu_item tl_menu_item" ui-sref-active="active" ui-sref="{{ usbmodem.getRef(modem, submenu) }}" ng-click="selectSecondLevelMenu(usbmodem)">{{ submenu.name | translate }}</a>\n                            </li>\n                        </ul>\n                    </li>\n                </ul>',controller:["$scope","$state","usbmodemRuntimeData","menuFuncs","navigationFilter","pageList","$timeout","funcs",function($scope,$state,runtimeData,menuFuncs,navigationFilter,pageList){function success(modem){var usbmodemNav;usbmodem.modem=modem,0==modem.length&&(navigationFilter.filter(),usbmodemNav={},usbmodemNav.id="usbmodem",usbmodemNav.openPage=!0,usbmodemNav.sref="usbmodem.empty.modem",$scope.selectFirstLevelMenu(usbmodemNav))}function update(){return runtimeData.pull().then(success)}var usbmodem={modem:[],selected:$state.params.modem,menu:function(){var navs=navigationFilter.filter(),menuList=menuFuncs.makeMenuList(navs,pageList);return _.find(menuList,function(o){return"usbmodem"==o.id})}(),getRef:function(modem,menu){var args={modem:modem.inst};return menu.sref+"("+JSON.stringify(args)+")"},selectModem:function(inst){usbmodem.selected=usbmodem.selected==inst?null:inst},isSingleMenu:function(){return!autoconf.BR2_PACKAGE_ANWEB_DSYSINIT}};$scope.usbmodem=usbmodem,$scope.$on("usb_update_modem_menu",update),update()}]}})}();