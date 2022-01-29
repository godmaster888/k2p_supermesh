#!/bin/bash

# HOWTO:
#
# Скрипт для запуска QEMU с полной эмуляцией железа X86, необходимого для работы
# виртуального роутера. На данный момент используется при работе с профилем DIR_QEMU.
#
# Для запуска нужно указать первым аргументом реально существующий в системе
# интерфейс, который будет связан с виртуальным LAN интерфейсом qemu.
# Опционально можно указать вторым аргументов другой интерфейс, он будет связан с
# WAN-интерфейсом qemu.
#
# Для предоставления QEMU доступа в реальную сеть будут созданы виртуальные
# TAP-интерфейсы, которые будут объединены в бридж с соответствующими им реальными
# интерфейсами.
#
# При наличии на реальном интерфейсе IP-адреса, он будет перенесен на бридж на время
# работы QEMU, затем будет восстановлен на реальный интерфейс.
#
# Консоль QEMU (/dev/console) будет связана с stdio оболочки.
#
# Для выхода из QEMU можно использовать poweroff -f из самой виртуальной системы,
# либо 'killall qemu-system-i386' из другого терминала.

ROOT=""

while [ "$PWD" != "/" ]; do
	if [ -f "$PWD/.sdk_root" ]; then
		ROOT=$PWD
		break;
	fi

	cd ..
done

if [ -z "$ROOT" ]; then
	echo "Must be run from the a sdk tree"
	exit 1
fi

case $EUID in
	0) : cool we are already root - fall through ;;
	*) # not root, become root for the rest of this session
		# (and ask for the sudo password only once)
		sudo $0 "$@"
		exit 1
		;;
esac

USERID=$(whoami)
QEMU_USER=${SUDO_USER:-$USERID}

echo "USER = $QEMU_USER"

START_INDEX=1

CURRENT_MAC="52:54:00"

NET_ARGS=""
TAPS=()
BRIDGES=()
iface_index=$START_INDEX

if [ -f $ROOT/output/images/qemu.config ]; then
	source $ROOT/output/images/qemu.config
else
	MEM=512M
fi

NPROC=2

success=true

