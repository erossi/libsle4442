/* This file is part of libsle4442.
 * Copyright (C) 2010, 2011, 2013 Enrico Rossi
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

/*!
 * \file sle.c
 * \brief API interface.
 *
 * Low level bit banging driver to read and write to
 * Sle4442 card.
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "sle.h"

/*! Initialize the port.
 *
 * Configure the I/O port's pin as:
 * Card present pin: IN
 * Reset: OUT
 * Clock: OUT
 * IO line: IN
 *
 * \note If an internal PULL-UP resistors is required
 * for the IN pin, the define SLE_MICRO_PULLUP macro.
 * For example if we have connected the card's pin
 * directly to the micro without external resistors.
 */
void sle_enable_port(void)
{
#ifdef SLE_MICRO_PULLUP
	SLE_PORT |= _BV(SLE_PRESENT) | _BV(SLE_IO);
#else
	SLE_PORT &= ~(_BV(SLE_PRESENT) | _BV(SLE_IO));
#endif

	/* Port setup */
	SLE_DDR |= _BV(SLE_RST) | _BV(SLE_CK);
}

/*!
 * Disable the port and restore all the used pins to IN.
 *
 * \note also restore internals pull-up resistors to OFF.
 */
void sle_disable_port(void)
{
	SLE_DDR &= ~(_BV(SLE_RST) | _BV(SLE_CK));
	SLE_PORT &= ~(_BV(SLE_PRESENT) | _BV(SLE_IO));
}

/*!
 * Initialize and allocate the structure and enable
 * the port connected to the card's reader.
 *
 * \return sle The allocated structure.
 */
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

/*!
 * Free the memory allocated to the structure and disable the
 * port connected to the card's reader.
 *
 * \param sle the allocated structure.
 * \note In theory this function should never be called, unless
 * the software have no need for the card reader.
 */
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

/*!
 * \brief reset the card.
 *
 * \param atr pointer to the string sent to reset the card.
 */
void sle_reset(uint8_t *atr)
{
	send_rst(atr);
}

/*!
 * Check for the presence of the card.
 *
 * The status of the card will be stored into the struct.
 *
 * \param sle the struct need to store the card's status.
 * \return Card present or not.
 */
uint8_t sle_present(struct sle_t *sle)
{

	if (SLE_PIN & _BV(SLE_PRESENT))
		sle->card_present=0;
	else
		sle->card_present=1;

	return(sle->card_present);
}

/*!
 * Dump the card's memory into the mm area.
 *
 * \param mm the storage area where to dump the memory card.
 * \warning the dump memory should be already allocated and
 * have enought space for the card (256 Byte).
 */
void sle_dump_memory(uint8_t *mm)
{
	int i;

	send_cmd(SLE_CMD_DUMP_MEMORY, 0, 0);

	for (i=0; i<256; i++)
		*(mm+i) = read_byte();

	ck_pulse(); /* leave the card to high imp. I/O line */
}

/*!
 * Dump the protected memory.
 *
 * \param mm allocated 4 Byte space for the dump.
 * \warning the space have to be already allocated.
 */
void sle_dump_prt_memory(uint8_t *mm)
{
	uint8_t i;

	send_cmd(SLE_CMD_DUMP_PRT_MEMORY, 0, 0);

	for (i=0; i<4; i++)
		*(mm+i) = read_byte();

	ck_pulse(); /* leave the card to high imp. I/O line */
}

/*!
 * Dump the secure memory.
 *
 * \param mm allocated 4 Byte space for the dump.
 * \warning the space have to be already allocated.
 */
void sle_dump_secmem(uint8_t *mm)
{
	uint8_t i;

	send_cmd(SLE_CMD_DUMP_SECMEM, 0, 0);

	for (i=0; i<4; i++)
		*(mm+i) = read_byte();

	ck_pulse(); /* leave the card to high imp. I/O line */
}

/*!
 * Dump the all memories, normal, secure and protected memory.
 *
 * \param sle struct with allocated space for the dump.
 * \warning the space have to be already allocated.
 */
void sle_dump_allmem(struct sle_t *sle)
{
	sle_dump_memory(sle->main_memory);
	sle_dump_prt_memory(sle->protected_memory);
	sle_dump_secmem(sle->security_memory);
}

/*!
 * Perform the authentication.
 *
 * See the datasheet on how to authenticate.
 *
 * \param sle the struct allocated for the card.
 * \param pin1 byte 1 of the pin number.
 * \param pin2 byte 2 of the pin number.
 * \param pin3 byte 3 of the pin number.
 */
void sle_auth(struct sle_t *sle, const uint8_t pin1, const uint8_t pin2,
		const uint8_t pin3)
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

/*!
 * Write a group of byte into the memory card.
 *
 * Write one byte at a time from the allocated memory in
 * the sle struct starting from base for len byte.
 * 
 * \param sle the allocated struct.
 * \param base the base memory to start copying from.
 * \param len how many byte to copy.
 */
void sle_write_memory(struct sle_t *sle, const uint8_t base,
		const uint8_t len)
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

/*!
 * Write the secure memory.
 *
 * Copy the 4 byte secure memory from the allocated struct to the card.
 *
 * \param sle the allocated struct.
 *
 */
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
