// file: zerofs.c
// author: awry

#include "zerofs.h"

#include "serio.h"
#include "lib/diskio.h"

#define ZEROFS_SIGN_TOKEN (0x69696969)
#define ZEROFS_INFO_SECTOR_INDEX (1)
#define ZEROFS_INFO_SECTOR_LENGTH (9)
#define ZEROFS_DATA_SECTOR_START (2)
#define ZEROFS_DATA_SECTOR_END (2097152)
#define ZEROFS_DATA_INDEX_START (0x00000000)
#define ZEROFS_DATA_INDEX_NOT_READY (0xffffffff)

typedef enum {
	RES_DISK_NOINIT = 4,
	RES_TOKEN_MISMATCH,
	RES_SECTOR_OUT_OF_RANGE,
	RES_OFFSET_OUT_OF_RANGE,
} status_t;

static uint32_t _index;

static uint8_t _update(uint32_t index);

static uint8_t _update(uint32_t index)
{
	uint8_t res;
	uint64_t header;

	header = (uint64_t) index << 32 | ZEROFS_SIGN_TOKEN;

	res = disk_writep(0, ZEROFS_INFO_SECTOR_INDEX);
	if (res)
		return res;

	res = disk_writep((uint8_t *) (&header), 8);
	if (res)
		return res;

	res = disk_writep(0, 0);

	return res;
}

uint8_t zerofs_is_busy(void)
{
	return (uint8_t) (_index == ZEROFS_DATA_INDEX_NOT_READY);
}

uint8_t zerofs_init(void)
{
	uint8_t res;

	_index = ZEROFS_DATA_INDEX_NOT_READY;

	res = disk_initialize();
	if (res)
		return RES_DISK_NOINIT;

	return 0;
}

uint8_t zerofs_mount(void)
{
	uint8_t res;
	uint64_t header;
	uint32_t sector;
	uint32_t token;
	uint16_t offset;

	_index = ZEROFS_DATA_INDEX_NOT_READY;

	// read zerofs header.
	res = disk_readp((uint8_t *) (&header), ZEROFS_INFO_SECTOR_INDEX, 0, 8);
	if (res)
		return res;

	token = (uint32_t) header;
	if (token != ZEROFS_SIGN_TOKEN) {
		// token dose not match.
		return RES_TOKEN_MISMATCH;
	}

	_index = (uint32_t) (header >> 32);
	sector = (_index / 512) + ZEROFS_DATA_SECTOR_START;
	offset = _index % 512;

	if (sector > ZEROFS_DATA_SECTOR_END
	    || sector < ZEROFS_DATA_SECTOR_START)
		return RES_SECTOR_OUT_OF_RANGE;

	if (offset > 512)
		return RES_OFFSET_OUT_OF_RANGE;

	return 0;
}

uint8_t zerofs_create(void)
{
	_index = ZEROFS_DATA_INDEX_NOT_READY;

	return _update(ZEROFS_DATA_INDEX_START);
}

uint8_t zerofs_read(char *data, uint32_t index, uint8_t count)
{
	uint8_t res;
	uint32_t sector;
	uint16_t offset;
	uint16_t i;

	while (count) {
		sector = (index / 512) + ZEROFS_DATA_SECTOR_START;
		offset = index % 512;

		if (sector > ZEROFS_DATA_SECTOR_END
		    || sector < ZEROFS_DATA_SECTOR_START)
			return RES_SECTOR_OUT_OF_RANGE;

		i = (count > 512 - offset) ? 512 - offset : count;

		res = disk_readp((uint8_t *) data, sector, offset, i);
		if (res)
			return res;

		index += i;
		count -= i;
		data += i;
	}

	return 0;
}

uint8_t zerofs_write(char *data, uint32_t index, uint8_t count)
{
	uint8_t res;
	char buffer[512];
	uint32_t sector;
	uint16_t offset;

	while (count) {
		sector = (index / 512) + ZEROFS_DATA_SECTOR_START;

		if (sector > ZEROFS_DATA_SECTOR_END
		    || sector < ZEROFS_DATA_SECTOR_START)
			return RES_SECTOR_OUT_OF_RANGE;

		res = disk_readp((uint8_t *) buffer, sector, 0, 512);
		if (res)
			return res;

		offset = index % 512;
		while (count && offset < 512) {
			buffer[offset++] = *data++;
			index++;
			count--;
		}

		res = disk_writep(0, sector);
		if (res)
			return res;

		res = disk_writep((uint8_t *) buffer, 512);
		if (res)
			return res;

		res = disk_writep(0, 0);
		if (res)
			return res;
	}

	return 0;
}

uint8_t zerofs_append(char *data, uint8_t count)
{
	uint8_t res;
	uint32_t sector;

	res = zerofs_write(data, _index, count);
	if (res)
		return res;

	_index += count;

	sector = (_index / 512) + ZEROFS_DATA_SECTOR_START;

	if (sector > ZEROFS_DATA_SECTOR_END
	    || sector < ZEROFS_DATA_SECTOR_START)
		_index = ZEROFS_DATA_INDEX_START;

	return _update(_index);
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
}
