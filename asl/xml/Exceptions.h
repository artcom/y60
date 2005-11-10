/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Exceptions.h,v $
//
//   $Revision: 1.7 $
//
// Description: Exception classes for xml
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_Exception_h_included_
#define _xml_Exception_h_included_

#include <asl/settings.h>
#include <asl/Exception.h>

#include <string>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    /** this exception is thrown during parsing when the parser
        aborts because of malformed xml data
    */
    class ParseException : public asl::Exception {
    public:
        ParseException(const std::string & what, const std::string & where, int parsed_until = 0)
            : asl::Exception(what,where,"dom::ParseException"),
              _myParseCompletionPos(parsed_until) {}
        /// returns the last character position parsed before abortion
        int parsedUntil() const {
            return _myParseCompletionPos;
        }
        void setParsedUntil(int thePos) {
            _myParseCompletionPos = thePos;
        }
	protected:
		ParseException(const std::string & what, const std::string & where,
				  const char * name,int parsed_until )
            : asl::Exception(what,where, name), _myParseCompletionPos(parsed_until) {}

    private:
        int _myParseCompletionPos;
    };

    #define DEFINE_NAMED_PARSE_EXCEPTION(CLASS,NAME,BASECLASS)\
    class CLASS : public BASECLASS {\
        public:\
            CLASS(const std::string & what, const std::string & where, int parsed_until = 0)\
                : BASECLASS(what,where, # NAME ,parsed_until) {}\
        protected:\
            CLASS(const std::string & what, const std::string & where, const char * name, int parsed_until)\
                : BASECLASS(what,where, name, parsed_until) {}\
    };

    #define DEFINE_PARSE_EXCEPTION(CLASS,BASECLASS) DEFINE_NAMED_PARSE_EXCEPTION(CLASS,CLASS,BASECLASS)
    #define DEFINE_NESTED_PARSE_EXCEPTION(INSIDECLASS,CLASS,BASECLASS) DEFINE_NAMED_PARSE_EXCEPTION(CLASS,INSIDECLASS::CLASS,BASECLASS)

    /** this exception is thrown when some illegal modification
        of Dom is attempted, e.g. using duplicate attribute values
        or adding children to nodes that are not allowed to have them
    */
    class DomException : public virtual ParseException {
    public:
        /** Error code the exception carries to indicate what has gone wrong
        */
        enum Code {
            INDEX_SIZE_ERR,
            WSTRING_SIZE_ERR,
            HIERARCHY_REQUEST_ERR,
            WRONG_DOCUMENT_ERR,
            INVALID_NAME_ERR,
            NO_DATA_ALLOWED_ERR,
            NO_MODIFICATION_ALLOWED_ERR,
            NOT_FOUND_ERR,
            NOT_SUPPORTED_ERR,
            INUSE_ATTRIBUTE_ERR
        };
        /// constructor
        DomException(const std::string & what, const std::string & where, Code error, int parsed_until=0)
            : ParseException(what,where,"dom::DomException",parsed_until),
              _myError(error) {}

        /// returns descriptive information depending on the error code value
        virtual const char * name() const {
            switch (_myError) {
                case INDEX_SIZE_ERR: return "DomException:INDEX_SIZE_ERR";
                case WSTRING_SIZE_ERR: return "DomException:INDEX_SIZE_ERR";
                case HIERARCHY_REQUEST_ERR: return "DomException:HIERARCHY_REQUEST_ERR";
                case WRONG_DOCUMENT_ERR: return "DomException:WRONG_DOCUMENT_ERR";
                case INVALID_NAME_ERR: return "DomException:INVALID_NAME_ERR";
                case NO_DATA_ALLOWED_ERR: return "DomException:NO_DATA_ALLOWED_ERR";
                case NO_MODIFICATION_ALLOWED_ERR: return "DomException:NO_MODIFICATION_ALLOWED_ERR";
                case NOT_FOUND_ERR: return "DomException:NOT_FOUND_ERR";
                case NOT_SUPPORTED_ERR: return "DomException:NOT_SUPPORTED_ERR";
                case INUSE_ATTRIBUTE_ERR: return "DomException:INUSE_ATTRIBUTE_ERR";
                default: return "dom::DomException";
            };
        }
        /// return the error code value
        Code Error() {
            return _myError;
        }
    private:
        Code _myError;
    };

    DEFINE_PARSE_EXCEPTION(FormatCorrupted,ParseException);
    DEFINE_PARSE_EXCEPTION(PatchMismatch,ParseException);
    DEFINE_PARSE_EXCEPTION(SyntaxError,ParseException);

  class EventException : public virtual asl::Exception {
    public:
        /** Error code the exception carries to indicate what has gone wrong
        */
        enum Code {
            UNSPECIFIED_EVENT_TYPE_ERR = 0,
            DISPATCH_REQUEST_ERR = 1
        };
        /// constructor
        EventException(const std::string & what, const std::string & where, Code error )
            : asl::Exception(what,where,"dom::EventException"),
              _myError(error) {}

        /// returns descriptive information depending on the error code value
        virtual const char * name() const {
            switch (_myError) {
                case UNSPECIFIED_EVENT_TYPE_ERR: return "dom::EventException:UNSPECIFIED_EVENT_TYPE_ERR";
                case DISPATCH_REQUEST_ERR: return "dom::EventException:DISPATCH_REQUEST_ERR";
                default: return "dom::EventException";
            };
        }
        /// return the error code value
        Code Error() {
            return _myError;
        }
        Code code() {
            return _myError;
        }
    private:
        Code _myError;
    };

    // This exception  is thrown, when you try to access an attribute
    // through a facade, that is not defined in the node and has no
    // default value
    DEFINE_EXCEPTION(MissingAttributeException, asl::Exception);

    // This exception is thrown, if the type of an attribute defined in
    // the schema does not match the type defined by the AttributePlug
    DEFINE_EXCEPTION(SchemaVsFacadeTypeMismatch, asl::Exception);

    // This exception is thrown, when you try to access an node value
    // on a node that must not have a node value
    DEFINE_EXCEPTION(ValueNotAllowed, asl::Exception);

    /* @} */
} //Namespace dom

#endif
