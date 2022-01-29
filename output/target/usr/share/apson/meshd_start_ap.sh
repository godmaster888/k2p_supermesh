#!/bin/sh
brif=${1}
ifname=${2}
security=${3}
encrypt=${4}
ssid=${5}

[ -L /sys/class/net/${ifname} ] && {
	# for openwrt-sdk only. remove this part before release to vendor. [START]
	# MAC address of MTK Wi-Fi interface will be "00:00:00:00:00:00" while it isn't ready.
	wait_for_driver=0
	ifconfig ${ifname}|grep "00:00:00:00:00:00" > /dev/null 2>&1
	while [ $? -eq 0 ]
	do
		wait_for_driver=1
		sleep 1
		ifconfig ${ifname}|grep "00:00:00:00:00:00" > /dev/null 2>&1
	done
	# waiting for driver ready really
	[ ${wait_for_driver} -eq 1 ] && sleep 2
	# for openwrt-sdk only. remove this part before release to vendor. [END]

	ifconfig ${ifname}|grep UP > /dev/null 2>&1
	[ $? -ne 0 ] && {
		ifconfig ${ifname} up
		sleep 1
	}

	iwpriv ${ifname} set AuthMode=${security}
	iwpriv ${ifname} set EncrypType=${encrypt}
	iwpriv ${ifname} set HideSSID=1
	iwpriv ${ifname} set ApMWDS=1
	iwpriv ${ifname} set SSID=${ssid}
	iwpriv ${ifname} set IgmpSnEnable=0

	[ ! -d /sys/class/net/${brif}/brif/${ifname} ] && brctl addif ${brif} ${ifname}
}

