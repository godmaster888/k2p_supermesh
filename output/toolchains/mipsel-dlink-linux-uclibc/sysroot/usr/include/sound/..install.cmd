cmd_/home/builder/buildroot_mtk/output/host/mipsel-dlink-linux-uclibc/sysroot/usr/include/sound/.install := /bin/bash scripts/headers_install.sh /home/builder/buildroot_mtk/output/host/mipsel-dlink-linux-uclibc/sysroot/usr/include/sound   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/asequencer.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/asound.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/asound_fm.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/compress_offload.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/compress_params.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/emu10k1.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/hdsp.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/hdspm.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/sb16_csp.h   /home/builder/buildroot_mtk/output/build/linux-headers-3.10/include/uapi/sound/sfnt_info.h ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/builder/buildroot_mtk/output/host/mipsel-dlink-linux-uclibc/sysroot/usr/include/sound/$$F; done; touch /home/builder/buildroot_mtk/output/host/mipsel-dlink-linux-uclibc/sysroot/usr/include/sound/.install