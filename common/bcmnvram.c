
/*
 * NVRAM variable manipulation (Linux kernel half)
 *
 * Copyright (C) 2012, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: nvram_linux.c,v 1.10 2010-09-17 04:51:19 $
 */

#include <linux/stddef.h>
#include <bcmnvram.h>
#include <asm/arch/bcmutils-nvram.h>
// #include "bcmutils.h"
#include <bcmendian.h>
#include <bcmnvram.h>
#include <sbsdram.h>
#include "configs/bcm95301x_svk.h"

struct nvram_tuple *(_nvram_realloc)(struct nvram_tuple *t, const char *name, const char *value);
int _nvram_read(void *buf);
static void(nvram_free)(void);
char *nvram_get(const char *name);
int nvram_set(const char *name, const char *value);
int nvram_unset(const char *name);
int nvram_getall(char *buf, int count);
int _nvram_commit(struct nvram_header *header);
int _nvram_init(void);
void nvram_exit(void);
uint8 nvram_calc_crc(struct nvram_header *nvh);

static struct nvram_tuple *(nvram_hash)[257];
static struct nvram_tuple *nvram_dead;
static unsigned char nflash_nvh[CONFIG_NVRAM_SIZE];

static struct nvram_header *nvram_header = NULL;

// WNC Steve 2013/06/26, temporarily
typedef struct nvpair {
    char *name;
    char *val;
} NV_PAIR;

