"use strict";!function(){angular.module(regdep("navfilters-generic"),[]).run(["navigationFilter","cookie",function(nfilter,cookie){nfilter.addFilter(function(navs,menus){var deviceMode=cookie.get("device_mode"),exclude=[];"ap"===deviceMode?_.each(navs,function(state,name){state.hideInAPMode&&exclude.push(name)}):"Firewall"==deviceMode?_.each(navs,function(state,name){state.hideInFirewallMode&&exclude.push(name)}):_.each(navs,function(state,name){state.hideInRouterMode&&exclude.push(name)});var telnetMenu=menus.filter(function(o){return"telnet"==o.id});return navs["system.telnet"]&&telnetMenu[0]&&(navs["system.telnet"].title="telnet_ssh_nav",telnetMenu[0].name="telnet_ssh_nav"),_.omit(navs,exclude)}),nfilter.addRules(function(rules){return rules.HideChangePasswordDialog=autoconf.BR2_PACKAGE_ANWEB_SWITCH,rules})}])}();