/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: Lexer.h,v $
//   $Author: pavel $
//   $Revision: 1.2 $
//   $Date: 2005/04/24 00:41:20 $
//
//
//=============================================================================

#ifndef _Y60_LEXER_INCLUDED_
#define _Y60_LEXER_INCLUDED_

#define DB2(x) // x
#define DB3(x) // x

namespace dys {

    typedef unsigned char Char;
    const int MAX_CHAR_VALUE = 255;
    typedef std::string String;
    const String EMPTY_STRING ("");

    inline bool is_whitespace(Char c) {
        return c == ' ' || c == '\t' || c == 0xb /*VT*/ || c == 0xC /*FF*/;
    }
    inline bool is_lineterm(Char c) {
        return c == '\n' || c == '\r';
    }
    inline bool is_alpha(Char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    inline bool is_printable(Char c) {
        return (c >= 0x20 && c < 0x7f);
    }
    inline bool is_underscore(Char c) {
        return c == '_';
    }
    inline bool is_colon(Char c) {
        return c == ':';
    }
    inline bool is_dot(Char c) {
        return c == '.';
    }
    inline bool is_decimalpoint(Char c) {
        return c == '.';
    }
    inline bool is_hyphen(Char c) {
        return c == '-';
    }
    inline bool is_plus(Char c) {
        return c == '+';
    }
    inline bool is_minus(Char c) {
        return c == '-';
    }
    inline bool is_exponent(Char c) {
        return c == 'E' || c == 'e';
    }
    inline bool is_dollar(Char c) {
        return c == '$';
    }
    inline bool is_digit(Char c) {
        return c >= '0' && c <= '9';
    }
    inline bool is_octaldigit(Char c) {
        return c >= '0' && c <= '7';
    }
    inline bool is_zero(Char c) {
        return c == '0';
    }
    inline bool is_x(Char c) {
        return c == 'x';
    }
 /*
    const String PUBID_SPECIAL_CHARS = "-'()+,./:=?;!*#@$_%";
    inline bool is_pubid_special(Char c) {
        for (unsigned int i = 0; i < PUBID_SPECIAL_CHARS.size();++i) {
            if (c == PUBID_SPECIAL_CHARS[i]) {
                return true;
            }
        }
        return false;
    }
    inline bool is_pubid(Char c) {
        return is_alpha(c) || is_digit(c) || is_space(c) || is_pubid_special(c);
    }
*/

    inline bool is_ABCDEF(Char c) {
        return c >= 'A' && c <= 'F';
    }
    inline bool is_abcdef(Char c) {
        return c >= 'a' && c <= 'f';
    }
    inline bool is_hexdigit(Char c) {
        return is_digit(c) || is_ABCDEF(c) || is_abcdef(c);
    }

    inline unsigned int digit_to_num(Char digit) {
        return digit - '0';
    }
    inline unsigned int ABCDEF_to_num(Char digit) {
        return digit - 'A' + 10;
    }
    inline unsigned int abcdef_to_num(Char digit) {
        return digit - 'a' + 10;
    }

    const Char AMP = '&';
    const Char LT = '<';
    const Char GT = '>';
    const Char QUOTE = '"';
    const Char APOS = '\'';
    const Char C9 = 9;
    const Char C10 = 10;
    const Char C13 = 13;
    const Char SLASH = '/';
    const Char EQUAL = '=';
    const Char SEMI = ';';
    const Char Cx = 'x';

    std::string as_decimal_entity(Char c) {
        unsigned char uc = c;
        String result = "#";
        result+=asl::as_string(static_cast<unsigned>(uc));
        result+=SEMI;
        return result;
    }

    /// convert a ascii hex digit to its binary value. ('2' -> 2, 'A' -> 10)
    /// returns false if not a hex digit
    bool hex_to_num(Char digit, unsigned int & num) {
        if (is_digit(digit)) {
            num = digit_to_num(digit);
            return true;
        } else if (is_ABCDEF(digit)) {
            num = ABCDEF_to_num(digit);
            return true;
        } else if (is_abcdef(digit)) {
            num = abcdef_to_num(digit);
            return true;
        }
        return false;
    }

    bool is_decimal_number(const String & s, unsigned int & r, unsigned int max_value = MAX_CHAR_VALUE) {
        if (s.size() == 0)
            return false;
        unsigned long result = 0;
        for (unsigned int i = 0; i< s.size();++i) {
            if (is_digit(s[i])) {
                result*=10;
                result+=digit_to_num(s[i]);
            } else
                return false;
            if (result > max_value)
                return false;
        }
        r = result;
        return true;
    }

    bool is_hex_number(const String & s, unsigned int & r, unsigned int max_value = MAX_CHAR_VALUE) {
        if (s.size() == 0)
            return false;
        unsigned long result = 0;
        for (unsigned int i = 0; i< s.size();++i) {
            unsigned int num;
            if (hex_to_num(s[i],num)) {
                result*=16;
                result+=num;
            } else
                return false;
            if (result > max_value)
                return false;
        }
        r = result;
        return true;
    }

    // returns the position of the first non-whitespace char
    int read_whitespace(const String & is, int pos) {
        while (pos < is.size() && (is_whitespace(is[pos]))) {
            ++pos;
        }
        return pos;
    }
    int read_lineterm(const String & is, int pos) {
        while (pos < is.size() && (is_lineterm(is[pos]))) {
            ++pos;
        }
        return pos;
    }
    // returns the position of the first non-digit char
    int read_digits(const String & is, int pos) {
        while (pos < is.size() && (is_digit(is[pos]))) {
            ++pos;
        }
        return pos;
    }
    // returns the position of the first non-digit char
    int read_octaldigits(const String & is, int pos) {
        while (pos < is.size() && (is_octaldigit(is[pos]))) {
            ++pos;
        }
        return pos;
    }
    // returns the position of the first non-digit char
    int read_hexdigits(const String & is, int pos) {
        while (pos < is.size() && (is_hexdigit(is[pos]))) {
            ++pos;
        }
        return pos;
    }
    inline bool is_sign(Char c) {
        return is_plus(c) || is_minus(c);
    }
    int read_decimal(const String & is,int pos) {
        int npos = pos;
        if (is_sign(is[npos]) && npos < is.size() ) {
            ++npos;
        }
        if (is_digit(is[npos]) && npos < is.size() ) {
            pos = npos + 1;
        }
        while (is_digit(is[pos]) && pos < is.size() ) {
            ++pos;
        }
        return pos;
    }
    int read_octal(const String & is,int pos) {
        int npos = pos;
        if (is_zero(is[npos]) && npos < is.size() ) {
            ++npos;
            if (is_octaldigit(is[npos]) && npos < is.size() ) {
                pos = npos + 1;

                while (is_octaldigit(is[pos]) && pos < is.size() ) {
                    ++pos;
                }
            }
        }
        return pos;
    }
    int read_hex(const String & is,int pos) {
        int npos = pos;
        if (is_zero(is[npos]) && npos < is.size() ) {
            ++npos;
            if (is_x(is[npos]) && npos < is.size() ) {
                ++npos;
                if (is_hexdigit(is[npos]) && npos < is.size() ) {
                    pos = npos + 1;
                    while (is_hexdigit(is[pos]) && pos < is.size() ) {
                        ++pos;
                    }
                }
            }
        }
        return pos;
    }
    int read_float(const String & is,int pos) {
        int npos = read_decimal(is, pos);
        if (npos > pos) {
            if (is_decimalpoint(is[npos])  && npos < is.size()) {
                pos = npos;
            }
            pos = read_digits(is, pos);
            if (is_exponent(is[pos]) && pos < is.size()) {
                return read_decimal(is, pos+1);
            }
        }
        return pos;
    }

    inline bool is_name_start_char(Char c) {
        return is_alpha(c) || is_underscore(c) || is_dollar(c);
    }
    inline bool is_name_char(Char c) {
        return is_name_start_char(c) || is_digit(c) || is_hyphen(c);
    }

    // returns the position of the first non-namechar
    int read_name(const String& is,int pos) {
        if ( is_name_start_char(is[pos]) && pos < is.size() ) {
            ++pos;
            while (is_name_char(is[pos]) && pos < is.size() ) {
                ++pos;
            }
        }
        return pos;
    }

    // returns pos + 1 if c is at pos
    inline int read_if_char(const String & is,int pos, Char c) {
        if ( is[pos] == c && pos < is.size() ) {
            return pos + 1;
        }
        return pos;
    }

    // returns position past end of s if s is the exact next part
    inline int read_if_string(const String & is,int pos, const String & s) {
        int i = 0;
        int n = asl::minimum(s.size(),is.size()-pos);
        while (i<n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size())
            return pos + i;
        return pos;
    }
/*
    // returns pos of next delim
    int read_text(const String & is,int pos, Char delim = LT) {
        int last_non_white = pos;
        while ( pos < is.size() && is[pos] != delim ) {
            if (!is_space(is[pos])) last_non_white = pos;
            ++pos;
        }
        return last_non_white+1;
    }
    */
    // returns position past second qoute char if at pos is a qoute char
    int read_quoted_text(const String & is,int pos,
        Char opening_qoute,Char closing_qoute )
    {
        int npos = pos;
        if (is[npos]==opening_qoute) {
            ++npos;
            while ( npos < is.size() && is[npos] != closing_qoute ) {
                if (is_lineterm(is[npos])) {
                    return pos;
                }
                ++npos;
            }
            if (is[npos]==closing_qoute) ++npos;
        }
        return npos;
    }
    int read_quoted_text(const String & is,int pos) {
        if (is[pos] == QUOTE)
            return read_quoted_text(is,pos,QUOTE,QUOTE);
        else
            return read_quoted_text(is,pos,APOS,APOS);
    }

    void copy_between_quotes(const String& is,int pos, int end_pos, String & dest) {
        dest=is.substr(pos+1,end_pos-pos-2);
    }
    // returns position past "right" if sequence starts with "left"
    int read_if_between(const String & is,int pos, const String & left, const String & right)
    {
        int left_end = read_if_string(is,pos,left);
        if (left_end > pos) {
            int right_begin = is.find(right,left_end);
            if (right_begin != String::npos)
                pos = right_begin + right.size();
        }
        return pos;
    }
    // returns until lineterm or eof if sequence starts with "left"
    int read_if_match_until_lineterm(const String & is,int pos, const String & left)
    {
        int npos = read_if_string(is,pos,left);
        if (npos > pos) {
            while (npos < is.size() && !is_lineterm(is[npos])) {
                ++npos;
            }
            return npos;
        }
        return pos;
    }

    typedef unsigned long TokenId;

    class Token {
    public:
        Token(TokenId theId, unsigned int theBegin, unsigned int theEnd)
            : myBegin(theBegin), myEnd(theEnd), myId(theId) {}
            virtual TokenId getId() const {
                return myId;
            }
            virtual unsigned int begin() const {
                return myBegin;
            }
            virtual unsigned int end() const {
                return myEnd;
            }
            virtual String getSection(const String & theText) {
                return theText.substr(myBegin, myEnd-myBegin);
            }
    private:
        TokenId myId;
        unsigned int myBegin;
        unsigned int myEnd;
    };
    typedef asl::Ptr<Token> TokenPtr;

    struct Definition {
        virtual unsigned int match(const String & is, unsigned int pos) const = 0;
    };
    typedef asl::Ptr<Definition> DefinitionPtr;

    struct ExactString : public Definition {
        ExactString(const String & theWord) : myWord(theWord) {}
        unsigned int match(const String & is, unsigned int pos) const {
            return read_if_string(is, pos, myWord);
        }
        const String myWord;
    };
    struct ExactChar : public Definition {
        ExactChar(Char theChar) : myChar(theChar) {}
        unsigned int match(const String & is, unsigned int pos) const {
            return read_if_char(is, pos, myChar);
        }
        const Char myChar;
    };

    struct WhiteSpace : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_whitespace(is, pos);
        }
    };

    struct Identifier : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_name(is, pos);
        }
    };
    struct StringLiteral : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_quoted_text(is, pos);
        }
    };
    struct DecimalLiteral : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_decimal(is, pos);
        }
    };
    struct HexLiteral : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_hex(is, pos);
        }
    };
    struct OctalLiteral : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_octal(is, pos);
        }
    };
    struct FloatLiteral : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_float(is, pos);
        }
    };
    struct BooleanLiteral : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return asl::maximum(read_if_string(is, pos, "true"), read_if_string(is, pos, "false"));
        }
    };
    struct CComment : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_if_between(is, pos, "/*", "*/");
        }
    };
    struct CppComment : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_if_match_until_lineterm(is, pos, "//");
        }
    };
    struct LineTermination : public Definition {
        unsigned int match(const String & is, unsigned int pos) const {
            return read_lineterm(is, pos);
        }
    };
    typedef std::vector<asl::Ptr<Token> > TokenList;
    typedef std::vector<asl::Ptr<Definition> > DefinitionList;
    typedef std::vector<const char *> TokenNameList;

    class Lexer {
    public:
        bool tokenize(const String & is, TokenList & theTokens) const {
            unsigned pos = 0;
            do {
                unsigned int myMaxPos = pos;
                unsigned int myMatchIndex = _myDefinitions.size();
                DB2(AC_TRACE << "Lexer::tokenize: pos =" << pos << std::endl);
                for (int i = 0; i < _myDefinitions.size(); ++i) {
                    unsigned int myMatchPos = _myDefinitions[i]->match(is, pos);
                    DB3(std::cerr << "Lexer::tokenize: checking " << getTokenName(i) << std::endl);
                    if (myMatchPos > myMaxPos) {
                        DB2(AC_TRACE << "Lexer::tokenize: " << getTokenName(i) << " matches, myMatchPos = " << myMatchPos << std::endl);
                        myMaxPos = myMatchPos;
                        myMatchIndex = i;
                    }
                }
                if (myMatchIndex < _myDefinitions.size()) {
                    DB2(AC_TRACE << "Lexer::tokenize: best match " << getTokenName(myMatchIndex) << std::endl);
                    theTokens.push_back(TokenPtr(new Token(myMatchIndex, pos, myMaxPos)));
                    pos = myMaxPos;
                } else {
                    std::cerr << "Lexer::tokenize: Error: no matching definition found" << std::endl;
                    std::cerr << is.substr(0,pos+1) << std::endl;
                    std::cerr << "<-- Lexer::tokenize: Error: no matching definition found" << std::endl;
                    std::cerr << is.substr(pos,100) << std::endl;
                    return false;
                }
            } while (pos < is.size());
            return true;
        }
        unsigned int define(Definition * theDefinition, const char * theName) {
            _myDefinitions.push_back(DefinitionPtr(theDefinition));
            _myTokenNames.push_back(theName);
            return _myDefinitions.size()-1;
        }
        const char * getTokenName(TokenId theId) const {
            if (theId < _myTokenNames.size()) {
                return _myTokenNames[theId];
            }
            std::cerr << "#ERROR: undefined id #"<<theId<<std::endl;
            return "TOKENID_IS_UNDEFINED";
        }
    private:
        DefinitionList _myDefinitions;
        TokenNameList _myTokenNames;
    };

