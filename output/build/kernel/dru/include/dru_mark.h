#ifndef DRU_MARK_H
#define DRU_MARK_H

/*
 * xxxxxxxx XXXXXXXX xxxxxxxx xxxxxxxx
 */

/* change if needed */
#define DRU_MARK_SHIFT		16
#define DRU_MARK_BITS_USED	8
#define DRU_MARK_GET_USED_PART(mark)	\
	( (mark) & (((1UL << DRU_MARK_BITS_USED) - 1) << DRU_MARK_SHIFT) )

/*
 * XXXXXXXX:
 * 	[1:0]: role
 * 		00: unspecified
 * 		01: LAN
 * 		10: WAN
 * 		11: reserved
 * 	[4:2]: type
 * 		000: unspecified/unknown
 * 		001: ethernet (except below)
 * 		010: bridge
 * 		011: ppp
 * 		100: wireless
 * 		other: reserved
 * 	[7:5]: reserved
 */
#define DRU_MARK_ROLE_LAN		(1UL << DRU_MARK_SHIFT)
#define DRU_MARK_ROLE_WAN		(2UL << DRU_MARK_SHIFT)
#define DRU_MARK_GET_ROLE(mark)		((mark) & (3UL << DRU_MARK_SHIFT))

#define DRU_MARK_TYPE_ETHER		( (1UL << 2) << DRU_MARK_SHIFT)
#define DRU_MARK_TYPE_BRIDGE		( (2UL << 2) << DRU_MARK_SHIFT)
#define DRU_MARK_TYPE_PPP		( (3UL << 2) << DRU_MARK_SHIFT)
#define DRU_MARK_TYPE_WIRELESS		( (4UL << 2) << DRU_MARK_SHIFT)
#define DRU_MARK_GET_TYPE(mark)		((mark) & ((7UL << 2) << DRU_MARK_SHIFT))

/*
 * #35747: skip MTK hwnat for definitely unoffloadable traffic
 * to avoid useless works.
 *
 * xxxxxxxx xxxxxxxx TRxxxxxx xxxxxxxx
 * 	[14]: R - skip Rx hook
 * 	[15]: T - skip Tx hook
 */
#define DRU_MARK_SKIP_HWNAT_RX		(1UL << 14)
#define DRU_MARK_SKIP_HWNAT_TX		(1UL << 15)

#endif

