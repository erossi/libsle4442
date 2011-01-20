/* This file is part of libsle4442.
 * Copyright (C) 2010, 2011 Enrico Rossi
 *
 * Libsle4442 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Libsle4442 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "sle.h"

void sle_enable_port(void) {
	/*
	   Inital PORT setup:
	   Card present 1
	   Reset 0
	   Clock 0
	   IO 1
	 */

#ifdef SLE_MICRO_PULLUP
	SLE_PORT |= _BV(SLE_PRESENT) | _BV(SLE_IO);
#else
	SLE_PORT &= ~(_BV(SLE_PRESENT) | _BV(SLE_IO));
#endif

	/*
	   Initial DDR setup:
	   Card Present - IN
	   Reset - OUT
	   Clock - OUT
	   IO - BiDirectional begin IN
	 */

	SLE_DDR |= _BV(SLE_RST) | _BV(SLE_CK);
}

void sle_disable_port(void) {
	SLE_DDR &= ~(_BV(SLE_RST) | _BV(SLE_CK));
	SLE_PORT &= ~(_BV(SLE_PRESENT) | _BV(SLE_IO));
}

struct sle_t* sle_init(void)
{
	struct sle_t *sle;

	sle = malloc(sizeof(struct sle_t));
	sle->atr = malloc(4);
	sle->main_memory = malloc(256);
	sle->protected_memory = malloc(4);
	sle->security_memory = malloc(4);
	/* Normally we should use only 1, but during the auth
	   it's wise to keep all the 5 processing result */
	sle->ck_proc = malloc(5);
	sle_enable_port();
	return(sle);
}

void sle_free(struct sle_t *sle)
{
	sle_disable_port();
	free(sle->ck_proc);
	free(sle->security_memory);
	free(sle->protected_memory);
	free(sle->main_memory);
	free(sle->atr);
	free(sle);
}

void sle_reset(uint8_t *atr)
{
	send_rst(atr);
}

/*! \brief Check for the presence of the card. */
uint8_t sle_present(struct sle_t *sle)
{

	if (SLE_PIN & _BV(SLE_PRESENT))
		sle->card_present=1;
	else
		sle->card_present=0;

	return(sle->card_present);
}

void sle_dump_memory(uint8_t *mm)
{
	int i;

	send_cmd(SLE_CMD_DUMP_MEMORY, 0, 0);

	for (i=0; i<256; i++)
		*(mm+i) = read_byte();

	ck_pulse(); /* leave the card to high imp. I/O line */
}

void sle_dump_prt_memory(uint8_t *mm)
{
	uint8_t i;

	send_cmd(SLE_CMD_DUMP_PRT_MEMORY, 0, 0);

	for (i=0; i<4; i++)
		*(mm+i) = read_byte();

	ck_pulse(); /* leave the card to high imp. I/O line */
}

void sle_dump_secmem(uint8_t *mm)
{
	uint8_t i;

	send_cmd(SLE_CMD_DUMP_SECMEM, 0, 0);

	for (i=0; i<4; i++)
		*(mm+i) = read_byte();

	ck_pulse(); /* leave the card to high imp. I/O line */
}

void sle_dump_allmem(struct sle_t *sle)
{
	sle_dump_memory(sle->main_memory);
	sle_dump_prt_memory(sle->protected_memory);
	sle_dump_secmem(sle->security_memory);
}

void sle_auth(struct sle_t *sle, const uint8_t pin1, const uint8_t pin2, const uint8_t pin3)
{
	/* dump secmem */
	sle_dump_secmem(sle->security_memory);

	/* Check error = 7, all available error bit = 1 */
	if (*(sle->security_memory) == 7) {

		/* write 0 to bit 3 */
		send_cmd(SLE_CMD_UPDATE_SECMEM, 0, 3);
		*(sle->ck_proc) = processing();

		/* Compare 3 byte PIN */
		send_cmd(SLE_CMD_COMPARE_VERIFICATION_DATA, 1, pin1);
		*(sle->ck_proc + 1) = processing();
		send_cmd(SLE_CMD_COMPARE_VERIFICATION_DATA, 2, pin2);
		*(sle->ck_proc + 2) = processing();
		send_cmd(SLE_CMD_COMPARE_VERIFICATION_DATA, 3, pin3);
		*(sle->ck_proc + 3) = processing();

		/* write 0xff to error */
		send_cmd(SLE_CMD_UPDATE_SECMEM, 0, 0xff);
		*(sle->ck_proc + 4) = processing();

		/* redump secmem */
		sle_dump_secmem(sle->security_memory);
	}

	/* if error = 7 then auth is OK */
	if (*(sle->security_memory) == 7)
		sle->auth = 1;
}

void sle_write_memory(struct sle_t *sle, const uint8_t base, const uint8_t len)
{
	uint8_t i, addr;

	if (sle->auth) {
		for (i=0; i<len; i++) {
			addr = base + i;
			send_cmd(SLE_CMD_UPDATE_MEMORY, addr, *(sle->main_memory + addr));
			*(sle->ck_proc) = processing();
		}
	}
}

void sle_write_secmem(struct sle_t *sle)
{
	uint8_t i;

	if (sle->auth) {
		for (i=0; i<4; i++) {
			send_cmd(SLE_CMD_UPDATE_SECMEM, i, *(sle->security_memory + i));
			*(sle->ck_proc + i) = processing();
		}
	}
}

