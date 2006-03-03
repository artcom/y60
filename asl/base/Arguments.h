/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

#include "Exception.h"
#include "string_functions.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace asl {

/*! \addtogroup aslbase */
/* @{ */

class Arguments {
public:
    enum { MAX_OPTIONS = 100 };

    struct AllowedOption {
        const char * theName;
        const char * theArgumentName;
    };

    enum {
        OPTION_START_CHAR = '-',
        IgnoreErrors = 0,
        BailOnUnknownOptions = 1,
    };

    Arguments(const AllowedOption * allowedOptions = 0);

    void addAllowedOptions(const AllowedOption * allowedOptions);
    const std::string & getOptionArgument(const std::string & option) const;

    DEFINE_EXCEPTION(IllegalOptionQuery,asl::Exception)

    bool haveOption(const std::string & option) const;
    bool parse(std::vector<std::string> myArgs, StringEncoding theEncoding=UTF8, int errorHandlingPolicy=BailOnUnknownOptions); 
    bool parse(int argc, const char * const argv[], StringEncoding theEncoding=Locale, int errorHandlingPolicy=BailOnUnknownOptions);
    void printUsage() const;
    void printRevision() const;
    void printVersion() const;
    void printCopyright() const;
    void setArgumentDescription(const char * theArgumentDescription);

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
    std::map<std::string,int> _options;
    std::vector<std::string> _allArguments;
    std::map<std::string,std::string> _allowedOptions;
    std::vector<std::string> _argumentNames;
    std::vector<int> _justArguments;
    std::string _programName;
    std::string _myArgumentDescription;
};

/* @} */

} // namespace asl

#endif // included_asl_Arguments_h

//=============================================================================
//
// $ Log: $
//
//=============================================================================


