# This Makefile fragment declares toolchain related helper functions.

# The copy_toolchain_lib_root function copies a toolchain library and
# its symbolic links from the sysroot directory to the target
# directory. Note that this function is used both by the external
# toolchain logic, and the glibc package, so care must be taken when
# changing this function.
#
# $1: library name pattern (can include glob wildcards)
#
copy_toolchain_lib_root = \
	LIBPATTERN="$(strip $1)"; \
	LIBPATHS=`find $(STAGING_DIR)/ -name "$${LIBPATTERN}" 2>/dev/null` ; \
	for LIBPATH in $${LIBPATHS} ; do \
		while true ; do \
			LIBNAME=`basename $${LIBPATH}`; \
			DESTDIR=`echo $${LIBPATH} | sed "s,^$(STAGING_DIR)/,," | xargs dirname` ; \
			mkdir -p $(TARGET_DIR)/$${DESTDIR}; \
			rm -fr $(TARGET_DIR)/$${DESTDIR}/$${LIBNAME}; \
			if test -h $${LIBPATH} ; then \
				cp -d $${LIBPATH} $(TARGET_DIR)/$${DESTDIR}/$${LIBNAME}; \
				LIBPATH="`readlink -f $${LIBPATH}`"; \
			elif test -f $${LIBPATH}; then \
				$(INSTALL) -D -m0755 $${LIBPATH} $(TARGET_DIR)/$${DESTDIR}/$${LIBNAME}; \
				break ; \
			else \
				exit -1; \
			fi; \
		done; \
	done
