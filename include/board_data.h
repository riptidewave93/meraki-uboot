#ifndef BOARD_DATA_H
#define BOARD_DATA_H

struct ar531x_boarddata {
    uint32_t magic;                       /* board data is valid */
#define AR531X_BD_MAGIC 0x35333131        /* "5311", for all 531x platforms */
    uint16_t cksum;                       /* checksum (starting with BD_REV 2) */
    uint16_t rev;                         /* revision of this struct */
    char     boardName[64];               /* Name of board */
    uint16_t major;                       /* Board major number */
    uint16_t minor;                       /* Board minor number */
    uint32_t config;                      /* Board configuration */
    uint16_t resetConfigGpio;             /* Reset factory GPIO pin */
    uint16_t sysLedGpio;                  /* System LED GPIO pin */

    uint32_t cpuFreq;                     /* CPU core frequency in Hz */
    uint32_t sysFreq;                     /* System frequency in Hz */
    uint32_t cntFreq;                     /* Calculated C0_COUNT frequency */

    uint8_t  wlan0Mac[6];
    uint8_t  enet0Mac[6];
    uint8_t  enet1Mac[6];

    uint16_t pciId;                       /* Pseudo PCIID for common code */
    uint16_t memCap;                      /* cap bank1 in MB */

    /* version 3 */
    uint8_t  wlan1Mac[6];                 /* (ar5212) */
    uint8_t  serial_number[12];	          /* serial number - no null padding */
};

#endif
