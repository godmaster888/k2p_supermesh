#ifndef __NAME2DEV_H__
#define __NAME2DEV_H__

/**
 * Преобразует имя раздела MTD в файл устройства в /dev.
 *
 * @param[in] name - имя раздела MTD.
 *
 * @note память, возвращаемую по указателю, освобождать не надо,
 * @note таблица сопоставления имя<->раздел формируется один раз для процесса.
 *
 * @return указатель на файл устройства, либо NULL в случае ошибки
 */
const char *mtd_name2dev(const char *name);
const char *d_mtd_name2dev(const char *name);


/**
 * Преобразует в файл устройства MTD-разделав в имя раздела.
 *
 * @param[in] name - имя раздела MTD.
 *
 * @note память, возвращаемую по указателю, освобождать не надо,
 * @note таблица сопоставления имя<->раздел формируется один раз для процесса.
 *
 * @return указатель на файл устройства, либо NULL в случае ошибки
 */
const char *mtd_dev2name(const char *dev);
const char *d_mtd_dev2name(const char *dev);

#endif
