#ifndef __HWDATA_IDS_H__
#define __HWDATA_IDS_H__

/**
 * Идентификаторы HW параметров
*/

#define HWDATA_MAC			0
#define HWDATA_CC			1
#define HWDATA_HWREV		2
#define HWDATA_SN			3
#define HWDATA_PCBA_SN		4
#define HWDATA_WIFI_PIN		5
#define HWDATA_CC_5G		6
#define HWDATA_DFS_EN		7
#define HWDATA_PWR_LIMIT	8
#define HWDATA_PWR_LIMIT_5G	9
#define HWDATA_MAC_5G		10
#define HWDATA_BOARDID		11
#define HWDATA_WIFI24_SSID	12
#define HWDATA_WIFI5_SSID	13
#define HWDATA_WIFI24_PSK	14
#define HWDATA_WIFI5_PSK	15
#define HWDATA_GMAC2		16
#define HWDATA_WPA_PSK		17
#define HWDATA_BARCODE		18


/**
 * Длины HW параметров
*/

#define D_HWDATA_WIFI_PIN_SIZE   9    ///< количество байт в пине, включая завершающий нуль
#define D_HWDATA_CC_SIZE         5    ///< количество байт в коде страны, включая завершающий нуль
#define D_HWDATA_HWREV_SIZE      10   ///< количество байт в аппаратной ревизии, включая завершающий нуль
#define D_HWDATA_SN_SIZE         14   ///< количество байт в серийном номере, включая завершающий нуль
#define D_HWDATA_WPA_PSK_SIZE    64   ///< количество байт в ключе WPA PSK, включая завершающий нуль
#define D_HWDATA_PCBA_SN_SIZE    33   ///< количество байт в серийном номере PCBA, включая завершающий нуль
#define D_HWDATA_BID_SIZE        13   ///< количество байт в boardId, включая завершающий нуль
#define D_HWDATA_DFS_SIZE        3
#define D_HWDATA_PWR_LIMIT_SIZE  5
#define D_HWDATA_WIFI_SSID_SIZE  36   ///< количество байт в SSID, включая завершающий нуль, чётное выравние и xor 16 bits
#define D_HWDATA_BARCODE_SIZE    21   ///< количество байт в barcode, включая завершающий нуль

#define WIFI_PIN_SIZE          D_HWDATA_WIFI_PIN_SIZE
#define CC_SIZE                D_HWDATA_CC_SIZE
#define HWREV_SIZE             D_HWDATA_HWREV_SIZE
#define DMS_SN_SIZE            D_HWDATA_SN_SIZE
#define DMS_PCBA_SN_SIZE       D_HWDATA_PCBA_SN_SIZE
#define BID_SIZE               D_HWDATA_BID_SIZE
#define DFS_SIZE               D_HWDATA_DFS_SIZE
#define PWR_LIMIT_SIZE         D_HWDATA_PWR_LIMIT_SIZE


/** Минимальная длина WPA PSK без нуль-терминанта. */
#define MIN_PSK_LEN 8

/** Максимальная длина WPA PSK без нуль-терминанта. */
#define MAX_PSK_LEN 63


/**
 * Атрибуты HW параметров
*/

/** Формы MAC адреса.*/
#define HWDATA_MAC_FORMAT 0
enum hwdata_mac_formats
{
	HWDATA_MAC_BINARY,			///< бинарная форма
	HWDATA_MAC_STRING_HUMAN,		///< строковая форма с двоеточиями
	HWDATA_MAC_STRING_PLAIN,	///< строковая форма без двоеточий
};

#endif
