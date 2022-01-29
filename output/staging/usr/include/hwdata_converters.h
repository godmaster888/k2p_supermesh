#ifndef __HWDATA_CONVERTERS_H__
#define __HWDATA_CONVERTERS_H__

#include "hwdata_buf.h"

/**
 * Перечисление возможных вариантов конвертации данных
 */
typedef enum {
	FOR_SYSTEM, /// данные конвертируются из формата хранения в системный формат
	FOR_STORE,  /// данные конвертируются из системного формата в формать хранения
} dir_t;


/**
 * Сводная функция конвертации MAC: xx:xx:xx:xx:xx:xx <-> бинарная форма
 *
 * @param[in] in - указатель данные мака в в одном из форматов
 * @param[in] dir - направление конвертации
 *
 * @retval возвращает указатель на выделенный в памяти буфер в случае успеха,
 * @retval NULL в противном случае
 */
buf_t *mac_converter(const char *in, dir_t dir);


/**
 * Конвертирует MAC из бинарной формы в строку вида xx:xx:xx:xx:xx:xx
 *
 * @param[in]  bmac - указатель на данные мака в бинарной форме
 * @param[out] hmac - указатель на данные сконвертированного мака
 *
 * @retval всегда 0
 */
int mac_bin2hunam(const char *bmac, char *hmac);


/**
 * Конвертирует MAC из строки вида xx:xx:xx:xx:xx:xx в бинарную форму
 *
 * @param[in] hmac - указатель данные мака в форме строки вида xx:xx:xx:xx:xx:xx
 * @param[in]  bmac - указатель на данные сконвертированного мака
 *
 * @retval возвращает 0 в случае успеха, -1 в противном случае
 */
int mac_hunam2bin(const char *hmac, char *bmac);


/**
 * Увеличивает MAC на cnt сохраняя OUI
 *
 * @param[in] bmac_in - указатель на данные мака в бинарной форме
 * @param[in] cnt     - дельта
 *
 * @retval отсутсвует
 */
void bmac_inc(char *bmac, int cnt);


/**
 * Фукнкция устраняет ошибку в формате хранения wifi pin
 */
buf_t *wifi_pin_fix(const char *in, dir_t dir);

#endif
