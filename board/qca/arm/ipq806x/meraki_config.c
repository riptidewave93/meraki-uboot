#include <common.h>
#include <i2c.h>
#include <board_data.h>
#include <meraki_config.h>
#include <asm/arch-ipq806x/iomap.h>

#define DISP_LINE_LEN          0x20
#define EEPROM_PAGE_SIZE       32
#define BOARD_DATA_READ_SIZE (sizeof(struct ar531x_boarddata) + (EEPROM_PAGE_SIZE - (sizeof(struct ar531x_boarddata) % EEPROM_PAGE_SIZE)))

extern void i2c_ipq_board_init(void);
extern int toggle_scl(uint16_t sda_pin, uint16_t scl_pin);

struct product_map_entry
{
    const char* board_name;
    const uint16_t major;
    const char* prompt;
    enum meraki_product product;
    const char* itb_config;
    const char* bootcmd;
};

struct eeprom_i2c_config
{
    const uint32_t i2c_gsbi;
    const uint32_t i2c_gsbi_base;
    const uint16_t gpio_0;
    const uint16_t gpio_1;
    const uint16_t addr;
};

static const struct product_map_entry product_map[] = {
    { "meraki_Yowie", 20, "(YOWIE)# ", MERAKI_BOARD_YOWIE, "config@1", "run meraki_bootkernel_boot"},
    { "meraki_Bigfoot", 21, "(BIGFOOT)# ", MERAKI_BOARD_BIGFOOT,   "config@2", "run meraki_bootkernel_boot" },
    { "meraki_Sasquatch", 22, "(SASQUATCH)# ", MERAKI_BOARD_SASQUATCH, "config@3", "run meraki_bootkernel_boot" },
    { "meraki_Wookie", 27, "(WOOKIE)# ", MERAKI_BOARD_WOOKIE, "config@4", "run meraki_bootkernel_boot" },
    { "meraki_Citizensnips", 40, "(CITIZENSNIPS)# ", MERAKI_BOARD_CITIZENSNIPS, "config@5", "run meraki_ubi_boot" },
    { "meraki_Doctoroctopus", 41, "(DOCTOROCTOPUS)# ", MERAKI_BOARD_DOCTOROCTOPUS, "config@6", "run meraki_ubi_boot" },

    { NULL, MERAKI_BOARD_UNKNOWN },
};

/* valid eeprom configuration for cryptids */
static const struct eeprom_i2c_config valid_eeprom_i2c_config[] = {
    { GSBI_2, I2C_GSBI2_BASE, 24, 25, 0x56 }, // Yowie, Citizen Snips
    { GSBI_7, I2C_GSBI7_BASE,  8,  9, 0x52 }, // Bigfoot, Sasquatch, Wookie, Doctor Octopus
};
#define NUM_VALID_EEPROM_CONFIG   (sizeof(valid_eeprom_i2c_config)/sizeof(valid_eeprom_i2c_config[0]))

static const struct product_map_entry *this_cryptid = NULL;

// Need to make sure we read a multiple of the page size.
static uint8_t board_data_buf[BOARD_DATA_READ_SIZE] = {0};
static struct ar531x_boarddata* board_data = NULL;

/*
 * Read eeprom and check magic
 *
 * There are two valid eeprom i2c setting for cryptids. We'll have to try
 * both since we don't know which one applies to the board we're runnig on.
 * We'll know which one it is when we see the magic number.
 */
static int read_board_data(void)
{
    struct ar531x_boarddata* bd = (struct ar531x_boarddata*)board_data_buf;

    uint8_t *linebuf;
    int i, nbytes, linebytes;
    unsigned int addr;
    uint8_t recover_i2c = 0;

 retry:
    for (i = 0; i < NUM_VALID_EEPROM_CONFIG; i++) {

        if (recover_i2c)
            toggle_scl(valid_eeprom_i2c_config[i].gpio_0, valid_eeprom_i2c_config[i].gpio_1);

        linebuf = board_data_buf;
        nbytes = BOARD_DATA_READ_SIZE;
        addr = 0;

        // Set new gboard params for eeprom i2c
        gboard_param->i2c_gsbi = valid_eeprom_i2c_config[i].i2c_gsbi;
        gboard_param->i2c_gsbi_base = valid_eeprom_i2c_config[i].i2c_gsbi_base;
        gboard_param->i2c_gpio[0].gpio = valid_eeprom_i2c_config[i].gpio_0;
        gboard_param->i2c_gpio[1].gpio = valid_eeprom_i2c_config[i].gpio_1;
        i2c_ipq_board_init();

        do {

            linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

            if (i2c_read(valid_eeprom_i2c_config[i].addr, addr, 2, linebuf, linebytes) != 0)
                break;

            linebuf += linebytes;
            addr += linebytes;
            nbytes -= linebytes;

        } while (nbytes > 0);

        if (ntohl(bd->magic) == AR531X_BD_MAGIC) {
            board_data = bd;
            break;
        }
    }

    if (!board_data) {
        puts("Bad Board Data Magic!\n");
        if (recover_i2c < 2) {
            puts("Retrying..\n");
            recover_i2c++;
            goto retry;
        }
    }

    return 0;
}

int meraki_config_get_product(void)
{
    if (!board_data)
        read_board_data();

    const struct product_map_entry* entry;
    for (entry = product_map; entry->board_name != NULL; entry++)
        if (entry->major == ntohs(board_data->major)) {
            printf("Product: %s\n",entry->board_name);
            this_cryptid = entry;
            return entry->product;
        }

    return MERAKI_BOARD_UNKNOWN;
}

char *get_meraki_prompt()
{
    if (this_cryptid)
        return this_cryptid->prompt;

    return CONFIG_SYS_PROMPT;
}

enum meraki_product get_meraki_product_id()
{
    if (this_cryptid)
        return this_cryptid->product;

    return MERAKI_BOARD_UNKNOWN;
}

void set_meraki_kernel_config()
{
    if (this_cryptid) {
        setenv("itb_config", this_cryptid->itb_config);
        setenv("bootcmd", this_cryptid->bootcmd);
    }
}

void meraki_cryptid_ethaddr(uchar *enetaddr, uint no_of_macs)
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
    return memcmp(dev_crt, board_data->serial_number, sizeof(board_data->serial_number)) == 0;
}