void add_nvram_params(void)
{
    int i;
    NV_PAIR nv[] = {
            {"boardtype", "0x646"},
            {"boardrev", "0x1100"},
            {"boardflags", "0x110"},
            {"boardflags2", "0"},
            {"sromrev", "8"},
            {"clkfreq", "800,533"},
            {"xtalfreq", "25000"},
            {"sdram_config", "0x146"},
            {"sdram_ncdl", "0"},
            {"et0phyaddr", "30"},
            {"et0mdcport", "0"},
            {"et0macaddr", "00:22:44:66:88:00"},
            {"vlan1ports", "0 1 2 3 4 5*"},
            {"vlan1hwname", "et0"},
            {"vlan2ports", "5"},
            {"vlan2hwname", "et0"},
            {"landevs", "vlan1 wl0 wl1 wl2"},
            {"wandevs", "vlan2"},
            {"lan_ipaddr", "192.168.1.1"},
            {"lan_netmask", "255.255.255.0"},
            {"boot_wait", "on"},
            {"wait_time", "3"},
            {"reset_gpio", "11"},
            {"watchdog", "3000"},
            {"devpath0", "pci/1/1/"},
            {"0:venid", "0x14E4"},
            {"0:boardvendor", "0x14E4"},
            {"0:sromrev", "9"},
            {"0:boardflags", "0x80001200"},
            {"0:boardflags2", "0x100000"},
            {"0:devid", "0x4332"},
            {"0:xtalfreq", "20000"},
            {"0:macaddr", "00:90:4C:0B:50:22"},
            {"0:aa2g", "7"},
            {"0:ag0", "0"},
            {"0:ag1", "0"},
            {"0:ag2", "0"},
            {"0:txchain", "7"},
            {"0:rxchain", "7"},
            {"0:antswitch", "0"},
            {"0:tssipos2g", "1"},
            {"0:extpagain2g", "3"},
            {"0:pdetrange2g", "3"},
            {"0:antswctl2g", "0"},
            {"0:elna2g", "2"},
            {"0:maxp2ga0", "0x74"},
            {"0:pa2gw0a0", "0xFE7C"},
            {"0:pa2gw1a0", "0x1F1B"},
            {"0:pa2gw2a0", "0xF89C"},
            {"0:maxp2ga1", "0x74"},
            {"0:pa2gw0a1", "0xFE85"},
            {"0:pa2gw1a1", "0x1EA5"},
            {"0:pa2gw2a1", "0xF8BF"},
            {"0:maxp2ga2", "0x74"},
            {"0:pa2gw0a2", "0xFE82"},
            {"0:pa2gw1a2", "0x1EC5"},
            {"0:pa2gw2a2", "0xF8B8"},
            {"0:cckbw202gpo", "0x2200"},
            {"0:cckbw20ul2gpo", "0x2200"},
            {"0:legofdmbw202gpo", "0x88765433"},
            {"0:legofdmbw20ul2gpo", "0x88765433"},
            {"0:mcsbw202gpo", "0x88765433"},
            {"0:mcsbw20ul2gpo", "0x88765433"},
            {"0:mcsbw402gpo", "0x99855433"},
            {"0:mcs32po", "0x3"},
            {"0:legofdm40duppo", "0"},
            {"0:parefldovoltage", "60"},
            {"0:ccode", "0"},
            {"0:regrev", "0"},
            {"0:ledbh0", "11"},
            {"0:ledbh1", "11"},
            {"0:ledbh2", "11"},
            {"0:ledbh3", "11"},
            {"0:ledbh12", "2"},
            {"0:leddc", "0xFFFF"},
            {"devpath1", "pci/2/1/"},
            {"1:venid", "0x14E4"},
            {"1:boardvendor", "0x14E4"},
            {"1:sromrev", "9"},
            {"1:boardflags", "0x90000200"},
            {"1:boardflags2", "0x200000"},
            {"1:devid", "0x4333"},
            {"1:xtalfreq", "20000"},
            {"1:macaddr", "00:90:4C:0B:60:55"},
            {"1:aa5g", "7"},
            {"1:ag0", "0"},
            {"1:ag1", "0"},
            {"1:ag2", "0"},
            {"1:txchain", "7"},
            {"1:rxchain", "7"},
            {"1:antswitch", "0"},
            {"1:subband5gver", "1"},
            {"1:tssipos5g", "1"},
            {"1:extpagain5g", "3"},
            {"1:pdetrange5g", "6"},
            {"1:antswctl5g", "0"},
            {"1:elna5g", "3"},
            {"1:maxp5gla0", "0x58"},
            {"1:pa5glw0a0", "0xFE91"},
            {"1:pa5glw1a0", "0x16AE"},
            {"1:pa5glw2a0", "0xFA81"},
            {"1:maxp5ga0", "0x68"},
            {"1:pa5gw0a0", "0xFE86"},
            {"1:pa5gw1a0", "0x1F24"},
            {"1:pa5gw2a0", "0xF89E"},
            {"1:maxp5gha0", "0x68"},
            {"1:pa5ghw0a0", "0xFE85"},
            {"1:pa5ghw1a0", "0x1ECF"},
            {"1:pa5ghw2a0", "0xF8AD"},
            {"1:maxp5gla1", "0x58"},
            {"1:pa5glw0a1", "0xFE91"},
            {"1:pa5glw1a1", "0x1693"},
            {"1:pa5glw2a1", "0xFA84"},
            {"1:maxp5ga1", "0x68"},
            {"1:pa5gw0a1", "0xFE85"},
            {"1:pa5gw1a1", "0x1EF8"},
            {"1:pa5gw2a1", "0xF8A7"},
            {"1:maxp5gha1", "0x68"},
            {"1:pa5ghw0a1", "0xFE86"},
            {"1:pa5ghw1a1", "0x1EA1"},
            {"1:pa5ghw2a1", "0xF8B8"},
            {"1:maxp5gla2", "0x58"},
            {"1:pa5glw0a2", "0xFEA1"},
            {"1:pa5glw1a2", "0x1651"},
            {"1:pa5glw2a2", "0xFA9F"},
            {"1:maxp5ga2", "0x68"},
            {"1:pa5gw0a2", "0xFE8B"},
            {"1:pa5gw1a2", "0x1EE2"},
            {"1:pa5gw2a2", "0xF8B5"},
            {"1:maxp5gha2", "0x68"},
            {"1:pa5ghw0a2", "0xFE8D"},
            {"1:pa5ghw1a2", "0x1EA5"},
            {"1:pa5ghw2a2", "0xF8C1"},
            {"1:legofdmbw205glpo", "0"},
            {"1:legofdmbw20ul5glpo", "0"},
            {"1:legofdmbw205gmpo", "0x50000000"},
            {"1:legofdmbw20ul5gmpo", "0x50000000"},
            {"1:legofdmbw205ghpo", "0x50000000"},
            {"1:legofdmbw20ul5ghpo", "0x50000000"},
            {"1:mcsbw205glpo", "0"},
            {"1:mcsbw20ul5glpo", "0"},
            {"1:mcsbw405glpo", "0"},
            {"1:mcsbw205gmpo", "0x50000000"},
            {"1:mcsbw20ul5gmpo", "0x50000000"},
            {"1:mcsbw405gmpo", "0x50000000"},
            {"1:mcsbw205ghpo", "0x50000000"},
            {"1:mcsbw20ul5ghpo", "0x50000000"},
            {"1:mcsbw405ghpo", "0x50000000"},
            {"1:mcs32po", "0"},
            {"1:legofdm40duppo", "0"},
            {"1:parefldovoltage", "35"},
            {"1:ccode", "0"},
            {"1:regrev", "0"},
            {"1:ledbh0", "11"},
            {"1:ledbh1", "11"},
            {"1:ledbh2", "11"},
            {"1:ledbh3", "11"},
            {"1:ledbh12", "2"},
            {"1:leddc", "0xFFFF"},
            {"devpath2", "pci/3/1/"},
            {"2:venid", "0x14e4"},
            {"2:boardvendor", "0x14e4"},
            {"2:sromrev", "8"},
            {"2:boardflags", "0x200"},
            {"2:boardflags2", "0x1800"},
            {"2:devid", "0x4359"},
            {"2:xtalfreq", "20000"},
            {"2:macaddr", "00:90:4c:04:30:11"},
            {"2:aa2g", "1"},
            {"2:aa5g", "1"},
            {"2:ag0", "2"},
            {"2:txchain", "1"},
            {"2:rxchain", "1"},
            {"2:antswitch", "0"},
            {"2:tssipos2g", "1"},
            {"2:extpagain2g", "2"},
            {"2:pdetrange2g", "2"},
            {"2:triso2g", "4"},
            {"2:antswctl2g", "0"},
            {"2:elna2g", "0"},
            {"2:maxp2ga0", "0x4c"},
            {"2:pa2gw0a0", "0xff3e"},
            {"2:pa2gw1a0", "0x1636"},
            {"2:pa2gw2a0", "0xfaca"},
            {"2:cck2gpo", "0"},
            {"2:ofdm2gpo0", "0x44444444"},
            {"2:mcs2gpo0", "0x4444"},
            {"2:mcs2gpo1", "0x4444"},
            {"2:mcs2gpo2", "0x4444"},
            {"2:mcs2gpo3", "0x4444"},
            {"2:mcs2gpo4", "0x6666"},
            {"2:mcs2gpo5", "0x6666"},
            {"2:mcs2gpo6", "0x6666"},
            {"2:mcs2gpo7", "0x6666"},
            {"2:tssipos5g", "1"},
            {"2:extpagain5g", "2"},
            {"2:pdetrange5g", "2"},
            {"2:triso5g", "2"},
            {"2:antswctl5g", "0"},
            {"2:elna5g", "0"},
            {"2:maxp5gla0", "0x40"},
            {"2:pa5glw0a0", "0xfe98"},
            {"2:pa5glw1a0", "0x13e1"},
            {"2:pa5glw2a0", "0xfb14"},
            {"2:maxp5ga0", "0x40"},
            {"2:pa5gw0a0", "0xfe98"},
            {"2:pa5gw1a0", "0x13e1"},
            {"2:pa5gw2a0", "0xfb14"},
            {"2:maxp5gha0", "0x40"},
            {"2:pa5ghw0a0", "0xfe9d"},
            {"2:pa5ghw1a0", "0x133d"},
            {"2:pa5ghw2a0", "0xfb36"},
            {"2:ofdm5glpo", "0x20000000"},
            {"2:ofdm5gpo", "0x20000000"},
            {"2:ofdm5ghpo", "0x20000000"},
            {"2:mcs5glpo0", "0"},
            {"2:mcs5glpo1", "0x2000"},
            {"2:mcs5glpo2", "0"},
            {"2:mcs5glpo3", "0x2000"},
            {"2:mcs5glpo4", "0x2222"},
            {"2:mcs5glpo5", "0x2222"},
            {"2:mcs5glpo6", "0x2222"},
            {"2:mcs5glpo7", "0x2222"},
            {"2:mcs5gpo0", "0"},
            {"2:mcs5gpo1", "0x2000"},
            {"2:mcs5gpo2", "0"},
            {"2:mcs5gpo3", "0x2000"},
            {"2:mcs5gpo4", "0x4444"},
            {"2:mcs5gpo5", "0x4444"},
            {"2:mcs5gpo6", "0x4444"},
            {"2:mcs5gpo7", "0x4444"},
            {"2:mcs5ghpo0", "0"},
            {"2:mcs5ghpo1", "0x2000"},
            {"2:mcs5ghpo2", "0"},
            {"2:mcs5ghpo3", "0x2000"},
            {"2:mcs5ghpo4", "0x4444"},
            {"2:mcs5ghpo5", "0x4444"},
            {"2:mcs5ghpo6", "0x4444"},
            {"2:mcs5ghpo7", "0x4444"},
            {"2:cddpo", "0"},
            {"2:stbcpo", "0"},
            {"2:bw40po", "0"},
            {"2:bwduppo", "0"},
            {"2:ccode", "0"},
            {"2:regrev", "0"},
            {"2:ledbh0", "11"},
            {"2:ledbh1", "11"},
            {"2:ledbh2", "11"},
            {"2:ledbh3", "2"},
            {"2:leddc", "0xffff"},
            {NULL, NULL},
    };

    printf("add wireless nvram parameters...\n");

    for (i=0; nv[i].name; i++)
    {
        nvram_set(nv[i].name, nv[i].val);
    }
    nvram_commit();
}
// WNC

