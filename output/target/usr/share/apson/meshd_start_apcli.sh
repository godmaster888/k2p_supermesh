#!/bin/sh
brif=${1}
ifname=${2}
security=${3}
encrypt=${4}
ssid=${5}
bssid=${6}
channel=${7}
ch_ifname=${8}

[ -L /sys/class/net/${ifname} ] && {
	# AP-Client had connected. do nothing.
	iwconfig ${ifname}|grep -i ${bssid} > /dev/null 2>&1
	[ $? -eq 0 ] && {
		return
	}

	ifconfig ${ifname}|grep UP > /dev/null 2>&1
	[ $? -ne 0 ] && {
		ifconfig ${ifname} up
		sleep 1
	}

	# as MTK suggest, use "set Channel" instead of "set ApCliChannel"
	[ ${channel} -ne 0 ] && {
		iwpriv ${ch_ifname} set Channel=${channel}
	}
	iwpriv ${ifname} set ApCliEnable=0
	iwpriv ${ifname} set ApCliAuthMode=${security}
	iwpriv ${ifname} set ApCliEncrypType=${encrypt}
	iwpriv ${ifname} set ApCliBssid=${bssid}
	iwpriv ${ifname} set ApCliSsid=${ssid}
	iwpriv ${ifname} set ApCliMWDS=1
	iwpriv ${ifname} set ApCliEnable=1

	[ ! -d /sys/class/net/${brif}/brif/${ifname} ] && brctl addif ${brif} ${ifname}
}

