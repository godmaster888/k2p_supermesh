#ifndef __HWDATA_DEFS_H__
#define __HWDATA_DEFS_H__

#include <syslog.h>

#define DEFAULT_PIN "12345670"

#define LIBNAME "hwdata"
#if 1
	#define SYSLOG(level, format, args...) syslog(level, LIBNAME " (%s): " format, __func__, ##args)
#else
	#define SYSLOG(level, format, args...) {FILE *fd =fopen("/dev/console", "w"); if (fd) {fprintf(fd, LIBNAME "(%s):" format "\n", __func__, ##args); fclose(fd);}}
#endif

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#endif
