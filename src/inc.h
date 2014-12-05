/** Includes
 * \file inc.h
 * \author 69DreamTeam
 * \brief All includes goes here!
 */

/** Standart includes		*/
#include <stdlib.h>			//!< malloc, etc.
#include <stdio.h>			//!< Standartna IO kniznica
#include <ctype.h>     			//!< PRIDANA KNIZNICA (funkcia isdigit)
#include <time.h>			//!< Praca s casom pouzite v kniznici 'error.h'
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

/** Our includes 		*/
#include "error.h"			    //!< Error handling
#include "lex_lexem.h"			//!< Praca s lexemami
#include "ial.h"		//!< Binarny strom
#include "syn_parser.h"
#include "int_interpret.h"
#include "init.h"               //!< Inicializacne funkcie
#include "sem.h"