struct nvram_tuple *_nvram_realloc(struct nvram_tuple *t, const char *name, const char *value)
{
	if (!(t = malloc(sizeof(struct nvram_tuple) + strlen(name) + 1 + strlen(value) + 1))) {
		printf("_nvram_realloc: our of memory\n");
		return NULL;
	}
    //printf("_nvram_realloc: %x size %x\n", t, sizeof(struct nvram_tuple) + strlen(name) + 1 + strlen(value) + 1);

	/* Copy name */
	t->name = (char *) &t[1];
	strcpy(t->name, name);

	/* Copy value */
	t->value = t->name + strlen(name) + 1;
	strcpy(t->value, value);

	return t;
}

/* Free all tuples. */
static void nvram_free(void)
{
	uint i;
	struct nvram_tuple *t, *next;

	/* Free hash table */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = next) {
			next = t->next;
            //printf("hash free %x\n", t);
            free(t);
		}
		nvram_hash[i] = NULL;
	}

	/* Free dead table */
	for (t = nvram_dead; t; t = next) {
		next = t->next;
        //printf("dead free %x", t);
        free(t);
	}
	nvram_dead = NULL;
}

/* String hash */
static inline uint hash(const char *s)
{
	uint hashval = 0;

	while (*s)
		hashval = 31 *hashval + *s++;

	return hashval;
}

