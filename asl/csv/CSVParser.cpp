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
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "CSVParser.h"

#include <iostream>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#   pragma warning (push,2)
#endif //defined(_MSC_VER)

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/version.hpp>
#if SPIRIT_VERSION < 0x2000           
    #error boost::spirit 2.0 or later required (boost 1.37+)
#endif
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#if defined(_MSC_VER)
#   pragma warning (pop)
#   pragma warning (disable:4503) // decorated name length exceeded
#   pragma warning (disable:4706) // assignment within conditional expression
#endif //defined(_MSC_VER)

using namespace boost::phoenix;
using namespace boost::spirit;
using namespace boost::spirit::qi;

#if SPIRIT_VERSION == 0x2000                // namespaces for spirit 2.0 (boost 1.39)
    using namespace boost::spirit::arg_names;
#endif

template <typename Iterator>
struct CSVParser : grammar<Iterator, std::vector<std::vector<std::string> >()> {
    CSVParser() : CSVParser::base_type(csvFile) {

        subField %= *(char_ - '"');
        optionalSpaces = *(lit(' ')|lit('\t'));

        simpleField %= +(char_ - ';' - eol - '\t' - ' ' - '"');
        quotedField %= '"' > escapedField > '"';

        rawField %= simpleField | quotedField;
		rawString = optionalSpaces >> -(rawField[_val=_1] >> optionalSpaces);

        escapedField = subField[_val+=_1] > *(lit("\"\"")[_val+='"'] > subField[_val+=_1]);

        csvStringList =  rawString[push_back(_val,_1)] > *(';' > rawString[push_back(_val,_1)]);
        csvRecord = csvStringList >> eol;
        csvFile = *(csvRecord[push_back(_val,_1)]);


        //error handling
        subField.name("subField");
        escapedField.name("escapedField");
        simpleField.name("simpleField");
        quotedField.name("quotedField");
        optionalSpaces.name("optionalSpaces");
        rawField.name("rawField");
        rawString.name("rawString");
        csvRecord.name("csvRecord");
        csvFile.name("csvFile");

        on_error<fail> (
            csvFile, std::cout
                << val("Error! Expecting ")
                << _4                               // what failed?
                << val(" here: \"")
                << construct<std::string>(_3, _2)   // iterators to error-pos, end
                << val("\"")
                << std::endl
        );
    }

    rule<Iterator, std::string()> subField;
    rule<Iterator, std::string()> escapedField;
    rule<Iterator, std::string()> simpleField;
    rule<Iterator, std::string()> quotedField;
    rule<Iterator, std::string()> optionalSpaces;
    rule<Iterator, std::string()> rawField;
    rule<Iterator, std::string()> rawString;
    rule<Iterator, std::vector<std::string>()> csvStringList;
    rule<Iterator, std::vector<std::string>()> csvRecord;
    rule<Iterator, std::vector<std::vector<std::string> >()> csvFile;
};

namespace asl {

void
parseCSV(std::string const& input, std::vector<std::vector<std::string> > & result) {
    typedef std::string::const_iterator iterator_type;
    typedef CSVParser<iterator_type> CSVParser;
    CSVParser ourCSVParser; 
    std::string::const_iterator s = input.begin();
    std::string::const_iterator e = input.end();
    bool r = parse(s, e, ourCSVParser, result);
    if (! r || s != e) {
        throw "KAPUTT";
    }
}

} // end of namespace asl
