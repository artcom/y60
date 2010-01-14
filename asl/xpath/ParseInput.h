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
** \file xpath/ParseInput.h
**
** \brief XPath paths parser
**
** \author Hendrik Schober
**
** \date 2008-07-11
**
*/

/**********************************************************************************************/

#if !defined(XPATH_PARSEINPUT_H)
#define XPATH_PARSEINPUT_H

#include "asl_xpath_settings.h"

/**********************************************************************************************/
#include <string>

/**********************************************************************************************/
#include <asl/base/string_functions.h>

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        class ParseInput {
        public:
            friend class ParseState;

            typedef std::string::size_type              index_type;
            typedef std::string::const_iterator         const_iterator;

            ParseInput( const std::string& str
                      , index_type         idx = 0 )  : str_(str), idx_(idx), idxError_(idx) {}

            std::string getString() const               {return str_;}

            std::string getRemainingString() const      {return std::string(begin()+getCurrentIndex(),end());}

            std::string getErrorString() const          {return std::string(begin()+getErrorIndex(),end());}

            bool eof() const                            {return idx_==str_.size();}

            bool eof(bool skipWS)                       {skipWhitespace_(skipWS);return eof();}

            index_type getCurrentIndex() const          {return idx_;}

            index_type getErrorIndex() const            {return idxError_;}

            const_iterator begin() const                {return str_.begin()+idx_;}
            const_iterator end  () const                {return str_.end();}

            void setErrorIndex()                        {if( idx_ > idxError_ ) idxError_=idx_;}

            void resetErrorIndex()                      {idxError_ = 0;}

            void forwardTo(index_type idx)              {idx_=idx;}

            // note: const_cast in order to circumvent GCC bug where the string changes when calling begin()
            void forwardTo(const_iterator it)           {idx_ = (it-const_cast<const std::string&>(str_).begin());}

            char getNextChar(bool skipWs = true)        {if(eof(true)) return '\0'; else return str_[idx_++];}

            char peekNextChar() const                   {if(eof()) return '\0'; else return str_[idx_];}

            char peekNextChar(bool skipWs)              {if(eof(true)) return '\0'; else return str_[idx_];}

            bool readIf(const std::string& if_str, bool skipWs = true);

            bool readIf(char if_ch, bool skipWs = true);

            template< typename StartFunc, typename Func >
            std::string readObject(StartFunc startFunc, Func func, bool skipWs = true);

            std::string readIdentifier(bool skipWs = true);

            std::string readQuotedText(char opening, char closing, bool skipWs = true);

            void skipWhitespace()                       {idx_ = asl::read_whitespace(str_,idx_);}

        private:
            ParseInput(const ParseInput&);
            ParseInput& operator=(const ParseInput&);

            std::string             str_;
            index_type              idx_;
            index_type              idxError_;

            void skipWhitespace_(bool skipWs)           {if(skipWs)skipWhitespace();}

            static bool is_identifier_start_char(char c){return asl::is_alpha(c) || asl::is_underscore(c);}
            static bool is_identifier_char(char c)      {return is_identifier_start_char(c) || asl::is_digit(c) || asl::is_hyphen(c);}
        };

        inline bool ParseInput::readIf(const std::string& if_str, bool skipWs)
        {
            skipWhitespace_(skipWs);
            const index_type idx_old = idx_;
            idx_ = asl::read_if_string(str_,idx_,if_str);
            return idx_old < idx_;
        }

        inline bool ParseInput::readIf(char if_ch, bool skipWs)
        {
            skipWhitespace_(skipWs);
            const index_type idx_old = idx_;
            idx_ = asl::read_if_char(str_,idx_,if_ch);
            return idx_old < idx_;
        }

        template< typename StartFunc, typename Func >
        inline std::string ParseInput::readObject( StartFunc startFunc, Func func, bool skipWs )
        {
            skipWhitespace_(skipWs);
            const index_type idx_old = idx_;
            idx_ = asl::read_object(str_,idx_,startFunc,func);
            return str_.substr(idx_old,idx_-idx_old);
        }

        inline std::string ParseInput::readIdentifier(bool skipWs)
        {
            return readObject(is_identifier_start_char,is_identifier_char,skipWs);
        }

        inline std::string ParseInput::readQuotedText(char opening, char closing, bool skipWs)
        {
            skipWhitespace_(skipWs);
            const index_type idx_old = idx_;
            idx_ = asl::read_quoted_text(str_,idx_,opening,closing);
            return str_.substr(idx_old+1,idx_-idx_old-2);
        }

        class ParseState {
        public:
            ParseState(ParseInput& pi)                : pi_(pi), idxOld_(pi.getCurrentIndex())
                                                      , committed_(false) {}

            ~ParseState()                               {doRollback();}

            ParseInput::index_type getOldIndex() const  {return idxOld_;}

            std::string getReadString() const           {return pi_.getString().substr(idxOld_,pi_.getCurrentIndex()-idxOld_);}

            void commit()                               {committed_=true; pi_.resetErrorIndex();}

            void rollback()                             {doRollback(); committed_=true;;}

            bool hasRead() const                        {return idxOld_ < pi_.getCurrentIndex();}

        private:
            ParseState(const ParseState&);
            ParseState& operator=(const ParseState&);

            ParseInput&             pi_;
            ParseInput::index_type  idxOld_;
            bool                    committed_;

            void doRollback()
            {
                if(!committed_) {
                    if( !pi_.getErrorIndex() ) {
                        pi_.setErrorIndex();
                    }
                    pi_.idx_=idxOld_;
                }
            }
        };

    }
}

/**********************************************************************************************/

#endif //!defined(XPATH_PARSEINPUT_H)

