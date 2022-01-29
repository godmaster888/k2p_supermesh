#ifndef __CHECKERS_H__
#define __CHECKERS_H__


/**
 * Проверяет валидность данных HW параметра [HWDATA_MAC].
 * что не все нули и что не мультикаст мак
 *
 * @param[in]  bmac - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_mac_is_correct(const char *bmac);

/**
 * Проверяет валидность данных HW параметра [HWDATA_CC].
 *
 * @param[in]  s - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_cc_is_correct(const char *s);


/**
 * Проверяет валидность данных/контрольную сумму HW параметра [HWDATA_WIFI_PIN].
 *
 * @param[in]  s - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_wifi_pin_is_correct(const char *s);


/**
 * Проверяет корректность введённого WPA PSK.
 *
 * @param[in] psk - строка для проверки.
 *
 * @retval 0  в случае корректности;
 * @retval -1 в случае ошибки.
 */
int d_hwdata_wifi_wpa_psk_is_correct(const char *psk);


/**
 * Проверяет валидность данных HW параметра [HWDATA_HWREV].
 *
 * @param[in]  s - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_hwrev_is_correct(const char *s);

/**
 * Проверяет валидность данных HW параметра [HWDATA_WIFI24_SSID / HWDATA_WIFI5_SSID].
 *
 * @param[in]  s - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_ssid_is_correct(const char *s);

/**
 * Проверяет валидность данных HW параметра [HWDATA_SN].
 *
 * @param[in]  sn - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_sn_is_correct(const char *sn);


/**
 * Проверяет валидность данных HW параметра [HWDATA_PCBA_SN].
 *
 * @param[in]  sn - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_pcba_sn_is_correct(const char *sn);

/**
 * Проверяет валидность данных HW параметра [HWDATA_BARCODE]
 *
 * @param[in]  bc - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_barcode_is_correct(const char *bc);


#ifdef BR2_MTN_WIFI_INFO
/**
 * Проверяет валидность данных HW параметра [HWDATA_DFS_EN].
 *
 * @param[in]  dfs - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_dfs_en_is_correct(const char *dfs);


/**
 * Проверяет валидность данных HW параметра [HWDATA_PWR_LIMIT/HWDATA_PWR_LIMIT_5G].
 *
 * @param[in]  pwr_limit - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_pwr_limit_is_corret(const char *pwr_limit);
#endif


#ifdef BR2_CAMEO_BULK_AGENT
/**
 * Проверяет валидность данных HW параметра [HWDATA_BOARDID].
 *
 * @param[in]  s - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int d_hwdata_board_id_is_correct(const char *s);
#endif


#ifdef BR2_FLASH_WIFI_CONFIG
/**
 * Проверяет валидность данных HW параметра [HWDATA_WIFI24_SSID / HWDATA_WIFI5_SSID / HWDATA_WIFI24_PSK / HWDATA_WIFI5_PSK].
 *
 * @param[in]  buf - указатель на буфер с данными HW параметра.
 *
 * @retval 0 в случае валидности данных, -1 в противном случае
 */
int flash_wifi_cfg_check(const char *buf);
#endif

#endif
