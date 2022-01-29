"use strict";!function(){angular.module("app").service("vpnUsersUtil",["funcs","cpe","cpex",function(funcs,cpe,cpex){function getConfig(){return config?config:[]}var path="Device.Network.Accounts.",config=null,attrs=null,__initConfig=null;return{pull:function(){return Promise.all([cpe.GetParameterValues([path]),cpe.GetParameterAttributes([path])]).then(function(response){var data=funcs.buildTree(response[0].result.ParameterList);return attrs=funcs.buildTreeAttributes(response[1].result.ParameterList),__initConfig=funcs.deepClone(data),config=function(data){return data=funcs.newConfig.normalize(funcs.fetchBranch(data,path)),_.map(data,function(elem,key){return elem.id=key,elem})}(data),Promise.resolve()},function(){return Promise.reject()})},apply:function(settings){var userPath=path+(settings.user.id?settings.user.id:"+1"),diff={};return funcs.setValue(userPath+".Login",settings.user.Login,diff),funcs.setValue(userPath+".Password",settings.user.Password,diff),cpe.ApplyDifference(diff)},getConfig:getConfig,removeUsers:function(items){function remove(links){if(0!=_.size(links))return Promise.reject("links");var newConfig=funcs.deepClone(__initConfig);_.each(items,function(item){var userPath=path+item.id;funcs.cutBranch(newConfig,userPath)});var diff=funcs.newConfig.makeDiff(__initConfig,newConfig,attrs);return cpe.ApplyDifference(diff)}return Promise.resolve(items).then(function(items){var links=[],queue=_.map(items,function(item){var userPath=path+item.id;return cpex.checkLinks(userPath).then(function(result){return links=links.concat(result),Promise.resolve()})});return Promise.all(queue).then(function(){return Promise.resolve(links)})}).then(remove)}}}])}();