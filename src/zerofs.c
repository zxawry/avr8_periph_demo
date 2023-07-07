// file: zerofs.c
// author: awry

#include "zerofs.h"

#include <avr/pgmspace.h>

#include "serio.h"
#include "lib/diskio.h"

#define ZEROFS_SIGN_TOKEN (0x69)
#define ZEROFS_INFO_SECTOR_INDEX (1)
#define ZEROFS_INFO_SECTOR_LENGTH (9)
#define ZEROFS_DATA_SECTOR_START (2)
#define ZEROFS_DATA_SECTOR_END (2097152)

typedef enum {
	RES_DISK_NOINIT = 4,
	RES_TOKEN_MISMATCH,
	RES_SECTOR_OUT_OF_RANGE,
	RES_OFFSET_OUT_OF_RANGE,
} status_t;

const uint8_t ZEROFS_INFO_SECTOR_CONTENT[] PROGMEM = {
	ZEROFS_SIGN_TOKEN,
	0x02, 0x00, 0x00, 0x00, // ZEROFS_SECTOR_POINTER
	0x00, 0x00, 0x00, 0x00, // ZEROFS_OFFSET_POINTER
};

static uint32_t sector_pointer;
static uint16_t offset_pointer;

uint8_t zerofs_init(void)
{
	uint8_t res;

	res = disk_initialize();
	if (res)
		return RES_DISK_NOINIT;

	return 0;
}

uint8_t zerofs_mount(void)
{
	uint8_t res;
	uint8_t data[32];

	sector_pointer = 0;
	offset_pointer = 0;

	// read info sector.
	res = disk_readp(data, ZEROFS_INFO_SECTOR_INDEX, 0, 32);
	if (res)
		return res;

	if (data[0] != ZEROFS_SIGN_TOKEN) {
		// token dose not match.
		return RES_TOKEN_MISMATCH;
	}

	sector_pointer = (uint32_t) data[1];
	sector_pointer |= (uint32_t) data[2] << 8;
	sector_pointer |= (uint32_t) data[3] << 16;
	sector_pointer |= (uint32_t) data[4] << 24;

	if (sector_pointer > ZEROFS_DATA_SECTOR_END || sector_pointer < ZEROFS_DATA_SECTOR_START)
		return RES_SECTOR_OUT_OF_RANGE;

	offset_pointer = (uint16_t) data[5];
	offset_pointer |= (uint16_t) data[6] << 8;

	if (offset_pointer > 512)
		return RES_OFFSET_OUT_OF_RANGE;

	return 0;
}

uint8_t zerofs_create(void)
{
	uint8_t res;
	uint16_t i;
	uint8_t data[512];

	for (i = 0; i < ZEROFS_INFO_SECTOR_LENGTH; i++)
		data[i] = pgm_read_byte(&(ZEROFS_INFO_SECTOR_CONTENT[i]));
	for (; i < 512; i++)
		data[i] = 0x00;

	res = disk_writep(0, ZEROFS_INFO_SECTOR_INDEX);
	if (res)
		return res;

	res = disk_writep(data, 512);
	if (res)
		return res;

	res = disk_writep(0, 0);
	if (res)
		return res;

	return 0;
}

uint8_t zerofs_read(char * data, uint32_t index, uint8_t count)
{
	uint8_t res;
	uint32_t sector;
	uint16_t offset;

	sector = (index / 512) + ZEROFS_DATA_SECTOR_START;
	offset = index % 512;

	if (sector > ZEROFS_DATA_SECTOR_END || sector < ZEROFS_DATA_SECTOR_START)
		return RES_SECTOR_OUT_OF_RANGE;

	if (count > 512 - offset) { // check for cross sector read
		res = disk_readp((uint8_t *) data, sector, offset, 512 - offset);
		if (res)
			return res;
		count -= (512 - offset);
		data += (512 - offset);
	}

	res = disk_readp((uint8_t *) data, sector + 1, 0, count);
	if (res)
		return res;

	return 0;
}

uint8_t zerofs_write(char * data, uint32_t index, uint8_t count)
{
	uint8_t res;
	char buffer[512];
	char *p;
	uint32_t sector;
	uint16_t offset;

	sector = (index / 512) + ZEROFS_DATA_SECTOR_START;

	if (sector > ZEROFS_DATA_SECTOR_END || sector < ZEROFS_DATA_SECTOR_START)
		return RES_SECTOR_OUT_OF_RANGE;

	res = disk_readp((uint8_t *) buffer, sector, 0, 512);
	if (res)
		return res;

	p = data;
	offset = index % 512;
	for (; offset < 512 && count; offset++, count--, p++)
		buffer[offset] = *p;

	res = disk_writep(0, sector);
	if (res)
		return res;

	res = disk_writep((uint8_t *) buffer, 512);
	if (res)
		return res;

	res = disk_writep(0, 0);
	if (res)
		return res;

	if (count == 0)
		return 0;

	res = disk_readp((uint8_t *) buffer, sector + 1, 0, 512);
	if (res)
		return res;

	offset = 0;
	for (; offset < 512 && count; offset++, count--, p++)
		buffer[offset] = *p;

	res = disk_writep(0, sector + 1);
	if (res)
		return res;

	res = disk_writep((uint8_t *) buffer, 512);
	if (res)
		return res;

	res = disk_writep(0, 0);
	if (res)
		return res;

	return 0;
}

uint8_t zerofs_print(uint32_t sector)
{
	uint16_t i;
	uint8_t res;
	uint8_t buffer[128];

	for (i = 0; i < 512; i += 128) {
		res = disk_readp(buffer, sector, i, 128);
		if (res)
			return res;
		put_dump(buffer, 128);
	}

	return 0;
/*
	uint8_t res;
	uint8_t buffer[512];

	res = disk_readp(buffer, sector, 0, 512);
	if (res)
		return res;

	put_dump(buffer, 128);
	put_dump(buffer + 128, 128);
	put_dump(buffer + 256, 128);
	put_dump(buffer + 384, 128);

	return 0;
*/
}
