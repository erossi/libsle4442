/* This file is part of libsle4442.
 * Copyright (C) 2010, 2013 Enrico Rossi
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
  \file sle_bbg.c
  \brief Bit banging driver to SLE4442.

  Low level bit banging driver to read and write to
  Sle4442 card.
  */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "sle_bbg.h"

/*!
 * Single clock pulse.
 *
 * This function generates a single clock cycle onto the
 * clock line.
 */
void ck_pulse(void)
{
	set_ck_1;
	ck_delay();
	set_ck_0;
	ck_delay();
}

/*!
 * Set's I/O line to one of: IN, OUT, 0 or 1.
 *
 * IN: The port is in input, the subsequent call with:
 *  0 - Disable internal pull-up resistor.
 *  1 - Enable internal pull-up resistor.
 *
 * OUT: The port in in output mode, the subsequent call with:
 *  0 - Line is logic 0 (GND).
 *  1 - Line is logic 1 (+5V).
 *
 * \param io  can be 0, 1, IN, OUT
 */
void set_io(const uint8_t io)
{
	switch (io) {
		case 0:
			SLE_PORT &= ~_BV(SLE_IO);
			break;
		case 1:
			SLE_PORT |= _BV(SLE_IO);
			break;
		case OUT:
			SLE_DDR |= _BV(SLE_IO);
			break;
		default:
			SLE_PORT |= _BV(SLE_IO);
			SLE_DDR &= ~_BV(SLE_IO);
	}
}

/*!
 * Send the START sequence.
 *
 * \warning IO line will be set to OUT.
 */
void send_start(void)
{
	set_ck_0; /* redundancy */
	set_io(OUT);
	set_io(1);
	set_ck_1;
	ck_delay();
	set_io(0);
	ck_delay_front();
	set_ck_0;
	ck_delay();
}

/*!
 * Send the STOP sequence.
 *
 * \warning IO line will be set to IN.
 */
void send_stop(void)
{
	set_io(0);
	ck_delay_front();
	set_ck_1;
	ck_delay_front();
	set_io(IN); /* with pull up IO goes 1 */
	ck_delay();
	set_ck_0;
	ck_delay();
}

/*!
 * Read a byte from IO line.
 *
 * The function geneates 8 clock pulse cycle and on each of them
 * a bit is read from the IO line.
 *
 * \return The byte read.
 * \note The IO line have to be already set to INPUT.
 */
uint8_t read_byte(void)
{
	uint8_t i;
	uint8_t byte = 0;

	for (i=0; i<8; i++) {
		set_ck_1;

		/* if we found a logic 1 on the IO line
		 * then set this bit in the byte.
		 */
		if (SLE_PIN & _BV(SLE_IO))
			byte |= _BV(i);

		ck_delay();
		set_ck_0;
		ck_delay();
	}

	return(byte);
}

/*!
 * Write a byte to the IO line.
 *
 * The function write a byte a singe bit at a time,
 * the bit is written on the 0 phase of the ck line, see datasheet.
 *
 * \param byte the byte to be sent.
 * \note the IO line should be already set to output.
 */
void send_byte(uint8_t byte)
{
	uint8_t i;

	for (i=0; i<8; i++) {
		if (byte & _BV(i))
			set_io(1);
		else
			set_io(0);

		ck_delay_front();
		set_ck_1;
		ck_delay();
		set_ck_0;
		ck_delay();
	}
}

/*!
 * Send the reset (rst) sequence to the card.
 *
 * See datasheet for details on the RST sequence.
 *
 * \param *atr ptr to mem area where this function stores
 * the 4 byte ATR returned after the RST.
 *
 * \warning The 4 byte ATR have to be already allocated.
 */
void send_rst(uint8_t *atr)
{
	uint8_t i;

	set_io(IN);
	set_rst_1;
	ck_delay_front();
	set_ck_1;
	ck_delay_reset();
	set_ck_0;
	ck_delay_front();
	set_rst_0;
	ck_delay();

	for (i=0; i<4; i++) {
		/* read bit in */
		*(atr+i) = read_byte();
	}
}

/*!
 * Send a complete command sequence.
 * 
 * \param control the control byte.
 * \param address the address byte.
 * \param data the data byte.
 * \warning After the command sequence, IO line is left in IN mode.
 */
void send_cmd(const uint8_t control, const uint8_t address, const uint8_t data)
{
	send_start();
	send_byte(control);
	send_byte(address);
	send_byte(data);
	send_stop();
}

/*!
 * Wait for the card to process the command.
 *
 * \return the number of clock cycle waited.
 * \warning if the card has problem and no bit
 * comes from the IO line, the system hangs here.
 */
uint8_t processing(void)
{
	uint8_t i = 0;

	while (!(SLE_PIN & _BV(SLE_IO))) {
		ck_pulse();
		i++;
	}

	ck_pulse();
	return (i);
}
