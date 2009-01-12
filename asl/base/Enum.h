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
// Description:  Collect and Display Profiling Information
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor (no docs for bitset)
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : fair
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : poor
//    dependencies           : ok
//    cheesyness             : fair
//
//    overall review status  : ok
//
//    recommendations:
//       - replace existing enums in Y60 with this thing
//       - try to find a compile-time check instead of run-time verify()
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _asl_base_Enum_h_included_
#define _asl_base_Enum_h_included_

#include "asl_base_settings.h"

#include "Exception.h"
#include "settings.h"

#include <vector>
#include <bitset>
#include <cstdlib>

namespace asl {

/*! @addtogroup aslbase */
/* @{ */

DEFINE_EXCEPTION(IllegalEnumValue, asl::Exception);

/** Slim wrapper for enumerations. It creates a new type from a native enum and
 * a char pointer array. It almost exactly behaves like a native enumeration
 * type. It supports string conversion through stream operators. This is usefull
 * for debugging, serialization/deserialization and also helps with implementing
 * language bindings. The two convinient macros DEFINE_ENUM() and IMPLEMENT_ENUM()
 * are provided to help with instanciation.
 *
 * @par Limitations:
 *    - The enum must start at zero. 
 *    - The enum must be consecutive. 
 *    - The enum must be terminated with a special token consisting of the enum's
 *      name plus "_MAX".
 *    - The corresponding char pointer array must be terminated with an empty string.
 *    - The names must only consist of characters and digits (<i>a-z, A-Z, 0-9</i>)
 *      and underscores (<i>_</i>).
 * 
 *
 * @warning At the moment the verify() method is called during static initialization.
 * It quits the application with exit code 1 if any error is detected. The other solution
 * i thought of is to throw an exception with some kind of construct-on-first-use trick.
 * Opinions?
 *
 * @par Example:
 * The following code goes into the header file:
 * @code
 * enum FruitEnum {
 *      APPLE,
 *      CHERRY,
 *      BANANA,
 *      PASSION_FRUIT,
 *      FruitEnum_MAX
 * };
 *
 * static const char * myFruitStrings[] = {
 *      "apple",
 *      "cherry",
 *      "banana",
 *      "passion_fruit",
 *      ""
 * };
 *
 * DEFINE_ENUM( Fruit, FruitEnum);
 * @endcode
 * In the corresponding cpp file do:
 * @code
 * IMPLEMENT_ENUM( Fruit, myFruitStrings );
 * @endcode
 * And here is how to use the resulting type Fruit
 * @code
 * 
 * int
 * main(int argc, char * argv[]) {
 *      Fruit myFruit(APPLE);
 *      switch (myFruit) {
 *          case BANANA:
 *          case APPLE:
 *              cerr << "No thanks. I hate " << myFruit << "s" << endl;
 *              break;
 *          default:
 *              cerr << "Hmmm ... yummy " << myFruit << endl;
 *              break;
 *      }
 *      
 *      myFruit = CHERRY;
 *
 *      FruitEnum myNativeFruit = myFruit;
 *      
 *      myFruit.fromString("passion_fruit");
 *
 *      for (unsigned i = 0; i < Fruit::MAX; ++i) {
 *          cerr << i << " = " << Fruit::getString(i) << endl;
 *      }
 *
 *      myFruit.fromString("elephant"); // throws IllegalEnumValue
 *
 *      return 0;
 * }
 * @endcode
 */
template <class ENUM, int THE_MAX >
class Enum {
    public:
        typedef ENUM Native;
        Enum() {};
        Enum( ENUM theValue ) : _myValue(theValue) {} 
        Enum( const Enum & theOther ) : _myValue(theOther._myValue) {} 
        
        /** Assignment operator for native enum values. */
        void operator=(ENUM theValue) {
            _myValue = theValue;
        }

        /** Assignment operator. */
        void operator=(const Enum & theOther) {
            _myValue = theOther._myValue;
        }

        /** Conversion operator for native enum values. */
        operator ENUM() const {
            return _myValue;
        }

