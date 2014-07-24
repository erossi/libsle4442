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

/*!
  \file sle.h
  \brief This is the API avariable to access to SLE4442.

  Details.
 */

#ifndef SLE_H_
#define SLE_H_

#ifdef MAKE_LIB

/* Include the low level interface only if the library
 * needs to be compiled.
 */
#include "sle_bbg.h"

/*!
  \def SLE_CMD_DUMP_MEMORY
  \brief SLE4442 Hex command to access the main memory.

  \def SLE_CMD_DUMP_SECMEM
  \brief SLE4442 Hex command to access the security memory.

  \def SLE_CMD_DUMP_PRT_MEMORY
  \brief SLE4442 Hex command to access the protected memory.
 */

#define SLE_CMD_DUMP_MEMORY 0x30
#define SLE_CMD_DUMP_SECMEM 0x31
#define SLE_CMD_DUMP_PRT_MEMORY 0x34
#define SLE_CMD_COMPARE_VERIFICATION_DATA 0x33
#define SLE_CMD_UPDATE_SECMEM 0x39
#define SLE_CMD_UPDATE_MEMORY 0x38

#endif /* Make lib */

/* Uncomment to enable internal pullup resistor,
   it will drain a lot of power in sleep modes */
#define SLE_MICRO_PULLUP

/*!
  \struct sle_t sle.h "sle.h"
  \brief The main struct which represent the status of the card.

  Keep in mind AtMega chips have only 1 KB RAM, dumping all the
  card memory into RAM may cause an out of memory.

  \var uint8_t *sle_t::atr
  \brief ptr to a 4 bytes ATR header returned by the ATZ command.

  \var uint8_t *sle_t::main_memory
  \brief ptr to a 256 bytes copy of the SLE 4442 memory.

  \var uint8_t *sle_t::protected_memory
  \brief ptr to a 32 bytes copy of the SLE 4442 memory.

  \var uint8_t* sle_t::security_memory
  \brief 4 bytes

  \var uint8_t* sle_t::ck_proc
  \brief processing clock counts

  \var uint8_t sle_t::card_present
  \brief card present 0=no; 1=yes

  \var uint8_t sle_t::auth
  \brief card auth 1 - ok, 0 - writing forbidden

  auth ok, I can write

 */

struct sle_t {
	uint8_t *atr;
	uint8_t *main_memory;
	uint8_t *protected_memory;
	uint8_t *security_memory;
	uint8_t *ck_proc;
	uint8_t card_present;
	uint8_t auth;
};

void sle_enable_port(void);
void sle_disable_port(void);
struct sle_t* sle_init(void);
void sle_free(struct sle_t *sle);
void sle_reset(uint8_t *atr);
uint8_t sle_present(struct sle_t *sle);
void sle_dump_memory(uint8_t *mm);
void sle_dump_prt_memory(uint8_t *mm);
void sle_dump_secmem(uint8_t *mm);
void sle_dump_allmem(struct sle_t *sle);
void sle_auth(struct sle_t *sle, const uint8_t pin1, const uint8_t pin2, const uint8_t pin3);
void sle_write_memory(struct sle_t *sle, const uint8_t addr, const uint8_t len);
void sle_write_secmem(struct sle_t *sle);

#endif
