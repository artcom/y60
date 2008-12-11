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
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

/*!
** \file xpath/Path.h
**
** \brief XPath parsed paths
**
** \author Hendrik Schober
**
** \date 2008-07-11
**
*/

/**********************************************************************************************/

#if !defined(XPATH_PATH_H)
#define XPATH_PATH_H

/**********************************************************************************************/
#include <string>
#include <iosfwd>

/**********************************************************************************************/
#include <asl/base/Exception.h>

/**********************************************************************************************/

namespace xpath {

    DEFINE_EXCEPTION(XPathError    , asl::Exception);

    DEFINE_EXCEPTION(ParseError    , XPathError    );
    DEFINE_EXCEPTION(SyntaxError   , ParseError    );
    DEFINE_EXCEPTION(NotImplemented, ParseError    );
    
    /*! A parsed XPath path
    ** 
    ** Paths are immutable and cheap to copy. 
    */
    class Path {
    public:
        struct StepList;
    
        /*! Parse an XPath from a string
        **
        */
        Path(const std::string& str);

        /*! Parse an XPath from a string
        **
        */
        Path(const std::string& str, std::string::size_type& idx);

        //! private ctor; disregard
        Path(const StepList&);

        //! copy ctor
        Path(const Path& rhs);

        //! dtor
        ~Path();

        //! assignment operator
        Path& operator=(const Path& rhs);

        //! serialization
        std::ostream& streamTo(std::ostream& os) const;

        //! get string representation of path
        std::string toString() const;

        //! string comparison
        int compare(const Path& rhs) const;

        //! needed for xpath API; disregard
        const StepList& getStepList() const;

    private:
        class Impl_;
        Impl_* myImpl_;
    };

    inline bool operator==(const Path& lhs, const Path& rhs)
    {return 0 == lhs.compare(rhs);}

    inline bool operator!=(const Path& lhs, const Path& rhs)
    {return !operator==(lhs,rhs);}

    inline bool operator< (const Path& lhs, const Path& rhs)
    {return 0 > lhs.compare(rhs);}

    inline bool operator> (const Path& lhs, const Path& rhs)
    {return rhs < lhs;}

    inline bool operator<=(const Path& lhs, const Path& rhs)
    {return !operator>(lhs,rhs);}

    inline bool operator>=(const Path& lhs, const Path& rhs)
    {return !operator<(lhs,rhs);}

    inline 
    std::ostream& operator<<(std::ostream& os, const Path& path)
    {path.streamTo(os);return os;}

}

/**********************************************************************************************/

#endif //!defined(XPATH_PATH_H)

