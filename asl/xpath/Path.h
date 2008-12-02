/**********************************************************************************************/

/*
**
** Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
**
** These coded instructions, statements, and computer programs contain
** unpublished proprietary information of ART+COM AG Berlin, and
** are copy protected by law. They may not be disclosed to third parties
** or copied or duplicated in any form, in whole or in part, without the
** specific, prior written permission of ART+COM AG Berlin.
**
*/

/**********************************************************************************************/

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

