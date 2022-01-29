#define CONFIG_DRU_NETDEV_MARK
#include "../../include/linux/sockios.h"
#include "../../dru/include/dru_mark.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>


static int bad_args(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
	int fd, i;
	struct ifreq ifr;
	uint32_t mark, x;
	int req;

	if (argc < 2) return bad_args("1+ argument(s) needed\n");

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);

	req = (argc == 2) ? SIOCGDRUMARK : SIOCSDRUMARK;

	if (req == SIOCSDRUMARK) {
		mark = 0;

		for (i = 2; i < argc; i++) {
#define CHECK_AND_SET_ROLE(role, token)		\
			if (!strcasecmp(argv[i], token)) {			\
				if (DRU_MARK_GET_ROLE(mark))			\
					return bad_args("roles conflict\n");	\
				mark |= role;					\
			}
#define CHECK_AND_SET_TYPE(type, token)		\
			if (!strcasecmp(argv[i], token)) {			\
				if (DRU_MARK_GET_TYPE(mark))			\
					return bad_args("types conflict\n");	\
				mark |= type;					\
			}
			if (!strcmp(argv[i], "0")) {
				mark = 0;
				break;
			}
			else CHECK_AND_SET_ROLE(DRU_MARK_ROLE_LAN,	"lan")
			else CHECK_AND_SET_ROLE(DRU_MARK_ROLE_WAN,	"wan")
			else CHECK_AND_SET_TYPE(DRU_MARK_TYPE_ETHER,	"ether")
			else CHECK_AND_SET_TYPE(DRU_MARK_TYPE_BRIDGE,	"bridge")
			else CHECK_AND_SET_TYPE(DRU_MARK_TYPE_PPP,	"ppp")
			else CHECK_AND_SET_TYPE(DRU_MARK_TYPE_WIRELESS,	"wireless")
			else
				return bad_args("unknown token \"%s\"\n", argv[i]);
		}

		ifr.ifr_metric = *(int *)&mark;
	}

	fd = socket(PF_INET, SOCK_DGRAM, 0);

	if (fd == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}

	i = ioctl(fd, req, &ifr);

	if (i == -1) {
		perror("ioctl");
		close(fd);
		return EXIT_FAILURE;
	}

	close(fd);

	if (req == SIOCSDRUMARK) return EXIT_SUCCESS;

	/* parse get */
	mark = *(uint32_t *)&ifr.ifr_metric;
	x = DRU_MARK_GET_USED_PART(mark);
	printf("0x%08x/0x%08x\n", x, mark);
	mark = x;

	fprintf(stderr, "set:");

	if (!mark) fprintf(stderr, " no bits!\n");
	else {
#define CHECK_BITS(bits, msg)			\
	if (mark & (bits)) {			\
		fprintf(stderr, " %s", msg);	\
		mark &= ~(bits);		\
	}

#define CHECK_ROLE(role, token)					\
			case role:				\
				fprintf(stderr, " %s", token);	\
				break
		x = DRU_MARK_GET_ROLE(mark);
		switch (x) {
			CHECK_ROLE(DRU_MARK_ROLE_LAN, "lan");
			CHECK_ROLE(DRU_MARK_ROLE_WAN, "wan");
			default:
				fprintf(stderr, " unknown_role=0x%08x", x);
		}
		mark &= ~x;

#define CHECK_TYPE CHECK_ROLE
		x = DRU_MARK_GET_TYPE(mark);
		switch (x) {
			CHECK_TYPE(DRU_MARK_TYPE_ETHER,		"ether");
			CHECK_TYPE(DRU_MARK_TYPE_BRIDGE,	"bridge");
			CHECK_TYPE(DRU_MARK_TYPE_PPP,		"ppp");
			CHECK_TYPE(DRU_MARK_TYPE_WIRELESS,	"wireless");
			default:
				fprintf(stderr, " unknown_type=0x%08x", x);
		}
		mark &= ~x;

		fprintf(stderr, "\nrest: ");

		if (!mark)
			fprintf(stderr, "absent, OK\n");
		else
			fprintf(stderr, "0x%08x, BAD\n", mark);
	}

	return EXIT_SUCCESS;
}
