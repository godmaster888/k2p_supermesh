#ifndef __MTD_UTILS_H__
#define __MTD_UTILS_H__

#include <mtd/mtd-user.h>
#include <sys/types.h> //size_t off_t

#include "name2dev.h"

/**
 * Возвращает информацию о MTD разделе.
 *
 * @param[in] mtd      - имя либо файл устройства MTD-раздела.
 * @param[in] mtd_info - указатель на структуру с информацией.
 *
 * @retval 0 в случае успеха, -1 в случае ошибки.
 */
int d_mtd_info(const char *mtd, struct mtd_info_user *mtd_info);

/**
 * Возвращает размер MTD-раздела.
 *
 * @param[in] mtd - имя либо файл устройства MTD-раздела.
 *
 * @retval размер раздела в случае успешного завершения;
 * @retval -1 в случае ошибки.
 */
ssize_t d_mtd_get_size(const char *mtd);

ssize_t d_mtd_get_size_raw(const char *mtd);

/**
 * Читает данные из MTD-раздела в буфер.
 *
 * @param[in]  mtd    - имя либо файл устройства MTD-раздела;
 * @param[in]  offset - cмещение данных на разделе;
 * @param[out] buffer - буфер для прочитанных данных.
 * @param[in]  size   - размер считываемых данных;
 *
 * @retval количество считанных байт в случае успешного завершения и в случае ошибки чтения отдельного сектора;
 * @retval -1 в случае другой ошибки.
 */
ssize_t d_mtd_read(const char *mtd, off_t offset, void *buffer, size_t size);


/**
 * Читает данные из MTD-раздела в буфер в raw режиме.
 * raw - режим чтения для nand "page_data + oob" без проверки ecc
 *
 * @param[in]  mtd    - имя либо файл устройства MTD-раздела;
 * @param[in]  offset - cмещение данных на разделе;
 * @param[out] buffer - буфер для прочитанных данных.
 * @param[in]  size   - размер считываемых данных;
 *
 * @retval количество считанных байт в случае успешного завершения и в случае ошибки чтения отдельного сектора;
 * @retval -1 в случае другой ошибки.
 */
ssize_t d_mtd_read_raw(const char *mtd, off_t offset, void *buffer, size_t size);


/**
 * Записывает данные из буфера на MTD-раздел.
 *
 * @param[in]  mtd    - имя либо файл устройства MTD-раздела;
 * @param[in]  offset - cмещение данных на разделе;
 * @param[in]  buffer - буфер с данными для записи.
 * @param[in]  size   - размер записываемых данных;
 *
 * @retval  количество записанных байт в случае успешного завершения и в случае ошибки записи отдельного блока;
 * @retval -1 в случае другой ошибки.
 */
ssize_t d_mtd_write(const char *mtd,  off_t offset, const void *buffer, size_t size);


/**
 * Записывает данные из буфера на MTD-раздел.
 * raw - режим записи для nand "page_data + oob" без рассчета ecc
 *
 * @param[in]  mtd    - имя либо файл устройства MTD-раздела;
 * @param[in]  offset - cмещение данных на разделе;
 * @param[out] buffer - буфер для прочитанных данных.
 * @param[in]  size   - размер считываемых данных;
 *
 * @retval количество считанных байт в случае успешного завершения и в случае ошибки чтения отдельного сектора;
 * @retval -1 в случае другой ошибки.
 */
ssize_t d_mtd_write_raw(const char *mtd, off_t offset, const void *buffer, size_t size);


/**
 * Коллбек обработки текущего состояния записи данных в MTD-раздел из функции mtd_write_ex().
 *
 * @param[in]     dev     - файл устройства MTD-раздела;
 * @param[in]     len     - общий размер заипсываемых данных;
 * @param[in]     cur_len - размер обработанных данных;
 * @param[in,out] cb_data - любая дополнительная информация (передаётся дополнительным входным паарметром).
 */
typedef void (* mtd_progress_cb)(const char *dev, size_t len, size_t cur_len, void *cb_data);



/**
 * Записывает данные из буфера на MTD-раздел (расширенный вариант с логированием прогресса).
 *
 * @param[in]     mtd         - имя либо файл устройства MTD-раздела;
 * @param[in]     offset      - cмещение данных на разделе;
 * @param[in]     buffer      - буфер с данными для записи.
 * @param[in]     size        - размер записываемых данных;
 * @param[in]     progress_cb - коллбек обработки текущего состояния записи данных на MTD-раздел;
 * @param[in,out] cb_data     - любая дополнительная информация для коллбека.
 *
 * @retval  количество записанных байт в случае успешного завершения и в случае ошибки записи отдельного блока;
 * @retval -1 в случае другой ошибки.
 */
ssize_t d_mtd_write_ex(const char *mtd, off_t offset, const void *buffer, size_t size,
						mtd_progress_cb progress_cb, void *cb_data);



/**
 * Стирает MTD-раздел.
 *
 * @param[in] mtd - имя либо файл устройства MTD-раздела.
 *
 * @retval  0 в случае успешного завершения;
 * @retval -1 в случае ошибки.
 */
int d_mtd_erase(const char *mtd);



/**
 * Копирует данные из одного MTD-раздела в другой.
 *
 * @note У разделов должны совпадать размеры блоков.
 *
 * @param[in] mtd_src - имя либо файл устройства раздела-источника;
 * @param[in] mtd_dst - имя либо файл устройства раздела-приёмника.
 *
 * @retval  количество записанных байт в случае успешного завершения;
 * @retval -1 в случае ошибки.
 */
ssize_t d_mtd_copy(const char *mtd_src, const char *mtd_dst);

#endif
