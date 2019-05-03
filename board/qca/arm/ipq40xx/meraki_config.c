#include <common.h>
#include <i2c.h>
#include <board_data.h>
#include <meraki_config.h>
#include "ipq40xx_cdp.h"

#define DISP_LINE_LEN          0x10
#define EEPROM_I2C_ADDR        CONFIG_EEPROM_I2C_ADDR
#define EEPROM_PAGE_SIZE 32
#define BOARD_DATA_READ_SIZE (sizeof(struct ar531x_boarddata) + (EEPROM_PAGE_SIZE - (sizeof(struct ar531x_boarddata) % EEPROM_PAGE_SIZE)))

struct product_map_entry
{
    const char* board_name;
    const uint16_t major;
    const char* prompt;
    enum meraki_product product;
    const char* itb_config;
};

static const struct product_map_entry product_map[] = {
    { "meraki_Stinkbug", 30, "STINKBUG # ", MERAKI_BOARD_STINKBUG, "config@1" },
    { "meraki_Ladybug", 31, "LADYBUG # ", MERAKI_BOARD_LADYBUG,   "config@3" },
    { "meraki_Noisy_Cricket", 32, "NOISY CRICKET # ", MERAKI_BOARD_NOISY_CRICKET, "config@2" },
    { "meraki_Fuzzy_Cricket", 36, "FUZZY CRICKET # ", MERAKI_BOARD_FUZZY_CRICKET, "config@4" },
    { "meraki_Maggot", 37, "MAGGOT # ", MERAKI_BOARD_MAGGOT, "config@4" },
    { "meraki_Dungbeetle_Omni", 38, "DUNGBEETLE OMNI # ", MERAKI_BOARD_DUNGBEETLE_OMNI, "config@5" },
    { "meraki_Dungbeetle_Patch", 39, "DUNGBEETLE PATCH # ", MERAKI_BOARD_DUNGBEETLE_PATCH, "config@6" },
    { "meraki_Grub", 44, "GRUB # ", MERAKI_BOARD_GRUB, "config@4" },
    { "meraki_Toe_biter_Omni", 45, "TOE_BITER OMNI # ", MERAKI_BOARD_TOE_BITER_OMNI, "config@5" },
    { "meraki_Toe_biter_Patch", 46, "TOE_BITER PATCH # ", MERAKI_BOARD_TOE_BITER_PATCH, "config@6" },

    { NULL, MERAKI_BOARD_UNKNOWN },
};

static struct product_map_entry *this_insect = NULL;

// Need to make sure we read a multiple of the page size.
static uint8_t board_data_buf[BOARD_DATA_READ_SIZE] = {0};
static struct ar531x_boarddata* board_data = NULL;

struct eeprom_i2c_config
{
	uint16_t gpio_scl;
	uint16_t gpio_scl_func;
	uint16_t gpio_sda;
	uint16_t gpio_sda_func;
	uint16_t eeprom_addr;
};

/* valid eeprom configuration for insects */
static const struct eeprom_i2c_config valid_eeprom_i2c_config[] = {
    { 20, 1, 21, 1, 0x50 }, // Stinkbug, Ladybug, Noisy Cricket
    { 10, 4, 11, 4, 0x56 }, // Maggot
};
#define NUM_VALID_EEPROM_CONFIG   (sizeof(valid_eeprom_i2c_config)/sizeof(valid_eeprom_i2c_config[0]))

static int read_board_data(void)
{
    struct ar531x_boarddata* bd = (struct ar531x_boarddata*)board_data_buf;

    uint8_t   *linebuf = (uint8_t *)bd;
    int nbytes = BOARD_DATA_READ_SIZE;
    int linebytes;
    unsigned int addr = 0;
    int i;

    for (i = 0; i < NUM_VALID_EEPROM_CONFIG; i++) {
        linebuf = (uint8_t *)bd;
        nbytes = BOARD_DATA_READ_SIZE;
        addr = 0;
        gpio_func_data_t *i2c0_gpio = gboard_param->i2c_cfg->i2c_gpio;

        i2c0_gpio[0].gpio = valid_eeprom_i2c_config[i].gpio_scl;
        i2c0_gpio[0].func = valid_eeprom_i2c_config[i].gpio_scl_func;
        i2c0_gpio[1].gpio = valid_eeprom_i2c_config[i].gpio_sda;
        i2c0_gpio[1].func = valid_eeprom_i2c_config[i].gpio_sda_func;
        i2c_qca_board_init();


    do {

      linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

      if (i2c_read(valid_eeprom_i2c_config[i].eeprom_addr, addr, 2, linebuf, linebytes) != 0)
        break;

      linebuf += linebytes;
      addr += linebytes;
      nbytes -= linebytes;

    } while (nbytes > 0);

    if (ntohl(bd->magic) == AR531X_BD_MAGIC) {
       board_data = bd;
       return 0;
    }

    }

    return 0;
}

int meraki_config_get_product(void)
{
    if (!board_data) {
        int err = read_board_data();
        if (err)
            return -1;
    }

    const struct product_map_entry* entry;
    for (entry = product_map; entry->board_name != NULL; entry++)
        if (entry->major == ntohs(board_data->major))
   {
            printf("Product: %s\n",entry->board_name);
            this_insect = entry;
            return entry->product;
   }

    return MERAKI_BOARD_UNKNOWN;
}

char *get_meraki_prompt()
{
   if (this_insect) {
      return this_insect->prompt;
   }

   return CONFIG_SYS_PROMPT;
}

enum meraki_product get_meraki_product_id()
{
   if (this_insect) {
      return this_insect->product;
   }

   return MERAKI_BOARD_UNKNOWN;
}

void set_meraki_kernel_config()
{
   if (this_insect) {
      setenv("itb_config", this_insect->itb_config);
   }
}

void meraki_insect_ethaddr(uchar *enetaddr, uint no_of_macs)
{
   int i;
   if (!board_data) {
      return;
   }

   for (i = 0; i < no_of_macs; i++) {
      memcpy(enetaddr, board_data->enet0Mac, 6);
      enetaddr += 6;
   }
}

bool meraki_check_unique_id(const void *dev_crt)
{
    const u32 *id = dev_crt;
    const u32 *qfprom_raw_serial_num = (const u32 *)0x00058008;
    return *qfprom_raw_serial_num == *id;
}
