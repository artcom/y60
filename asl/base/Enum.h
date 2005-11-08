#include "Exception.h"

#include <vector>

namespace asl {

/*! @addtogroup aslbase */
/* @{ */

DEFINE_EXCEPTION(IllegalEnumValue, asl::Exception);

/** Slim wrapper for enumerations. It creates a new type from a native enum and
 * a char pointer array. It almost exactly behaves like a native enumeration
 * type. It supports string conversion through stream operators. This is usefull
 * for debugging, serialization/deserialization and also helps with implementing
 * language bindings. The two convinient macros DEFINE_ENUM() and VERIFY_ENUM()
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
 * const char * FruitStrings[] = {
 *      "apple",
 *      "cherry",
 *      "banana",
 *      "passion_fruit",
 *      ""
 * };
 *
 * DEFINE_ENUM( Fruit, FruitEnum, FruitStrings );
 * @endcode
 * In the corresponding cpp file do:
 * @code
 * VERIFY_ENUM( Fruit );
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
template <class ENUM, int THE_MAX, const char ** STRINGS>
class Enum {
    public:
        Enum() {};
        Enum( ENUM theValue ) : _myValue(theValue) {} 
        
        /** Assignment operator for native enum values. */
        void operator=(ENUM theValue) {
            _myValue = theValue;
        }

        /** Conversion operator for native enum values. */
        operator ENUM() {
            return _myValue;
        }

        enum Max {
            MAX = THE_MAX
        };
        ENUM max() const {
            return THE_MAX;
        }
        /** Converts @p theString to an enum. 
         * @throw IllegalEnumValue @p theString is not a legal identifier.
         * */
        void fromString(const std::string & theString) {
            for (unsigned i = 0; i < THE_MAX; ++i) {
                if (theString == STRINGS[i]) {
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

            while ( ! is.eof() ) {
                is >> myChar;
                if (is.eof()) {
                    break;
                }
                if (((myChar >= 'A' && myChar <= 'Z') || myChar >= 'a' && myChar <= 'z') ||
                    (myChar >= '0' && myChar <= '9') || (myChar == '_'))
                {
                    myWord += myChar;
                } else {
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
            return STRINGS[_myValue];
        }

        /** Prints the string identifier to the output stream @p os. */
        std::ostream & print(std::ostream & os = std::cerr) const {
            return os << STRINGS[_myValue];
        }
        
        /** Static helper function to iterate over valid identifiers. */
        static const char * getString(unsigned theIndex) {
            return STRINGS[theIndex];
        }

        /** Performs some simple checks and quits the application if
         * any error is detected. Tries to print helpful error messages.
         */
        static bool verify(const char * theFile, unsigned theLine) {
            for (unsigned i = 0; i < THE_MAX; ++i) {
                if (STRINGS[i] == 0 ) {
                    std::cerr << "### FATAL: Not enough strings for enum "
                              << _ourName << " defined in file '" << theFile 
                              << "' at line " << theLine << std::endl;
                    exit(1);
                }
            }
            if ( std::string("") != STRINGS[THE_MAX]) {
                std::cerr << "### FATAL: The string array for enum " << _ourName 
                          << " defined in file '" << theFile << "' at line " << theLine
                          << " has too many items or is not terminated with an "
                          << "empty string." << std::endl;
                exit(1);
            }
            return true;
        }
    private:
        ENUM _myValue;
        static const char * _ourName ;
        static bool  _ourVerifiedFlag; 
};

/* @} */

} // end of namespace

/** ostream operator for Enum 
 * @relates asl::Enum
 */
template <class ENUM, int THE_MAX, const char ** STRINGS>
std::ostream &
operator<<(std::ostream & os, const asl::Enum<ENUM, THE_MAX, STRINGS> & theEnum) {
    theEnum.print( os );
    return os;
}

/** istream operator for Enum 
 * @relates asl::Enum
 */
template <class ENUM, int THE_MAX, const char ** STRINGS>
std::istream &
operator>>(std::istream & is, asl::Enum<ENUM, THE_MAX, STRINGS> & theEnum) {
    theEnum.parse( is );
    return is;
}

/** Helper macro. Creates a typedef.
 * @relates asl::Enum
 */
#define DEFINE_ENUM( THE_NAME, THE_ENUM, THE_STRINGS) \
    typedef asl::Enum<THE_ENUM, THE_ENUM ## _MAX, THE_STRINGS> THE_NAME; 

/** Helper macro. Runs the verify() method during static initialization.
 * @relates asl::Enum
 */
#define VERIFY_ENUM( THE_NAME ) \
    template <> const char * THE_NAME ::_ourName = #THE_NAME; \
    template <> bool THE_NAME ::_ourVerifiedFlag( THE_NAME ::verify(__FILE__, __LINE__));