/* (Re)initialize the hash table. Should be locked. */
static int nvram_rehash(struct nvram_header *header)
{
	char buf[] = "0xXXXXXXXX", *name, *value, *end, *eq;

	/* (Re)initialize hash table */
	nvram_free();

	/* Parse and set "name=value\0 ... \0\0" */
	name = (char *) &header[1];
	end = (char *) header + CONFIG_NVRAM_SIZE - 2;
	end[0] = end[1] = '\0';
	for (; *name; name = value + strlen(value) + 1) {
		if (!(eq = strchr(name, '=')))
			break;
		*eq = '\0';
		value = eq + 1;
		nvram_set(name, value);
		*eq = '=';
	}

	/* Set special SDRAM parameters */
	if (!nvram_get("sdram_init")) {
		sprintf(buf, "0x%04X", (uint16)(header->crc_ver_init >> 16));
		nvram_set("sdram_init", buf);
	}
	if (!nvram_get("sdram_config")) {
		sprintf(buf, "0x%04X", (uint16)(header->config_refresh & 0xffff));
		nvram_set("sdram_config", buf);
	}
	if (!nvram_get("sdram_refresh")) {
		sprintf(buf, "0x%04X", (uint16)((header->config_refresh >> 16) & 0xffff));
		nvram_set("sdram_refresh", buf);
	}
	if (!nvram_get("sdram_ncdl")) {
		sprintf(buf, "0x%08X", header->config_ncdl);
		nvram_set("sdram_ncdl", buf);
	}

	return 0;
}

