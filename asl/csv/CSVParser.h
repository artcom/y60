/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: CSV Parser using on Boost::Spirit
// see BNF from http://www.boyet.com/articles/csvparser.html
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ASL_CSV_PARSER_INCLUDED_
#define _ASL_CSV_PARSER_INCLUDED_

#include "asl_csv_settings.h"

#include <string>
#include <vector>

/*! \addtogroup aslcsv */
/* @{ */

namespace asl {

ASL_CSV_DECL void
parseCSV(std::string const& input, std::vector<std::vector<std::string> > & result);

}

/* @} */

#endif