        enum Max {
            MAX = THE_MAX
        };
        ENUM max() const {
            return ENUM(THE_MAX);
        }
        /** Converts @p theString to an enum. 
         * @throw IllegalEnumValue @p theString is not a legal identifier.
         * */
        void fromString(const std::string & theString) {
            for (unsigned i = 0; i < THE_MAX; ++i) {
                if (theString == _ourStrings[i]) {
                    _myValue = static_cast<ENUM>(i);
                    return;
                }
            }
            throw IllegalEnumValue(std::string("Illegal enumeration value '") + 
                    theString + "' for enum " + _ourName, PLUS_FILE_LINE);
        }

        /** Parses the input stream @p is for a word consisting of characters and
         * digits (<i>a-z, A-Z, 0-9</i>) and underscores (<i>_</i>).
         * The failbit of the stream is set if the word is not a valid identifier.
         */
        std::istream & parse(std::istream & is) {
            std::string myWord;
            char myChar;

            while ( is.peek() != std::char_traits< char >::eof()) {
                is >> myChar;
                if ( isspace(myChar) && myWord.empty() ) {
                    continue;
                }
                if ( isalnum(myChar) || (myChar == '_'))
                {
                    myWord += myChar;
                } else {
                    is.unget();
                    break;
                }
            } 

            try {
                fromString( myWord );
            } catch (const IllegalEnumValue & ex) {
                std::cerr << ex; // XXX
                is.setstate(std::ios::failbit);
            }

            return is;
        }

        /** Returns the current value as a string identifier. */
        std::string asString() const {
            return _ourStrings[_myValue];
        }

        /** Prints the string identifier to the output stream @p os. */
        std::ostream & print(std::ostream & os = std::cerr) const {
            return os << _ourStrings[_myValue];
        }
        
        /** Static helper function to iterate over valid identifiers. */
        static const char * getString(unsigned theIndex) {
            return _ourStrings[theIndex];
        }

        /** Performs some simple checks and quits the application if
         * any error is detected. Tries to print helpful error messages.
         */
#ifdef verify
	#ifndef _SETTING_NO_UNDEF_WARNING_ 
		#warning Symbol 'verify' defined as macro, undefining. (Outrageous namespace pollution by Apples AssertMacros.h, revealing arrogance and incompetence)
	#endif
#undef verify
#endif
        static bool verify(const char * theFile, unsigned theLine) {
            for (unsigned i = 0; i < THE_MAX; ++i) {
                if (_ourStrings[i] == 0 ) {
                    std::cerr << "### FATAL: Not enough strings for enum "
                              << _ourName << " defined in file '" << theFile 
                              << "' at line " << theLine << std::endl;
                    exit(1);
                }
            }
            if ( std::string("") != _ourStrings[THE_MAX]) {
                std::cerr << "### FATAL: The string array for enum " << _ourName 
                          << " defined in file '" << theFile << "' at line " << theLine
                          << " has too many items or is not terminated with an "
                          << "empty string." << std::endl;
                exit(1);
            }
            return true;
        }
        static const char * getName() {
            return _ourName;
        }
    private:
        ENUM _myValue;
        static const char ** _ourStrings ;
        static const char * _ourName ;
        static bool  _ourVerifiedFlag; 
};


#define BIT( theBit ) (1 << theBit )

template <class THE_ENUM>
class Bitset : public std::bitset<THE_ENUM::MAX> {
    public:
        typedef Unsigned32 int_type; 
        typedef THE_ENUM Flags;

        Bitset(int_type theValue = 0) :
            std::bitset<THE_ENUM::MAX>(theValue) 
        {}

        operator int_type () const {
            return this->to_ulong();
        }

