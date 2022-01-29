#ifndef __GENERIC_STRUCT_H__
#define __GENERIC_STRUCT_H__

#include <sys/types.h>

/**
 * Структура, описывающая архитектурно-зависимую часть HW параметра,
 * для архитектуры generic - большинство устройств, хранящих HW параметры
 * напрямую на разделе MTD
 *
 * mtd_name - имя раздела, содержащего HW параметр
 * offset   - смещение данных HW парметра от начала раздела
 * size     - размер данных HW параметра
 */
typedef struct {
	char *mtd_name;
	off_t offset;
	size_t size;
} hwdata_arch_t;

#endif