int _nvram_read(void *buf)
{
	uint32 *src, *dst;
	uint i;

	if (!nvram_header) {
        printf("NVRAM header not found\n");
		return -1;
    }

	src = (uint32 *) nvram_header;
	dst = (uint32 *) buf;

	for (i = 0; i < sizeof(struct nvram_header); i += 4)
		*dst++ = *src++;

	for (; i < nvram_header->len && i < CONFIG_NVRAM_SIZE; i += 4)
		*dst++ = ltoh32(*src++);

	return 0;
}

/* Get the value of an NVRAM variable. Should be locked. */
char *nvram_get(const char *name)
{
	uint i;
	struct nvram_tuple *t;
	char *value;

	if (!name)
		return NULL;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (t = nvram_hash[i]; t && strcmp(t->name, name); t = t->next);

	value = t ? t->value : NULL;

	return value;
}

/* Set the value of an NVRAM variable. Should be locked. */
int nvram_set(const char *name, const char *value)
{
	uint i;
	struct nvram_tuple *t, *u, **prev;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (prev = &nvram_hash[i], t = *prev; t && strcmp(t->name, name);
	     prev = &t->next, t = *prev);

	/* (Re)allocate tuple */
	if (!(u = _nvram_realloc(t, name, value)))
		return -12; /* -ENOMEM */

	/* Value reallocated */
	if (t && t == u)
		return 0;

	/* Move old tuple to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	/* Add new tuple to the hash table */
	u->next = nvram_hash[i];
	nvram_hash[i] = u;

	return 0;
}

/* Unset the value of an NVRAM variable. Should be locked. */
int nvram_unset(const char *name)
{
	uint i;
	struct nvram_tuple *t, **prev;

	if (!name)
		return 0;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (prev = &nvram_hash[i], t = *prev; t && strcmp(t->name, name);
	     prev = &t->next, t = *prev);

	/* Move it to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	return 0;
}

/* Get all NVRAM variables. Should be locked. */
int nvram_getall(char *buf, int count)
{
	uint i;
	struct nvram_tuple *t;
	int len = 0;

	//bzero(buf, count);
        memset(buf,0,count);

	/* Write name=value\0 ... \0\0 */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = t->next) {
			if ((count - len) > (strlen(t->name) + 1 + strlen(t->value) + 1))
				len += sprintf(buf + len, "%s=%s", t->name, t->value) + 1;
			else
				break;
		}
	}

	return 0;
}

