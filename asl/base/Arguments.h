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
// Description: command line argument parser
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : perfect
//    formatting             : ok
//    documentation          : poor
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendation: 
//       - improve documentation
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef included_asl_Arguments_h
#define included_asl_Arguments_h

#include "asl_base_settings.h"

#include "Exception.h"
#include "string_functions.h"

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace asl {

/*! \addtogroup aslbase */
/* @{ */

class ASL_BASE_EXPORT Arguments {
public:
    enum { MAX_OPTIONS = 100 };

    struct AllowedOption {
        const char * theName;
        const char * theArgumentName;
    };
    struct AllowedOptionWithDocumentation {
        const char * theName;
        const char * theArgumentName;
        const char * theDocumentation;
	};

    enum {
        OPTION_START_CHAR = '-',
        IgnoreErrors = 0,
        BailOnUnknownOptions = 1,
    };

    Arguments(const AllowedOption * allowedOptions = 0);

    void addAllowedOptions(const AllowedOption * allowedOptions);
    void addAllowedOptionsWithDocumentation(const AllowedOptionWithDocumentation * allowedOptions);
    const std::string & getOptionArgument(const std::string & option) const;

    DEFINE_EXCEPTION(IllegalOptionQuery,asl::Exception)

    bool haveOption(const std::string & option) const;
    bool parse(std::vector<std::string> myArgs, StringEncoding theEncoding=UTF8, int errorHandlingPolicy=BailOnUnknownOptions); 
    bool parse(int argc, const char * const argv[], StringEncoding theEncoding=Locale, int errorHandlingPolicy=BailOnUnknownOptions);
    void printUsage() const;
    void printHelp() const;
    void printRevision() const;
    void printVersion() const;
    void printCopyright() const;
    void setShortDescription(const std::string & theDescription);
    void setLongDescription(const std::string & theDescription);

    int getCount() const {
        return _justArguments.size();
    }
    int getOptionCount() const {
        return _options.size();
    }
    const std::string & getArgument(int i) const {
        return _allArguments[_justArguments[i]];
    }
    const std::string & getProgramName() const {
        return _programName;
    }

    std::vector<std::string> getOptionNames() const;

private:
    void addUsageString(std::ostringstream & os) const;

	// storage of the parsed arguments
    std::map<std::string,int> _options;
    std::vector<std::string> _allArguments;
    std::string _programName;
	std::vector<int> _justArguments;
		
	// storage of the argument specification
    std::map<std::string,std::string> _allowedOptions;
    std::map<std::string,std::string> _optionDescriptions;
	
	std::vector<std::string> _argumentNames;
    std::vector<std::string> _argumentDescriptions;

    std::string _myGeneralShortDescription;
    std::string _myGeneralLongDescription;
};

/* @} */

} // namespace asl

#endif // included_asl_Arguments_h
