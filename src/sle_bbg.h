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
  \file sle_bbg.h
  \brief Bit banging driver to SLE4442.

  Low level bit banging driver to read and write to
  Sle4442 card.
 */

#ifndef SLE_BBG_H
#define SLE_BBG_H

/*!
 * \def SLE_PORT
 * Port the card's reader is conneted to.
 *
 * \def SLE_PIN
 * The input port definition where the card is connected to.
 *
 * \def SLE_DDR
 * The data direction register to handle the port's line direction.
 *
 * \def SLE_PRESENT
 * PIN connected to the present card switch.
 *
 * \def SLE_RST
 * PIN connected to the reset pin of the card.
 *
 * \def SLE_CK
 * clock pin of the card.
 *
 * \def SLE_IO
 * I/O pin of the card.
 */

#define SLE_PORT PORTA
#define SLE_PIN PINA
#define SLE_DDR DDRA
#define SLE_PRESENT 3
#define SLE_RST 0
#define SLE_CK 1
#define SLE_IO 2

/* You should not change anything below */

/*!
 * \def IN
 * a simple macro to define IN as 3.
 *
 * \def OUT
 * a simple macro to define OUT as 2.
 */
#define OUT 2
#define IN 3

/*!
 * \def ck_delay()
 * The delay in usec for half of the wave.
 *
 * example:
 *
 * 10Khz = 100usec.
 *
 * 20Khz = 50usec, half wave = 25 usec.
 *
 * Remeber delay is only HALF of the freq.
 * 20Khz total
 *
 * \def ck_delay_front()
 * The MIN delay for a front phase between fronts of ck
 * and/or rst and/or IO.
 *
 * \def ck_delay_reset()
 * delay reset = 1.
 *
 * \def set_ck_1
 * CK line to 1.
 *
 * \def set_ck_0
 * CK line to 0.
 *
 * \def set_rst_1
 * RST line to 1.
 *
 * \def set_rst_0
 * RST line to 0.
 *
 */

#define ck_delay() _delay_us(25)
#define ck_delay_front() _delay_us(4)
#define ck_delay_reset() _delay_us(50)
#define set_ck_1 SLE_PORT |= (1<<SLE_CK)
#define set_ck_0 SLE_PORT &= ~(1<<SLE_CK)
#define set_rst_1 SLE_PORT |= (1<<SLE_RST)
#define set_rst_0 SLE_PORT &= ~(1<<SLE_RST)

void ck_pulse(void);
void set_io(const uint8_t io);
uint8_t read_byte(void);
void send_byte(uint8_t byte);
void send_rst(uint8_t *atr);
void send_cmd(const uint8_t control, const uint8_t address, const uint8_t data);
uint8_t processing(void);

#endif