/* Regenerate NVRAM. Should be locked. */
int _nvram_commit(struct nvram_header *header)
{
	char *init, *config, *refresh, *ncdl;
	char *ptr, *end;
	int i;
	struct nvram_tuple *t;

	/* Regenerate header */
	header->magic = NVRAM_MAGIC;
	header->crc_ver_init = (NVRAM_VERSION << 8);
	if (!(init = nvram_get("sdram_init")) ||
	    !(config = nvram_get("sdram_config")) ||
	    !(refresh = nvram_get("sdram_refresh")) ||
	    !(ncdl = nvram_get("sdram_ncdl"))) {
		header->crc_ver_init |= SDRAM_INIT << 16;
		header->config_refresh = SDRAM_CONFIG;
		header->config_refresh |= SDRAM_REFRESH << 16;
		header->config_ncdl = 0;
	} else {
		header->crc_ver_init |= (bcm_strtoul(init, NULL, 0) & 0xffff) << 16;
		header->config_refresh = bcm_strtoul(config, NULL, 0) & 0xffff;
		header->config_refresh |= (bcm_strtoul(refresh, NULL, 0) & 0xffff) << 16;
		header->config_ncdl = bcm_strtoul(ncdl, NULL, 0);
	}

	/* Clear data area */
	ptr = (char *) header + sizeof(struct nvram_header);
	//bzero(ptr, nvram_space - sizeof(struct nvram_header));
        memset(ptr, 0,CONFIG_NVRAM_SIZE - sizeof(struct nvram_header));
	/* Leave space for a double NUL at the end */
	end = (char *) header + CONFIG_NVRAM_SIZE - 2;

	/* Write out all tuples */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = t->next) {
			if ((ptr + strlen(t->name) + 1 + strlen(t->value) + 1) > end)
				break;
			ptr += sprintf(ptr, "%s=%s", t->name, t->value) + 1;
		}
	}

	/* End with a double NUL */
	ptr += 2;

	/* Set new length */
	header->len = ROUNDUP(ptr - (char *) header, 4);

	/* Set new CRC8 */
	header->crc_ver_init |= nvram_calc_crc(header);

	/* Reinitialize hash table */
	return nvram_rehash(header);
}

/* Initialize hash table. Should be locked. */
int _nvram_init(void)
{
	struct nvram_header *header;
	int ret;

	if (!(header = (struct nvram_header *) malloc(CONFIG_NVRAM_SIZE))) {
		printf("nvram_init: out of memory\n");
		return -12; /* -ENOMEM */
	}

	if ((ret = _nvram_read(header)) == 0 && header->magic == NVRAM_MAGIC) {
		nvram_rehash(header);
    }

    free(header);
	return ret;
}

/* Free hash table. Should be locked. */
void nvram_exit(void)
{
	nvram_free();
}

/* returns the CRC8 of the nvram */
uint8 nvram_calc_crc(struct nvram_header *nvh)
{
	struct nvram_header tmp;
	uint8 crc;

	/* Little-endian CRC8 over the last 11 bytes of the header */
	tmp.crc_ver_init = htol32((nvh->crc_ver_init & NVRAM_CRC_VER_MASK));
	tmp.config_refresh = htol32(nvh->config_refresh);
	tmp.config_ncdl = htol32(nvh->config_ncdl);

	crc = hndcrc8((uint8 *) &tmp + NVRAM_CRC_START_POSITION,
		sizeof(struct nvram_header) - NVRAM_CRC_START_POSITION,
		CRC8_INIT_VALUE);

	/* Continue CRC8 over data bytes */
	crc = hndcrc8((uint8 *) &nvh[1], nvh->len - sizeof(struct nvram_header), crc);

	return crc;
}

extern unsigned char embedded_nvram[];

static struct nvram_header *find_nvram(uint32 start_addr, bool *isemb)
{
	struct nvram_header *nvh;
	uint32 off; 
	uint8 tmp_crc;
    
