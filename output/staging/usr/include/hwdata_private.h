#ifndef __HWDATA_PRIVATE_H__
#define __HWDATA_PRIVATE_H__

#include <sys/types.h>
#include "hwdata_arch_struct.h"
#include "hwdata_converters.h"

/**
 * Структура, описывающая базовую (общую) часть HW параметра
 *
 * id        - идентификатор HW параметра
 * converter - функция преобразования данных
 * checker   - функция проверки валидности данных HW параметра
 */
typedef struct {
	unsigned int id;
	buf_t * (* converter)(const char *in, dir_t dir);
	int (* checker)(const char *value);
} hwdata_base_t;

/**
 * Структура, описывающая HW параметр
 *
 * base - структура, описыващая базовую часть HW параметра
 * arch - структура, описыващая архитектурно-зависимую часть HW параметра
 */
typedef struct {
	hwdata_base_t base;
	hwdata_arch_t arch;
} hwdata_t;

#endif
