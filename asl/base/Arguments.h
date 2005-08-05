// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// See the .cpp for the full disclaimer
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef included_asl_Arguments_h
#define included_asl_Arguments_h

#include "Exception.h"

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

    Arguments();
    Arguments(const AllowedOption * allowedOptions);

    void addAllowedOptions(const AllowedOption * allowedOptions);
    const std::string & getOptionArgument(const std::string & option) const;

    DEFINE_EXCEPTION(IllegalOptionQuery,asl::Exception)

    bool haveOption(const std::string & option) const;
    bool parse(std::vector<std::string> myArgs, int errorHandlingPolicy=BailOnUnknownOptions); 
    bool parse(int argc, const char * const argv[], int errorHandlingPolicy=BailOnUnknownOptions);
    void printUsage() const;
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


