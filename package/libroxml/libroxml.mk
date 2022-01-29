################################################################################
#
# libroxml
#
################################################################################

LIBROXML_VERSION = master
LIBROXML_SITE = $(DLINK_GIT_STORAGE)/libroxml
LIBROXML_INSTALL_STAGING = YES
LIBROXML_LICENSE = LGPL-2.1+ with static link exception
LIBROXML_LICENSE_FILES = License.txt

LIBROXML_CONF_OPT = --disable-silent-rules --disable-xpath --enable-smallbuffer --disable-rocat --disable-roxml --enable-static=no --disable-float

# libroxml forgets to compile/link with -pthread, even though it uses
# thread functions breaking static linking
LIBROXML_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -pthread" LIBS="-pthread"

$(eval $(call AUTOTARGETS))

