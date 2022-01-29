#ifndef __PARA_DEF__
#define __PARA_DEF__

#define BUFF_LEN_16     16
#define BUFF_LEN_32     32
#define BUFF_LEN_64     64
#define BUFF_LEN_128    128
#define BUFF_LEN_256    256
#define BUFF_LEN_512    512
#define BUFF_LEN_1024   1024
#define BUFF_LEN_4096   4096

#define IPV4_STR_LEN    16
#define MAC_BUFF_LEN    6
typedef unsigned char MAC_T[MAC_BUFF_LEN];

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#define BYTE    unsigned char
#define CHAR    char
#define PCHAR   char*
#define UCHAR   unsigned char
#define PUCHAR  unsigned char*
#define INT     int
#define UINT    unsigned int
#define LONG    long
#define ULONG   unsigned long
#define LLONG   long long
#define ULLONG  unsigned long long
#define BOOL    int
#define USHORT  unsigned short
#define SHORT   short

#define APSON_STATUS    int
#define APSON_SUCCESS	0
#define APSON_FAILED	-1

#define VOID    void
#define PVOID   void* //Kyo say.

#define TRUE    1
#define FALSE   0

#define CHECKSUM_BYTE   4

/* String Compare */
#define STR_EQUAL       0

#endif // __PARA_DEF__
