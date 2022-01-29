"use strict";!function(){window.goAway=function(href){window.onbeforeunload=function(){},location.href=href},appDeps=appDeps.concat(["ngTouch","app.config","ui.router","nw","device","dcc.device","dcc","ngDialog","app-module-funcs","svgIcon"]);var app=angular.module("wizard",appDeps);app.constant("queryString",function(a){if(""==a)return{};var i,p,b={};for(i=0;i<a.length;++i)p=a[i].split("=",2),2==p.length&&(b[p[0]]=decodeURIComponent(p[1].replace(/\+/g," ")));return b}(window.location.search.substr(1).split("&"))),window.GET_PARAMS=function(a){if(""==a)return{};var i,p,b={};for(i=0;i<a.length;++i)p=a[i].split("="),2==p.length&&(b[p[0]]=decodeURIComponent(p[1].replace(/\+/g," ")));return b}(window.location.search.substr(1).split("&")),window.URL_HASH=window.GET_PARAMS?window.GET_PARAMS.redirect_id:"",app.constant("endpointConfig",{somovd:"/jsonrpc",devinfo:"/devinfo",dcc_remote_update:"/dcc_remote_update",dcc_update_gwif_connection:"/dcc_update_gwif_connection",dcc_get_rofiles:"/dcc_get_profiles",dcc_apply:"/dcc_apply"}),app.config(["$stateProvider","$injector","pageList","$urlRouterProvider","navigation","endpointConfig","$controllerProvider","$filterProvider","$compileProvider",function($stateProvider,$injector,pageList,$urlRouterProvider,navigation,endpointConfig,$controllerProvider,$filterProvider,$compileProvider){app.controllerProvider=$controllerProvider,app.filterProvider=$filterProvider,app.compileProvider=$compileProvider,_.each(navigation,function(elem,inx){var views,page,state=null;elem.page?_.isObject(elem.page)?(views={},_.each(elem.page,function(_elem,_inx){var view,_page=pageList[_elem];_page&&(console.log(_page.html),view={templateUrl:_.isArray(_page.html)?_.first(_page.html):_page.html,controller:_page.ctrl,controllerAs:_page.ctrlAlias},views[_inx]=view)}),state={views:views,url:elem.url,params:elem.url?void 0:elem.params}):(page=pageList[elem.page],state=page?{templateUrl:_.isArray(page.html)?_.first(page.html):page.html,controller:page.ctrl,controllerAs:page.ctrlAlias,url:elem.url,params:elem.params}:{template:'<div class="content_padding"><h3>Page not found</h3></div>',url:elem.url,params:elem.url?void 0:elem.params}):state={"abstract":!0,url:elem.url,template:"<ui-view/>"},state&&$stateProvider.state(inx,state)}),$urlRouterProvider.otherwise("/loading")}]),app.controller("wizardCtrl",["$q","$http","$rootScope","$scope","translate","$state","devinfo","$window","stepManager","$timeout","profiles","cookie","regions","device","detectLang","autoAuthStorage","endpointConfig","fieldConfig","supportInfoLoader","queryString","overlay-core","ports",function($q,$http,$rootScope,$scope,translate,$state,devinfo,$window,stepManager,$timeout,profiles,cookie,regions,device,detectLang,autoAuthStorage,endpointConfig,fieldConfig,supportInfoLoader,queryString,overlay,ports){function getWANStatus(ports){var etherwan=_.find(ports,function(port){return port.isEtherwan});if(etherwan)return etherwan.status>0;var wan=_.find(ports,function(port){return port.isWan});return wan&&wan.status>0}function initWizard(){function initWizardPorts(){return ports.pull().then(function(availPorts){$rootScope.rootBoxInfo={ports:_.map(_.sortBy(availPorts,"position"),function(o){return o.name})},$rootScope.rootAvailPorts=availPorts,$rootScope.rootLANPorts=_.filter(availPorts,function(port){return/^(port|LAN)\d$/.test(port.name)&&!port.isWan}),$rootScope.rootWANPorts=_.filter(availPorts,function(port){return/^(internet|port5|WAN)$/.test(port.name)&&port.isWan}),$rootScope.rootWANPort=_.first($rootScope.rootWANPorts)},function(){return angularDelay(1500).then(function(){return console.log(">>>>> RELOAD initWizardPorts"),initWizardPorts()})})}function initWizard(){return function(){return devinfo.once("notice").then(function(data){return angularDelay(3e3).then(function(){return data})})}().then(function(info){var data;return info&&_.isBoolean(info.factorySettings)?(isResetPageFlag()&&(info.factorySettings=!0),console.log("is factory ==",info.factorySettings),info.factorySettings?(devinfo.init({need_auth:!0}),$rootScope.gNeedAuth=!0,$rootScope.gAutoAuth=!0):(data=autoAuthStorage.getData(),data&&$rootScope.setAutoAuth(data.username,data.password)),Promise.resolve()):refreshInitWizard()},refreshInitWizard)}function initWizardData(){return function(){var areas=["net","version","wifi","wifi_general","notice","ports","box","client"].join("|");return devinfo.once(areas)}().then(function(data){var lang;return data&&data.version?(isResetPageFlag()&&(data.factorySettings=!0),1==_.size($rootScope.rootLangs)?translate.changeLanguage($rootScope.rootLangs[0]):data.factorySettings?(lang=detectLang(),translate.changeLanguage(lang),console.log("use browser lang:",lang)):(console.log("use devinfo lang:",data.lang),translate.changeLanguage(data.lang)),$rootScope.gDeviceInfo=data,_.size($rootScope.gDeviceInfo.supportMail)&&($rootScope.gDeviceInfo.supportMail=$rootScope.gDeviceInfo.supportMail.replace(/>|</g,"")),$rootScope.gIsDSL=data.deviceClass&&"DSL"==data.deviceClass,$rootScope.gWANStatus=data.dsl?!!parseInt(data.dsl.adslTrainingState):getWANStatus(data.availPorts),$rootScope.rootStartState=window.dbgStartState?window.dbgStartState:data.deviceStartState,data.client&&($rootScope.rootIsWIFIClient="WLAN"==data.client.name),$rootScope.rootAutoGeneratedSSID=autoconf.BR2_PACKAGE_ANWEB_SSIDGEN||autoconf.BR2_PACKAGE_ANWEB_CUSTOM_ISRAEL_38418||autoconf.BR2_PACKAGE_ANWEB_SSID_ON_FLASH&&data.wifi_general&&/^dlink-/.test(data.wifi_general.f24.ssid),$rootScope.rootIsSupport5G=data.wifi_general&&data.wifi_general.f5.exists,$rootScope.rootDeviceIP=data.lan&&data.lan.length?data.lan[0].ip:"",$rootScope.rootNoRebootLAN=data.lanNeedReboot===!1,$rootScope.devMode=data.deviceMode?{device_mode:data.deviceMode}:{},Promise.resolve(data)):refreshInitWizardData()},refreshInitWizardData)}function refreshInitWizard(){return angularDelay(1500).then(function(){return console.log(">>>>> RELOAD initWizard"),initWizard()})}function refreshInitWizardData(){return angularDelay(1500).then(function(){return console.log(">>>>> RELOAD initData"),initWizardData()})}return initWizard().then(device.authCheck.bind(device)).then(initWizardPorts).then(initWizardData).then(function(initData){return stepManager.init(initData).then(angularDelay.bind(null,1e3)).then(function(){$rootScope.isInited=!0,$rootScope.showOverlay(!1),$rootScope.showView(!0),$rootScope.$broadcast("FirstDevinfoPulled",initData),clearResetPageFlag()})}).then(function(){})["catch"](console.log.bind(console))}function angularDelay(duration){return new Promise(function(resolve){$timeout(resolve,duration)})}function isResetPageFlag(){return!!sessionStorage.wizard_after_reset_page}function clearResetPageFlag(){sessionStorage.removeItem("wizard_after_reset_page")}$rootScope.autoconf=autoconf,$rootScope.rootLangs=autoconf.__lang_arr,$rootScope.rootUpdateChecked=!1,$rootScope.rootUpdateVersion="",$rootScope.rootReqErrorCount=0,$rootScope.rootAdminURL="/admin",$rootScope.gWANStatus=!1,$rootScope.gDongleData=null,$rootScope.gDeviceAvail=!0,$rootScope.gAvailOverlay=!0,$rootScope.gAvailPorts=[],$rootScope.gDeviceInfo={},$rootScope.gIsDSL=!1,$rootScope.gReversPortNames=!1,$rootScope.gIsShowOverlay=!1,$rootScope.gShowAuthDialog=!1,$rootScope.gAwayURL=stepManager.getAwayURL(),$rootScope.gAutoAuth=!0,$rootScope.gAutoAuthSkipErr=!1,$rootScope.gShowView=!0,$rootScope.gFirstCableStep="checkcable",$rootScope.gLanPorts=[],$rootScope.gPorts=[],$rootScope.device=device,$rootScope.gAuth={username:autoconf.BR2_PACKAGE_ANWEB_ADMIN_NAME,password:autoconf.BR2_PACKAGE_ANWEB_ADMIN_PASS},devinfo.init({need_auth:!1}),$rootScope.getCredentialsCallback=["$rootScope","ngDialog","authParams","$http","$q","authHelper",function($rootScope,ngDialog,authParams,$http,$q,authHelper){function dialog(){return $rootScope.showOverlay(!1),ngDialog.open({template:autoconf.BR2_PACKAGE_ANWEB_HQ_ADMIN_APP?"../general/dialogs/hq_login2/dialog.tpl.html":"../general/dialogs/login2/dialog.tpl.html",className:"login_dialog",closeByDocument:!1,closeByEscape:!1,controller:"LoginDialog2Ctrl",showClose:!1,data:{authReason:authParams.authReason,staySignedIn:authParams.staySignedIn,getErrorMessage:authHelper.getErrorMessage,login:authHelper.login}}).closePromise.then(function(data){return data.value})}var deferred,username,password;return $rootScope.gAutoAuth?(deferred=$q.defer(),username=$rootScope.gAuth.username,password=$rootScope.gAuth.password,authHelper.login(username,password,!1).then(function(data){deferred.resolve(data)})["catch"](function(){dialog().then(deferred.resolve)["catch"](deferred.reject)})["finally"](function(){(username!==$rootScope.autoconf.BR2_PACKAGE_ANWEB_ADMIN_NAME||password!==$rootScope.autoconf.BR2_PACKAGE_ANWEB_ADMIN_PASS)&&($rootScope.gAutoAuth=!1)}),deferred.promise):dialog()}],$rootScope.$watch("gIsShowOverlay",function(value){return value&&!$rootScope.gShowAuthDialog?void overlay.simple.start({id:1}):void overlay.simple.stop({id:1})}),$rootScope.$watch("gDeviceAvail",function(value){return!value&&$rootScope.gAvailOverlay?void overlay.notAvailable.start("basic",3):void overlay.notAvailable.stop()}),window.onbeforeunload=function(){return translate("dcc_unsaved_warn")},translate.changeLanguage(1==_.size($rootScope.rootLangs)?$rootScope.rootLangs[0]:"eng"),$scope.$on("NeedAuth",function(){$rootScope.rootAuthDialogIsOpen=!0}),$scope.$on("Authed",function(){$rootScope.rootAuthDialogIsOpen=!1}),$rootScope.getRedirectUrl=$scope.getRedirectUrl=function(){return URL_HASH?document.location.protocol+"//"+document.location.host+"/redirect_to?redirect_id="+URL_HASH:$scope.gAwayURL},$rootScope.isAutoupdateEnabled=function(){return!$scope.rootUpdateChecked},$rootScope.exitFromWizard=function(firstData){firstData?device.system.save().then(function(){goAway($rootScope.rootAdminURL)}):goAway($rootScope.rootAdminURL)},$rootScope.setAutoAuth=function(username,password){autoAuthStorage.setData(username,password),$rootScope.gAutoAuth=!0,$rootScope.gAuth={username:username,password:password}},$rootScope.setAutoAuthFromProfile=function(){$scope.nativeData&&$scope.nativeData.Config.SystemPassword&&$scope.setAutoAuth($scope.nativeData.Config.SystemPassword.Login,$scope.nativeData.Config.SystemPassword.Password)},$rootScope.setNeedAuth=function(val){$rootScope.gNeedAuth=val},$rootScope.reauth=function(){return $rootScope.gAutoAuth=!1,devinfo.once("net")},$rootScope.nextStep=function(params){var name=$state.$current.name;stepMap[name]&&stepMap[name].next&&$state.go(stepMap[name].next,params)},$rootScope.prevStep=function(params){var name=$state.$current.name;stepMap[name]&&stepMap[name].prev&&$state.go(stepMap[name].prev,params)},$rootScope.scrollToTop=function(){_.defer(function(){document.body.scrollTop=0,document.documentElement.scrollTop=0})},$rootScope.showOverlay=function(val){$rootScope.gIsShowOverlay=val},$rootScope.showAvailOverlay=function(val){$rootScope.gAvailOverlay=val},$rootScope.showView=function(val){$rootScope.gShowView=val},$rootScope.overlayIsShowed=function(){return $rootScope.gIsShowOverlay},$rootScope.searchStateLogic=function(data){$rootScope.providers=_.groupBy(data,"Provider"),_.size($rootScope.providers)>1?$state.go("provlist"):1==_.size($rootScope.providers)?($rootScope.servicelist=_.find($rootScope.providers,function(){return!0}),_.size($rootScope.servicelist)>1?$state.go("servicelist"):1==_.size($rootScope.servicelist)?($rootScope.selectedProfile=_.first($rootScope.servicelist),$state.go("master")):$state.go("search_fail")):$state.go("search_fail")},$scope.$on("$stateChangeSuccess",function(event,state,params,fromState){$rootScope.showAvailOverlay(!0),$rootScope.previousState=fromState.name,$rootScope.currentState=state.name}),$rootScope.showOverlay(!0),$rootScope.showView(!1),$scope.translateWifiSecurityMode=function(mode){return"None"==mode?"Open":mode},$scope.translatePinStatus=function(status){return"PUK"==status?"PUK":"puk2_required"==status?"PUK2":"PIN"},$scope.showVLANSection=function(){return $scope.nativeData&&$scope.nativeData.Config.VLAN&&_.size($scope.nativeData.Config.VLAN.services)>0},$scope.showPortPanel=function(){return $scope.nativeData&&$scope.nativeData.Config.VLAN?_.find($scope.nativeData.Config.VLAN.groups.all,function(port){return port.service}):!1},devinfo.onceAndSubscribe("dongle",function(data){data&&($rootScope.gDongleData=data.dongle)}),$rootScope.$watch("gDongleData.status",function(status){$rootScope.rootDongleReadyStamp="ready"==status?_.now():null,console.log("rootDongleReadyStamp is",$rootScope.rootDongleReadyStamp)}),initWizard(),window.P=function(native){$rootScope.selectedProfile=native,$state.go("master")},window.$rootScope=$rootScope}])}();