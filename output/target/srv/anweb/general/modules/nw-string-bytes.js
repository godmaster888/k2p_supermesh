"use strict";!function(){var nw=angular.module(regdep("nw-string-bytes"),[]);nw.directive("nwStringBytesMax",function(){return{restrict:"A",require:"ngModel",link:function($scope,$element,$attrs,$model){function validator(value){var len=$scope.$eval($attrs.nwStringBytesMax),isLessThanAttr=(new TextEncoder).encode(value).length<=len;return $attrs.ngMaxlength||$attrs.disabled||_.isUndefined(value)||!value.toString().length?$model.$setValidity("msg_invalid_name_length",!0):$model.$setValidity("msg_invalid_name_length",isLessThanAttr),value}$model.$parsers.unshift(validator),$model.$formatters.unshift(validator)}}})}();