# Makefile for buildroot2
#
# Copyright (C) 1999-2005 by Erik Andersen <andersen@codepoet.org>
# Copyright (C) 2006-2010 by the Buildroot developers <buildroot@uclibc.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

#--------------------------------------------------------------
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------

# Set and export the version string
export BR2_VERSION := 2010.11
export DLINK_STORAGE := http://rd/~git
export DLINK_GIT_STORAGE := git+ssh://git@rd
export DLINK_SDK_VERSION := dsysinit

ifneq (${BRANCH},)
export DLINK_BRANCH:=$$${BRANCH}
else
export DLINK_BRANCH:=master
endif
export HOSTNAME:=$(shell hostname)
export FW_BRANCH:=${DLINK_BRANCH}
# This top-level Makefile can *not* be executed in parallel
.NOTPARALLEL:

# absolute path
TOPDIR:=$(shell pwd)
CONFIG_CONFIG_IN=Config.in
CONFIG=package/config

# Compute the full local version string so packages can use it as-is
# Need to export it, so it can be got from environment in children (eg. mconf)
export BR2_VERSION_FULL:=$(BR2_VERSION)$(shell $(TOPDIR)/scripts/setlocalversion)

ifeq ($(wildcard package/private/private.mk),)
# No private here. This is public, GPL exported, sdk.
IS_PUBLIC := y
endif

noconfig_targets:=menuconfig nconfig gconfig xconfig config oldconfig olddefconfig randconfig \
	defconfig %_defconfig savedefconfig allyesconfig allnoconfig syncconfig release \
	randpackageconfig allyespackageconfig allnopackageconfig \
	source-check help