if [ $# -eq 0 ]; then
	echo "Syntax: $0 <lan_iface> [<iface2>] [<ifacen>]"
	exit 1
fi

function run_cmd() {
	local cmd_line="$@... "
	printf "%-60s" "$cmd_line"

	$($@)
	local ret=$?

	if [ $ret -eq 0 ]; then
		echo "OK"
	else
		echo "FAIL[$ret]"
		success=false
	fi
}

function create_bridge() {
	# Get name of newly created TAP device; see https://bbs.archlinux.org/viewtopic.php?pid=1285079#p1285079
	local precreationg=$(ip -o link show | awk -F": " '{print $2}' | sort)
	run_cmd sudo ip link add type bridge
	local postcreation=$(ip -o link show | awk -F": " '{print $2}' | sort)
	IFACE=$(comm -13 <(echo "$precreationg") <(echo "$postcreation"))
}

function create_tap() {
	# Get name of newly created TAP device; see https://bbs.archlinux.org/viewtopic.php?pid=1285079#p1285079
	local precreationg=$(ip -o tuntap show | awk -F":" '{print $1}' | sort)
	run_cmd sudo ip tuntap add user $QEMU_USER mode tap
	local postcreation=$(ip -o tuntap show | awk -F":" '{print $1}' | sort)
	IFACE=$(comm -13 <(echo "$precreationg") <(echo "$postcreation"))
}

function delete_tap() {
	local IFACE=$1

	run_cmd sudo ip link set dev $IFACE down
	run_cmd sudo ip tuntap del $IFACE mode tap
}

function delete_bridge() {
	local IFACE=$1

	run_cmd sudo ip link del $IFACE
}

function qemu_ifup() {
	local br=$1
	local eth=$2
	local tap=$3

	local iplist=$(ip -4 addr show $eth | grep -oP "(?<=inet ).*" | cut -d' ' -f1)

	for ip in $iplist; do
		run_cmd sudo ip addr del $ip dev $eth
	done

	run_cmd sudo ip link set $tap up promisc on
	run_cmd sudo ip link set $eth master $br
	run_cmd sudo ip link set $tap master $br
	run_cmd sudo ip link set $br up

	for ip in $iplist; do
		run_cmd sudo ip addr add $ip dev $br
	done
}

function qemu_ifdown() {
	local br=$1
	local eth=$2
	local tap=$3

	local iplist=$(ip -4 addr show $br | grep -oP "(?<=inet ).*" | cut -d' ' -f1)

	for ip in $iplist; do
		run_cmd sudo ip addr del $ip dev $br
	done

	run_cmd sudo ip link set $tap down
	run_cmd sudo ip link set $br down
	run_cmd sudo ip link set $eth nomaster
	run_cmd sudo ip link set $tap nomaster

	for ip in $iplist; do
		run_cmd sudo ip addr add $ip dev $eth
	done
}

function convert_mac() {
	local IFACE=$1
	local IFACE_MAC
	local iface_hash

	IFACE_MAC=$(ip link show $IFACE | grep -oP "(?<=link/ether ).*" | cut -d' ' -f1)

	iface_hash=$(echo -n "$IFACE $IFACE_MAC" | md5sum | cut -d ' ' -f1)
	iface_mac_end_dotted=${iface_hash:0:2}:${iface_hash:2:2}:${iface_hash:4:2}

	MAC=$CURRENT_MAC:$iface_mac_end_dotted

	echo "MAC for $IFACE is $MAC"
}

function qemu_net_arg() {
	local iface=$1
	local mac=$2
	local vlan="mynet$3"

	echo -netdev tap,id="$vlan",ifname="$iface",script=no,downscript=no -device virtio-net,id=link-$iface,netdev="$vlan",mac="$mac"
}

function qemu_usbnet_arg() {
	local iface=$1
	local mac=$2
	local vlan="mynet$3"

	echo -usb -netdev tap,id="$vlan",ifname="$iface",script=no,downscript=no -device usb-net,id=usb-$iface,netdev="$vlan",mac="$mac"
}

#-----------------------

printf "%-60s" "Checking kvm..."

if [ -c "/dev/kvm" ]; then
	echo "YES"
	KVM=-enable-kvm

	if ! sudo -iu $QEMU_USER test -w "/dev/kvm"; then
		echo "Root access is required for KVM. Please append $USER to KVM group."
		QEMU_USER=$USERID
	fi
else
	echo "NO"
fi

printf "%-60s" "Getted memory ..."
echo "$MEM"

# Подготавливаем нужные TAP-устройства, которые свяжут QEMU с реальной сетью
echo "> Creating TAP..."

for iface in $*; do
	convert_mac ${iface//--usbnet=}

	create_tap; current_tap=$IFACE
	create_bridge; current_bridge=$IFACE
	current_mac=$MAC

	TAPS+=($current_tap)
	BRIDGES+=($current_bridge)

	qemu_ifup $current_bridge ${iface//--usbnet=} $current_tap

	if [[ $iface == --usbnet=* ]]; then
		NET_ARGS="$NET_ARGS $(qemu_usbnet_arg $current_tap $current_mac $iface_index)"
	else
		NET_ARGS="$NET_ARGS $(qemu_net_arg $current_tap $current_mac $iface_index)"
	fi

	iface_index=$((iface_index + 1))
done

VIRTFS="-virtfs local,path=$ROOT/output/,mount_tag=host0,security_model=mapped-file,id=host0"

printf "%-60s" "Checking additional storage drive..."
if [ "$USE_STORAGE" = "y" ]; then
	echo "YES"

	if [ -z $STORAGE_DISK ]; then
		STORAGE_DISK="$ROOT/output/images/storage.ext2"
	fi

	if ! [ -f "$STORAGE_DISK" ]; then
		printf "%-60s" "Creating drive..."
		sudo -iu $QEMU_USER truncate -s16M $STORAGE_DISK
		sudo -iu $QEMU_USER mke2fs $STORAGE_DISK
	fi

	STORAGE_DRIVE="-drive file=$STORAGE_DISK,index=1,media=disk,format=raw"
else
	echo "NO"
	STORAGE_DRIVE=""
fi

# Если все поднято успешно, то запускаем виртуальную машину
if $success; then
	echo "> QEMU run (1 sec)..."
	sleep 1

	sudo -iu $QEMU_USER qemu-system-i386 \
		-kernel $ROOT/output/images/bzImage \
		-drive file=$ROOT/output/images/rootfs.ext2,index=0,media=disk,format=raw \
		$STORAGE_DRIVE \
		-smp $NPROC \
		-m $MEM \
		$KVM \
		-append "console=ttyS0 root=/dev/sda rw storage=/dev/sdb rw virtfs_tag=host0" \
		$NET_ARGS \
		$VIRTFS \
		-nographic

else
	echo "> Reverting changes..."
fi

echo "> ------"


# Удаляем TAP-интерфейсы из бриджей и сам бриджи, возвращаем IP на реальные интерфейсы
echo "> Unloading LAN network..."

iface_index=$START_INDEX
for iface in $*; do
	current_index=$((iface_index - 1))
	qemu_ifdown ${BRIDGES[$current_index]} ${iface//--usbnet=} ${TAPS[$current_index]}
	delete_tap ${TAPS[$current_index]}
	delete_bridge ${BRIDGES[$current_index]}

	iface_index=$((iface_index + 1))
done

echo "> Done."
