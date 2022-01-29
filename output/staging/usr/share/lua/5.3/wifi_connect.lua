JSON=require("JSON")
function survey_sort(a,b)return a.SSID<b.SSID end;function instance_count(a)local b=0
for c,d in pairs(a)do if tonumber(c)~=nil then b=b+1 end end;return b end
confapi.login("admin","admin")confapi.set_timeout(20000)
printf("Choose Radio instance: ")io.flush()radio_num=tonumber(io.read())
radio=confapi.get_branch(
"Device.WiFi.Radio."..tostring(radio_num))radio_stat_link=radio.Statistics
if radio_num==nil or radio_num<1 or radio==
nil then print("Invalid index")os.exit()end
endpoints=confapi.get_branch("Device.WiFi.Radio."..tostring(radio_num)..".EndPoint.")endpoint_num=0
if instance_count(endpoints)>0 then
for a,b in pairs(endpoints)do if
tonumber(a)~=nil then endpoint_num=tonumber(a)break end end
print("Reuse Endpoint #"..tostring(endpoint_num))else print("Creating new Endpoint")
diff={Device={WiFi={Radio={[tostring(radio_num)]={EndPoint={["+1"]={}}}}}}}ret=confapi.apply(diff)
endpoint_num=tonumber(ret.result.InstanceNumberList["+1"])
if endpoint_num==nil then print("Cannot create EndPoint")os.exit()end end
survey=confapi.get_branch(radio_stat_link.."SiteSurvey.")surveydup={}for a,b in pairs(survey)do if(tonumber(a)~=nil)then
table.insert(surveydup,b)end end
survey=surveydup;table.sort(survey,survey_sort)last_ssid=""for a,b in pairs(survey)do
printf("%02i %-32s %18s %i\n",a,b.SSID,b.BSSID,b.Channel)last_ssid=b.SSID end;printf(
"Select AP [1-"..#survey.."]: ")
io.flush()ret=tonumber(io.read())
if
ret==nil or ret<1 or ret>#survey then print("Invalid index")os.exit()end;ap=survey[ret]
print("Connecting to '"..ap.SSID.."'")printf("Enter password: ")io.flush()password=io.read()if#
password<8 or#password>63 then
print("Invalid password")os.exit()end
endpoint_iface=confapi.get_branch(
"Device.WiFi.Radio."..tostring(radio_num)..
".EndPoint."..tonumber(endpoint_num)..".Interface.")
dhcp_list=confapi.get_branch("Device.Network.Connection.DHCP.")dhcp_num="+4"
for a,b in pairs(dhcp_list)do if tonumber(a)~=nil then if b.Interface==endpoint_iface then
dhcp_num=a;break end end end
diff={Device={WiFi={StationProfile={["+1"]={SSID=ap.SSID,Security={KeyPassphrase=password,EncryptionType=ap.EncryptionType,ModeEnabled=ap.ModeEnabled}}},Radio={[tostring(radio_num)]={EndPoint={[tostring(endpoint_num)]={Enable=true,Profile="Device.WiFi.StationProfile.+1."}}}}},Network={Connection={DHCP={[dhcp_num]={Enable=true,Interface=endpoint_iface}}},Group={["1"]={WAN={["+2"]={ConnectivityCheckEnable=true,Link=
"Device.Network.Connection.DHCP."..dhcp_num.."."}}}}},Firewall={IPv4={Masquerade={["+3"]={Source={Interface="Device.Network.Interface.Bridge.1."},Dest={Interface=endpoint_iface}}}}}}}print()print("Connecting...")
ret=confapi.apply(diff)staprofile=ret.result.InstanceNumberList["+1"]
wangroup_item=ret.result.InstanceNumberList["+2"]masquerade=ret.result.InstanceNumberList["+3"]
if
dhcp_num=="+4"then dhcp_num=ret.result.InstanceNumberList[dhcp_num]end
confapi.set("Device.Network.Connection.DHCP."..tostring(dhcp_num)..".Name",
"WiFiClient-"..
radio.OperatingFrequencyBand.."-#"..tostring(staprofile))print()print("Created/used:")
printf("EndPoint:        Device.WiFi.Radio.%d.EndPoint.%s.\n",radio_num,endpoint_num)
printf("Station profile: Device.WiFi.StationProfile.%s.\n",staprofile)
printf("DHCP-client:     Device.Network.Connection.DHCP.%s.\n",dhcp_num)
printf("WANgroup entry:  Device.Network.Group.1.WAN.%s.Link -> Device.Network.Connection.DHCP.%s.\n",wangroup_item,dhcp_num)
printf("Masquerade:      Device.Firewall.IPv4.Masquerade.%s. (LAN -> %s)\n\n",wangroup_item,endpoint_iface)print("All done!")