#ifndef __HWDATA_BUF_H__
#define __HWDATA_BUF_H__

/**
 * Структура, описывающая буфер
 *
 * p    - указатель на начало
 * size - размер буфера
 */
typedef struct {
	char *p;
	size_t size;
} buf_t;


/**
 * Выделяет память под новый буфер
 *
 * @param[in] size - трубуемая длина
 *
 * @retval указатель на стркутуру, описывающюю буфер в случае успешного завершения,
 * @retval NULL в противном случае
 */
buf_t *new_buf(size_t size);


/**
 * Освобождает память занятую буфером
 *
 * @param[in] buf - указатель на структуру, описывающую буфер
 *
 * @retval ни чего не возвращает
 */
void free_buf(buf_t *buf);


/**
 * Копирует данные из буфера
 *
 * @param[in] dest - адрес назначения для данных
 * @param[in] buf  - указатель на буфер-источник данных
 *
 * @retval ни чего не возвращает
 */
void copy_buf(char *dest, buf_t *buf);

#endif
