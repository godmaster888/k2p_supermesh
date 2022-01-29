"use strict";angular.module(regdep("app.page-details"),["app.config","app.navigation"]).factory("pageDetails",["navigation","pageList",function(navigation,pageList){function getUrl(state){var ancs=getStateAncestors(state),parts=[];for(var s in ancs)parts.push(navigation[ancs[s]].url);return"#"+parts.join("")}function getMenuPath(state){var menu,ancs=getStateAncestors(state),parts=[];for(var s in ancs)menu=navigation[ancs[s]].menu,menu&&parts.push(menu.name);return parts}function makeInfoObj(state,page){var menu,obj={state:state,url:getUrl(state,"url"),features:page.features};navigation[state].title&&(obj.title=navigation[state].title);var menuState=findMenuState(state);return menuState&&(menu=navigation[menuState].menu,obj.menu={name:menu.name,desc:menu.desc,path:getMenuPath(state),state:menuState}),obj}function findMenuState(state){var i,s,ancs=getStateAncestors(state);for(i=ancs.length-1;i>=0;i--)if(s=navigation[ancs[i]],s.menu&&s.menu.sref==state)return ancs[i];return null}function getStateAncestors(state){var i,ancestors=[],parts=state.split(".");for(i=1;i<=parts.length;i++)ancestors.push(parts.slice(0,i).join("."));return ancestors}var navs,page,obj,defaultView,pages={};for(var pageName in pageList){navs=[],page=pageList[pageName];for(var key in navigation)obj=navigation[key],_.isString(obj.page)?obj.page==pageName&&navs.push(makeInfoObj(key,page)):_.isObject(obj.page)&&(defaultView=obj.page[""],defaultView==pageName&&navs.push(makeInfoObj(key,page)));navs.length&&(pages[pageName]=navs)}return pages}]);