    *isemb = FALSE;
    for (off = 0; off < CONFIG_NVRAM_PARTITION_SIZE; off += CONFIG_NVRAM_SIZE) {
		nvh = (struct nvram_header *)(CONFIG_NAND_FLASH_BASE_ADDR + start_addr + off);
		if (nvh->magic != NVRAM_MAGIC)
			continue;
        printf("NVRAM_MAGIC found at offset %x\n", start_addr + off);

// WNC Steve 2013/07/10
        /* Read into the nand_nvram */
        if (readnvram(start_addr + off, (u_char *)nflash_nvh)) {
            printf("error reading env\n");
            continue;
        }
// WNC

	tmp_crc = (uint8) (((struct nvram_header *)nflash_nvh)->crc_ver_init & ~(NVRAM_CRC_VER_MASK));

        printf("nflash_nvh magic: %x\n", ((struct nvram_header *)nflash_nvh)->magic);
        printf("nflash_nvh CRC: %x\n", tmp_crc);
        printf("nflash_nvh CRC calc: %x\n", nvram_calc_crc(nflash_nvh));

        if (nvram_calc_crc(nflash_nvh) == tmp_crc) 
            return (struct nvram_header *)nflash_nvh;
    }

    printf("find_nvram: nvram not found, trying embedded nvram next\n");

    nvh = (struct nvram_header *)embedded_nvram;

    if (nvh->magic != NVRAM_MAGIC) {
	printf("find_nvram: no embedded_nvram\n");
	goto no_found;
    }

    tmp_crc = (uint8) (nvh->crc_ver_init & ~(NVRAM_CRC_VER_MASK));
    printf("embedded nvram magic: %x\n", nvh->magic);
    printf("embedded nvram CRC: %x\n", tmp_crc);
    printf("embedded nvram CRC calc: %x\n", nvram_calc_crc(nvh));

    if (nvram_calc_crc(nvh) == tmp_crc) {
	*isemb = TRUE;
        return (nvh);
    }
 

no_found:
    printf("find_nvram: no nvram found\n");
	return (NULL);
}


int nvram_init()
{
	bool isemb;
	int ret;
	static int nvram_status = -1;

	/* Check for previous 'restore defaults' condition */
	if (nvram_status == 1)
		return 1;

	/* Check whether nvram already initilized */
	if (nvram_status == 0)
		return 0;

	/* Check for NVRAM in primary partition */
	nvram_header = find_nvram(CONFIG_NVRAM_OFFSET,&isemb);

    if (nvram_header == NULL) {
        printf("Restoring NVRAM Default Configuration\n");
    }

    ret = _nvram_init();

    if (isemb) nvram_commit();

// WNC Steve 2013/06/25, temporarily...
    if (isemb )//|| !nvram_header)
        add_nvram_params();
// WNC

    printf("_nvram_init: ret %x\n", ret);

	if (ret == 0) {
		/* Restore defaults if embedded NVRAM used */
		if (nvram_header) {
			ret = 1;
		}
	}
    nvram_status = ret;
    printf("nvram_init: ret %x\n", ret);
	return ret;
}

int nvram_find()
{
	int ret;
	bool isemb;
	struct nvram_header *nvh;

	/* Check for NVRAM in primary partition */
        nvh = find_nvram(CONFIG_NVRAM_OFFSET,&isemb);

    if (nvh == NULL) {
        printf("Cannot find NVRAM in Default Configuration\n");
	ret = 0;
    } else
	ret =1;

	return ret;
}


int nvram_commit()
{
	struct nvram_header *header;
	bool isemb;
    int ret;

    if (!(header = (struct nvram_header *) malloc(CONFIG_NVRAM_SIZE))) {
            printf("nvram_commit: out of memory\n");
            return 1;
    }

    /* Regenerate NVRAM */
    ret = _nvram_commit(header);
    if (ret) {
       printf("nvram commit failed\n");
       return 1;
    }

    printf("nvram_commit: Writing flash partitions\n");

// WNC Steve 2013/07/10
    /* Write to both primary and redundant areas */
    writenvram(CONFIG_NVRAM_OFFSET, (u_char *)(header));
// WNC

    free(header);

    nvram_header = find_nvram(CONFIG_NVRAM_OFFSET,&isemb);

    return 0;
}