# Strip quotes and then whitespaces
qstrip=$(strip $(subst ",,$(1)))
#"))

# Variables for use in Make constructs
comma:=,
empty:=
space:=$(empty) $(empty)

ifneq ("$(origin O)", "command line")
O:=output
CONFIG_DIR:=$(TOPDIR)
NEED_WRAPPER=
else
# other packages might also support Linux-style out of tree builds
# with the O=<dir> syntax (E.G. Busybox does). As make automatically
# forwards command line variable definitions those packages get very
# confused. Fix this by telling make to not do so
MAKEOVERRIDES =
# strangely enough O is still passed to submakes with MAKEOVERRIDES
# (with make 3.81 atleast), the only thing that changes is the output
# of the origin function (command line -> environment).
# Unfortunately some packages don't look at origin (E.G. uClibc 0.9.31+)
# To really make O go away, we have to override it.
override O:=$(O)
CONFIG_DIR:=$(O)
# we need to pass O= everywhere we call back into the toplevel makefile
EXTRAMAKEARGS = O=$(O)
NEED_WRAPPER=y
endif

# подмена профиля для сборки откатной прошивки
ifneq ($(ROLLBACK),)
  ROLLBACK_FILE := $(TOPDIR)/profiles/$(ROLLBACK:.bootstrap=)/$(ROLLBACK)

  ifeq ("$(wildcard $(ROLLBACK_FILE))","")
    $(error $(ROLLBACK_FILE) is not found)
  endif

  ifeq ($(shell grep "BR2_DLINK_DEVICE_IS_CUSTOM=y" $(ROLLBACK_FILE)),)
    $(error $(ROLLBACK) is not a custom)
  endif

  ifeq ($(shell grep "BR2_USE_CUSTOM_LABEL=y\|BR2_DUAL_SIGN_FW=y" $(ROLLBACK_FILE)),)
    $(error $(ROLLBACK) is not locked)
  endif

  PROFILE := $(shell grep "BR2_DLINK_GENERIC_PROFILE=" $(ROLLBACK_FILE) | cut -d '=' -f2 | tr -d '"')

  ifeq ($(PROFILE),)
    $(error $(ROLLBACK) has no generic profile)
  endif

  ROLLBACK_CUSTOM_LABEL := $(shell grep "BR2_CUSTOM_LABEL=" $(ROLLBACK_FILE) | cut -d '=' -f2 | tr -d '"')
endif

SAVED_PROFILE := $(shell cat $(TOPDIR)/.last_profile 2>/dev/null)

ifeq ($(PROFILE),)
PROFILE := $(SAVED_PROFILE)
endif

ifeq ($(PROFILE),)
ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
$(error PROFILE is not set. To build sdk run: make PROFILE=<PROFILE>)
endif
endif

REAL_PROFILE := $(PROFILE:.bootstrap=)
SAVED_PROFILE_LINK := $(shell readlink $(TOPDIR)/.config 2>/dev/null)

# farisey: для бутстрап-профилей
REL_PROF_DIR := profiles/$(REAL_PROFILE)
export PROFILE_DIR=$(TOPDIR)/$(REL_PROF_DIR)
export PROFILE_FILE=$(PROFILE_DIR)/$(PROFILE)

ifeq ($(filter syncconfig,$(MAKECMDGOALS)),)
# устанавливаем симлинк на профиль, если он изменился
ifneq ($(SAVED_PROFILE_LINK),$(PROFILE_FILE))
  $(shell echo $(PROFILE) > .last_profile)
  $(shell unlink $(TOPDIR)/.profile_dir 2>/dev/null; ln -s profiles/$(REAL_PROFILE)/            $(TOPDIR)/.profile_dir)
  $(shell unlink $(TOPDIR)/.config 2>/dev/null;      ln -s profiles/$(REAL_PROFILE)/$(PROFILE)  $(TOPDIR)/.config)
endif
endif

export BR2_CONFIG = $(CONFIG_DIR)/.config

ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
include $(BR2_CONFIG)
endif

# farisey: avoiding -DDAP_1620A1_MT7620A_BEZEQ.bootstrap in gcc cmdline
# the profile file was already included above, so we do not need
# env value.
# UPDATE: add PROFILE_FULL to save .bootstrap suffix
# to get correct linux config in a kernel repo.
# See linux.mk
# Following are equal for non-bootstrap builds.
PROFILE_FULL := $(PROFILE)
PROFILE := $(REAL_PROFILE)
export PROFILE_NAME=$(PROFILE)

# custom firmware name set if needed
ifneq ($(ROLLBACK),)
  PROFILE_ALIAS=$(ROLLBACK)
else
  ifneq ($(PROFILE_ALIAS),)
    ifeq ($(PROFILE_ALIAS),"")
      PROFILE_ALIAS=$(PROFILE)
    endif
  else
    PROFILE_ALIAS=$(PROFILE)
  endif
endif

# Override BR2_DL_DIR if shell variable defined
ifneq ($(BUILDROOT_DL_DIR),)
BR2_DL_DIR:=$(BUILDROOT_DL_DIR)
endif

# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE=$(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE=0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet=
  Q=
ifndef VERBOSE
  VERBOSE=1
endif
else
  quiet=quiet_
  Q=@
endif

# we want bash as shell
SHELL:=$(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi; fi)

# kconfig uses CONFIG_SHELL
CONFIG_SHELL:=$(SHELL)

export SHELL CONFIG_SHELL quiet Q KBUILD_VERBOSE VERBOSE

ifndef HOSTAR
HOSTAR:=ar
endif
ifndef HOSTAS
HOSTAS:=as
endif
ifndef HOSTCC
HOSTCC:=gcc
else
endif
ifndef HOSTCXX
HOSTCXX:=g++
endif
ifndef HOSTFC
HOSTFC:=gfortran
endif
ifndef HOSTCPP
HOSTCPP:=cpp
endif
ifndef HOSTLD
HOSTLD:=ld
endif
ifndef HOSTLN
HOSTLN:=ln
endif
ifndef HOSTNM
HOSTNM:=nm
endif
HOSTAR:=$(shell which $(HOSTAR) || type -p $(HOSTAR) || echo ar)
HOSTAS:=$(shell which $(HOSTAS) || type -p $(HOSTAS) || echo as)
HOSTCC:=$(shell which $(HOSTCC) || type -p $(HOSTCC) || echo gcc)
HOSTCXX:=$(shell which $(HOSTCXX) || type -p $(HOSTCXX) || echo g++)
HOSTFC:=$(shell which $(HOSTLD) || type -p $(HOSTLD) || echo || which g77 || type -p g77 || echo gfortran)
HOSTCPP:=$(shell which $(HOSTCPP) || type -p $(HOSTCPP) || echo cpp)
HOSTLD:=$(shell which $(HOSTLD) || type -p $(HOSTLD) || echo ld)
HOSTLN:=$(shell which $(HOSTLN) || type -p $(HOSTLN) || echo ln)
HOSTNM:=$(shell which $(HOSTNM) || type -p $(HOSTNM) || echo nm)

export HOSTAR HOSTAS HOSTCC HOSTCXX HOSTFC HOSTLD

# bash prints the name of the directory on 'cd <dir>' if CDPATH is
# set, so unset it here to not cause problems. Notice that the export
# line doesn't affect the environment of $(shell ..) calls, so
# explictly throw away any output from 'cd' here.
export CDPATH:=
BASE_DIR := $(shell mkdir -p $(O) && cd $(O) >/dev/null && pwd)
$(if $(BASE_DIR),, $(error output directory "$(O)" does not exist))

BUILD_DIR:=$(BASE_DIR)/build

STAGING_DIR:=$(BASE_DIR)/staging

export BR2_AUTOHEADER_DIR := $(STAGING_DIR)/usr/include/autoheader
export BR2_AUTOHEADER := $(BR2_AUTOHEADER_DIR)/autoconf.h

# packages compiled for the host goes here
export HOST_DIR:=$(BASE_DIR)/host


export D_KCONFIG_AUTOCONFIG:=$(BUILD_DIR)/buildroot-config/auto.conf
export D_KCONFIG_AUTOHEADER:=$(BUILD_DIR)/buildroot-config/autoconf.h
export D_KCONFIG_AUTOCMAKE:=$(BUILD_DIR)/buildroot-config/auto.cmake

ifeq ($(BR2_HAVE_DOT_CONFIG),y)

# cc-option
# Usage: cflags-y+=$(call cc-option, -march=winchip-c6, -march=i586)
# sets -march=winchip-c6 if supported else falls back to -march=i586
# without checking the latter.
cc-option=$(shell if $(TARGET_CC) $(TARGET_CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
	> /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

#############################################################
#
# Hide troublesome environment variables from sub processes
#
#############################################################
unexport CROSS_COMPILE
unexport ARCH

GNU_HOST_NAME:=$(shell package/gnuconfig/config.guess)

#############################################################
#
# Setup the proper filename extensions for the host
#
##############################################################
ifneq ($(findstring linux,$(GNU_HOST_NAME)),)
HOST_EXEEXT:=
HOST_LIBEXT:=.a
HOST_SHREXT:=.so
endif

# The preferred type of libs we build for the target
ifeq ($(BR2_STATIC_LIBS),y)
LIBTGTEXT=.a
#PREFERRED_LIB_FLAGS:=--disable-shared --enable-static
else
LIBTGTEXT=.so
#PREFERRED_LIB_FLAGS:=--disable-static --enable-shared
endif
PREFERRED_LIB_FLAGS:=--enable-static --enable-shared

##############################################################
#
# The list of stuff to build for the target toolchain
# along with the packages to build for the target.
#
##############################################################
BASE_TARGETS:=toolchain
TARGETS:=

# silent mode requested?
QUIET:=$(if $(findstring s,$(MAKEFLAGS)),-q)

# Strip off the annoying quoting
ARCH:=$(call qstrip,$(TARGET_ARCH))
ifeq ($(ARCH),xtensa)
ARCH:=$(ARCH)_$(call qstrip,$(BR2_xtensa_core_name))
endif
ifeq (${BR2_rlx},y)
KERNEL_ARCH:=rlx
  ifeq (${RTL8197F},y)
    ifeq ($(BR2_RTK_SDK_TELECOM),y)
    KERNEL_ARCH:=mips
    else
    KERNEL_ARCH:=mips-ori
    endif
  endif
else
ifeq (${BR2_rlx_9607},y)
KERNEL_ARCH:=rlx
else
KERNEL_ARCH:=$(shell echo "$(ARCH)" | sed -e "s/-.*//" \
	-e s/i.86/i386/ -e s/sun4u/sparc64/ \
	-e s/arm.*/arm/ -e s/sa110/arm/ \
	-e s/parisc64/parisc/ \
	-e s/powerpc64/powerpc/ \
	-e s/ppc.*/powerpc/ -e s/mips.*/mips/ \
	-e s/sh.*/sh/)
endif
endif

ZCAT := $(call qstrip,$(BR2_ZCAT))
BZCAT := $(call qstrip,$(BR2_BZCAT))
XZCAT := $(call qstrip,$(BR2_XZCAT))
LZCAT := $(call qstrip,$(BR2_LZCAT))
TAR_OPTIONS=$(call qstrip,$(BR2_TAR_OPTIONS)) -xf

PYTHON2 := $(shell which python2 || which python)

GNU_TARGET_SUFFIX:=-$(call qstrip,$(BR2_GNU_TARGET_SUFFIX))

# Quotes are needed for spaces and all in the original PATH content.
BR_PATH = "$(HOST_DIR)/bin:$(HOST_DIR)/sbin:$(HOST_DIR)/usr/bin:$(HOST_DIR)/usr/sbin:$(PATH)"

BR_GRAPH_OUT := $(or $(BR2_GRAPH_OUT),pdf)
GRAPHS_DIR := $(BASE_DIR)/graphs

BR_GRAPH_OUT := $(or $(BR2_GRAPH_OUT),pdf)
GRAPHS_DIR := $(BASE_DIR)/graphs

# stamp (dependency) files go here
STAMP_DIR:=$(BASE_DIR)/stamps

BINARIES_DIR:=$(BASE_DIR)/images
export TARGET_DIR:=$(BASE_DIR)/target
TOOLCHAIN_DIR=$(BASE_DIR)/toolchains
TARGET_SKELETON=$(TOPDIR)/fs/skeleton
BR2_DEPENDS_DIR=$(BUILD_DIR)/buildroot-config

BT := $(TOPDIR)/.build_time
export DATE_EPOCH := $(shell test -f $(BT) && cat $(BT) || sh -c "date +%s | tee $(BT)")

# date -r @<unix_timestamp> only in GNU
BUILD_DATE := $(shell date +%Y%m%d_%H%M --date=@$(DATE_EPOCH))
export BUILD_TIME := $(shell date +%m%d%H%M%Y --utc --date=@$(DATE_EPOCH))
export DATE := $(shell LC_TIME=c date --date=@$(DATE_EPOCH))

#include toolchain/Makefile.in
include package/Makefile.in
include support/dependencies/dependencies.mk
include system/system.mk

#############################################################
#
# You should probably leave this stuff alone unless you know
# what you are doing.
#
#############################################################
all: world

# We also need the various per-package makefiles, which also add
# each selected package to TARGETS if that package was selected
# in the .config file.

include target/Makefile.in
include toolchain/toolchain.mk
include boot/common.mk


ifneq ($(BUILDVER), )
FW_VERSION=$(BUILDVER)
else
ifneq ($(ROLLBACK),)
  FW_VERSION=rollback
else
  FW_VERSION=develop
endif
TARGET_CFLAGS += -DDEVELOP
export DEVELOP=y
endif
export FW_VERSION

# farisey: #18758: перенёс это сверху, чтоб сборка
# ядра видела если DEVELOP=y выставлен.
# Для глубины клонирования.
# Почему-то, если export ниже include, то внутри
# того .mk этот export не виден.
include linux/linux.mk

# алексеева попросила, чтоб имена различались
ifeq ($(IS_INTER),y)
FW_IMG_NM_SUFF = ".internal"
DATAMODEL_EXTRA_CFLAGS += -DINTERNAL
else
FW_IMG_NM_SUFF =
endif

ifeq ($(BR2_DRU_BOOTSTRAP), y)

ifeq ($(BR2_DRU_BOOTSTRAP_STAGE1), y)
FW_IMG_NM_TRANSIT_SUFF = ".transit.stage1"
else ifeq ($(BR2_DRU_BOOTSTRAP_STAGE2), y)
FW_IMG_NM_TRANSIT_SUFF = ".transit.stage2"
else
FW_IMG_NM_TRANSIT_SUFF = ".transit"
endif	# STAGE

endif	# BOOTSTRAP


# datamodel version
ifeq ($(DEVELOP),y)
DM_VERSION_SIGN = $(shell $(TOPDIR)/scripts/check_repos.sh sign)
else
DM_VERSION_SIGN = ""
endif

DM_VERSION = "$(shell cat $(STAGING_DIR)/etc/dlink/dm_version.dm 2>/dev/null || echo "0.0.0")$(DM_VERSION_SIGN)"

ifneq ($(FWNAME),)
export FW_IMAGE_NAME = $(FWNAME).bin
else
export FW_IMAGE_NAME = $(subst ",,$(BUILD_DATE)_$(PROFILE_ALIAS)_$(FW_VERSION)_$(DM_VERSION)_sdk-$(DLINK_SDK_VERSION)$(FW_IMG_NM_TRANSIT_SUFF)$(FW_IMG_NM_SUFF).bin)
endif

include vendor/dlink/*.mk

# CFLAGS before including packages
TARGET_CFLAGS_REF       := $(TARGET_CFLAGS)
# List of explicitly allowed additional CFLAGS
TARGET_CFLAGS_WHITELIST := -funwind-tables -rdynamic
include package/*/*.mk

# Detect changes of the TARGET_CFLAGS
TARGET_CFLAGS_UNKNOWN := $(filter-out $(TARGET_CFLAGS_REF) $(TARGET_CFLAGS_WHITELIST), $(TARGET_CFLAGS))
ifneq ($(TARGET_CFLAGS_UNKNOWN),)
$(error Extra CFLAGS are detected in TARGET_CFLAGS: $(TARGET_CFLAGS_UNKNOWN))
endif

###
# Base packege set
###

ifneq ($(BR2_QEMU_MEMORY),)
TARGETS+=configure_qemu
endif

TARGETS+=target-finalize

include fs/common.mk

include target/pack.mk

ifneq (${GETRELEASE},)
ifeq ($(DOCKER_GPL_BUILD),y)
include $(TOPDIR)/.hashes_release
else
include $(GETRELEASE)
endif
endif

TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS))
TARGETS_SOURCE:=$(patsubst %,%-source,$(TARGETS) $(BASE_TARGETS))
TARGETS_DIRCLEAN:=$(patsubst %,%-dirclean,$(TARGETS))
TARGETS_ALL:=$(patsubst %,__real_tgt_%,$(TARGETS))

# host-* dependencies have to be handled specially, as those aren't
# visible in Kconfig and hence not added to a variable like TARGETS.
# instead, find all the host-* targets listed in each <PKG>_DEPENDENCIES
# variable for each enabled target.
# Notice: this only works for newstyle gentargets/autotargets packages
TARGETS_HOST_DEPS = $(sort $(filter host-%,$(foreach dep,\
		$(addsuffix _DEPENDENCIES,$(call UPPERCASE,$(TARGETS))),\
		$($(dep)))))
# Host packages can in turn have their own dependencies. Likewise find
# all the package names listed in the HOST_<PKG>_DEPENDENCIES for each
# host package found above. Ideally this should be done recursively until
# no more packages are found, but that's hard to do in make, so limit to
# 1 level for now.
HOST_DEPS = $(sort $(foreach dep,\
		$(addsuffix _DEPENDENCIES,$(call UPPERCASE,$(TARGETS_HOST_DEPS))),\
		$($(dep))))
HOST_SOURCE += $(addsuffix -source,$(sort $(TARGETS_HOST_DEPS) $(HOST_DEPS)))

# all targets depend on the crosscompiler and it's prerequisites
$(TARGETS_ALL): __real_tgt_%: $(BASE_TARGETS) %

# Now we are sure we have all the packages scanned and defined. We now
# check for each package in the list of enabled packages, that all its
# dependencies are indeed enabled.
#
# Only trigger the check for default builds. If the user forces building
# a package, even if not enabled in the configuration, we want to accept
# it. However; we also want to be able to force checking the dependencies
# if the user so desires. Forcing a dependency check is useful in the case
# of test-pkg, as we want to make sure during testing, that a package has
# all the dependencies selected in the config file.
#
ifeq ($(MAKECMDGOALS),)
BR_FORCE_CHECK_DEPENDENCIES = YES
endif

ifeq ($(BR_FORCE_CHECK_DEPENDENCIES),YES)

define CHECK_ONE_DEPENDENCY
ifeq ($$($(2)_TYPE),target)
ifeq ($$($(2)_IS_VIRTUAL),)
ifneq ($$($$($(2)_KCONFIG_VAR)),y)
$$(error $$($(2)_NAME) is in the dependency chain of $$($(1)_NAME) that \
has added it to its _DEPENDENCIES variable without selecting it or \
depending on it from Config.in)
endif
endif
endif
endef

$(foreach pkg,$(call UPPERCASE,$(TARGETS)),\
	$(foreach dep,$(call UPPERCASE,$($(pkg)_DEPENDENCIES)),\
		$(eval $(call CHECK_ONE_DEPENDENCY,$(pkg),$(dep))$(sep))))

endif

dirs: $(DL_DIR) $(BUILD_DIR) $(STAGING_DIR) $(TARGET_DIR) \
	$(HOST_DIR) $(BR2_DEPENDS_DIR) $(BINARIES_DIR) $(STAMP_DIR)

$(BASE_TARGETS): dirs rebuild_toolchainfile

$(BUILD_DIR)/buildroot-config/auto.conf: $(BR2_CONFIG)
ifeq ($(IS_PUBLIC),y)
	$(Q)echo skip syncconfig
else
	$(MAKE1) $(EXTRAMAKEARGS) syncconfig
endif

$(TOPDIR)/.git/hook_setted:
	$(Q)if [ -f $(TOPDIR)/.git/config ]; then \
		if ! grep -q "patch = " $(TOPDIR)/.git/config ; then \
		echo -e "[alias]\\n\\tpatch = \"!sh $(TOPDIR)/scripts/patch.sh\"" >> $(TOPDIR)/.git/config; \
		fi; \
		touch $@; \
	fi

.PHONY: prepare_git_hook
prepare_git_hook: $(TOPDIR)/.git/hook_setted

prepare: $(BUILD_DIR)/buildroot-config/auto.conf prepare_git_hook

world: prepare dirs $(BASE_TARGETS) $(TARGETS_ALL)

rebuild_toolchainfile:
	@echo -en "\
	set(CMAKE_SYSTEM_NAME Linux)\n\
	set(CMAKE_C_COMPILER $(CMAKE_TARGET_CC))\n\
	set(CMAKE_CXX_COMPILER $(CMAKE_TARGET_CXX))\n\
	set(CMAKE_C_FLAGS \"\$${CMAKE_C_FLAGS} $(CMAKE_TARGET_CFLAGS) -isystem ${STAGING_DIR}/usr/include\" CACHE STRING \"Buildroot CFLAGS\" FORCE)\n\
	set(CMAKE_CXX_FLAGS \"\$${CMAKE_CXX_FLAGS} $(CMAKE_TARGET_CXXFLAGS)\" CACHE STRING \"Buildroot CXXFLAGS\" FORCE)\n\
	include_directories(SYSTEM ${STAGING_DIR}/usr/include)\n\
	set(CMAKE_INSTALL_SO_NO_EXE 0)\n\
	set(CMAKE_PROGRAM_PATH \"$(HOST_DIR)/usr/bin;$(HOST_DIR)/usr/sbin\")\n\
	set(CMAKE_FIND_ROOT_PATH \"$(STAGING_DIR)\")\n\
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)\n\
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)\n\
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)\n\
	set(ENV{PKG_CONFIG_SYSROOT_DIR} \"$(STAGING_DIR)\")\n\
	set(CMAKE_EXE_LINKER_FLAGS \"${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib\" CACHE STRING \"Flags used by the linker during the creation of dll's.\")\n\
	" > $(O)/toolchainfile.cmake.bak
#farisey: заменим '"token"' на 'token'
	@sed -ri s/\(\'\"\|\"\'\)/\'/g $(O)/toolchainfile.cmake.bak

	@if ! cmp -s $(O)/toolchainfile.cmake $(O)/toolchainfile.cmake.bak; then mv $(O)/toolchainfile.cmake.bak $(O)/toolchainfile.cmake; else rm -f $(O)/toolchainfile.cmake.bak; fi

.PHONY: all world dirs distclean_soft distclean source outputmakefile \
	$(BASE_TARGETS) $(TARGETS) $(TARGETS_ALL) \
	$(TARGETS_CLEAN) $(TARGETS_DIRCLEAN) $(TARGETS_SOURCE) \
	$(DL_DIR) $(TOOLCHAIN_DIR) $(BUILD_DIR) $(STAGING_DIR) $(TARGET_DIR) \
	$(HOST_DIR) $(BR2_DEPENDS_DIR) $(BINARIES_DIR) $(STAMP_DIR)

# When stripping, obey to BR2_STRIP_EXCLUDE_DIRS and
# BR2_STRIP_EXCLUDE_FILES
STRIP_FIND_COMMON_CMD = \
   find $(TARGET_DIR) \
   $(if $(call qstrip,$(BR2_STRIP_EXCLUDE_DIRS)), \
       \( $(call finddirclauses,$(TARGET_DIR),$(call qstrip,$(BR2_STRIP_EXCLUDE_DIRS))) \) \
       -prune -o \
   ) \
   $(if $(call qstrip,$(BR2_STRIP_EXCLUDE_FILES)), \
       -not \( $(call findfileclauses,$(call qstrip,$(BR2_STRIP_EXCLUDE_FILES))) \) )

# Regular stripping for everything, except libpthread, ld-*.so and
# kernel modules:
# - libpthread.so: a non-stripped libpthread shared library is needed for
#   proper debugging of pthread programs using gdb.
# - ld.so: a non-stripped dynamic linker library is needed for valgrind
# - kernel modules (*.ko): do not function properly when stripped like normal
#   applications and libraries. Normally kernel modules are already excluded
#   by the executable permission check, so the explicit exclusion is only
#   done for kernel modules with incorrect permissions.
STRIP_FIND_CMD = \
   $(STRIP_FIND_COMMON_CMD) \
   -type f \( -perm /111 -o -name '*.so*' \) \
   -not \( $(call findfileclauses,libpthread*.so* ld-*.so* *.ko) \) \
   -print0

# Special stripping (only debugging symbols) for libpthread and ld-*.so.
STRIP_FIND_SPECIAL_LIBS_CMD = \
   $(STRIP_FIND_COMMON_CMD) \
   \( -name 'ld-*.so*' -o -name 'libpthread*.so*' \) \
   -print0

#############################################################
#
# staging and target directories do NOT list these as
# dependencies anywhere else
#
#############################################################
$(DL_DIR) $(TOOLCHAIN_DIR) $(BUILD_DIR) $(HOST_DIR) $(BINARIES_DIR) $(STAMP_DIR):
	@mkdir -p $@

$(STAGING_DIR):
	@mkdir -p $(STAGING_DIR)/bin
	@mkdir -p $(STAGING_DIR)/lib
	@mkdir -p $(STAGING_DIR)/usr/lib
	@mkdir -p $(STAGING_DIR)/usr/include
	@mkdir -p $(STAGING_DIR)/usr/share
	@mkdir -p $(STAGING_DIR)/usr/bin

ifeq ($(BR2_ROOTFS_SKELETON_CUSTOM),y)
TARGET_SKELETON=$(BR2_ROOTFS_SKELETON_CUSTOM_PATH)
endif

$(BUILD_DIR)/.root:
	$(Q)mkdir -p $(TARGET_DIR)
	$(Q)if ! [ -d "$(TARGET_DIR)/bin" ]; then \
		if [ -d "$(TARGET_SKELETON)" ]; then \
			cp -fa $(TARGET_SKELETON)/* $(TARGET_DIR)/; \
		fi; \
		if [ -d "$(TARGET_SKELETON_PATCH)" ]; then \
			toolchain/patch-kernel.sh $(TARGET_DIR) $(TARGET_SKELETON_PATCH)/ \*patch\*; \
		fi; \
		touch $(STAGING_DIR)/.fakeroot.00000; \
	fi
	$(Q)-find $(TARGET_DIR) -type d -name CVS -print0 -o -name .svn -print0 | xargs -0 rm -rf
	$(Q)-find $(TARGET_DIR) -type f \( -name .empty -o -name '*~' \) -print0 | xargs -0 rm -rf
	$(Q)touch $@

$(TARGET_DIR): $(BUILD_DIR)/.root

ifeq ($(BR2_ENABLE_LOCALE_PURGE),y)
LOCALE_WHITELIST=$(BUILD_DIR)/locales.nopurge
LOCALE_NOPURGE=$(call qstrip,$(BR2_ENABLE_LOCALE_WHITELIST))

define PURGE_LOCALES
	$(Q)rm -f $(LOCALE_WHITELIST)
	$(Q)for i in $(LOCALE_NOPURGE); do echo $$i >> $(LOCALE_WHITELIST); done

	$(Q)for dir in $(wildcard $(addprefix $(TARGET_DIR),/usr/share/locale /usr/share/X11/locale /usr/man /usr/share/man)); \
	do \
		for lang in $$(cd $$dir; ls .|grep -v man); \
		do \
			grep -qx $$lang $(LOCALE_WHITELIST) || rm -rf $$dir/$$lang; \
		done; \
	done
endef

TARGET_FINALIZE_HOOKS += PURGE_LOCALES
endif

.PHONY: target-pre-finalize
target-pre-finalize:
	@$(call MESSAGE,"Prepare finalizing target directory")

gen_hashes: target-pre-finalize

.PHONY: target-finalize
target-finalize: gen_hashes
	@$(call MESSAGE,"Finalizing target directory")
	$(foreach hook,$(TARGET_FINALIZE_HOOKS),$($(hook))$(sep))
ifeq ($(BR2_BOOTLOADER_UPDATE_FROM_FW),y)
	$(Q)-cp $(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE) $(TARGET_DIR)/bin/uboot.img
endif
ifeq ($(BR2_SUPPORT_JFFS_DATA),y)
	$(Q)mkdir -p $(TARGET_DIR)/jffs
endif
ifeq ($(BR2_HAVE_DEVFILES),y)
	$(Q)( scripts/copy.sh $(STAGING_DIR) $(TARGET_DIR) )
else
	$(Q)rm -rf $(TARGET_DIR)/usr/include $(TARGET_DIR)/include $(TARGET_DIR)/usr/lib/pkgconfig $(TARGET_DIR)/usr/share/aclocal
	$(Q)find $(TARGET_DIR)/lib \( -name '*.a' -o -name '*.la' \) -print0 | xargs -0 rm -f
	$(Q)find $(TARGET_DIR)/usr/lib \( -name '*.a' -o -name '*.la' \) -print0 | xargs -0 rm -f
endif
ifneq ($(BR2_PACKAGE_GDB),y)
	$(Q)rm -rf $(TARGET_DIR)/usr/share/gdb
endif
ifneq ($(BR2_PACKAGE_GDB_SERVER),y)
	$(Q)rm -f $(TARGET_DIR)/usr/bin/gdbserver
endif
ifneq ($(BR2_HAVE_DOCUMENTATION),y)
	$(Q)rm -rf $(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/man $(TARGET_DIR)/share/man $(TARGET_DIR)/man
	$(Q)rm -rf $(TARGET_DIR)/usr/info $(TARGET_DIR)/usr/share/info $(TARGET_DIR)/share/info
	$(Q)rm -rf $(TARGET_DIR)/usr/doc $(TARGET_DIR)/usr/share/doc $(TARGET_DIR)/share/doc
	$(Q)rm -rf $(TARGET_DIR)/usr/share/gtk-doc
	$(Q)[ -d $(TARGET_DIR)/usr/share ] && rmdir --ignore-fail-on-non-empty $(TARGET_DIR)/usr/share 2>/dev/null || echo >/dev/null # Error suppress
	$(Q)[ -d $(TARGET_DIR)/share ] && rmdir --ignore-fail-on-non-empty $(TARGET_DIR)/share 2>/dev/null || echo >/dev/null # Error suppress
endif
	$(STRIP_FIND_CMD) | xargs -0 $(STRIPCMD) 2>/dev/null || true
	$(STRIP_FIND_SPECIAL_LIBS_CMD) | xargs -0 -r $(STRIPCMD) $(STRIP_STRIP_DEBUG) 2>/dev/null || true

	$(Q)mkdir -p $(TARGET_DIR)/etc
#    Mandatory configuration file and auxilliary cache directory
#    for recent versions of ldconfig
	$(Q)echo "/lib:/lib/private:/usr/lib" > $(TARGET_DIR)/etc/ld.so.conf

	$(Q)echo "NAME:           $(PROFILE_ALIAS)"                            > $(TARGET_DIR)/VERSION
	$(Q)echo "VERSION:        $(call qstrip,$(FW_VERSION))"               >> $(TARGET_DIR)/VERSION
	$(Q)echo "DATAMODEL:      $(call qstrip,$(DM_VERSION))"               >> $(TARGET_DIR)/VERSION
	$(Q)echo "SYSBUILDTIME:   $(DATE)"                                    >> $(TARGET_DIR)/VERSION
	$(Q)echo "VENDOR:         $(call qstrip,$(DLINK_VENDOR))"             >> $(TARGET_DIR)/VERSION
	$(Q)echo "BUGS:           $(call qstrip,$(DLINK_BUGS))"               >> $(TARGET_DIR)/VERSION
	$(Q)echo "SUMMARY:        Root filesystem image for $(PROFILE_ALIAS)" >> $(TARGET_DIR)/VERSION

ifneq ($(BR2_ROOTFS_POST_BUILD_SCRIPT),"")
	$(Q)$(BR2_ROOTFS_POST_BUILD_SCRIPT) $(TARGET_DIR)
endif
	$(Q)rm -f $(BT)

source: $(TARGETS_SOURCE) $(HOST_SOURCE)

_source-check:
	$(MAKE) DL_MODE=SOURCE_CHECK $(EXTRAMAKEARGS) source

external-deps:
	@$(MAKE) -Bs DL_MODE=SHOW_EXTERNAL_DEPS $(EXTRAMAKEARGS) source | sort -u

show-targets:
	@echo $(TARGETS)

ifeq ($(IS_PUBLIC),y)
include public.mk
else
include gpl.mk
endif

else # ifeq ($(BR2_HAVE_DOT_CONFIG),y)

all: menuconfig

# configuration
# ---------------------------------------------------------------------------

HOSTCFLAGS=$(CFLAGS_FOR_BUILD)
export HOSTCFLAGS

$(BUILD_DIR)/buildroot-config/%onf:
	mkdir -p $(@D)/lxdialog
	$(MAKE) CC="$(HOSTCC)" obj=$(@D) -C $(CONFIG) -f Makefile.br $(@F)

ifeq ($(IS_PUBLIC),y)
COMMON_CONFIG_ENV = echo -e '\n\nYou cannot configure a public build: not implemented\n\n'
else
COMMON_CONFIG_ENV = \
	KCONFIG_OVERWRITECONFIG=y \
	KCONFIG_AUTOCONFIG=$(BUILD_DIR)/buildroot-config/auto.conf \
	KCONFIG_AUTOHEADER=$(BUILD_DIR)/buildroot-config/autoconf.h \
	KCONFIG_TRISTATE=$(BUILD_DIR)/buildroot-config/tristate.config \
	KCONFIG_AUTOCMAKE=$(BUILD_DIR)/buildroot-config/auto.cmake \
	PROFILE_NAME=$(PROFILE_NAME) \
	BR2_CONFIG=$(BR2_CONFIG)
endif

xconfig: $(BUILD_DIR)/buildroot-config/qconf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)

gconfig: $(BUILD_DIR)/buildroot-config/gconf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) srctree=$(TOPDIR) $< $(CONFIG_CONFIG_IN)

menuconfig: $(BUILD_DIR)/buildroot-config/mconf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)

nconfig: $(BUILD_DIR)/buildroot-config/nconf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)

config: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)

oldconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --oldconfig $(CONFIG_CONFIG_IN)

olddefconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --olddefconfig $(CONFIG_CONFIG_IN)

randconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --randconfig $(CONFIG_CONFIG_IN)

allyesconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --allyesconfig $(CONFIG_CONFIG_IN)

allnoconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --allnoconfig $(CONFIG_CONFIG_IN)

randpackageconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@grep -v BR2_PACKAGE_ $(CONFIG_DIR)/.config > $(CONFIG_DIR)/.config.nopkg
	@$(COMMON_CONFIG_ENV) \
		KCONFIG_ALLCONFIG=$(CONFIG_DIR)/.config.nopkg \
		$< --randconfig $(CONFIG_CONFIG_IN)
	@rm -f $(CONFIG_DIR)/.config.nopkg

allyespackageconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@grep -v BR2_PACKAGE_ $(CONFIG_DIR)/.config > $(CONFIG_DIR)/.config.nopkg
	@$(COMMON_CONFIG_ENV) \
		KCONFIG_ALLCONFIG=$(CONFIG_DIR)/.config.nopkg \
		$< --allyesconfig $(CONFIG_CONFIG_IN)
	@rm -f $(CONFIG_DIR)/.config.nopkg

allnopackageconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@grep -v BR2_PACKAGE_ $(CONFIG_DIR)/.config > $(CONFIG_DIR)/.config.nopkg
	@$(COMMON_CONFIG_ENV) \
		KCONFIG_ALLCONFIG=$(CONFIG_DIR)/.config.nopkg \
		$< --allnoconfig $(CONFIG_CONFIG_IN)
	@rm -f $(CONFIG_DIR)/.config.nopkg

syncconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile $(BR2_CONFIG)
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --$@ $(CONFIG_CONFIG_IN)
	@mkdir -p $(BR2_AUTOHEADER_DIR)
	@cp -f $(D_KCONFIG_AUTOHEADER) $(BR2_AUTOHEADER)

defconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --defconfig $(CONFIG_CONFIG_IN)

%_defconfig: $(BUILD_DIR)/buildroot-config/conf $(TOPDIR)/configs/%_defconfig outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --defconfig=$(TOPDIR)/configs/$@ $(CONFIG_CONFIG_IN)

savedefconfig: $(BUILD_DIR)/buildroot-config/conf outputmakefile
	@mkdir -p $(BUILD_DIR)/buildroot-config
	@$(COMMON_CONFIG_ENV) $< --savedefconfig=$(TOPDIR)/defconfig $(CONFIG_CONFIG_IN)

# check if download URLs are outdated
source-check: allyesconfig
	$(MAKE) $(EXTRAMAKEARGS) _source-check

endif # ifeq ($(BR2_HAVE_DOT_CONFIG),y)

#############################################################
#
# Cleanup and misc junk
#
#############################################################

# outputmakefile generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of make in the
# output directory.
outputmakefile:
ifeq ($(NEED_WRAPPER),y)
	$(Q)$(TOPDIR)/scripts/mkmakefile $(TOPDIR) $(O)
endif

distclean_soft:
	$(Q)chmod -R 755 output
	$(Q)rm -rf $(STAGING_DIR) $(TARGET_DIR) $(BINARIES_DIR) $(HOST_DIR) \
		$(STAMP_DIR) $(BUILD_DIR) $(TOOLCHAIN_DIR) .last_profile .config .profile_dir

distclean: distclean_soft
ifeq ($(DL_DIR),$(TOPDIR)/dl)
	$(Q)rm -rf $(DL_DIR)
endif
ifeq ($(O),output)
	$(Q)rm -rf $(O)
endif
	$(Q)rm -rf $(CONFIG_DIR)/.config $(CONFIG_DIR)/.config.old $(CONFIG_DIR)/.auto.deps

configured: dirs kernel-headers uclibc-config busybox-config linux26-config

prepatch:	gcc-patched binutils-patched gdb-patched uclibc-patched

cross: $(BASE_TARGETS)

help:
	@echo 'Cleaning:'
	@echo '  distclean_soft         - delete all files created by build'
	@echo '  distclean              - delete all non-source files (including .config)'
	@echo
	@echo 'Build:'
	@echo '  all                    - make world'
	@echo
	@echo 'Configuration:'
	@echo '  menuconfig             - interactive curses-based configurator'
	@echo '  xconfig                - interactive Qt-based configurator'
	@echo '  gconfig                - interactive GTK-based configurator'
	@echo '  oldconfig              - resolve any unresolved symbols in .config'
	@echo '  syncconfig             - Same as oldconfig, but quietly, additionally update deps'
	@echo '  olddefconfig           - Same as syncconfig but sets new symbols to their default value'
	@echo '  randconfig             - New config with random answer to all options'
	@echo '  defconfig              - New config with default answer to all options'
	@echo '  allyesconfig           - New config where all options are accepted with yes'
	@echo '  allnoconfig            - New config where all options are answered with no'
	@echo '  randpackageconfig      - New config with random answer to package options'
	@echo '  allyespackageconfig    - New config where pkg options are accepted with yes'
	@echo '  allnopackageconfig     - New config where package options are answered with no'
	@echo '  configured             - make {uclibc/busybox/linux26}-config'
	@echo
	@echo 'Miscellaneous:'
	@echo '  source                 - download all sources needed for offline-build'
	@echo '  source-check           - check all packages for valid download URLs'
	@echo '  external-deps          - list external packages used'
	@echo
	@$(foreach b, $(notdir $(wildcard $(TOPDIR)/configs/*_defconfig)), \
	  printf "  %-35s - Build for %s\\n" $(b) $(b:_defconfig=);)
	@echo
	@echo 'See docs/README and docs/buildroot.html for further details'
	@echo

release: OUT=buildroot-$(BR2_VERSION)

release:
	$(Q)git archive --format=tar --prefix=$(OUT)/ master|gzip -9 >$(OUT).tar.gz

.PHONY: $(noconfig_targets)

ifeq (${RELEASE_FILE},)
# farisey: девелопная заварка
# хеши сделаем и тут тоже
RELEASE_FILE = dev_hashes
# ну и флаг удаления хеша
RM_HASH_FILE = y

ifneq ($(TAG),)
$(error Deprecated variable TAG)
endif
ifneq ($(NOTAGS),)
$(warning Deprecated variable NOTAGS)
endif

${RELEASE_FILE}:
	@echo "# develop on $(shell whoami)@$(shell hostname)" >$@
	@echo "# in $(PWD)" >>$@
	@echo "# at $(DATE)" >>$@
	@echo  >>$@
	@echo "PROFILE=${PROFILE}" >>$@
	@echo "SDK_HASH=$(shell git show | (read a b; echo $$b))" >>$@
	@echo "SDK_BRANCH=`git rev-parse --abbrev-ref HEAD`" >>$@
endif

gen_hashes: ${RELEASE_FILE}
	@echo "THE_UUID=$(shell cat /proc/sys/kernel/random/uuid)" >>${RELEASE_FILE}
	@echo "BUILD_DATAMODEL_VERSION='$(DM_VERSION)'" >>$(RELEASE_FILE)
	$(Q)./scripts/gen_hashes.sh ${RELEASE_FILE}
	$(Q)cp -rf ${RELEASE_FILE} ${TARGET_DIR}/etc/.hashes
ifeq ($(RM_HASH_FILE),y)
	$(Q)rm -f ${RELEASE_FILE}
endif

# Graphs
GRAPHS_DIR := $(BASE_DIR)/graphs

graph-build: $(O)/build/build-time.log
	@install -d $(GRAPHS_DIR)
	$(Q)$(foreach o,name build duration,./scripts/graph-build-time \
					--type=histogram --order=$(o) --input=$(<) \
					--output=$(GRAPHS_DIR)/build.hist-$(o).$(BR_GRAPH_OUT) \
					$(if $(BR2_GRAPH_ALT),--alternate-colors)$(sep))
	$(Q)$(foreach t,packages steps,./scripts/graph-build-time \
				   --type=pie-$(t) --input=$(<) \
				   --output=$(GRAPHS_DIR)/build.pie-$(t).$(BR_GRAPH_OUT) \
				   $(if $(BR2_GRAPH_ALT),--alternate-colors)$(sep))
graph-size:
	$(Q)mkdir -p $(GRAPHS_DIR)
	$(Q)$(TOPDIR)/scripts/size-stats --builddir $(BASE_DIR) \
		--graph $(GRAPHS_DIR)/graph-size.$(BR_GRAPH_OUT) \
		--file-size-csv $(GRAPHS_DIR)/file-size-stats.csv \
		--package-size-csv $(GRAPHS_DIR)/package-size-stats.csv

graph-squash:
	$(Q)mkdir -p $(GRAPHS_DIR)
	$(Q)$(TOPDIR)/scripts/size-stats-squash --builddir $(BASE_DIR) \
		--graph $(GRAPHS_DIR)/graph-size-compressed.$(BR_GRAPH_OUT) \
		--package-size-csv $(GRAPHS_DIR)/package-size-compressed.csv \
		--with-kernel

configure_qemu:
	$(Q)echo "MEM=$(BR2_QEMU_MEMORY)" > ./output/images/qemu.config
	$(Q)echo "USE_STORAGE=$(BR2_QEMU_USE_STORAGE)" >> ./output/images/qemu.config

include Makefile.docker
