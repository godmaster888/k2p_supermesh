#ifndef __HWDATA_H__
#define __HWDATA_H__

#include "hwdata_ids.h"
#include "hwdata_checkers.h"

#include "hwdata_arch_cfg.h" //для проброски наружу имени раздела и смещений, пока еще это нужно, потом нужно будет убрать
#include "hwdata_defs.h" //только для DEFAULT_PIN

/**
 * Получает указанный HW параметр из хранилища.
 *
 * @param[in]   id     - идентификатор HW параметра.
 * @param[out]  buffer - буфер для полученных данных.
 *
 * @retval 0 в случае успешного завершнеия, -1 в случае ошибки
 */
int hwdata_get(unsigned int id, char *buffer);
int d_hwdata_get(unsigned int id, char *buffer);


/**
 * Устанавливает указанный HW параметр в хранилища.
 *
 * @param[in]  id     - идентификатор HW параметра.
 * @param[in]  buffer - буфер с данными HW параметра.
 *
 * @retval 0 в случае успешного завершнеия, -1 в случае ошибки
 */
int hwdata_set(unsigned int id, const char *buffer);
int d_hwdata_set(unsigned int id, const char *buffer);


/**
 * Получает значения атрибута HW параметра.
 *
 * @param[in]  param_id - идентификатор HW параметра.
 * @param[in]  attr_id  - идентификатор атрибута HW параметра.
 *
 * @retval значение указанного атрибута в случае успешного завершнеия,
 * @retval -1 в случае ошибки
 */
int hwdata_get_attr(unsigned int param_id, unsigned int attr_id);
int d_hwdata_get_attr(unsigned int param_id, unsigned int attr_id);

#endif
