#!/bin/bash

BACKUP=$1
PROFILE=${2:-$(cat .last_profile)}
DECODER=$(realpath ./output/host/usr/sbin/config_decoder)
DATAMODEL_DIR=$(realpath ./output/build/datamodel-*)

BASE_DM=$(realpath ./output/build/datamodel-*/buildroot-build/dm.json | tail -n 1)

profile_dir="./profiles/$PROFILE"
profile_file="$profile_dir/$PROFILE"

if ! command -v jq > /dev/null; then
	echo "Please install 'jq':"
	echo
	echo "	sudo apt install jq"
	exit 1
fi

is_enabled() {
	grep "$1=y" "$profile_file" >/dev/null 2>&1
}

devmode_human_list="Router AccessPoint Firewall"

devmode_to_human()
{
	case "$1" in
		dir )      echo "Router";;
		dap )      echo "AccessPoint";;
		firewall ) echo "Firewall";;
	esac
}

devmode_from_human()
{
	case "$1" in
		Router )      echo "dir";;
		AccessPoint ) echo "dap";;
		Firewall )    echo "firewall";;
	esac
}

convert_device_mode()
{
	devmode=$1
	prefix="$(devmode_from_human $devmode)"
	full_dm="$(realpath ./output/build/datamodel-*/buildroot-build/full_${prefix}_dm.json | tail -n 1)"

	# В резервной копии конфиг и права доступа лежат в виде JSON-объектов с ключами Config и Permissions
	cat "$tempdir/$devmode/config.json" | jq '.Config'      > "$tempdir/tmp.json" && mv "$tempdir/tmp.json" "$tempdir/$devmode/config.json"

	if is_enabled "BR2_CRYPTO_CONFIG_FULL"; then
		# расшифровываем весь конфиг
		echo "Decoding config..."
		mv "$tempdir/$devmode/config.json" "$tempdir/$devmode/config.bin"
		$DECODER decode "$tempdir/$devmode/config.bin" "$tempdir/$devmode/config.json" || exit 1
		$DECODER decode "$tempdir/$devmode/perms.bin" "$tempdir/$devmode/perms.json"
	fi

	if is_enabled "BR2_CRYPTO_CONFIG_PASSWORDS"; then
		# расшифровываем пароли в конфиге
		echo "Decoding secure fields..."
		$DECODER decrypt_secure "$BASE_DM" "$tempdir/$devmode/config.json" "$tempdir/$devmode/config.json" || exit 1
		$DECODER decode "$tempdir/$devmode/perms.bin" "$tempdir/$devmode/perms.json"
	fi

	if is_enabled "BR2_CRYPTO_CONFIG_ACCORDING_PERM"; then
		# восстанавливаем shadow
		echo "Merging shadow.bin..."
		$DECODER merge_shadow "$tempdir/$devmode/config.json" "$tempdir/$devmode/shadow.bin" "$tempdir/$devmode/config.json" || exit 1
		$DECODER decode "$tempdir/$devmode/perms.bin" "$tempdir/$devmode/perms.json"
	fi

	# собственно, конвертация конфига и прав доступа в нужный формат
	$DECODER backup_to_defconfig "${full_dm}" "$BASE_DM" "$tempdir/$devmode/config.json" "$profile_dir/${prefix}_config.default" || exit 1

	if [ "$(cat $tempdir/$devmode/perms.json)" != "null" ]; then
		cat "$tempdir/$devmode/perms.json"  | jq '.Permissions' > "$tempdir/tmp.json" && mv "$tempdir/tmp.json" "$tempdir/$devmode/perms.json"
	elif [ "$(cat $tempdir/$devmode/perm.json)" != "null" ]; then
		cat "$tempdir/$devmode/perm.json"  | jq '.Permissions' > "$tempdir/tmp.json" && mv "$tempdir/tmp.json" "$tempdir/$devmode/perms.json"
	fi

	if [ "$(cat $tempdir/$devmode/perms.json)" != "null" ]; then
		$DECODER backup_to_permission "$tempdir/$devmode/perms.json" "$profile_dir/${prefix}_permission.default" || exit 1
	fi
}

# Создаем временную директорию и удаляем ее при завершении скрипта
tempdir=$(mktemp -d)
trap "rm -rf $tempdir" EXIT

# Распаковываем backup.tar.gz во временную директорию
if ! tar -C $tempdir -xvf "$BACKUP"; then
	echo "Cannot unpack backup tarball"
	exit 1
fi

default_mode=$(grep BR2_DEFAULT_DEVICE_MODE "$profile_file" | cut -d'=' -f2 | tr -d '"')
default_mode_human=$(devmode_to_human $default_mode)

# Конвертер из старого формата (дамп дефолтного режима работы) в новый (дамп всех режимов работы по субдирам)
if [ ! -f "$tempdir/device_mode" ] && [ -f "$tempdir/config.json" ]; then
	mkdir -p "$tempdir/$default_mode_human"
	test -f "$tempdir/config.json" && mv "$tempdir/config.json" "$tempdir/$default_mode_human/config.json"
	test -f "$tempdir/perm.json"   && mv "$tempdir/perm.json"   "$tempdir/$default_mode_human/perms.json"
	test -f "$tempdir/perms.json"  && mv "$tempdir/perms.json"  "$tempdir/$default_mode_human/perms.json"
	echo "$default_mode_human" > "$tempdir/device_mode"
fi

device_mode="$(devmode_from_human $(cat "$tempdir/device_mode"))"

if [ "$device_mode" != "$default_mode" ]; then
	echo "DEVICE MODE HAS CHANGED"
	echo "Previous: $default_mode"
	echo "New:      $device_mode"

	echo "BR2_DEFAULT_$(echo "$device_mode" | tr '[[:lower:]]' '[[:upper:]]')_MODE=y" >> "$profile_file"
	make PROFILE="$PROFILE" olddefconfig >/dev/null

	echo "Reconfiguring system and datamodel..."
	make prepare {host-,}deuteron-{clean,reconfigure} datamodel-{clean,reconfigure}

	echo "Clearing datamodel stamps..."
	rm -fv $DATAMODEL_DIR/.stamp_built
	rm -fv $DATAMODEL_DIR/.stamp_staging_installed
	rm -fv $DATAMODEL_DIR/.stamp_target_installed

fi

for devmode in $devmode_human_list; do
	prefix=$(devmode_from_human $devmode)

	# Сбрасываем дефолтные конфиги и права доступа к исходному состоянию
	if [ -f "$profile_dir/${prefix}_config.default" ]; then
		rm $profile_dir/${prefix}_config.default
		rm $profile_dir/${prefix}_permission.default
		git checkout -- $profile_dir/${prefix}_config.default
		git checkout -- $profile_dir/${prefix}_permission.default
	fi

	if [ -d "$tempdir/$devmode" ]; then
		convert_device_mode $devmode
	fi
done

echo "Done."
