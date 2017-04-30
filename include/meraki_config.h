#ifndef __MERAKI_CONFIG_H__
#define __MERAKI_CONFIG_H__

enum meraki_product
{
    MERAKI_BOARD_UNKNOWN = 0,

    MERAKI_BOARD_STINKBUG,
    MERAKI_BOARD_LADYBUG,
    MERAKI_BOARD_NOISY_CRICKET,
    MERAKI_BOARD_FUZZY_CRICKET
};

extern char *get_insect_prompt (void);
extern enum meraki_product get_insect_id (void);
extern void set_insect_config(void);

#endif // __MERAKI_CONFIG_H__
