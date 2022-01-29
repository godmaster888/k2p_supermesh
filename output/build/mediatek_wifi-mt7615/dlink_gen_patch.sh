#!/bin/sh
curr_commit=$(git log -n1|grep commit|cut -d' ' -f2)
pure_commit="8850e3a255b3498adbaa77fbcd50f5eb4dbc66eb"
files=$(git diff --name-only ${pure_commit})
priv="mt_wifi/embedded/dlink_mesh"
skip="dlink_gen_patch.sh"

[ -f 202-dlink_mesh_vendor.patch ] && rm 202-dlink_mesh_vendor.patch

for f in ${files}
do
	# skip unnecessary file
	[ "x$(basename ${f})" == "x${skip}" ] && continue
	# patch for vendor. skip d-link private files
	[ "x$(dirname ${f})" == "x${priv}" ] && continue

	git diff ${pure_commit} -- ${f} >> 202-dlink_mesh_vendor.patch
done

[ -f 201-dlink_mesh_private.patch ] && rm 201-dlink_mesh_private.patch

for f in ${files}
do
	# skip unnecessary file
	[ "x$(basename ${f})" == "x${skip}" ] && continue
	# d-link private files
	[ "x$(dirname ${f})" != "x${priv}" ] && continue

	git diff ${pure_commit} -- ${f} >> 201-dlink_mesh_private.patch
done

sed -i "0,/XX==COMMIT==XX/s/XX==COMMIT==XX/${curr_commit:0:8}/" 201-dlink_mesh_private.patch