#define DEFINITION(THE_ID, THE_DEFINITION) THE_ID(define(new THE_DEFINITION, # THE_ID))

    class DyscriptLexer : public Lexer {
    public:
        DyscriptLexer() :
            DEFINITION(WHITE_SPACE, WhiteSpace),
            DEFINITION(IDENTIFIER, Identifier),
            DEFINITION(C_COMMENT, CComment),
            DEFINITION(CPP_COMMENT, CppComment),
            DEFINITION(STRING_LITERAL, StringLiteral),
            DEFINITION(DECIMAL_LITERAL, DecimalLiteral),
            DEFINITION(HEX_LITERAL, HexLiteral),
            DEFINITION(OCTAL_LITERAL, OctalLiteral),
            DEFINITION(FLOAT_LITERAL, FloatLiteral),
            DEFINITION(NULL_LITERAL, ExactString("null")),
            DEFINITION(BOOLEAN_LITERAL, BooleanLiteral),
            DEFINITION(LINE_TERM, LineTermination),

            DEFINITION(COLON, ExactChar(':')),
            DEFINITION(QUESTION_MARK, ExactChar('?')),
            DEFINITION(SEMICOLON, ExactChar(';')),
            DEFINITION(OPEN_BRACE, ExactChar('(')),
            DEFINITION(CLOSE_BRACE, ExactChar(')')),
            DEFINITION(OPEN_BRACKET, ExactChar('[')),
            DEFINITION(CLOSE_BRACKET, ExactChar(']')),
            DEFINITION(OPEN_CURLY_BRACE, ExactChar('{')),
            DEFINITION(CLOSE_CURLY_BRACE, ExactChar('}')),
            DEFINITION(DOT, ExactChar('.')),
            DEFINITION(COMMA, ExactChar(',')),
            DEFINITION(MINUS, ExactChar('-')),
            DEFINITION(PLUS, ExactChar('+')),
            DEFINITION(DIV, ExactChar('/')),
            DEFINITION(MULT, ExactChar('*')),
            DEFINITION(MODULO, ExactChar('%')),
            DEFINITION(ASSIGN, ExactChar('=')),
            DEFINITION(LESS, ExactChar('<')),
            DEFINITION(GREATER, ExactChar('>')),
            DEFINITION(EQUAL, ExactString("==")),
            DEFINITION(NOT_EQUAL, ExactString("!=")),
            DEFINITION(LESS_EQUAL, ExactString("<=")),
            DEFINITION(GREATER_EQUAL, ExactString(">=")),
            DEFINITION(ASSIGN_PLUS, ExactString("+=")),
            DEFINITION(ASSIGN_MINUS, ExactString("-=")),
            DEFINITION(ASSIGN_DIV, ExactString("/=")),
            DEFINITION(ASSIGN_MULT, ExactString("*=")),
            DEFINITION(ASSIGN_MODULO, ExactString("%=")),
            DEFINITION(SHIFT_LEFT, ExactString("<<")),
            DEFINITION(SHIFT_RIGHT, ExactString(">>")),
            DEFINITION(SHIFT_RIGHT_UNSIGNED, ExactString(">>>")),
            DEFINITION(NOT, ExactChar('!')),
            DEFINITION(AND, ExactString("&&")),
            DEFINITION(OR, ExactString("||")),
            DEFINITION(BIT_XOR, ExactChar('~')),
            DEFINITION(BIT_AND, ExactChar('&')),
            DEFINITION(BIT_OR, ExactChar('|')),
            DEFINITION(ASSIGN_BIT_XOR, ExactString("~=")),
            DEFINITION(ASSIGN_BIT_AND, ExactString("&=")),
            DEFINITION(ASSIGN_BIT_OR, ExactString("|=")),
            DEFINITION(ASSIGN_SHIFT_LEFT, ExactString("<<=")),
            DEFINITION(ASSIGN_SHIFT_RIGHT, ExactString(">>=")),
            DEFINITION(ASSIGN_SHIFT_RIGHT_UNSIGNED, ExactString(">>>=")),
            DEFINITION(INCREMENT, ExactString("++")),
            DEFINITION(DECREMENT, ExactString("--")),

            DEFINITION(BREAK, ExactString("break")),
            DEFINITION(FOR, ExactString("for")),
            DEFINITION(NEW, ExactString("new")),
            DEFINITION(VAR, ExactString("var")),
            DEFINITION(CONTINUE, ExactString("continue")),
            DEFINITION(FUNCTION, ExactString("function")),
            DEFINITION(RETURN, ExactString("return")),
            DEFINITION(VOID_TOKEN, ExactString("void")),
            DEFINITION(IF, ExactString("if")),
            DEFINITION(THIS_TOKEN, ExactString("this")),
            DEFINITION(WHILE, ExactString("while")),
            DEFINITION(ELSE, ExactString("else")),
            DEFINITION(TYPEOF, ExactString("typeof")),
            DEFINITION(WITH, ExactString("with")),

            DEFINITION(CASE, ExactString("case")),
            DEFINITION(DEBUGGER, ExactString("debugger")),
            DEFINITION(T_DECL, ExactString("export")),
            DEFINITION(SUPER, ExactString("super")),
            DEFINITION(DEFAULT, ExactString("default")),
            DEFINITION(EXTENDS, ExactString("extends")),
            DEFINITION(SWITCH, ExactString("switch")),
            DEFINITION(CLASS, ExactString("class")),
            DEFINITION(DO, ExactString("do")),
            DEFINITION(FINALLY, ExactString("finally")),
            DEFINITION(THROW, ExactString("throw")),
            DEFINITION(CONST_TOKEN, ExactString("const")),
            DEFINITION(ENUM, ExactString("enum")),
            DEFINITION(IMPORT, ExactString("import")),
            DEFINITION(TRY, ExactString("try")),
            DEFINITION(CATCH, ExactString("catch"))
            {}


        // literals
        const TokenId STRING_LITERAL;
        const TokenId DECIMAL_LITERAL;
        const TokenId HEX_LITERAL;
        const TokenId OCTAL_LITERAL;
        const TokenId FLOAT_LITERAL;
        const TokenId BOOLEAN_LITERAL;
        const TokenId NULL_LITERAL;
        const TokenId LINE_TERM;

        // comments
        const TokenId C_COMMENT;
        const TokenId CPP_COMMENT;

        // Punctuators
        const TokenId COLON;
        const TokenId QUESTION_MARK;
        const TokenId SEMICOLON;
        const TokenId OPEN_BRACE;
        const TokenId CLOSE_BRACE;
        const TokenId OPEN_BRACKET;
        const TokenId CLOSE_BRACKET;
        const TokenId OPEN_CURLY_BRACE;
        const TokenId CLOSE_CURLY_BRACE;
        const TokenId DOT;
        const TokenId COMMA;
        const TokenId MINUS;
        const TokenId PLUS;
        const TokenId DIV;
        const TokenId MULT;
        const TokenId MODULO;
        const TokenId ASSIGN;
        const TokenId LESS;
        const TokenId GREATER;
        const TokenId EQUAL;
        const TokenId NOT_EQUAL;
        const TokenId LESS_EQUAL;
        const TokenId GREATER_EQUAL;
        const TokenId ASSIGN_PLUS;
        const TokenId ASSIGN_MINUS;
        const TokenId ASSIGN_DIV;
        const TokenId ASSIGN_MULT;
        const TokenId ASSIGN_MODULO;
        const TokenId SHIFT_LEFT;
        const TokenId SHIFT_RIGHT;
        const TokenId SHIFT_RIGHT_UNSIGNED;
        const TokenId NOT;
        const TokenId AND;
        const TokenId OR;
        const TokenId BIT_XOR;
        const TokenId BIT_AND;
        const TokenId BIT_OR;
        const TokenId ASSIGN_BIT_XOR;
        const TokenId ASSIGN_BIT_AND;
        const TokenId ASSIGN_BIT_OR;
        const TokenId INCREMENT;
        const TokenId DECREMENT;
        const TokenId ASSIGN_SHIFT_LEFT;
        const TokenId ASSIGN_SHIFT_RIGHT;
        const TokenId ASSIGN_SHIFT_RIGHT_UNSIGNED;

        // reserved words
        const TokenId BREAK;
        const TokenId FOR;
        const TokenId NEW;
        const TokenId VAR;
        const TokenId CONTINUE;
        const TokenId FUNCTION;
        const TokenId RETURN;
        const TokenId VOID_TOKEN;
        const TokenId IF;
        const TokenId THIS_TOKEN;
        const TokenId WHILE;
        const TokenId ELSE;
        const TokenId TYPEOF;
        const TokenId WITH;

        const TokenId CASE;
        const TokenId DEBUGGER;
        const TokenId T_DECL;
        const TokenId SUPER;
        const TokenId DEFAULT;
        const TokenId EXTENDS;
        const TokenId SWITCH;
        const TokenId CLASS;
        const TokenId DO;
        const TokenId FINALLY;
        const TokenId THROW;
        const TokenId CONST_TOKEN;
        const TokenId ENUM;
        const TokenId IMPORT;
        const TokenId TRY;
        const TokenId CATCH;

        // whitespace and general identifier have lower priority
        const TokenId WHITE_SPACE;
        const TokenId IDENTIFIER;
     };

} // namespace
#endif
