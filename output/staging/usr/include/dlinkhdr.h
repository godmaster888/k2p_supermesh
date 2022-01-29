#ifndef __DLINKHDR_H__
#define __DLINKHDR_H__

#include <stdint.h>
#include <string.h>

#define DLINKHDR_SIGN "dlinkhdr"
#define DLINKHDR_SIGN_LEN (sizeof(DLINKHDR_SIGN) - 1)

typedef struct {
	char sign[DLINKHDR_SIGN_LEN];
	uint32_t hdr_data_size; // размер данных (атрибутов) в заголовке
	uint32_t payload_size;  // размер полезных данных после заголовка
	uint32_t hdr_crc;       // crc всех константных данных до hdr_data
	uint32_t hdr_data_crc;  // crc данных hdr_data
	uint8_t reserved[32];   // для будущего использования
	char hdr_data[];
} __attribute__((__packed__)) dlinkhdr_t;


#define DLINKHDR_SIZE(hdr) (sizeof(dlinkhdr_t) + be32toh((hdr)->hdr_data_size))

#define dlinkhdr_foreach(hdr, p) \
	for (p = hdr->hdr_data; \
		p && p < (hdr->hdr_data + be32toh(hdr->hdr_data_size) - 1); \
		p = strchr(p, 0) ? strchr(p, 0) + 1 : 0)


extern uint32_t crc_32(const uint8_t *buf, size_t size);

static inline int dlinkhdr_check(dlinkhdr_t *hdr)
{
	char buf[sizeof(dlinkhdr_t)];
	memcpy(buf, hdr, sizeof(dlinkhdr_t));

	dlinkhdr_t *tmp_hdr = (dlinkhdr_t *)buf;
	uint32_t cksum = be32toh(tmp_hdr->hdr_crc);
	tmp_hdr->hdr_crc = 0; // обнуляем для рассчета

	uint32_t calc_cksum = crc_32((uint8_t *)tmp_hdr, sizeof(dlinkhdr_t));

	if (calc_cksum != cksum)
		return -1;

	calc_cksum = crc_32((uint8_t *)(hdr->hdr_data), be32toh(hdr->hdr_data_size));

	if (calc_cksum != be32toh(hdr->hdr_data_crc))
		return -1;

	return 0;
}

#endif