        std::istream & parse(std::istream & is) {
            this->reset();
            char myChar;
            is >> myChar;
            if (myChar != '[') {
                is.setstate(std::ios::failbit);
            }

            // special case: empty set
            if (is.good() && is.peek() == ']') {
                return is;
            }

            THE_ENUM myEnum;
            while ( is.good() && ! is.eof() ) {
                
                myEnum.parse(is);
                
                if (this->test(myEnum)) {
                    is.setstate(std::ios::failbit);
                }
                if ( is.fail() ) {
                    break;
                }
                this->set(myEnum);

                is >> myChar;

                if (myChar == ']') {
                    break;
                } else if (myChar != ',') {
                    is.setstate(std::ios::failbit);
                }
            } 

            return is;
        }
        /** Prints the string identifier to the output stream @p os. */
        std::ostream & print(std::ostream & os = std::cerr) const {
            os << '[';
            int mySetBitCount = 0;
            int myTotalBitCount = this->count();

            for (std::size_t i = 0; i < this->size(); ++i) {
                if (this->test(i)) {
                    //os << THE_ENUM(static_cast<typename THE_ENUM::Native>(i));
                    os << THE_ENUM::getString(i);
                    if (++mySetBitCount < myTotalBitCount) {
                        os << ',';
                    }
                }
            }
            os << ']';
            return os;
        }
        static const char * getName() {
            return _ourName;
        }
    private:
        operator long() const;
        static const char * _ourName ;
};

/* @} */


/** ostream operator for Enum 
 * @relates asl::Enum
 */
template <class ENUM, int THE_MAX>
std::ostream &
operator<<(std::ostream & os, const asl::Enum<ENUM, THE_MAX> & theEnum) {
    theEnum.print( os );
    return os;
}

/** istream operator for Enum 
 * @relates asl::Enum
 */
template <class ENUM, int THE_MAX>
std::istream &
operator>>(std::istream & is, asl::Enum<ENUM, THE_MAX> & theEnum) {
    theEnum.parse( is );
    return is;
}

/** ostream operator for Bitset 
 * @relates asl::Bitset
 */
template <class ENUM>
std::ostream &
operator<<(std::ostream & os, const asl::Bitset<ENUM> & theBitset) {
    theBitset.print( os );
    return os;
}

/** istream operator for Bitset 
 * @relates asl::Bitset
 */
template <class ENUM>
std::istream &
operator>>(std::istream & is, asl::Bitset<ENUM> & theBitset) {
    theBitset.parse( is );
    return is;
}

} // end of namespace

#if defined(AC_BUILT_WITH_CMAKE) && defined(WIN32)
#   define ASL_BASE_EXPORT_STATICS(THE_ENUM, DLL_EXPORT_TOKEN) template class DLL_EXPORT_TOKEN asl::Enum<THE_ENUM, THE_ENUM ## _MAX>;
#else
#   define ASL_BASE_EXPORT_STATICS(THE_ENUM, DLL_EXPORT_TOKEN)
#endif

/** Helper macro. Creates a typedef.
 * @relates asl::Enum
 */
#define DEFINE_ENUM( THE_NAME, THE_ENUM, DLL_EXPORT_TOKEN) \
    ASL_BASE_EXPORT_STATICS(THE_ENUM,DLL_EXPORT_TOKEN) \
    typedef asl::Enum<THE_ENUM, THE_ENUM ## _MAX> THE_NAME; 

/** Helper macro. Creates a typedef.
 * @relates asl::Bitset
 */
#define DEFINE_BITSET( THE_BITSET_NAME, THE_ENUM_NAME, THE_ENUM, DLL_EXPORT_TOKEN) \
    DEFINE_ENUM( THE_ENUM_NAME, THE_ENUM, DLL_EXPORT_TOKEN ); \
    typedef asl::Bitset<THE_ENUM_NAME> THE_BITSET_NAME; 

/** Helper macro. Runs the verify() method during static initialization.
 * @relates asl::Enum
 */
#define IMPLEMENT_ENUM( THE_NAME, THE_STRINGS ) \
    template <> const char ** THE_NAME ::_ourStrings = THE_STRINGS; \
    template <> const char * THE_NAME ::_ourName = #THE_NAME; \
    template <> bool THE_NAME ::_ourVerifiedFlag( THE_NAME ::verify(__FILE__, __LINE__));

/** Helper macro. Runs the verify() method during static initialization.
 * @relates asl::Bitset
 */
#define IMPLEMENT_BITSET( THE_BITSET_NAME, THE_ENUM_NAME, THE_STRINGS ) \
    template <> const char * THE_BITSET_NAME ::_ourName = #THE_BITSET_NAME; \
    IMPLEMENT_ENUM( THE_ENUM_NAME, THE_STRINGS );

#endif

