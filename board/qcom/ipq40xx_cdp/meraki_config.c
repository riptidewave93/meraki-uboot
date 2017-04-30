#include <common.h>
#include <i2c.h>
#include <board_data.h>
#include <meraki_config.h>

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

    { NULL, MERAKI_BOARD_UNKNOWN },
};

static struct product_map_entry *this_insect = NULL;

// Need to make sure we read a multiple of the page size.
static uint8_t board_data_buf[BOARD_DATA_READ_SIZE] = {0};
static struct ar531x_boarddata* board_data = NULL;

static int read_board_data(void)
{
    struct ar531x_boarddata* bd = (struct ar531_boarddata*)board_data_buf;

    uint8_t   *linebuf = bd;
    int nbytes = BOARD_DATA_READ_SIZE;
    int linebytes;
    unsigned int addr = 0;

    do {

      linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

      if (i2c_read(CONFIG_EEPROM_I2C_ADDR, addr, 2, linebuf, linebytes) != 0)
         puts ("Error reading the chip.\n");

      linebuf += linebytes;
      addr += linebytes;
      nbytes -= linebytes;

    } while (nbytes > 0);

    if (ntohl(bd->magic) != AR531X_BD_MAGIC) {
         puts ("Bad Board Data Magic.\n");
    }

    board_data = bd;

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

char *get_insect_prompt()
{
   if (this_insect) {
      return this_insect->prompt;
   }

   return CONFIG_SYS_PROMPT;
}

enum meraki_product get_insect_id()
{
   if (this_insect) {
      return this_insect->product;
   }

   return MERAKI_BOARD_UNKNOWN;
}

void set_insect_config()
{
   if (this_insect) {
      setenv("itb_config", this_insect->itb_config);
   }
}
