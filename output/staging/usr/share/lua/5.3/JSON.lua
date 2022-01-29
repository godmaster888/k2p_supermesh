local y='20170416.23'
local a="-[ JSON.lua package by Jeffrey Friedl (http://regex.info/blog/lua/json) version 20170416.23 ]-"local b={VERSION=y,AUTHOR_NOTE=a}local c="  "
local d={pretty=true,indent=c,align_keys=false,array_newline=false}
local e={__tostring=function()return"JSON array"end}e.__index=e
local f={__tostring=function()return"JSON object"end}f.__index=f
function b:newArray(x)return setmetatable(x or{},e)end;function b:newObject(x)return setmetatable(x or{},f)end
local function g(x)return
type(x)=='number'and x or x.N end
local h={__tostring=function(x)return x.S end,__unm=function(x)return g(x)end,__concat=function(x,z)
return tostring(x)..tostring(z)end,__add=function(x,z)return g(x)+g(z)end,__sub=function(x,z)
return g(x)-g(z)end,__mul=function(x,z)return g(x)*g(z)end,__div=function(x,z)return g(x)/g(z)end,__mod=function(x,z)return
g(x)%g(z)end,__pow=function(x,z)return g(x)^g(z)end,__lt=function(x,z)return
g(x)<g(z)end,__eq=function(x,z)return g(x)==g(z)end,__le=function(x,z)
return g(x)<=g(z)end}h.__index=h
function b:asNumber(x)
if getmetatable(x)==h then return x elseif type(x)=='table'and
type(x.S)=='string'and type(x.N)=='number'then return
setmetatable(x,h)else local z={S=tostring(x),N=tonumber(x)}return
setmetatable(z,h)end end
function b:forceString(x)if type(x)=='table'and type(x.S)=='string'then return x.S else return
tostring(x)end end
function b:forceNumber(x)if type(x)=='table'and type(x.N)=='number'then return x.N else return
tonumber(x)end end
function b:isNumber(x)if type(x)=='number'then return x elseif
type(x)=='table'and type(x.N)=='number'then return x.N else return nil end end
function b:isString(x)if type(x)=='string'then return x elseif
type(x)=='table'and type(x.S)=='string'then return x.S else return nil end end
local function i(x)
if x<=127 then return string.char(x)elseif x<=2047 then local z=math.floor(x/0x40)local A=x-
(0x40*z)return string.char(0xC0+z,0x80+A)elseif x<=65535 then local z=math.floor(
x/0x1000)local A=x-0x1000*z
local B=math.floor(A/0x40)local C=A-0x40*B;z=0xE0+z;B=0x80+B;C=0x80+C
if

(z==0xE0 and B<0xA0)or(z==0xED and B>0x9F)or
(z==0xF0 and B<0x90)or(z==0xF4 and B>0x8F)then return"?"else return string.char(z,B,C)end else local z=math.floor(x/0x40000)local A=x-0x40000*z
local B=math.floor(A/0x1000)A=A-0x1000*B;local C=math.floor(A/0x40)local D=A-0x40*C;return string.char(
0xF0+z,0x80+B,0x80+C,0x80+D)end end
function b:onDecodeError(x,z,A,B)if z then
if A then
x=string.format("%s at byte %d of: %s",x,A,z)else x=string.format("%s: %s",x,z)end end;if B~=nil then x=x.." ("..
b:encode(B)..")"end
if self.assert then
self.assert(false,x)else print(debug.traceback())assert(false,x)end end;function b:onTrailingGarbage(x,z,A,B)
return self:onDecodeError("trailing garbage",x,z,B)end
b.onDecodeOfNilError=b.onDecodeError;b.onDecodeOfHTMLError=b.onDecodeError
function b:onEncodeError(x,z)if z~=nil then x=x..
" ("..b:encode(z)..")"end;if self.assert then
self.assert(false,x)else assert(false,x)end end
local function j(x,z,A,B)
local C=z:match('^-?[1-9]%d*',A)or z:match("^-?0",A)if not C then x:onDecodeError("expected number",z,A,B.etc)
return nil,A end;local D=A+C:len()local E=
z:match('^%.%d+',D)or""D=D+E:len()
local F=z:match('^[eE][-+]?%d+',D)or""D=D+F:len()local G=C..E..F
if B.decodeNumbersAsObjects then local I=false
if

