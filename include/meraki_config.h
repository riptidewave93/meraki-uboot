#ifndef __MERAKI_CONFIG_H__
#define __MERAKI_CONFIG_H__

enum meraki_product
{
    MERAKI_BOARD_UNKNOWN = 0,

    MERAKI_BOARD_STINKBUG,
    MERAKI_BOARD_LADYBUG,
    MERAKI_BOARD_NOISY_CRICKET,
    MERAKI_BOARD_MAGGOT,
    MERAKI_BOARD_DUNGBEETLE_OMNI,
    MERAKI_BOARD_DUNGBEETLE_PATCH,
    MERAKI_BOARD_GRUB,
    MERAKI_BOARD_TOE_BITER_OMNI,
    MERAKI_BOARD_TOE_BITER_PATCH,
    MERAKI_BOARD_FUZZY_CRICKET,
    MERAKI_BOARD_YOWIE,
    MERAKI_BOARD_CITIZENSNIPS,
    MERAKI_BOARD_BIGFOOT,
    MERAKI_BOARD_SASQUATCH,
    MERAKI_BOARD_DOCTOROCTOPUS,
    MERAKI_BOARD_WOOKIE
};

extern char *get_meraki_prompt (void);
extern enum meraki_product get_meraki_product_id (void);
extern void set_meraki_config(void);
extern bool meraki_check_unique_id(const void *);

#endif // __MERAKI_CONFIG_H__
