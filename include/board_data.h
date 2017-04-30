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
};

#endif