not B.decodeIntegerObjectificationLength and not B.decodeDecimalObjectificationLength then I=true elseif

(B.decodeIntegerObjectificationLength and(C:len()>=B.decodeIntegerObjectificationLength or
F:len()>0))or
(B.decodeDecimalObjectificationLength and(E:len()>=B.decodeDecimalObjectificationLength or
F:len()>0))then I=true end;if I then return b:asNumber(G),D end else
if

(B.decodeIntegerStringificationLength and
(C:len()>=
B.decodeIntegerStringificationLength or F:len()>0))or
(B.decodeDecimalStringificationLength and(E:len()>=B.decodeDecimalStringificationLength or
F:len()>0))then return G,D end end;local H=tonumber(G)if not H then
x:onDecodeError("bad number",z,A,B.etc)return nil,A end;return H,D end
local function k(x,z,A,B)if z:sub(A,A)~='"'then
x:onDecodeError("expected string's opening quote",z,A,B.etc)return nil,A end;local C=A+1
local D=z:len()local E=""
while C<=D do local F=z:sub(C,C)if F=='"'then return E,C+1 end
if F~='\\'then E=E..F;C=C+1 elseif
z:match('^\\b',C)then E=E.."\b"C=C+2 elseif z:match('^\\f',C)then E=E.."\f"C=C+2 elseif z:match('^\\n',C)then
E=E.."\n"C=C+2 elseif z:match('^\\r',C)then E=E.."\r"C=C+2 elseif z:match('^\\t',C)then E=E.."\t"C=C+2 else
local G=z:match('^\\u([0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF])',C)
if G then C=C+6;local H=tonumber(G,16)
if H>=0xD800 and H<=0xDBFF then
local I=z:match('^\\u([dD][cdefCDEF][0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF])',C)if I then C=C+6
H=0x2400+ (H-0xD800)*0x400+tonumber(I,16)else end end;E=E..i(H)else E=E..z:match('^\\(.)',C)C=C+2 end end end;x:onDecodeError("unclosed string",z,A,B.etc)return nil,A end
local function l(x,z)local A,B=x:find("^[ \n\r\t]+",z)if B then return B+1 else return z end end;local m
local function n(x,z,A,B)if z:sub(A,A)~='{'then
x:onDecodeError("expected '{'",z,A,B.etc)return nil,A end;local C=l(z,A+1)local D=x.strictTypes and
x:newObject{}or{}if z:sub(C,C)=='}'then return D,
C+1 end;local E=z:len()
while C<=E do local F,G=k(x,z,C,B)C=l(z,G)if
z:sub(C,C)~=':'then x:onDecodeError("expected colon",z,C,B.etc)
return nil,C end;C=l(z,C+1)local H,G=m(x,z,C,B)
D[F]=H;C=l(z,G)local I=z:sub(C,C)if I=='}'then return D,C+1 end;if z:sub(C,C)~=','then
x:onDecodeError("expected comma or '}'",z,C,B.etc)return nil,C end;C=l(z,C+1)end;x:onDecodeError("unclosed '{'",z,A,B.etc)return nil,A end
local function o(x,z,A,B)if z:sub(A,A)~='['then
x:onDecodeError("expected '['",z,A,B.etc)return nil,A end;local C=l(z,A+1)local D=x.strictTypes and
x:newArray{}or{}if z:sub(C,C)==']'then
return D,C+1 end;local E=1;local F=z:len()
while C<=F do local G,H=m(x,z,C,B)D[E]=G;E=E+1
C=l(z,H)local I=z:sub(C,C)if I==']'then return D,C+1 end;if z:sub(C,C)~=','then
x:onDecodeError("expected comma or ']'",z,C,B.etc)return nil,C end;C=l(z,C+1)end;x:onDecodeError("unclosed '['",z,A,B.etc)return nil,C end
m=function(x,z,A,B)A=l(z,A)if A>z:len()then
x:onDecodeError("unexpected end of string",z,nil,B.etc)return nil,A end
if z:find('^"',A)then return
k(x,z,A,B.etc)elseif z:find('^[-0123456789 ]',A)then return j(x,z,A,B)elseif z:find('^%{',A)then
return n(x,z,A,B)elseif z:find('^%[',A)then return o(x,z,A,B)elseif z:find('^true',A)then return true,A+4 elseif z:find('^false',A)then return
false,A+5 elseif z:find('^null',A)then return nil,A+4 else
x:onDecodeError("can't parse JSON",z,A,B.etc)return nil,1 end end
function b:decode(x,z,A)if type(A)~='table'then A={}end;if z~=nil then A.etc=z end
if type(self)~=
'table'or self.__index~=b then
local E="JSON:decode must be called in method format"b:onDecodeError(E,nil,nil,A.etc)return nil,E end
if x==nil then local E="nil passed to JSON:decode()"
self:onDecodeOfNilError(E,nil,nil,A.etc)return nil,E elseif type(x)~='string'then local E="expected string argument to JSON:decode()"
self:onDecodeError(string.format("%s, got %s",E,type(x)),
nil,nil,A.etc)return nil,E end;if x:match('^%s*$')then return nil end
if x:match('^%s*<')then
local E="HTML passed to JSON:decode()"self:onDecodeOfHTMLError(E,x,nil,A.etc)return nil,E end
if x:sub(1,1):byte()==0 or(x:len()>=2 and
x:sub(2,2):byte()==0)then
local E="JSON package groks only UTF-8, sorry"self:onDecodeError(E,x,nil,A.etc)return nil,E end;if A.decodeNumbersAsObjects==nil then
A.decodeNumbersAsObjects=self.decodeNumbersAsObjects end;if
A.decodeIntegerObjectificationLength==nil then
A.decodeIntegerObjectificationLength=self.decodeIntegerObjectificationLength end;if
A.decodeDecimalObjectificationLength==nil then
A.decodeDecimalObjectificationLength=self.decodeDecimalObjectificationLength end;if
A.decodeIntegerStringificationLength==nil then
A.decodeIntegerStringificationLength=self.decodeIntegerStringificationLength end;if
A.decodeDecimalStringificationLength==nil then
A.decodeDecimalStringificationLength=self.decodeDecimalStringificationLength end
local B,C,D=pcall(m,self,x,1,A)
if B then local E=nil;if D~=#x+1 then D=l(x,D)if D~=#x+1 then
C,E=self:onTrailingGarbage(x,D,C,A.etc)end end
return C,E else local E=C
if self.assert then self.assert(false,E)else assert(false,E)end;return nil,E end end
local function p(x)
if x=="\n"then return"\\n"elseif x=="\r"then return"\\r"elseif x=="\t"then return"\\t"elseif x=="\b"then return"\\b"elseif x=="\f"then return"\\f"elseif x=='"'then return
'\\"'elseif x=='\\'then return'\\\\'else
return string.format("\\u%04x",x:byte())end end;local q='['..'"'..
'%\\'..'%z'..'\001'..'-'..'\031'..']'
local r=i(0x2028)local s=i(0x2029)
local function t(x,z)local A=x:gsub(q,p)if z.stringsAreUtf8 then
A=A:gsub(r,'\\u2028'):gsub(s,'\\u2029')end;return'"'..A..'"'end
local function u(x,z,A)local B={}local C={}local D=false;local E
for G in pairs(z)do
if type(G)=='string'then
table.insert(B,G)elseif type(G)=='number'then table.insert(C,G)if G<=0 or G>=math.huge then
D=true elseif not E or G>E then E=G end else
x:onEncodeError(
"can't encode table with a key of type "..type(G),A)end end
if#B==0 and not D then
if#C>0 then return nil,E elseif tostring(z)=="JSON array"then return nil elseif tostring(z)==
"JSON object"then return{}else return nil end end;local F
if#C>0 then if x.noKeyConversion then
x:onEncodeError("a table with both numeric and string keys could be an object or array; aborting",A)end;F={}for G,H in pairs(z)do
F[G]=H end
for G,H in ipairs(C)do local I=tostring(H)
if F[I]==nil then
table.insert(B,I)F[I]=z[H]else
x:onEncodeError("conflict converting table with mixed-type keys into a JSON object: key "..H..
" exists both as a string and a number.",A)end end end;return B,nil,F end;local v
function v(x,z,A,B,C,D,E)
if z==nil or
(not E and C and C.null and z==C.null)then return'null'elseif type(z)=='string'then return t(z,C)elseif
type(z)=='number'then
if z~=z then return"null"elseif z>=math.huge then return"1e+9999"elseif z<=-math.huge then return"-1e+9999"else return
tostring(z)end elseif type(z)=='boolean'then return tostring(z)elseif type(z)~='table'then
x:onEncodeError("can't convert "..
type(z).." to JSON",B)elseif getmetatable(z)==h then return tostring(z)else local F=z
if type(C)~='table'then C={}end;if type(D)~='string'then D=""end;if A[F]then
x:onEncodeError("table "..
tostring(F).." is a child of itself",B)else A[F]=true end;local G
local H,I,J=u(x,F,B)
if I then local K
if C.array_newline then K=D..tostring(C.indent or"")else K=D end;local L={}
for M=1,I do table.insert(L,v(x,F[M],A,B,C,K))end
if C.array_newline then G="[\n"..
K..table.concat(L,",\n"..K).."\n"..D.."]"elseif C.pretty then G="[ "..
table.concat(L,", ").." ]"else G="["..
table.concat(L,",").."]"end elseif H then local K=J or F
if C.pretty then local L={}local M=0;for R,S in ipairs(H)do
local T=v(x,tostring(S),A,B,C,D,true)if C.align_keys then M=math.max(M,#T)end
table.insert(L,T)end;local N=D..
tostring(C.indent or"")
local O=N..string.rep(" ",M)..
(C.align_keys and"  "or"")
local P="%s%"..string.format("%d",M).."s: %s"local Q={}for R,S in ipairs(H)do local T=v(x,K[S],A,B,C,O)
table.insert(Q,string.format(P,N,L[R],T))end;G="{\n"..table.concat(Q,",\n").."\n"..D..
"}"else
local L={}for M,N in ipairs(H)do local O=v(x,K[N],A,B,C,D)
local P=v(x,tostring(N),A,B,C,D,true)
table.insert(L,string.format("%s:%s",P,O))end;G="{"..
table.concat(L,",").."}"end else G="[]"end;A[F]=false;return G end end
local function w(x,z,A,B)local C=v(x,z,{},A,B)if C==nil then return C else return C end end
function b:encode(x,z,A)if type(self)~='table'or self.__index~=b then
b:onEncodeError("JSON:encode must be called in method format",z)end;if type(A)~='table'then
A={}end;return w(self,x,z,A)end
function b:encode_pretty(x,z,A)if type(self)~='table'or self.__index~=b then
b:onEncodeError("JSON:encode_pretty must be called in method format",z)end;if
type(A)~='table'then A=d end;return w(self,x,z,A)end;function b.__tostring()return"JSON encode/decode package"end
b.__index=b;function b:new(x)local z={}if x then for A,B in pairs(x)do z[A]=B end end;return
setmetatable(z,b)end
return b:new()