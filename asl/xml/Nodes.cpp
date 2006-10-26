/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Nodes.cpp,v $
//
//   $Revision: 1.47 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Nodes.h"
#include "Schema.h"
#include "SchemaStrings.h"

#include <asl/Time.h>
#include <asl/Logger.h>
#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/file_functions.h>
#include <asl/os_functions.h>

#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>

#include <iostream>

#define DB(x)  // x
#define DB2(x) //  x
#define DBS(x)  // x
#define DB_Y(x) // x

using namespace std;

namespace dom {

    const char * NodeTypeName[16] = {
        "X_NO_NODE",
        "ELEMENT_NODE",
        "ATTRIBUTE_NODE",
        "TEXT_NODE",
        "CDATA_SECTION_NODE",
        "ENTITY_REFERENCE_NODE",
        "ENTITY_NODE",
        "PROCESSING_INSTRUCTION_NODE",
        "COMMENT_NODE",
        "DOCUMENT_NODE",
        "DOCUMENT_TYPE_NODE",
        "DOCUMENT_FRAGMENT_NODE",
        "NOTATION_NODE",
        "UNUSED_13",
        "UNUSED_14",
        "X_END_NODE"
    };


    asl::Unsigned32 dom::UniqueId::_myCounter(0);

    std::ostream& operator<<(std::ostream& os, const UniqueId & uid) {
        os << uid._myCount << "@" << uid._ptrValue;
        return os;
    }

    typedef unsigned char Char;
    const int MAX_CHAR_VALUE = 255;
    typedef DOMString String;
    const String EMPTY_STRING ("");

    inline bool is_space(Char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    inline bool is_alpha(Char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    inline bool is_printable(Char c) {
        return (c >= 0x20 && c < 0x7f);
    }
    inline bool is_ascii(Char c) {
        return (c <= 0x7f);
    }
    inline bool is_utf8_sequence_followup(Char c) {
        return (c >= 0x80) && (c <= 0xBF);
    }
    inline bool is_utf8_sequence_start(Char c) {
        return (c & 0xC0) == 0xC0;
    }
    inline bool is_utf8_multibyte(Char c) {
        return is_utf8_sequence_start(c) || is_utf8_sequence_followup(c);
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
    inline bool is_hyphen(Char c) {
        return c == '-';
    }

    inline bool is_digit(Char c) {
        return c >= '0' && c <= '9';
    }

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


    inline bool is_ABCDEF(Char c) {
        return c >= 'A' && c <= 'F';
    }
    inline bool is_abcdef(Char c) {
        return c >= 'a' && c <= 'f';
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

   /*
    datachar    ::= '&amp;' | '&lt;' | '&gt;' | '&quot;'
    | '&#9;'| '&#10;'| '&#13;'
    | (Char - ('&' | '<' | '>' | '"' | #x9 | #xA | #xD))

    */

    const String ENT_AMP("&amp;");
    const String ENT_LT("&lt;");
    const String ENT_GT("&gt;");
    const String ENT_QUOT("&quot;");
    const String ENT_APOS("&apos;");
    const String ENT_9("&#9;");
    const String ENT_10("&#10;");
    const String ENT_13("&#13;");
    const String ENT_DEC("&#");
    const String ENT_SEMI(";");

    const String COMMENT_BGN("<!--");
    const String COMMENT_END("-->");

    const String CDATA_BGN("<![CDATA[");
    const String CDATA_END("]]>");

    const String DOCTYPE_BGN("<!DOCTYPE");
    const String DOCTYPE_END(">");

    const String ENTITY_BGN("<!ENTITY");
    const String ENTITY_END(">");

    const String PI_BGN("<?");
    const String PI_END("?>");

    const String ETAG_BGN("</");
    const String ETAG_END(">");


    std::string as_decimal_entity(Char c) {
        unsigned char uc = c;
        String result = ENT_DEC;
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
        unsigned int result = 0;
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
        unsigned int result = 0;
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
        while (pos < is.size() && (is_space(is[pos]))) {
            ++pos;
        }
        return pos;
    }

    inline bool is_name_start_char(Char c) {
        return is_alpha(c) || is_underscore(c) || is_colon(c);
    }
    inline bool is_name_char(Char c) {
        return is_name_start_char(c) || is_digit(c) || is_dot(c) || is_hyphen(c);
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

    // returns pos of next delim
    int read_text(const String & is,int pos, Char delim = LT) {
        int last_non_white = pos;
        while ( pos < is.size() && is[pos] != delim ) {
            if (!is_space(is[pos])) last_non_white = pos;
            ++pos;
        }
        return last_non_white+1;
    }
    // returns position past second qoute char if at pos is a qoute char
    int read_quoted_text(const String & is,int pos,
        Char opening_qoute,Char closing_qoute ) {
        if (is[pos]==opening_qoute) {
            ++pos;
            while ( pos < is.size() && is[pos] != closing_qoute ) {
                ++pos;
            }
            if (is[pos]==closing_qoute) ++pos;
        }
        return pos;
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

    // utility for previous function to copy what's between into string dest
    void copy_between(const String & is,int pos, const String & left, const String & right,
        int right_end_pos, String & dest)
    {
        int begin = pos+left.size();
        dest = is.substr(begin,right_end_pos-begin-right.size());
    }

    // read <!-- comment -->
    int read_comment(const String& is,int pos,String & comment) {
        int new_pos = read_if_between(is,pos,COMMENT_BGN,COMMENT_END);
        if (new_pos > pos) {
            copy_between(is,pos,COMMENT_BGN,COMMENT_END,new_pos,comment);
        }
        return new_pos;
    }

    // read <?target processing_instruction?>
    int read_processing_instruction(const String & is,int pos,
                                    String & target,String & proc_instr)
    {
        int new_pos = read_if_between(is,pos,PI_BGN,PI_END);
        if (new_pos > pos) {
            int instr_pos = read_name(is,pos+2);
            if (instr_pos > pos+2) {
                target = is.substr(pos+2,instr_pos-pos-2);
                instr_pos = read_whitespace(is,instr_pos);
                proc_instr = is.substr(instr_pos,new_pos-instr_pos-2);
            }
        }
        return new_pos;
    }

    // read <![CDATA[ some otherwise entity-encoded <characters> may appear <!here/> ]]>
    int read_cdata(const String & is,int pos,String & cdata) {
        int new_pos = read_if_between(is,pos,CDATA_BGN,CDATA_END);
        if (new_pos > pos) {
            copy_between(is,pos,CDATA_BGN,CDATA_END,new_pos,cdata);
        }
        return new_pos;
    }

    String entity_decode_data(const String & s, int global_pos, const Node * doctype);

    // read PUBLIC "PubidLiteral is alternative URI/-" "URL is SystemLiteral"
    // or   SYSTEM "URL is SystemLiteral"
    // PubidLiteral is set to empty when not a public id
    int read_ExternalID(const String & is, int pos, String & PubidLiteral, String & SystemLiteral) {
        int new_pos = read_if_string(is,pos,"PUBLIC");
        if (new_pos == pos) {
            new_pos = read_if_string(is,pos,"SYSTEM");
            PubidLiteral.resize(0);
        } else {
            // read PubidLiteral
            DB(AC_TRACE << "PUBLIC");
            int pubid_start_pos = read_whitespace(is, new_pos);
            int pubid_end_pos = read_quoted_text(is, pubid_start_pos);
            if (pubid_end_pos > pubid_start_pos) {
                copy_between_quotes(is,pubid_start_pos,pubid_end_pos ,PubidLiteral);
                for (int i = 0; i < PubidLiteral.size(); ++i) {
                    if (!is_pubid(PubidLiteral[i])) {
                        throw ParseException("bad character in PUBLIC id",
                                              PLUS_FILE_LINE,pubid_start_pos + i + 1);
                    }
                }
                new_pos = pubid_end_pos;
                goto readSystemLiteral;
                //return pubid_end_pos;
            } else {
                throw ParseException("PUBLIC id literal missing",
                        PLUS_FILE_LINE,pubid_end_pos);
            }
            return pos;
        }
        if (new_pos > pos) {
readSystemLiteral:
            // read SystemLiteral
            int syslit_start_pos = read_whitespace(is, new_pos);
            int syslit_end_pos = read_quoted_text(is, syslit_start_pos);
            if (syslit_end_pos > syslit_start_pos) {
                copy_between_quotes(is,syslit_start_pos, syslit_end_pos, SystemLiteral);
                return syslit_end_pos;
            } else {
                throw ParseException("PUBLIC or SYSTEM id literal missing",
                        PLUS_FILE_LINE,syslit_end_pos);
            }
         }
        return pos;
    }
    // read <!DOCTYPE name (externalID) ([ notation & entitity node may appear here ]) >
    //                 read until here ^
    int read_document_type_start(const String & is,int pos,
                                 String & name,
                                 String & PubidLiteral,
                                 String & SystemLiteral)
    {
        int new_pos = read_if_string(is,pos,DOCTYPE_BGN);
        if (new_pos > pos) {
            // <!DOCTYPE found
            DB(AC_DEBUG <<DOCTYPE_BGN << " - found");
            new_pos = read_whitespace(is, new_pos);
            int next_pos = read_name(is,new_pos);
            if (next_pos > new_pos) {
                // name ok
                name = is.substr(new_pos,next_pos-new_pos);
                DB(AC_DEBUG <<DOCTYPE_BGN << " - name ok: '" << name << "'");
                int id_pos = read_whitespace(is,next_pos);

                // look for external id
                int children_pos = read_ExternalID(is,id_pos,PubidLiteral,SystemLiteral);
                if (children_pos > id_pos) {
                    DB(AC_DEBUG <<DOCTYPE_BGN << " - external ids PubidLiteral: '"
                            << PubidLiteral << "'");
                    DB(AC_DEBUG <<DOCTYPE_BGN << " - external ids SystemLiteral: '"
                            << SystemLiteral << "'");
                    // all complete
                    return children_pos;
                }
                // has no external id
                return next_pos;
            }
        }
        return pos;
    }

    // read <!ENTITY (%) name EntityValue|(ExternalID (NdataDecl)) >
    int read_entity_declaration(const String & is,int pos,
                                 String & name,
                                 String & value,
                                 bool & isParsedEntity,
                                 String & PubidLiteral,
                                 String & SystemLiteral,
                                 String & NDataName,
                                 const Node * doctype)
    {
        int new_pos = read_if_string(is,pos,ENTITY_BGN);
        if (new_pos > pos) {
            // <!ENTITY found
            DB(AC_DEBUG << ENTITY_BGN << " - found");
            new_pos = read_whitespace(is, new_pos);
            if (is[new_pos] == '%') {
                new_pos = read_whitespace(is, new_pos+1);
                isParsedEntity = true;
                DB(AC_DEBUG <<ENTITY_BGN << " - is parsed");
            } else {
                isParsedEntity = false;
                DB(AC_DEBUG <<ENTITY_BGN << " - is not parsed");
            }
            int next_pos = read_name(is,new_pos);
            if (next_pos > new_pos) {
                name = is.substr(new_pos,next_pos-new_pos);
                DB(AC_DEBUG <<ENTITY_BGN << " - name ok: '" << name << "'");
                int id_pos = read_whitespace(is,next_pos);
                int id_end_pos = read_ExternalID(is,id_pos,PubidLiteral,SystemLiteral);
                if (id_end_pos > id_pos) {
                    // ExternalID found
                    DB(AC_DEBUG <<ENTITY_BGN << " - external ids PubidLiteral: '"
                            << PubidLiteral << "'");
                    DB(AC_DEBUG <<ENTITY_BGN << " - external ids SystemLiteral: '"
                            << SystemLiteral << "'");
                    if (!isParsedEntity) {
                        // see if NDataDecl is present
                        int ndata_decl_pos = read_whitespace(is,id_end_pos);
                        int ndata_pos = read_if_string(is, ndata_decl_pos, "NDATA");
                        if (ndata_pos > ndata_decl_pos) {
                            int ndata_name_pos = read_whitespace(is, ndata_pos);
                            int ndata_name_end = read_name(is, ndata_name_pos);
                            if (ndata_name_end > ndata_name_pos) {
                                NDataName = is.substr(ndata_name_pos, ndata_name_end - ndata_name_pos);
                                // ok, correct NDataDecl parsed
                                id_end_pos = ndata_name_end;
                            } else {
                                throw ParseException("bad or missing name after NDATA declaration",
                                        PLUS_FILE_LINE,ndata_name_end);
                            }
                        }
                        // no NDataDecl present, we should be almost ready now
                    }
                } else {
                    // no ExternalID, so EntityValue required
                    DB(AC_DEBUG <<ENTITY_BGN << " - value required");
                    int ev_start = read_whitespace(is,id_pos);
                    int ev_end = read_quoted_text(is, ev_start);
                    if (ev_end > ev_start) {
                        string entityValue;
                        copy_between_quotes(is,ev_start,ev_end, entityValue);
                        DB(AC_DEBUG <<ENTITY_BGN << " - value ok: '" << entityValue<< "'");
                        value = entity_decode_data(entityValue, ev_start + 1,doctype);
                        id_end_pos = ev_end;
                    }
                }
                int final_pos = read_whitespace(is, id_end_pos);
                if (is[final_pos] == '>') {
                    // all went fine
                    return final_pos + 1;
                }
            }
        }
        return pos;
    }

    // read <id param1="value1" p2="v2">
    int read_start_tag(const String& is,int pos,String & id, int & params_begin, int & params_end) {
        int tag_end = read_quoted_text(is,pos,LT,GT);
        if (tag_end > pos) {
            int id_end = read_name(is,pos+1);
            if (id_end > pos + 1) {
                id = is.substr(pos+1,id_end-pos-1);
                params_begin = id_end;
                params_end = tag_end - 1;
            } else {
                throw ParseException("malformed name in start tag", PLUS_FILE_LINE,tag_end);
            }
        }
        return tag_end;
    }

    // read </id>
    int read_end_tag(const String& is,int pos,String& id)   {
        int tag_end = read_if_between(is,pos,ETAG_BGN,ETAG_END);
        if (tag_end > pos) {
            int id_end = read_name(is,pos+2);
            if (id_end>pos) {
                id = is.substr(pos+2,id_end-pos-2);
            } else {
                throw ParseException("malformed name in end tag", PLUS_FILE_LINE,pos);
            }
        }
        return tag_end;
    }

    // read <id/> or <id param1="value1" p2="v2"/>
    int read_empty_tag(const String & is,int pos,String & id,int & params_begin, int & params_end) {
        int tag_end = read_quoted_text(is,pos,LT,GT);
        if (tag_end > pos+2 && is[tag_end-2] == SLASH) {
            int id_end = read_name(is,pos+1);
//PORT            const char* id_end_ptr = &is[id_end];
            if (id_end > pos+1) {
                id = is.substr(pos+1,id_end - pos-1);
                params_begin = read_whitespace (is, id_end);
                params_end = tag_end - 2;
                pos = tag_end;
            } else {
                throw ParseException("malformed name in empty tag", PLUS_FILE_LINE,pos);
            }
        }
        return pos;
    }

    // read name="value"
    int read_attribute(const String & is,int pos,String & name,String & value) {
        int nw_pos = read_whitespace(is,pos);
        int name_end = read_name(is,nw_pos);
        if (name_end > nw_pos) {
            name = is.substr(nw_pos,name_end-nw_pos);
            nw_pos = read_whitespace(is,name_end);
            int eq_pos = read_if_char(is,nw_pos,EQUAL);
            if (eq_pos > nw_pos) {
                nw_pos = read_whitespace(is,eq_pos);
                int val_end = read_quoted_text(is,nw_pos);
                if (val_end > nw_pos) {
                    copy_between_quotes(is,nw_pos,val_end,value);
                    return val_end;
                }
            }
        } else {
            // check for just some whitespace
            if (nw_pos > pos) {
                name.resize(0);
                return nw_pos;
            }
        }
        return pos;
    }

    String entity_encode_data(const String & s, Char quote_char) {
        String result;
        result.reserve(s.size() + s.size()/10);
        for (int i = 0 ; i < s.size();++i) {
            switch (s[i]) {
            case AMP: result.append(ENT_AMP);
                break;
            case LT: result.append(ENT_LT);
                break;
            case GT: result.append(ENT_GT);
                break;
                /*
            case C9: result.append(ENT_9);
                break;
            case C10: result.append(ENT_10);
                break;
            case C13: result.append(ENT_13);
                break;
                */
            case QUOTE:
                if (s[i] == quote_char)
                    result.append(ENT_QUOT);
                else
                    result+=s[i];
                break;
            case APOS:
                if (s[i] == quote_char)
                    result.append(ENT_APOS);
                else
                    result+=s[i];
                break;
            default:
                if (is_printable(s[i]) || is_space(s[i]) || is_utf8_multibyte(s[i])) {
                    result+=s[i];
                } else {
                    result.append(as_decimal_entity(s[i]));
                }
                break;
            }
        }
        return result;
    }

    String entity_decode_data(const String & s, int global_pos, const Node * doctype) {
        String result;
        result.reserve(s.size());
        int i = 0;
        while (i < s.size()) {
            if (s[i] == AMP) {
                int next_pos = i;
                if ((next_pos = read_if_string(s,i,ENT_AMP)) > i)
                    result+=AMP;
                else if ((next_pos = read_if_string(s,i,ENT_LT)) > i)
                    result+=LT;
                else if ((next_pos = read_if_string(s,i,ENT_GT)) > i)
                    result+=GT;
                else if ((next_pos = read_if_string(s,i,ENT_9)) > i)
                    result+=C9;
                else if ((next_pos = read_if_string(s,i,ENT_10)) > i)
                    result+=C10;
                else if ((next_pos = read_if_string(s,i,ENT_13)) > i)
                    result+=C13;
                else if ((next_pos = read_if_string(s,i,ENT_QUOT)) > i)
                    result+=QUOTE;
                else if ((next_pos = read_if_string(s,i,ENT_APOS)) > i)
                    result+=APOS;
                else {
                    next_pos = read_if_between(s,i,ENT_DEC,ENT_SEMI);
                    if (next_pos > i) {
                        String entity_name;
                        copy_between(s,i,ENT_DEC,ENT_SEMI,next_pos,entity_name);
                        unsigned int value;
                        if (is_decimal_number(entity_name,value)) {
                            result+=value;
                        } else
                            if (entity_name[0] == Cx) {
                                String hex_string = entity_name.substr(1);
                                if (is_hex_number(hex_string,value)) {
                                    result+=value;
                                } else {
                                    String error_msg;
                                    error_msg = "illegal entity number code '" + s.substr(i,next_pos) + "'";
                                    throw ParseException(error_msg,
                                        PLUS_FILE_LINE,global_pos+next_pos);
                                }
                            } else {
                                String error_msg;
                                error_msg = "illegally formed number code '" + s.substr(i,next_pos) + "'";
                                throw ParseException(error_msg,
                                    PLUS_FILE_LINE,global_pos+next_pos);
                            }
                    } else {
                        next_pos = read_quoted_text(s,i,AMP,SEMI);
                        String entity_name;
                        copy_between_quotes(s,i,next_pos,entity_name);
                        bool found = false;
                        if (doctype) {
                            for (int child = 0; child < doctype->childNodesLength(); ++child) {
                                if (doctype->childNode(child)->nodeType() == Node::ENTITY_NODE &&
                                    doctype->childNode(child)->nodeName() == entity_name) {
                                        result+=doctype->childNode(child)->nodeValue();
                                        found = true;
                                }
                            }
                        }
                        if (!found) {
                            String error_msg;
                            error_msg = "unknown entity reference (missing';'?)  '" + s.substr(i,next_pos) + "'";
                            throw ParseException(error_msg,
                                    PLUS_FILE_LINE,global_pos+i+entity_name.size());
                        }
                    }
                }
                i = next_pos;
            } else {
                result+=s[i];
                ++i;
            }
        }
        return result;
    }

} // namespace dom

using namespace dom;

const dom::Node dom::Node::X_NO_NODE_;
dom::Node dom::Node::Prototype;

// check if value exist; otherwise create one
void
dom::Node::ensureValue() const {
    DB2(AC_TRACE << "ensureValue("<<NodeTypeName[_myType]<<","<<_myName<<")" << endl);
    if (!_myValue) {
        if (!mayHaveValue()) {
            std::string myError = "Node of type ";
            myError += NodeTypeName[_myType];
            myError += " must not have a value";
            throw ValueNotAllowed(myError,PLUS_FILE_LINE);
        }
        if (_mySchemaInfo) {
            if (_mySchemaInfo->_myValueFactory) {
                DBS(AC_ERROR << "ensureValue(): creating value for type " <<_mySchemaInfo->getTypeName()<< endl);
                const_cast<ValuePtr&>(_myValue)
                    = _mySchemaInfo->_myValueFactory->createValue(_mySchemaInfo->getTypeName(), const_cast<Node*>(this));
                if (_myValue) {
                    return;
                }
                AC_WARNING << "no factory method found for type '"<<_mySchemaInfo->getTypeName()<<endl;
            } else {
                DB(AC_WARNING << "schemainfo but no factory "<<endl);
            }
        }
        // TODO: consult schema and valuefactory to select apropriate type
        DBS(AC_ERROR << "ensureValue(): creating string value for unknown type "<< endl);
        const_cast<ValuePtr&>(_myValue) = ValuePtr(new StringValue(const_cast<Node*>(this)));
    }
}
// check if value exist; otherwise create one
void
dom::Node::assignValue(const asl::ReadableBlock & theValue) {
    DB2(AC_TRACE << "assignValue("<<NodeTypeName[_myType]<<","<<_myName<<")" << endl);
    if (!_myValue) {
        if (!mayHaveValue()) {
            std::string myError = "Node of type ";
            myError += NodeTypeName[_myType];
            myError += " must not have a value";
            throw ValueNotAllowed(myError,PLUS_FILE_LINE);
        }
        if (_mySchemaInfo) {
            if (_mySchemaInfo->_myValueFactory) {
                DB2(AC_TRACE << "assignValue(): creating value for type '" <<_mySchemaInfo->getTypeName()<<"'"<< endl);
                const_cast<ValuePtr&>(_myValue)
                    = _mySchemaInfo->_myValueFactory->createValue(_mySchemaInfo->getTypeName(), theValue, this);
                if (_myValue) {
                    return;
                }
                AC_WARNING << "no factory method found for type '"<<_mySchemaInfo->getTypeName()<<"'"<<endl;
            }
            throw Schema::NotInFactory("Value for type without Factory can not be assigned","dom::Node::assignValue");
        }
        throw Schema::NotAvailable("Value without type information can not be assigned","dom::Node::assignValue");
    } else {
        _myValue->set(theValue);
    }
}

// check the value type if a value exists
bool
dom::Node::checkMyValueTypeFits() const {
    if (_myValue) {
        return checkValueTypeFits(*_myValue);
            /*
        if (!checkValueTypeFits(*_myValue)) {
                throw Schema::ValueTypeMismatch(std::string("Allowed type = '")+_mySchemaInfo->getTypeName()+
                    "', actual type ='" + _myValue->name()+"'",
                    "dom::Node::checkMyValueType");
        }
        */
    }
    return true;
}

// check that the type of theValue is the same as defined by schema and factory if they exist
bool
dom::Node::checkValueTypeFits(const dom::ValueBase & theValue) const {
    if (!mayHaveValue()) {
        std::string myError = "Node of type ";
        myError += NodeTypeName[_myType];
        myError += " must not have a value";
        throw ValueNotAllowed(myError,PLUS_FILE_LINE);
    }
    if (_mySchemaInfo) {
        if (_mySchemaInfo->_myValueFactory) {
            if (_mySchemaInfo->_myType) {
                const ValuePtr myPrototype = _mySchemaInfo->_myValueFactory->findPrototype(_mySchemaInfo->getTypeName());
                if (myPrototype) {
                    if (myPrototype->getTypeInfo() == theValue.getTypeInfo()) {
                        if (string(myPrototype->name()) != theValue.name()) {
                            throw Schema::InternalError(std::string("type id is the same but type names do not match, prototype typename='") +
                                myPrototype->name()+"', value typename ='"+theValue.name()+"'",
                                "dom::Node::checkValueType");
                        }
                        return true;
                    }
                    return false;
                }
                throw Schema::InternalError(std::string("Node has schemainfo but no prototype.") +
                    " schema type = " + _mySchemaInfo->getTypeName(), "Node::checkValueTypeNode");
            }
            throw Schema::InternalError(std::string("Node has schemainfo but no type"), "Node::checkValueTypeNode");
        }
        throw Schema::InternalError(std::string("Node has schemainfo but no factory"), "Node::checkValueTypeNode");
    }
    return true;
}

dom::Node::SchemaInfo::SchemaInfo()  : _myDepthLevel(0) {
//  _mySchema = NodePtr(new Schema);
}

SchemaPtr
dom::Node::SchemaInfo::getSchema() {
    return dynamic_cast_Ptr<Schema>(_mySchema);
}

const SchemaPtr
dom::Node::SchemaInfo::getSchema() const {
    return dynamic_cast_Ptr<Schema>(_mySchema);
}

const DOMString &
dom::Node::SchemaInfo::getTypeName() const {
    DBS(AC_ERROR<<"dom::Node::SchemaInfo::getTypeName()"<<endl);
    if (_myType) {
        DBS(AC_ERROR<<"dom::Node::SchemaInfo::getTypeName(): _myType = "<< *_myType<<endl);
        NodePtr myTypeNameAttr = _myType->getAttribute(ATTR_NAME);
        if (myTypeNameAttr) {
            DBS(AC_ERROR<<"dom::Node::SchemaInfo::getTypeName(): returning myTypeNameAttr = "<< myTypeNameAttr->nodeValue()<<endl);
            return myTypeNameAttr->nodeValue();
        }
    }
    DBS(AC_ERROR<<"dom::Node::SchemaInfo::getTypeName(): typename not found, returning 'string'"<<endl);
    static DOMString myStringType("string");
    return myStringType;
}

const SchemaPtr
dom::Node::getSchema() const {
    if (_mySchemaInfo) {
        return _mySchemaInfo->getSchema();
    }
    return SchemaPtr(0);
}

#if 1
asl::Unsigned64 
dom::Node::bumpVersion() {
    if (_myParent) {
        _myVersion = _myParent->bumpVersion();
    } else {
        ++_myVersion;
    }
    return _myVersion;
}
#else
// TODO: fix this faster, non recursive version which seems to have a bug
asl::Unsigned64 
dom::Node::bumpVersion() {
    Node * myNode = this;
    asl::Unsigned64 myVersion;
    Node * myParent = 0;
    do {
        myParent = myNode->parentNode();
        if (!myParent) {
            myVersion = myNode->nodeVersion()+1;
        }
        myNode = myParent;
    } while (myParent);
    
    myNode = this;
    myParent = myNode->parentNode();
    while (myParent) {
        myNode->nodeVersion(myVersion);
        myNode = myParent;
        myParent = myParent->parentNode();
    };
    return myVersion;
}
#endif

void
dom::Node::addSchema(const DOMString & theSchemaString, const DOMString & theNSPrefix)  {
    const dom::Document mySchemaDoc(theSchemaString);
    if (!mySchemaDoc) {
        throw SchemaNotParsed(std::string("Could not parse schema '")+theSchemaString+"'","dom::Node::addSchema");
    }
    addSchema(mySchemaDoc, theNSPrefix);
}

void
dom::Node::addSchema(const dom::Node & theSchemaDoc, const DOMString & theNSPrefix)  {
    dom::NodePtr mySchemaRoot = theSchemaDoc.childNode(XS_SCHEMA);
    if (!mySchemaRoot) {
        throw SchemaNotParsed(std::string("No xs:schema element in '")+asl::as_string(theSchemaDoc)
            +"'","dom::Node::addSchema");
    }
    makeSchemaInfo(false);
    //makeSchemaInfo(true);
    if (!_mySchemaInfo->_mySchema) {
        _mySchemaInfo->_mySchema = NodePtr(new Schema);
    }
    dom::NodePtr myNewStuff = _mySchemaInfo->_mySchema->appendChild(mySchemaRoot->cloneNode());
    _mySchemaInfo->getSchema()->preprocess(myNewStuff, theNSPrefix);
}

void
dom::Node::loadSchemas(const DOMString & theURLList)  {
    std::vector<DOMString> myURLs = asl::splitString(theURLList," \t\n\r");

    for (int i = 0; i < myURLs.size();++i) {
        if (myURLs[i].find("urn:") == 0) {
            // ignore urn
        } else {
            std::string mySchemaString = asl::readFile(myURLs[i]); // TODO: use real URL loader here
            if (!mySchemaString.size()) {
                throw LoadFileFailed(std::string("Could not load file '")+myURLs[i]+"'","dom::Node::loadSchemas");
            }
            addSchema(mySchemaString,"");
        }
    }
}

void
dom::Node::checkSchemas() const {
    if (_mySchemaInfo && _mySchemaInfo->_mySchema) {
        _mySchemaInfo->getSchema()->check();
    } else {
        throw Exception("No Schema exists","dom::Node::loadSchemas");
    }
}

const dom::NodePtr
dom::Node::childNodeByAttribute(const DOMString & theElementName,
                                const DOMString & theAttributeName,
                                const DOMString & theAttributeValue,
                                int theIndex) const
{
    for (int i = 0; i < childNodes().length(); ++i) {
        DB(AC_TRACE << "dom::Node::childNodeByAttribute: index="<<i<<endl);
        if (childNode(i)->nodeType() == dom::Node::ELEMENT_NODE) {
            DB(AC_TRACE << "dom::Node::childNodeByAttribute: ELEMENT_NODE"<<endl);
            if (childNode(i)->nodeName() == theElementName) {
                DB(AC_TRACE << "dom::Node::childNodeByAttribute: name match: "<<childNode(i)->nodeName()<<endl);
                NodePtr myAttribute = childNode(i)->getAttribute(theAttributeName);
                DB(AC_TRACE << "dom::Node::childNodeByAttribute: attribute: "<<myAttribute<<endl);
                if (myAttribute && myAttribute->nodeValue() == theAttributeValue) {
                    DB(AC_TRACE << "dom::Node::childNodeByAttribute: attribute value match: "<<myAttribute->nodeValue()<<endl);
                    if (theIndex-- == 0) {
                        DB(AC_TRACE << "dom::Node::childNodeByAttribute: theIndex: "<<theIndex<<endl);
                        return childNode(i);
                    }
                }
            }
        }
    }
    return dom::NodePtr(0);
}

dom::NodePtr
dom::Node::childNodeByAttribute(const DOMString & theElementName,
                                const DOMString & theAttributeName,
                                const DOMString & theAttributeValue,
                                int theIndex)
{
    for (int i = 0; i < childNodes().length(); ++i) {
        DB(AC_TRACE << "dom::Node::childNodeByAttribute: index="<<i<<endl);
        if (childNode(i)->nodeType() == dom::Node::ELEMENT_NODE) {
            DB(AC_TRACE << "dom::Node::childNodeByAttribute: ELEMENT_NODE"<<endl);
            if (childNode(i)->nodeName() == theElementName) {
                DB(AC_TRACE << "dom::Node::childNodeByAttribute: name match: "<<childNode(i)->nodeName()<<endl);
                NodePtr myAttribute = childNode(i)->getAttribute(theAttributeName);
                DB(AC_TRACE << "dom::Node::childNodeByAttribute: attribute: "<<myAttribute<<endl);
                if (myAttribute && myAttribute->nodeValue() == theAttributeValue) {
                    DB(AC_TRACE << "dom::Node::childNodeByAttribute: attribute value match: "<<myAttribute->nodeValue()<<endl);
                    if (theIndex-- == 0) {
                        DB(AC_TRACE << "dom::Node::childNodeByAttribute: theIndex: "<<theIndex<<endl);
                        return childNode(i);
                    }
                }
            }
        }
    }
    return dom::NodePtr(0);
}

void
Node::getNodesByAttribute(const DOMString & theElementName,
                          const DOMString & theAttributeName,
                          const DOMString & theAttributeValue,
                          std::vector<NodePtr> & theResults) const
{
    for (int i = 0; i < childNodes().length(); ++i) {
        const NodePtr myChild = childNode(i);
        if (myChild->nodeType() == dom::Node::ELEMENT_NODE) {
            const NodePtr myAttribute      = myChild->getAttribute(theAttributeName);
            const DOMString & myElementName  = myChild->nodeName();
            if (myAttribute && myElementName == theElementName) {
                const DOMString & myElementValue = myAttribute->nodeValue();
                if (theAttributeValue == "" || myElementValue == theAttributeValue) {
                    theResults.push_back(myChild);
                }
            }
            myChild->getNodesByAttribute(theElementName, theAttributeName,
                                         theAttributeValue, theResults );
        }
    }
}

const NodePtr
Node::getChildElementById(const DOMString & theId, const DOMString & theIdAttribute) const {
    for (int i = 0; i < childNodes().length(); ++i) {
        const NodePtr myChild = childNode(i);
        if (myChild->nodeType() == dom::Node::ELEMENT_NODE) {
            const NodePtr myIdAttribute = myChild->getAttribute(theIdAttribute);
            if (myIdAttribute) {
                if (myIdAttribute->nodeValue() == theId) {
                    return myChild;
                }
            }

            const NodePtr myResult = myChild->getChildElementById(theId, theIdAttribute);
            if (myResult) {
                return myResult;
            }
        }
    }
    return NodePtr(0);
}

NodePtr
Node::getChildElementById(const DOMString & theId, const DOMString & theIdAttribute) {
    for (int i = 0; i < childNodes().length(); ++i) {
        const NodePtr myChild = childNode(i);
        if (myChild->nodeType() == dom::Node::ELEMENT_NODE) {
            const NodePtr myIdAttribute = myChild->getAttribute(theIdAttribute);
            if (myIdAttribute) {
                if (myIdAttribute->nodeValue() == theId) {
                    return myChild;
                }
            }

            const NodePtr myResult = myChild->getChildElementById(theId, theIdAttribute);
            if (myResult) {
                return myResult;
            }
        }
    }
    return NodePtr(0);
}

const NodePtr
Node::getElementById(const DOMString & theId, const DOMString & theIdAttribute) const {
    if (_mySchemaInfo) {
        const NodeIDRegistryPtr myIDRegistry = getIDRegistry();
        return myIDRegistry->getElementById(theId, theIdAttribute);
    }

    const Node * myNode = this;
    while (myNode->parentNode()) {
        myNode = myNode->parentNode();
    }

    return myNode->getChildElementById(theId, theIdAttribute);
}

NodePtr
Node::getElementById(const DOMString & theId, const DOMString & theIdAttribute) {
    if (_mySchemaInfo) {
        NodeIDRegistryPtr myIDRegistry = getIDRegistry();
        return myIDRegistry->getElementById(theId, theIdAttribute);
    }
    Node * myNode = this;
    while (myNode->parentNode()) {
        myNode = myNode->parentNode();
    }

    return myNode->getChildElementById(theId, theIdAttribute);
}

// create a node by type;name is set automatically if appropriate
dom::Node::Node(NodeType type, Node * theParent) :
    _myType(type),
    _myParseCompletionPos(0),
    _myDocSize(0),
    _myParent(theParent),
    _myChildren(this),
    _myAttributes(this),
    _myFacade(0),
    _myVersion(0)
{
    switch (type) {
    case DOCUMENT_NODE:
    case DOCUMENT_FRAGMENT_NODE:
        _myName = getNameOfType(type);
        break;
    case TEXT_NODE:
    case COMMENT_NODE:
    case CDATA_SECTION_NODE:
        _myName = getNameOfType(type);
       break;
    default:
        throw asl::Exception(
            "this contructor must not be called for this node type");
        break;
    }
}

// create text, comment, cdata  or element node w/o attributes
dom::Node::Node(NodeType type, const String & name_or_value, Node * theParent) :
    _myType(type),
    _myParseCompletionPos(0),
    _myDocSize(0),
    _myChildren(this),
    _myAttributes(this),
    _myParent(theParent),
    _myFacade(0),
    _myVersion(0)
{
    switch (type) {
        case TEXT_NODE:
        case COMMENT_NODE:
        case CDATA_SECTION_NODE:
            _myName = getNameOfType(type);
            nodeValue(name_or_value);
            break;
        case ELEMENT_NODE:
        case ENTITY_REFERENCE_NODE:
        case DOCUMENT_TYPE_NODE:
            _myName = name_or_value;
            break;
        default:
            throw asl::Exception(
                "this contructor must not be called for this node type");
            break;
    }
}
/// makes a deep copy
dom::Node::Node(const Node & n, Node * theParent) :
    _myType(n._myType),
    _myName(n._myName),
    _myValue(n._myValue ? n._myValue->clone(this) : n._myValue),
    _myParseCompletionPos(n._myParseCompletionPos),
    _myDocSize(n._myDocSize),
    _myChildren(this),
    _myAttributes(this),
    _myParent(theParent),
    _myFacade(0),
    _mySchemaInfo(n._mySchemaInfo),
    _myVersion(0)
{
    if (_myValue) {
        _myValue->update();
    }
    for (int child = 0; child < n._myChildren.size(); ++child) {
        _myChildren.append(n._myChildren.item(child)->cloneNode(DEEP,this));
    }
    for (int attr = 0; attr < n._myAttributes.size(); ++attr) {
        _myAttributes.append(n._myAttributes.item(attr)->cloneNode(DEEP,this));
    }
}
/// makes a deep copy
dom::Node::Node(const Node & n) :
    _myType(n._myType),
    _myName(n._myName),
    _myValue(n._myValue ? n._myValue->clone(this) : n._myValue),
    _myParseCompletionPos(n._myParseCompletionPos),
    _myDocSize(n._myDocSize),
    _myChildren(this),
    _myAttributes(this),
    _myParent(0),
    _myFacade(0),
    _mySchemaInfo(n._mySchemaInfo),
    _myVersion(0)
{
    for (int attr = 0; attr < n._myAttributes.size(); ++attr) {
        _myAttributes.append(n._myAttributes.item(attr)->cloneNode(DEEP,this));
    }
    for (int child = 0; child < n._myChildren.size(); ++child) {
        _myChildren.append(n._myChildren.item(child)->cloneNode(DEEP,this));
    }
}

/// makes a deep copy
Node &
dom::Node::operator=(const Node & n) {

    _myName.resize(0);
    //_myParent = 0;
    //_mySelf = NodePtr(0);
    _myIDRegistry = NodeIDRegistryPtr(0);
    _myValue = ValuePtr(0);
    _myChildren.resize(0);
    _myAttributes.resize(0);
    _mySchemaInfo=SchemaInfoPtr(0);
    _myFacade=FacadePtr(0);
    _myEventListeners=EventListenerMap();
    _myCapturingEventListeners=EventListenerMap();
    ::new(this) Node(n, _myParent);
    return *this;
}

/// returns a shallow copy (type, name, value, attributes) or
/// a deep copy (type, name, value, attributes, children)
/// Note: Shallow copy does a deep copy on the attributes
NodePtr
dom::Node::cloneNode(CloneDepth depth, Node * theParent) const {
    if (depth == DEEP) {
        NodePtr myResult(new Node(*this, theParent));
        //myResult->_myParent = theParent;
        return myResult;
    }
    ValuePtr myCloneValue = _myValue ? _myValue->clone(0) : ValuePtr(0);
    NodePtr result(new Node(_myType,_myName,myCloneValue, theParent));
    result->_mySchemaInfo = _mySchemaInfo;
    //result->_myParent = theParent;
    if (myCloneValue) {
        myCloneValue->setNodePtr(&(*result));
    }

    for (int attr = 0; attr < _myAttributes.size(); ++attr) {
        result->appendAttribute(_myAttributes.item(attr)->cloneNode(DEEP,&(*result)));
    }
    return result;
}

/// set node name, must start with alpha, '_' or ':'
/// and continue with alpha, digit, '_' , '-' or ':'
void
dom::Node::setName(const String& name) {
    if (read_name(name,0)!=name.size())
        throw DomException(JUST_FILE_LINE,DomException::INVALID_NAME_ERR);
    _myName = name;
}

std::ostream&
dom::Node::print(std::ostream& os, const String& indent) const {
    switch (_myType) {
        case ELEMENT_NODE:
            {
                os << indent << "<" << _myName;
                for (int attr = 0; attr < _myAttributes.size();++attr) {
                    os << " " << _myAttributes.item(attr)->nodeName() << "=";

                    const NodePtr myAttribute = _myAttributes.item(attr);
                    DOMString myAttributeValue;
                    unsigned int myChildCount = myAttribute->childNodes().length();
                    if (myChildCount == 0) {
                        myAttributeValue = myAttribute->nodeValue();
                    }
                    bool apos = myAttributeValue.find("\'") != String::npos;
                    bool qout = myAttributeValue.find("\"") != String::npos;

                    char myQuote = '"';
                    if (qout && !apos) {
                        myQuote='\'';
                    }
                    os << myQuote << entity_encode_data(myAttributeValue,myQuote);

                    for (int i = 0; i<myChildCount;++i) {
                        if (myAttribute->childNode(i)->nodeType() == dom::Node::TEXT_NODE) {
                            os << entity_encode_data(myAttribute->childNode(i)->nodeValue(),myQuote);
                        } else {
                            if (myAttribute->childNode(i)->nodeType() == dom::Node::ENTITY_REFERENCE_NODE) {
                                os << "&" << myAttribute->childNode(i)->nodeName() << ";";
                            } else {
                                throw DomException("Illegal attribute child"
                                    ,PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
                            }
                        }
                    }
                    os << myQuote;
                }
                if (_myChildren.size()==0)
                    os << "/>" << std::endl;
                else {
                    os << ">";
                    if (_myChildren.size()>1 || _myChildren[0]._myType==ELEMENT_NODE)
                        os << std::endl;
                }
            }
            break;
        case TEXT_NODE:
            os << indent;
            os << entity_encode_data(nodeValue(),'"');
            if (indent.size()) os << std::endl;
            break;
        case CDATA_SECTION_NODE:
            os << indent << "<![CDATA[" << nodeValue() << "]]>" << std::endl;
            break;
        case PROCESSING_INSTRUCTION_NODE:
            os << indent << "<?" << nodeName()  << " " << nodeValue() << "?>" << std::endl;
            break;
        case COMMENT_NODE:
            os << indent << "<!--" << nodeValue() << "-->" << std::endl;
            break;
        case DOCUMENT_NODE:
            break;
        case DOCUMENT_TYPE_NODE:
            os << indent << "<!DOCTYPE " << nodeName();
            if (_myChildren.size()>0) {
                os << " ["  << endl;
            }
            break;
        case ENTITY_REFERENCE_NODE:
            os << "&" << nodeName() << ";";
            break;
        case ENTITY_NODE:
            os << indent << "<!ENTITY " << nodeName()  << " \"" << entity_encode_data(nodeValue(),'"') << "\""<< ">"
                << std::endl;
            break;
        default:
            os << indent << "<!-- unserializeable node type " << nodeType()
                << ",name = " << nodeName() <<
                ",val = " << (mayHaveValue() ? nodeValue() : DOMString()) << "-->" << std::endl;
    }

    for (int child = 0; child < _myChildren.size();++child) {
        if (nodeType() == DOCUMENT_NODE ||
                (_myChildren.size() == 1 &&
                    _myChildren[0]._myType != ELEMENT_NODE &&
                    nodeType() != DOCUMENT_TYPE_NODE) )
        {
            _myChildren.item(child)->print(os,"");
        }
        else {
            _myChildren.item(child)->print(os,indent + "    ");
        }
    }

    switch (nodeType()) {
        case ELEMENT_NODE:
            if (_myChildren.size()>0) {
                if (_myChildren.size() > 1 ||
                        _myChildren[0]._myType == ELEMENT_NODE)
                    os << indent;
                os << "</" << nodeName() << ">" << std::endl;
            }
            break;
        case DOCUMENT_TYPE_NODE:
            if (_myChildren.size()>0) {
                os << "]";
            }
            os << ">" << endl;
            break;
        default:
            break;
    }
    return os;
}

// This function is called on an empty document node and starts to build a dom
// with the current node as root element
int
dom::Node::parseAll(const String& is) {
    int offset = 0;
    //JB: ignore UTF-8 byte order mark (0xEFBBBF)
    if (is[0]=='\xEF' && is[1]=='\xBB' && is[2]=='\xBF') {
        AC_DEBUG << "Skipping UTF-8 byte order mark signatur.";
        offset = 3;
    }

    _myDocSize = is.size() - offset;
    try {
        if (_myType == X_NO_NODE)
            _myType = DOCUMENT_NODE;

        _myName = getNameOfType(_myType);
        int pos = offset;
        int completed_pos = pos;
        Node * doctype = 0;
        do {
            // add every node as child
            pos = completed_pos;
            NodePtr new_child(new Node(is,pos,this,doctype));
            completed_pos = new_child->parsedUntil();

            if (new_child) {
                if ((new_child->nodeType() != X_END_NODE) &&
                    (new_child->nodeType() != X_NO_NODE)) {
                    _myChildren.appendWhileParsing(new_child);
                }
                if (new_child->nodeType() == DOCUMENT_TYPE_NODE) {
                    doctype = &(*new_child);
                }
            }
        } while (completed_pos > pos);
        // finish eventually trailing whitespace
        _myParseCompletionPos = read_whitespace(is,pos);
    }

    catch (ParseException & pex) {
        _myParseCompletionPos = pex.parsedUntil();

        DBE(AC_ERROR << "parsing aborted at character position "
            << _myParseCompletionPos << std::endl);

        DBE(int myDumpStartPos = asl::maximum(0, _myParseCompletionPos - 10000));
        DBE(AC_ERROR << is.substr(myDumpStartPos,_myParseCompletionPos) << "#### " << std::endl);
        DBE(AC_ERROR << pex.name() << "," << pex.what() << std::endl);
        DBE(AC_ERROR << " bailed out in " << pex.where() << std::endl);

        if (_myParseCompletionPos == (is.size() - offset)) {
            --_myParseCompletionPos;
        }
    }
    setVersion(1);
    return _myParseCompletionPos;
}

void Node::parseFile(const std::string & theFileName) {
    DOMString myXmlString;
    if (asl::readFile(asl::expandEnvironment(theFileName), myXmlString)) {
        parseAll(myXmlString);
    } else {
        *this = Node();
    }
}


DEFINE_EXCEPTION(SchemaInfoWithoutSchema, asl::Exception);
DEFINE_EXCEPTION(SchemaInfoWithoutFactory, asl::Exception);

void Node::makeSchemaInfo(bool forceNew) {
    DB(AC_TRACE << "makeSchemaInfo("<<forceNew<<"),type = "<< NodeTypeName[nodeType()] << endl);
    if (!_mySchemaInfo || forceNew) {
        DB(AC_TRACE << "making new SchemaInfo for node type "<< NodeTypeName[nodeType()] << endl);
        _mySchemaInfo = SchemaInfoPtr(new SchemaInfo);
        if (_myParent) {
            DB(AC_TRACE << "parent exists" << endl);
            if (_myParent->_mySchemaInfo) {
                DB(AC_TRACE << "parent has SchemaInfo, initializing SchemaInfo to Schema "<< (void*)&(*_myParent->_mySchemaInfo->_mySchema)
                    << ", level=" << _mySchemaInfo->_myDepthLevel+1<<" , factory = " << (void*)&(*_myParent->_mySchemaInfo->_myValueFactory)<< endl);
                if (!_myParent->_mySchemaInfo->_mySchema) {
                    string myError = string(" parent node name='")+
                        _myParent->nodeName()+"', my node name = '"+nodeName()+"'";
                    throw SchemaInfoWithoutSchema(myError,"Node::makeSchemaInfo");
                }
                if (!_myParent->_mySchemaInfo->_myValueFactory) {
                    string myError = string("Node::makeSchemaInfo: parent node name='")+
                        _myParent->nodeName()+"', my node name = '"+nodeName()+"'";
                    throw SchemaInfoWithoutFactory(myError,"Node::makeSchemaInfo");
                }
                _mySchemaInfo->_myDepthLevel = _myParent->_mySchemaInfo->_myDepthLevel+1;
                _mySchemaInfo->_mySchema = _myParent->_mySchemaInfo->_mySchema;
                _mySchemaInfo->_myValueFactory = _myParent->_mySchemaInfo->_myValueFactory;
                _mySchemaInfo->_myFacadeFactory = _myParent->_mySchemaInfo->_myFacadeFactory;
            }
        }
    }
}

Node::~Node() {}

void
Node::checkSchemaForText(asl::AC_SIZE_TYPE theParsePos) {
    DB(AC_TRACE<<"checkSchemaForText()"<<endl);
    if (!_myParent  || !_myParent->_mySchemaInfo) {
        DB(AC_TRACE<<"checkSchemaForText(): give up, no schema is present"<<endl);
        return; // give up if no schema is present
    }
    makeSchemaInfo(false);
    if (!_mySchemaInfo->_mySchema) {
        DB(AC_TRACE<<"checkSchemaForText(): give up, still no schema is present"<<endl);
        return; // give up if still no schema is present
    }
    // we have a schema, and our parent element determines our type
    if (_myParent->_mySchemaInfo->_myType) {
        NodePtr myParentType = _myParent->_mySchemaInfo->_myType;
        DB(AC_TRACE<<"checkSchemaForText(): myParentType="<<*myParentType<<endl);
        NodePtr myTypeNameAttr = myParentType->getAttribute(ATTR_NAME);
        if (myTypeNameAttr) {
            _mySchemaInfo->_myType = _myParent->_mySchemaInfo->_myType;
            DBS(AC_ERROR<<"dom::Node::checkSchemaForText(): found type with name = "<< myTypeNameAttr->nodeValue()<<endl);
            return;
        }
        while (true) {
            NodePtr mySimpleContent = myParentType->childNode(XS_SIMPLECONTENT);
            if (mySimpleContent) {
                DBS(AC_ERROR<<"dom::Node::checkSchemaForText(): mySimpleContent = "<< *mySimpleContent<<endl);
                NodePtr myExtension = mySimpleContent->childNode(XS_EXTENSION);
                if (myExtension) {
                    DBS(AC_ERROR<<"dom::Node::SchemaInfo::getTypeName(): myExtension = "<< *myExtension<<endl);
                    NodePtr myBase = myExtension->getAttribute(ATTR_BASE);
                    if (myBase) {
                        DBS(AC_ERROR<<"dom::Node::checkSchemaForText(): base type has name  "<< myBase->nodeValue()<<endl);
                        NodePtr myType = _mySchemaInfo->getSchema()->findTopLevelTypeDeclaration(myBase->nodeValue());
                        if (myType) {
                            if (myType->nodeName() == XS_SIMPLETYPE) {
                                _mySchemaInfo->_myType = myType;
                                return;
                            }
                            // check next level
                            myParentType = myType;
                            continue;
                         } else {
                            // we did'nt find a declaration yet, so lets look if it is a builtin type
                            myType = Schema::getBuiltinSchema().findTopLevelTypeDeclaration(myBase->nodeValue());
                            if (myType) {
                               _mySchemaInfo->_myType = myType;
                                return;
                            }
                            // check if complex type has a extension base type
                            throw Schema::TextWithUndeclaredType(
                                string("Could not find declaration for text value type = ")+
                                myBase->nodeValue(),
                                "Node::checkSchemaForText",theParsePos);
                        }
                    }
                }
            }
            break;
        };
        // we could not find a declaration for our type
        throw Schema::TextNotAllowed(
            string("Could not find text value type in ")+
            asl::as_string(*_myParent->_mySchemaInfo->_myType)+
            ", parent = "+asl::as_string(*_myParent),
            "Node::checkSchemaForText", theParsePos);
    }
    throw Schema::InternalError("Text parent has no Schema declaration","Node::checkSchemaForText",theParsePos);
}

// This function checks if our type declaration contains just one allowed child element type
// type so we can treat ourself as an array type

void
Node::checkForArrayType(NodePtr mySchemaType, asl::AC_SIZE_TYPE theParsePos) {
    if (!mySchemaType || _myType!=ELEMENT_NODE) {
        return;
    }
    if (mySchemaType->nodeName() != XS_COMPLEXTYPE) {
        return;
    }
    NodePtr myComplexContent = mySchemaType->childNode(XS_COMPLEXCONTENT);
    NodePtr mySequence;
    NodePtr myExtension;
    if (myComplexContent) {
        mySequence = myComplexContent->childNode(XS_SEQUENCE);
        myExtension = myComplexContent->childNode(XS_EXTENSION);
    } else {
        mySequence = mySchemaType->childNode(XS_SEQUENCE);
    }
    if (!mySequence) {
        return;
    }
    int myElementChilds = mySequence->childNodesLength(XS_ELEMENT);
    if (myElementChilds != 1) {
        return;
    }
    NodePtr myElementDecl = mySequence->childNode(XS_ELEMENT);
    NodePtr myNameAttr = myElementDecl->getAttribute(ATTR_NAME);

    if (myNameAttr) {
        if (_mySchemaInfo->getSchema()->substitutionsGroupExists(myNameAttr->nodeValue())) {
            return;
        }
    } else {
        NodePtr myRefAttr = myElementDecl->getAttribute(ATTR_REF);
        if (!myRefAttr) {
            throw Schema::MalformedDeclaration(string("Element Declaration musr have either 'name' or 'ref' attribute, node = ")
                +asl::as_string(*myElementDecl),
                "Node::checkForArrayType", theParsePos);
        }
        if (_mySchemaInfo->getSchema()->substitutionsGroupExists(myRefAttr->nodeValue())) {
            return;
        }
        NodePtr myReferencedElementDecl = _mySchemaInfo->getSchema()->findTopLevelElementDeclaration(myRefAttr->nodeValue());
        if (!myReferencedElementDecl) {
            throw Schema::CantResolveReference(asl::as_string(*myElementDecl), "Node::checkForArrayType", theParsePos);
        }
        myElementDecl = myReferencedElementDecl;
    }
    // we have now the only allowed child element declaration in myElementDecl

    NodePtr myElemType = _mySchemaInfo->getSchema()->findType(myElementDecl, theParsePos);
    if (!myElemType) {
        throw Schema::ElementWithoutType(asl::as_string(*myElementDecl), "Node::checkForArrayType", theParsePos);
    }
    NodePtr myTypeName = myElemType->getAttribute(ATTR_NAME);
    if (!myTypeName) {
         throw Schema::TypeWithoutName(asl::as_string(*myElemType), "Node::checkForArrayType", theParsePos);
    }
    // now we now our element type; if there is a factory
    ValuePtr myValuePrototype = _mySchemaInfo->_myValueFactory->findPrototype(myTypeName->nodeValue());
    if (myValuePrototype) {
    //TODO: check what really needs to be done
        //_mySchemaInfo->_myArrayValuePrototype = myValuePrototype;
    }
}

NodePtr
Node::checkSchemaForElement(const DOMString & theName, asl::AC_SIZE_TYPE theParsePos) {
    // first of all, don't try to create a SchemaInfo if our parent has none
    if (_myParent) {
        if (!_myParent->_mySchemaInfo) {
            return NodePtr(0); // give up if no schema is present
        }
    }
    makeSchemaInfo(false);
    if (!_mySchemaInfo->_mySchema) {
        return NodePtr(0); // give up if still no schema is present
    }

    DB(AC_TRACE << "Node::checkSchemaForElement: theName = " << theName << endl;
       AC_TRACE << "_mySchemaInfo->_myDepthLevel = " << _mySchemaInfo->_myDepthLevel << endl;)

    // we have a schema, so let us locate our element
    if (_mySchemaInfo->_myDepthLevel > 1) {
        // we are on a deeper level, so lets look for our declaration at the children of our parent
        //if (!_mySchemaInfo->_mySchemaSearchRoot) {
        if (!_myParent->_mySchemaInfo->_mySchemaDeclaration) {
            // we should never get here
            throw Schema::InternalError(
                string("Can't find parent declaration for element '")+theName+"'",
                "Node::checkSchemaForElement", theParsePos);
        }
        if (!_myParent->_mySchemaInfo->_myType) {
            throw Schema::InternalError(string("Can't find parent type of element '")+theName+"'",
                "Node::checkSchemaForElement", theParsePos);
        }
        _mySchemaInfo->_mySchemaDeclaration = _mySchemaInfo->getSchema()->findElementDeclaration(
            _myParent->nodeName(),
            _myParent->_mySchemaInfo->_myType,
            theName,
            theParsePos);
    } else {
        _mySchemaInfo->_mySchemaDeclaration = _mySchemaInfo->getSchema()->findTopLevelElementDeclaration(theName);
    }

    DB(AC_TRACE << "_mySchemaInfo->_mySchemaDeclaration = " << (_mySchemaInfo->_mySchemaDeclaration ? asl::as_string(*_mySchemaInfo->_mySchemaDeclaration) : string("null")) << endl;);

    // if we dont have a declaration now, we are sorry
    if (!_mySchemaInfo->_mySchemaDeclaration) {
        throw Schema::UndeclaredElement(theName,"Node::checkSchemaForElement", theParsePos);
    }

    // now that we have our element declaration, let us determine our type
    _mySchemaInfo->_myType = _mySchemaInfo->getSchema()->findType(_mySchemaInfo->_mySchemaDeclaration, theParsePos);
    return _mySchemaInfo->_myType;
}
int
dom::Node::parseNextNode(const String & is, int pos, const Node * parent, const Node * doctype) {
    _myDocSize = is.size();
    int nw_pos;
    while ((nw_pos = read_whitespace(is,pos)) < is.size()) {

        // check for comment
        String myValueString;
        int e_end_pos = read_comment(is,nw_pos,myValueString);
        if (e_end_pos > nw_pos) {
            // is comment
            _myType = COMMENT_NODE;
            nodeValue(myValueString);
            _myName = getNameOfType(_myType);
            _myDocSize = _myParseCompletionPos = e_end_pos;
            // comment node ready
            return _myParseCompletionPos;
        }

        // check for doctype node
        String pubid;
        int dt_child_pos = read_document_type_start(is,nw_pos,_myName, pubid, myValueString);
        if (dt_child_pos > nw_pos) {
            // is document_type
            _myType = DOCUMENT_TYPE_NODE;
            nodeValue(myValueString);
            // id's ready, check for child nodes now
            int completed_pos = read_whitespace(is,dt_child_pos);
            if (is[completed_pos] == '[') {
                ++completed_pos;
                int child_bgn_pos = completed_pos;
                do {
                    DB(AC_DEBUG <<"dom::parse_next_node: DOCUMENT_TYPE_NODE completed_pos = "
                        << completed_pos);
                    child_bgn_pos = completed_pos;
                    NodePtr new_child(new Node(is,child_bgn_pos,this,this));
                    completed_pos = new_child->parsedUntil();

                    DB(AC_DEBUG <<"dom::parse_next_node: DOCUMENT_TYPE_NODE new_child->parsedUntil() = "
                        << completed_pos);
                    DB(AC_DEBUG <<is.substr(0,completed_pos) << "}}}");
                    DB(AC_DEBUG <<"dom::parse_next_node: DOCUMENT_TYPE_NODE new_child->nodeType() = "
                        << NodeTypeName[new_child->nodeType()]);

                    if (new_child && completed_pos > child_bgn_pos) {
                        if (new_child->nodeType() != X_NO_NODE) {
                            DB(AC_DEBUG <<"dom::parse_next_node: DOCUMENT_TYPE_NODE added child "
                                << NodeTypeName[new_child->nodeType()]);
                            _myChildren.appendWhileParsing(new_child);
                        }
                    }
                    completed_pos = read_whitespace(is, completed_pos);
                    DB(AC_DEBUG <<"dom::parse_next_node: DOCUMENT_TYPE_NODE checking for ] at pos "
                        << completed_pos);
                    if (is[completed_pos] == ']') {
                        // force loop end condition
                        child_bgn_pos = completed_pos = completed_pos + 1;
                    } else {
                        if (!new_child) {
                            throw ParseException("<!DOCTYPE declaration with missing ']' or invalid node",
                                PLUS_FILE_LINE,completed_pos);
                        }
                    }
                } while (completed_pos > child_bgn_pos);
            }
            completed_pos = read_whitespace(is, completed_pos);
            if (is[completed_pos] == '>') {
                _myDocSize = _myParseCompletionPos = completed_pos + 1;
                return _myParseCompletionPos;
            }
            throw ParseException("<!DOCTYPE declaration with missing '>'",
                PLUS_FILE_LINE,completed_pos);
        }

        // check for entity declaration
        bool isParsedEntity;
        String PubidLiteral;
        String SystemLiteral;
        String NDataName;
        e_end_pos = read_entity_declaration(is,nw_pos,_myName,myValueString, isParsedEntity,
            PubidLiteral, SystemLiteral, NDataName, doctype);
        if (e_end_pos > nw_pos) {
            // is cdata
            _myType = ENTITY_NODE;
            nodeValue(myValueString);
            _myDocSize = _myParseCompletionPos = e_end_pos;
            // entity node ready
            return _myParseCompletionPos;
        }

        // check for cdata
        e_end_pos = read_cdata(is,nw_pos,myValueString);
        if (e_end_pos > nw_pos) {
            // is cdata
            _myType = CDATA_SECTION_NODE;
            _myName = getNameOfType(_myType);
            nodeValue(myValueString);
            _myDocSize = _myParseCompletionPos = e_end_pos;
            // cdata node ready
            return _myParseCompletionPos;
        }

        // check for processing_instruction
        e_end_pos = read_processing_instruction(is,nw_pos,_myName,myValueString);
        if (e_end_pos > nw_pos) {
            // is processing_instruction
            _myType = PROCESSING_INSTRUCTION_NODE;
            nodeValue(myValueString);
            _myDocSize = _myParseCompletionPos = e_end_pos;
            // processing_instruction node ready
            return _myParseCompletionPos;
        }

        // check for empty tag (<tag/> or <tag param="value"/>)
        //String params;
        int params_begin = 0;
        int params_end = 0;
        int tag_end_pos = read_empty_tag(is,nw_pos,_myName,params_begin,params_end);
        if (tag_end_pos > nw_pos) {
            // is empty tag
            _myType = ELEMENT_NODE;
            parseSystemAttributes(is,params_begin,params_end,doctype);
            checkSchemaForElement(_myName,tag_end_pos);
            //checkForArrayType(tag_end_pos);
            if (params_begin != params_end) {
                if (parseAttributes(is,params_begin,params_end,doctype) == params_begin) {
                    throw ParseException("element attribute problem",
                        PLUS_FILE_LINE,tag_end_pos);
                }
            }
            _myDocSize = _myParseCompletionPos = tag_end_pos;
            // empty tag element node ready
            return _myParseCompletionPos;
        }

        // check for end tag (</tag>)
        String end_tag_name;
        tag_end_pos = read_end_tag(is,nw_pos,_myName);
        if (tag_end_pos > nw_pos) {
            if (!parent || _myName != parent->nodeName()) {
                String error_msg;
                error_msg = String("unexpected end tag name'") + _myName ;
                if (parent)
                    error_msg += "', expected '" + parent->nodeName() + "'";
                throw ParseException(error_msg,
                    PLUS_FILE_LINE,tag_end_pos);
            }
            _myType = X_END_NODE;
            _myDocSize = _myParseCompletionPos = tag_end_pos;
            // empty tag element node ready
            return _myParseCompletionPos;
        }

        // check for start tag (<tag> or <tag param="value">)
        params_begin = 0;
        params_end = 0;
        tag_end_pos = read_start_tag(is,nw_pos,_myName,params_begin,params_end);
        if (tag_end_pos > nw_pos) {
            // start tag of element
            _myType = ELEMENT_NODE;
            parseSystemAttributes(is,params_begin,params_end,doctype);
            checkSchemaForElement(_myName,tag_end_pos);
            // check for params
            if (params_begin != params_end) {
                if (parseAttributes(is,params_begin,params_end,doctype) == params_begin) {
                    throw ParseException("element attribute problem",
                        PLUS_FILE_LINE,tag_end_pos);
                }
            }
            // params ready, check for child nodes now
            int completed_pos = tag_end_pos;
            do {
                tag_end_pos = completed_pos;
                NodePtr new_child(new Node(is,tag_end_pos,this,doctype));
                completed_pos = new_child->parsedUntil();

                if (new_child && completed_pos > tag_end_pos) {
                    if ((new_child->nodeType() != X_END_NODE) &&
                        (new_child->nodeType() != X_NO_NODE)) {
                            _myChildren.appendWhileParsing(new_child);
                        } else {
                            if (new_child->nodeName() == nodeName()) {
                                tag_end_pos = completed_pos;
                            }
                        }
                }
            } while (completed_pos > tag_end_pos);
            _myDocSize = _myParseCompletionPos = completed_pos;
            // start tag element node & all children ready
            return _myParseCompletionPos;
        }

        // check for some text before the next markup starts
        // int text_start_pos = read_whitespace(is,pos);
        int text_end_pos = read_text(is,nw_pos);
        if (text_end_pos > nw_pos) {
            if (text_end_pos == is.size()) {
                throw ParseException("trailing text outside of element node",
                    PLUS_FILE_LINE,pos);
            }
            _myType = TEXT_NODE;
            _myName = getNameOfType(_myType);
            checkSchemaForText(text_end_pos);
            try {
                nodeValue(entity_decode_data(is.substr(nw_pos,text_end_pos - nw_pos),nw_pos,doctype));
            }
            catch (ParseException & ex) {
                ex.setParsedUntil(text_end_pos);
                throw;
            }
            _myDocSize = _myParseCompletionPos = text_end_pos;
            return _myParseCompletionPos;
        }

        throw ParseException("no legal node found, maybe missing end",
            PLUS_FILE_LINE,pos);
    }

    if (pos == is.size()) {
        if (parent &&
            parent->nodeType() != Node::DOCUMENT_NODE &&
            parent->nodeType() != Node::DOCUMENT_FRAGMENT_NODE)
        {
            String error_msg;
            error_msg = String("unexpected end of file, missing closing tag for node '") + parent->nodeName() + "'";
            throw ParseException(error_msg, PLUS_FILE_LINE, pos);
        }
    }

    _myDocSize = _myParseCompletionPos = pos;
    return _myParseCompletionPos;
}

/// returns reference to attribute node with the given name
/// if no attribute with the given name exists, a new attribute
/// node with this name is created. if the node is not an attribute node,
/// an exception is thrown

Node &
dom::Node::operator[](const String & name) {
    if (_myType != ELEMENT_NODE)
        throw DomException(JUST_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);

    NodePtr result_ptr = getAttribute(name);
    if (result_ptr)
        return *result_ptr;

    return *appendAttribute(name);
}

const Node&
dom::Node::operator[](const String& name) const {
    if (nodeType() == X_NO_NODE)
        return X_NO_NODE_;

    NodePtr result_ptr = getAttribute(name);
    if (result_ptr)
        return *result_ptr;

    return X_NO_NODE_;
}

NodePtr
dom::Node::appendAttribute(NodePtr theNewAttribute) {
    checkName(theNewAttribute->nodeName(),theNewAttribute->nodeType());
    if (attributesLength(theNewAttribute->nodeName())) {
        throw DomException(string("Can't append attribute name " +
                    theNewAttribute->nodeName() +
                    " to node type ") +
                NodeTypeName[nodeType()]+
                ", name = " + nodeName() +
                " because attribute with this name already exists.",
                PLUS_FILE_LINE,DomException::INUSE_ATTRIBUTE_ERR);
    }
    if (_myType != ELEMENT_NODE) {
        throw DomException(string("Can't append attribute name '" +
                    theNewAttribute->nodeName() +
                    "' to node type ") +
                NodeTypeName[nodeType()]+
                ", name = " + nodeName(),
                PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
    }
    if (theNewAttribute->nodeType()!=ATTRIBUTE_NODE) {
        throw DomException(string("Can't append node of type '") +
                NodeTypeName[theNewAttribute->nodeType()] +
                "', name='" + theNewAttribute->nodeName() +
                "' as attribute to node type '" +
                NodeTypeName[nodeType()]+
                "', name='" + nodeName() + "'",
                PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
    }
    checkAndUpdateAttributeSchemaInfo(*theNewAttribute, this);
    _myAttributes.append(theNewAttribute);
    return theNewAttribute;
}

NodePtr
dom::Node::replaceChild(NodePtr theNewChild, NodePtr theOldChild) {
    int myOldIndex = _myChildren.findIndex(&(*theOldChild));
    if (myOldIndex < 0) {
        return NodePtr(0);
    }
    checkName(theNewChild->nodeName(), theNewChild->nodeType());
    if (theNewChild->nodeType() == DOCUMENT_FRAGMENT_NODE) {
        /*
        for (int i = 0; i < theNewChild->childNodesLength();++i) {
            appendChild(theNewChild->childNode(i));
        }
        */
        return NodePtr(0);
    }
    if (!allowedToHaveChild(theNewChild->nodeType())) {
        string errorMessage;
        errorMessage = "Replace a node with child of type ";
        errorMessage += NodeTypeName[theNewChild->nodeType()];
        errorMessage += " with name '";
        errorMessage += theNewChild->nodeName();
        errorMessage += "' as new child of node of type ";
        errorMessage += NodeTypeName[nodeType()];
        errorMessage += " with name '";
        errorMessage += nodeName();
        errorMessage += "' is not allowed";
        throw DomException(errorMessage,PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
    }
    try {
        _myChildren[myOldIndex].reparent(0, 0);
        checkAndUpdateChildrenSchemaInfo(*theNewChild, this);
    } catch (...) {
        _myChildren[myOldIndex].reparent(this, this);
        throw;
    }
    _myChildren.setItem(myOldIndex,theNewChild);
    return theNewChild;
}


NodePtr
dom::Node::appendChild(NodePtr theNewChild) {
    checkName(theNewChild->nodeName(), theNewChild->nodeType());
    if (theNewChild->nodeType() == DOCUMENT_FRAGMENT_NODE) {
        for (int i = 0; i < theNewChild->childNodesLength();++i) {
            appendChild(theNewChild->childNode(i));
        }
        return NodePtr(0);
    }
    if (!allowedToHaveChild(theNewChild->nodeType())) {
        string errorMessage;
        errorMessage = "Add child of type ";
        errorMessage += NodeTypeName[theNewChild->nodeType()];
        errorMessage += " with name '";
        errorMessage += theNewChild->nodeName();
        errorMessage += "' to node of type ";
        errorMessage += NodeTypeName[nodeType()];
        errorMessage += " with name '";
        errorMessage += nodeName();
        errorMessage += "' is not allowed";
        throw DomException(errorMessage,PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
    }
    dom::Node * oldParent = theNewChild->parentNode();
    if (oldParent) {
        oldParent->removeChild(theNewChild);
    }
    checkAndUpdateChildrenSchemaInfo(*theNewChild, this);
    _myChildren.append(theNewChild);
    return theNewChild;
}

NodePtr 
dom::Node::insertBefore(NodePtr theNewChild, NodePtr theRefChild) {
    int myRefIndex = _myChildren.findIndex(&(*theRefChild));
    if (myRefIndex < 0) {
        return NodePtr(0);
    }
    checkName(theNewChild->nodeName(), theNewChild->nodeType());
    if (theNewChild->nodeType() == DOCUMENT_FRAGMENT_NODE) {
        for (int i = 0; i < theNewChild->childNodesLength();++i) {
            insertBefore(theNewChild->childNode(i), theRefChild);
        }
        return NodePtr(0);
    }
    if (!allowedToHaveChild(theNewChild->nodeType())) {
        string errorMessage;
        errorMessage = "Insert child of type ";
        errorMessage += NodeTypeName[theNewChild->nodeType()];
        errorMessage += " with name '";
        errorMessage += theNewChild->nodeName();
        errorMessage += "' to node of type ";
        errorMessage += NodeTypeName[nodeType()];
        errorMessage += " with name '";
        errorMessage += nodeName();
        errorMessage += "' is not allowed";
        throw DomException(errorMessage,PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
    }
    dom::Node * oldParent = theNewChild->parentNode();
    if (oldParent) {
        oldParent->removeChild(theNewChild);
    }
    checkAndUpdateChildrenSchemaInfo(*theNewChild, this);
    _myChildren.insert(myRefIndex, theNewChild);
    return theNewChild;
}

void
dom::Node::checkAndUpdateAttributeSchemaInfo(Node & theNewAttribute, Node * theTopNewParent) {
    DB(AC_TRACE << "checkAndUpdateAttributeSchemaInfo("<<theNewAttribute.nodeName()<<")"<< endl);
    theNewAttribute.reparent(this, theTopNewParent);
    if (_mySchemaInfo) {
        DB(AC_TRACE << "checkAndUpdateAttributeSchemaInfo("<<theNewAttribute.nodeName()<<") parent has SchemaInfo"<< endl);
        if (theNewAttribute.nodeType() == ATTRIBUTE_NODE) {
            theNewAttribute.makeSchemaInfo(true);
            NodePtr myAttributeDecl = getAttributeDeclaration(theNewAttribute.nodeName());
            if (!myAttributeDecl) {
                throw Schema::UndeclaredAttribute(
                    std::string("Element <")+nodeName()+"> has no declaration for an attribute named '"+theNewAttribute.nodeName()+"'",
                                "dom::Node::checkAndUpdateAttributesSchemaInfo");
            }
            NodePtr myAttributeType = _mySchemaInfo->getSchema()->getAttributeType(myAttributeDecl);
            if (!myAttributeType) {
                    throw Schema::AttributeWithoutType(
                        std::string("Element <")+nodeName()+"> has no valid type declaration for an attribute named '"+theNewAttribute.nodeName()+"'",
                                    "dom::Node::checkAndUpdateAttributesSchemaInfo");
            }
            NodePtr myDataTypeNameAttr = myAttributeType->getAttribute(ATTR_NAME);
            if (!myDataTypeNameAttr) {
                throw Schema::InternalError(
                    std::string("Element <")+nodeName()+"> attribute '"+theNewAttribute.nodeName()+"' type declaration has no type name",
                                "dom::Node::checkAndUpdateAttributesSchemaInfo");
            }
            const string & myDataTypeName = myDataTypeNameAttr->nodeValue();

            theNewAttribute._mySchemaInfo->_mySchemaDeclaration = myAttributeDecl;
            theNewAttribute._mySchemaInfo->_myType = myAttributeType;
            if (!theNewAttribute.checkMyValueTypeFits()) {
                ValuePtr myDifferentTypeValue = theNewAttribute._myValue;
                theNewAttribute.trashValue();
                theNewAttribute.nodeValueWrapperPtr(myDifferentTypeValue);
            }
            if (theNewAttribute._myValue) {
                theNewAttribute._myValue->setNodePtr(&theNewAttribute);
            }
            return;
        }
    }
}


void
dom::Node::checkAndUpdateChildrenSchemaInfo(Node & theNewChild, Node * theTopNewParent) {
    theNewChild.reparent(this, theTopNewParent);
    if (_mySchemaInfo) {
        theNewChild.makeSchemaInfo(true);
        if (theNewChild.nodeType() == ELEMENT_NODE) {
            theNewChild.checkSchemaForElement(theNewChild.nodeName(),0);
        } else {
            if (theNewChild.nodeType() == TEXT_NODE) {
                theNewChild.checkSchemaForText(0);
                if (!theNewChild.checkMyValueTypeFits()) {
                    ValuePtr myDifferentTypeValue = theNewChild._myValue;
                    theNewChild.trashValue();
                    theNewChild.nodeValueWrapperPtr(myDifferentTypeValue);
                }
            }
        }
    }
    if (theNewChild._myValue) {
        theNewChild._myValue->setNodePtr(&theNewChild);
    }
    for (int i = 0; i< theNewChild.attributes().length();++i) {
        theNewChild.checkAndUpdateAttributeSchemaInfo(*theNewChild.getAttribute(i), theTopNewParent);
    }
    for (int i = 0; i< theNewChild.childNodes().length();++i) {
        theNewChild.checkAndUpdateChildrenSchemaInfo(*theNewChild.childNode(i), theTopNewParent);
    }
}

/// returns reference to n'th child node with the given name
/// if no n'th child with the given name exists, a new child node
/// with this name is created; by default, an element node is
/// created; however, the following special names create nodes
/// of other types:
/// "#text" -> creates text node
/// "#cdata-section" -> creates CDATA section
/// "#comment" -> creates comment
Node &
dom::Node::operator()(const String & name, int n) {
    NodePtr result_ptr = childNode(name, n);
    if (result_ptr)
        return *result_ptr;

    if (name == "#text")
        return *appendChild(Text());
    if (name == "#comment")
        return *appendChild(Comment());
    if (name == "#cdata-section")
        return *appendChild(CDataSection(this));

    return *appendChild(Element(name, this));
}


const Node &
dom::Node::operator()(const String & name, int n) const {

    if (nodeType() == X_NO_NODE) return X_NO_NODE_;

    NodePtr result_ptr = childNode(name, n);
    if (result_ptr)
        return *result_ptr;

    return X_NO_NODE_;
}

//static
String
dom::Node::getNameOfType(NodeType type) {
    switch(type) {
    case TEXT_NODE: return "#text";
    case CDATA_SECTION_NODE: return "#cdata-section";
    case COMMENT_NODE: return "#comment";
    case DOCUMENT_NODE: return "#document";
    case DOCUMENT_FRAGMENT_NODE: return "#document-fragment";
    default:return EMPTY_STRING;
    }
}

// search for the attribute declaration of one the element's attributes
const NodePtr
dom::Node::getAttributeDeclaration(const DOMString & theAttributeName) const {
    if (_mySchemaInfo && _mySchemaInfo->getSchema() && _mySchemaInfo->_myType) {
        return _mySchemaInfo->getSchema()->getAttributeDeclaration(_mySchemaInfo->_myType, theAttributeName);
    }
    return NodePtr(0);
}

int
dom::Node::parseSystemAttributes(const String & is, int pos, int end_pos, const Node * doctype) {

    String myAttributeName;
    String myAttributeValue;
    int par_end = read_attribute(is, pos, myAttributeName, myAttributeValue);
    while (par_end > pos && pos < end_pos) {

        if (myAttributeName.size()) {
            std::string myDecodedData = entity_decode_data(myAttributeValue, par_end, doctype);
            bool isSystemAttribute = false;

            if (myAttributeName == XSI_SCHEMALOCATION ||
                myAttributeName == XSI_NONAMESPACESCHEMALOCATION) {
                if (_myParent) {
                    _myParent->loadSchemas(myDecodedData);
                }
                isSystemAttribute = true;
            }
            if (myAttributeName.find(XMLNS) == 0) {
                isSystemAttribute = true;
            }
        }
        pos = par_end;
        par_end = read_attribute(is, pos, myAttributeName, myAttributeValue);
    }
    return par_end;
}

int
dom::Node::parseAttributes(const String & is, int pos, int end_pos, const Node * doctype) {

    String myAttributeName;
    String myAttributeValue;
    int par_end = read_attribute(is, pos, myAttributeName, myAttributeValue);
    while (par_end > pos && pos < end_pos) {

        if (myAttributeName.size()) {

            std::string myDecodedData = entity_decode_data(myAttributeValue, par_end, doctype);

            bool isSystemAttribute = false;

            if (myAttributeName == XSI_SCHEMALOCATION ||
                myAttributeName == XSI_NONAMESPACESCHEMALOCATION) {
                //loadSchemas(myDecodedData);
                isSystemAttribute = true;
            }
            if (myAttributeName.find(XMLNS) == 0) {
                isSystemAttribute = true;
            }

            NodePtr myAttributeDecl;
            if (!isSystemAttribute) {
                myAttributeDecl = getAttributeDeclaration(myAttributeName);
            }
            if (myAttributeDecl) {
                // lets create an typed attribute value
                NodePtr myAttributeType = _mySchemaInfo->getSchema()->getAttributeType(myAttributeDecl);
                if (!myAttributeType) {
                    throw Schema::AttributeWithoutType(
                        std::string("Element <")+_myName+"> has no valid type declaration for an attribute named '"+myAttributeName+"'",
                                    "dom::Node::parseAttributes", par_end);
                }
                NodePtr myDataTypeNameAttr = myAttributeType->getAttribute(ATTR_NAME);
                if (!myDataTypeNameAttr) {
                    throw Schema::InternalError(
                        std::string("Element <")+_myName+"> attribute '"+myAttributeName+"' type declaration has no type name",
                                    "dom::Node::parseAttributes", par_end);
                }
                const string & myDataTypeName = myDataTypeNameAttr->nodeValue();
                ValuePtr myTypedAttribute;
                try {
                    myTypedAttribute = _mySchemaInfo->_myValueFactory->createValue(myDataTypeName,myDecodedData,0);
                }
                catch (ParseException & ex) {
                    ex.setParsedUntil(par_end);
                    throw;
                }
                NodePtr new_node(new Node(ATTRIBUTE_NODE,myAttributeName, myTypedAttribute, this));
                new_node->_myParent = this;
                _myAttributes.appendWhileParsing(new_node);
                new_node->makeSchemaInfo(false);
                new_node->_mySchemaInfo->_mySchemaDeclaration = myAttributeDecl;
                new_node->_mySchemaInfo->_myType = myAttributeType;
            } else {
                // we have no attribute declaration
                if (!isSystemAttribute && _mySchemaInfo && _mySchemaInfo->getSchema()) {
                    // we have a schema and should have an attribute declaration but don't, so complain
                    throw Schema::UndeclaredAttribute(
                        std::string("Element <")+_myName+"> has no declaration for an attribute named '"+myAttributeName+"'",
                                    "dom::Node::parseAttributes", par_end);
                }
                // we have no schema declaration, so we just proceed without type
                NodePtr new_node(new Node(ATTRIBUTE_NODE,myAttributeName, ValuePtr(new StringValue(myDecodedData, 0)),this));
                try {
                    _myAttributes.appendWhileParsing(new_node);
                }
                catch (ParseException & dex) {
                    dex.setParsedUntil(par_end);
                    throw;
                }
            }
        }
        pos = par_end;
        par_end = read_attribute(is, pos, myAttributeName, myAttributeValue);
    }
    if (par_end != end_pos) {
        int nw_pos = read_whitespace(is,pos);
        if (nw_pos < end_pos) {
            throw SyntaxError(
                std::string("Element <")+_myName+"> contains extra unparseable characters '"+myAttributeName+"'",
                "dom::Node::parseAttributes", par_end);
        }
    }
    return par_end;
}

void
dom::Node::checkName(const String& name, NodeType type) {
    if (name.size() && getNameOfType(type).size() && name == getNameOfType(type))
        return;
    if (name.size() && read_name(name,0)==name.size())
        return;
    string errorMessage;
    errorMessage = "dom::Node::checkName failed: name = '";
    errorMessage += name;
    errorMessage += "', type = ";
    errorMessage += NodeTypeName[type];
    throw DomException(errorMessage,PLUS_FILE_LINE,DomException::INVALID_NAME_ERR);
}

int
dom::Node::getNumberOfChildrenWithType(NodeType type) const {
    int counter = 0;
    for (int i = 0; i < _myChildren.size(); ++i)
        if (type == _myChildren[i].nodeType()) ++counter;
        return counter;
}


/// returns true if node is allowed to have a value
bool
dom::Node::mayHaveValue() const {
    return
       (nodeType() == ATTRIBUTE_NODE ||
        nodeType() == PROCESSING_INSTRUCTION_NODE ||
        nodeType() == COMMENT_NODE ||
        nodeType() == ENTITY_NODE ||
        nodeType() == DOCUMENT_TYPE_NODE ||
        nodeType() == TEXT_NODE ||
        nodeType() == CDATA_SECTION_NODE);
}

/// assign a new node value; if the node type is not element, attribute
/// cdata or comment, an exception is thrown
void
dom::Node::nodeValue(const String& value) {
    if (!mayHaveValue()) {
        string errorMessage;
        errorMessage = "dom::Node::nodeValue(value='";
        errorMessage += value;
        errorMessage += "') failed, nodeName = ";
        errorMessage += nodeName();
        errorMessage += "', nodeType = ";
        errorMessage += NodeTypeName[nodeType()];
        throw DomException(errorMessage,PLUS_FILE_LINE,DomException::NO_DATA_ALLOWED_ERR);
    }
    ensureValue();
    _myValue->setString(value);
}
/// assign a new node value; if the node type is not element, attribute
/// cdata or comment, an exception is thrown
Node &
dom::Node::operator=(const String& value) {
    nodeValue(value);
    return *this;
}

enum {
    hasName=1<<4,
    hasValue=1<<5,
    hasAttributes=1<<6,
    hasChildren=1<<7,
    hasTypedValue=1<<8,
    hasNameIndex=1<<9,
    isUnmodifiedProxy=1<<10,
    isPatch=1<<11
};

void dumpType(unsigned short theType) {
    std::cerr << "Type = " << (void*)(ptrdiff_t)theType << ", "<< dom::NodeTypeName[theType&0xf] << ",";
    if (theType&hasName) cerr << "N ";
    if (theType&hasValue) cerr << "V ";
    if (theType&hasTypedValue) cerr << "TV ";
    if (theType&hasAttributes) cerr << "A ";
    if (theType&hasChildren) cerr << "C ";
    if (theType&hasNameIndex) cerr << "I ";
    if (theType&isUnmodifiedProxy) cerr << "U ";
    if (theType&isPatch) cerr << "P ";
    cerr << endl;
}
#undef DB
#define DB(x) // x

void
dom::Node::binarize(asl::WriteableStream & theDest, Dictionaries * theDicts, asl::Unsigned64 theIncludeVersion) const {
    asl::Ptr<Dictionaries,ThreadingModel> myDicts;
    if (!theDicts) {
        myDicts = asl::Ptr<Dictionaries,ThreadingModel>(new Dictionaries);
        theDicts = &(*myDicts);
    }
    unsigned short myNodeType = nodeType();
    if (_myVersion < theIncludeVersion) {
        myNodeType|=isUnmodifiedProxy;
    }
    if (theIncludeVersion != 0) {
        myNodeType|=isPatch;
    }

    unsigned int myNameIndex = 0;
    if (nodeName().size() &&
        !(myNodeType&isUnmodifiedProxy) &&
        nodeType()!=TEXT_NODE && 
        nodeType()!=DOCUMENT_NODE && 
        nodeType()!=CDATA_SECTION_NODE && 
        nodeType()!=COMMENT_NODE && 
        nodeType()!=DOCUMENT_FRAGMENT_NODE) 
    {
        myNodeType|=hasName;
        if (nodeType() == ELEMENT_NODE) {
            if (!theDicts->_myElementNames.enterName(nodeName(),myNameIndex)) {
                myNodeType|=hasNameIndex;
            }
        }
        if (nodeType() == ATTRIBUTE_NODE) {
            if (!theDicts->_myAttributeNames.enterName(nodeName(),myNameIndex)) {
                myNodeType|=hasNameIndex;
            }
        }
    }

    if (!(myNodeType&isUnmodifiedProxy)) {
        if (_myValue) {
            myNodeType|=hasValue;
            if (_mySchemaInfo && _mySchemaInfo->_myType) {
                myNodeType|=hasTypedValue;
            }
        }
        if (attributes().size()) myNodeType|=hasAttributes;
        if (childNodes().size()) myNodeType|=hasChildren;
    }
    DB(AC_TRACE << "Write myNodeType (Word) = " << myNodeType << endl);
    DB(dumpType(myNodeType));
    theDest.appendUnsigned16(myNodeType);
    DB(AC_TRACE << "Write uniqueID (LongLong+Long) = " << getUniqueId() << endl);
    getUniqueId().append(theDest);
   
    if (myNodeType&isUnmodifiedProxy) {
        DB(AC_TRACE << "Finshed Node because isUnmodifiedProxy == true" << endl);
        return;
    }

    if (myNodeType&hasName) {
        if (myNodeType&hasNameIndex) {
            DB(AC_TRACE << "Write myNameIndex (CountedString) = " << myNameIndex << endl);
            theDest.appendUnsigned(myNameIndex);
        } else {
            DB(AC_TRACE << "Write nodeName (CountedString) = " << nodeName() << endl);
            theDest.appendCountedString(nodeName());
        }
    }

    if (myNodeType&hasValue) {
        if (myNodeType&hasTypedValue) {
#ifdef EXTERNAL_BINARIZE
            //theDest.appendCountedString(_mySchemaInfo->getTypeName());
            DB(AC_TRACE << "Write Binary Len (Count) = " << nodeValueWrapper().size() << endl);
            theDest.appendUnsigned(nodeValueWrapper().accessReadableBlock().size());
            DB(AC_TRACE << "Write Binary Data (Block) = " << nodeValueWrapper() << endl);
            theDest.append(nodeValueWrapper().accessReadableBlock());
#else
            nodeValueWrapper().binarize(theDest);
#endif
        } else {
            DB(AC_TRACE << "Write nodeValue (CountedString) = " << nodeValue() << endl);
            theDest.appendCountedString(nodeValue());
        }

    }
    if (myNodeType&hasAttributes) {
        attributes().binarize(theDest,*theDicts,theIncludeVersion);
    }
    if (myNodeType&hasChildren) {
        childNodes().binarize(theDest,*theDicts,theIncludeVersion);
    }
/*
    if (myDicts) {
        theDicts->_myElementNames.dump();
        theDicts->_myAttributeNames.dump();
    }
    */
}

#ifdef PATCH_STATISTIC
#define PS(x) x
#else
#define PS(x)
#endif

asl::AC_SIZE_TYPE
dom::Node::debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos, Dictionaries * theDicts, bool thePatchFlag, bool & theUnmodifiedProxyFlag) {
    DB(AC_TRACE << "dom::Node::debinarize theSource.size() = " << theSource.size() << ", thePos = " << thePos << endl);
    asl::AC_SIZE_TYPE theOldPos = thePos;
    asl::Ptr<Dictionaries,ThreadingModel> myDicts;
    if (!theDicts) {
        myDicts = asl::Ptr<Dictionaries,ThreadingModel>(new Dictionaries);
        theDicts = &(*myDicts);
    }
    unsigned short myNodeType;
    thePos = theSource.readUnsigned16(myNodeType, thePos);
    DB(AC_TRACE << "Read myNodeType (Word) = " << myNodeType << endl);
    DB(dumpType(myNodeType));

    UniqueId myUniqueID(theSource, thePos);
    DB(AC_TRACE << "Read myUniqueID (LongLong) = " << myUniqueID << endl);

    theUnmodifiedProxyFlag = myNodeType&isUnmodifiedProxy;
    if (theUnmodifiedProxyFlag) {
        PS(++theDicts->_myPatchStat.unmodifiedNodes);
        if (!thePatchFlag) {
            throw PatchMismatch("Encountered 'unmodified proxy' while not in patch mode", PLUS_FILE_LINE);
        }
        if (myUniqueID == getUniqueId()) {
            return thePos;
        } 
        //else {
        //    throw PatchMismatch("Encountered 'unmodified proxy' with no corresponding patch", PLUS_FILE_LINE);
        //}
    }
    if (thePatchFlag && myUniqueID != getUniqueId()) {
        DB(AC_TRACE << "myUniqueID mismatch myUniqueID=" << myUniqueID << "!="<<getUniqueId()<<"(getUniqueId())"<<endl);
        return theOldPos;
    }

    NodeType myType = static_cast<NodeType>(myNodeType&0xf);
    if (myUniqueID == getUniqueId()) {
        if (myType != _myType) {
            throw FormatCorrupted("Node with matching unique id but different node type encountered in stream",PLUS_FILE_LINE);
        }
        if (myNodeType&hasName) {
            //throw FormatCorrupted("Node with matching unique id must not have a name",PLUS_FILE_LINE);
        }
    }

    _myType = myType;
    _myUniqueId.set(myUniqueID);
    if (myNodeType&hasName) {
        if (myNodeType&hasNameIndex) {
            asl::AC_SIZE_TYPE myIndex = static_cast<asl::AC_SIZE_TYPE>(-1);
            thePos = theSource.readUnsigned(myIndex, thePos);
            if (_myType == ELEMENT_NODE) {
                _myName = theDicts->_myElementNames.lookupName(myIndex);
            } else if (_myType == ATTRIBUTE_NODE) {
                _myName = theDicts->_myAttributeNames.lookupName(myIndex);
            } else {
                throw FormatCorrupted("NameIndex and nodeType are not comptible",PLUS_FILE_LINE);
            }
        } else {
            thePos = theSource.readCountedString(_myName, thePos);
            if (_myType == ELEMENT_NODE) {
                unsigned int myIndex = 0;
                if (!theDicts->_myElementNames.enterName(_myName, myIndex)) {
                    throw FormatCorrupted("Name is already in _myElementNames Dictionary",PLUS_FILE_LINE);
                }
            } else if (_myType == ATTRIBUTE_NODE) {
                unsigned int myIndex = 0;
                if (!theDicts->_myAttributeNames.enterName(_myName, myIndex)) {
                    throw FormatCorrupted("Name is already in _myAttributeNames Dictionary",PLUS_FILE_LINE);
                }
            }
        }
        DB(AC_TRACE << "Read nodeName (CountedString) = " << _myName << endl);
        PS(++theDicts->_myPatchStat.newNames);
    }
    switch (_myType) {
        case ELEMENT_NODE:
            checkSchemaForElement(_myName,thePos);
            break;
        case TEXT_NODE:
            _myName = getNameOfType(TEXT_NODE);
            checkSchemaForText(thePos);
            break;
        case ATTRIBUTE_NODE:
            if (!_myParent) {
                throw Schema::InternalError("Attribute has no parent",PLUS_FILE_LINE);
            }
            _myParent->checkAndUpdateAttributeSchemaInfo(*this, _myParent);
            break;
    }
    if (myNodeType&hasValue) {
        if (myNodeType&hasTypedValue) {
            if (!thePatchFlag) {
                if (_myValue) {
                    throw Schema::InternalError(string("Node type ")+NodeTypeName[nodeType()]+"; name "+_myName+" already has a value but must not",PLUS_FILE_LINE);
                }
                if (!_mySchemaInfo) {
                    throw Schema::InternalError(string("Node type ")+NodeTypeName[nodeType()]+"; name "+_myName+" has has no Schema Info but must have one",PLUS_FILE_LINE);
                }
            }
#ifdef EXTERNAL_BINARIZE
            asl::AC_SIZE_TYPE mySize;
            thePos = theSource.readUnsigned(mySize,thePos);
            DB(AC_TRACE << "Read Binary Len (Count) = " << mySize << endl;);

            assignValue(asl::ReadableBlockAdapter(theSource.begin()+thePos, theSource.begin()+thePos + mySize));
            thePos += mySize;
            DB(AC_TRACE << "Read nodeValue (Bytes) = " << nodeValueWrapper() << endl);
#else
            ensureValue();
            thePos = nodeValueWrapper().debinarize(theSource, thePos);
#endif
        } else {
            ensureValue();
            DOMString myStringValue;
            thePos = theSource.readCountedString(myStringValue, thePos);
            _myValue->setString(myStringValue);
            DB(AC_TRACE << "Read nodeValue (CountedString) = " << nodeValue() << endl);
        }
        DB(AC_TRACE << "Value = '"<<nodeValue()<<"'"<< endl);
        PS(++theDicts->_myPatchStat.newValues);
    }
    if (myNodeType&hasAttributes) thePos = _myAttributes.debinarize(theSource, thePos, *theDicts, thePatchFlag);
    if (myNodeType&hasChildren) thePos = _myChildren.debinarize(theSource, thePos, *theDicts, thePatchFlag);
    return thePos;
}

#undef DB
#define DB(x) // x

dom::NodePtr
dom::Node::nextSibling() {
    if (_myParent) {
        if (nodeType() == Node::ATTRIBUTE_NODE) {
            return _myParent->_myAttributes.nextSibling(this);
        } else {
            return _myParent->_myChildren.nextSibling(this);
        }
    }
    return NodePtr(0);
}

const dom::NodePtr
dom::Node::nextSibling() const {
    if (_myParent) {
        if (nodeType() == Node::ATTRIBUTE_NODE) {
            return _myParent->_myAttributes.nextSibling(this);
        } else {
            return _myParent->_myChildren.nextSibling(this);
        }
    }
    return NodePtr(0);
}

dom::NodePtr
dom::Node::previousSibling() {
    if (_myParent) {
        if (nodeType() == Node::ATTRIBUTE_NODE) {
            return _myParent->_myAttributes.previousSibling(this);
        } else {
            return _myParent->_myChildren.previousSibling(this);
        }
    }
    return NodePtr(0);
}

const dom::NodePtr
dom::Node::previousSibling() const {
    if (_myParent) {
        if (nodeType() == Node::ATTRIBUTE_NODE) {
            return _myParent->_myAttributes.previousSibling(this);
        } else {
            return _myParent->_myChildren.previousSibling(this);
        }
    }
    return NodePtr(0);
}
dom::NodePtr
dom::Node::removeChild(dom::NodePtr theChild) {
    return _myChildren.removeItem(_myChildren.findIndex(&(*theChild)));
}

Node * 
Node::getRootNode() {
    Node * myNode = this;
    while (myNode->parentNode()) {
        if (myNode->parentNode()->nodeType() == DOCUMENT_NODE) {
            break;
        }
        myNode = myNode->parentNode();
    }
    return myNode;
}

const Node *
Node::getRootNode() const
{
    const Node * myNode = this;
    while (myNode->parentNode()) {
        if (myNode->parentNode()->nodeType() == DOCUMENT_NODE) {
            break;
        }
        myNode = myNode->parentNode();
    }
    return myNode;
}

bool
Node::hasFacade() const {
    if (_myType != ELEMENT_NODE) {
        return false;
    }
    if (!_myFacade) {
        FacadeFactoryPtr myFactory = getFacadeFactory();
        if (myFactory) {
            _myFacade = FacadePtr(myFactory->createFacade(nodeName(), *const_cast<Node*>(this)));
			if (!_myFacade) {
				_myFacade = FacadePtr(myFactory->createFacade(nodeName(), 
					                  *const_cast<Node*>(this), parentNode()->nodeName()));
			}
        }
        if (_myFacade) {
            _myFacade->setSelf(_myFacade);
        } else {
            return false;
        }        
    }
    return true;
}

FacadePtr
Node::getFacade() {
    if (hasFacade()) {
        DB(AC_DEBUG << "returning facade for node " << nodeName());
        return _myFacade;
    } else {
        throw Facade::Exception(string("No facade registered for node type ") + nodeName(), PLUS_FILE_LINE);
    }
}

const FacadePtr
Node::getFacade() const {
    return const_cast<Node*>(this)->getFacade();
}

void
Node::nodeValueWrapperPtr(ValuePtr theValue) {
    if (checkValueTypeFits(*theValue)) {
        ValuePtr myOldValue = _myValue;
        _myValue = theValue;

        if (_myValue) {
            _myValue->setSelf(_myValue);
            _myValue->setNodePtr(this);
            if (myOldValue) {
                _myValue->transferDependenciesFrom(*myOldValue);
            }
        }
    } else {
        ensureValue();
        _myValue->setString(theValue->getString());
    }
}

void
Node::markPrecursorDependenciesOutdated() {
   if (_myValue) {
        _myValue->markPrecursorDependenciesOutdated();
    }
    if (_myFacade) {
        _myFacade->markPrecursorDependenciesOutdated();
    } 
}

void
Node::reparent(Node * theNewParent, Node * theTopNewParent) { 
    Node * myOldParent = _myParent;
    _myParent = theNewParent;
    if (_myValue) {
        _myValue->reparent();
    }
    _myAttributes.reparent(this, theTopNewParent);
    _myChildren.reparent(this, theTopNewParent);

    markPrecursorDependenciesOutdated();
        
    // [CH] Some thoughts about the next step:
    // - The problem is that the parent node might have 'implicit' dependencies, i.e.
    //   an attribute might depend on all child nodes (such as the bounding box)
    // - This dependencies must be kept up to date, if new children arrive or leave
    // - There are two possibilities: 
    //   (a) The child is responsible for telling the parent 'here I am', by the
    //       'affects' command. Therefore the child facade has to be created here and now.
    //   (b) The parents dependencies could be marked outdated. Then the parent will reset
    //       its dependencies, on first get-value, which would trigger the creation of 
    //       the child facade
    // - The old variant (a), had two disadvantages:
    //   (a) The facade is created earlier than neccessary, creating headaces with 
    //       construction order in some cases.
    //   (b) The 'affects' command is nessessary in the child facade.

#ifdef OLD_PARENT_DEPENDENCY_UPDATE
    // Variant a: Create a facade to register dependencies new with the new parent
    hasFacade();
#else
    // Variant b: In case a parent value depends on the values of all children
    if (_myParent) {
        _myParent->markPrecursorDependenciesOutdated();
    }
#endif
    bumpVersion();
}

/*
returns true if is allowed to have child:

  Allowed children:

    Document -- Element (maximum of one), ProcessingInstruction, Comment, DocumentType
    DocumentFragment -- Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
    DocumentType -- Notation, Entity
    EntityReference -- Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
    Element -- Element, Text, Comment, ProcessingInstruction, CDATASection, EntityReference
    Attribute -- Text, EntityReference
*/
bool
dom::Node::allowedToHaveChild(NodeType type) const {
    switch(_myType) {
        case DOCUMENT_NODE:
            if ((type == ELEMENT_NODE && getNumberOfChildrenWithType(ELEMENT_NODE) == 0)||
                type == PROCESSING_INSTRUCTION_NODE ||
                type == DOCUMENT_TYPE_NODE ||
                type == COMMENT_NODE)
            {
                return true;
            }
            break;
        case DOCUMENT_FRAGMENT_NODE:
        case ELEMENT_NODE:
        case ENTITY_REFERENCE_NODE:
            if (type == ELEMENT_NODE ||
                type == PROCESSING_INSTRUCTION_NODE ||
                type == COMMENT_NODE ||
                type == TEXT_NODE ||
                type == ENTITY_REFERENCE_NODE ||
                type == CDATA_SECTION_NODE)
            {
                return true;
            }
            break;
        case ATTRIBUTE_NODE:
            if (type == TEXT_NODE ||
                type == ENTITY_REFERENCE_NODE)
                return true;
            break;
        case DOCUMENT_TYPE_NODE:
            if (type == NOTATION_NODE ||
                type == ENTITY_NODE)
            {
                return true;
            }
            break;

        default:
            return false;
    }
    return false;
}
// EventTarget Interface
void
dom::Node::addEventListener(const DOMString & type,
                        EventListenerPtr listener,
                        bool useCapture)
{
    if (useCapture) {
        _myCapturingEventListeners[type][&(*listener)]=listener;
    } else {
        _myEventListeners[type][&(*listener)]=listener;
    }
}

void
dom::Node::removeEventListener(const DOMString & type,
                            EventListenerPtr listener,
                            bool useCapture)
{
        if (useCapture) {
        _myCapturingEventListeners[type].erase(&(*listener));
    } else {
        _myEventListeners[type].erase(&(*listener));
    }
}
bool
dom::Node::dispatchEvent(EventPtr evt) {
    // gather list event targets
    std::vector<NodePtr> myTargets;
    Node * myNode = this;
    while (myNode) {
        NodePtr myNodePtr = myNode->self().lock();
        if (myNodePtr) {
            myTargets.push_back(myNodePtr);
        }
        myNode = myNode->parentNode();
    }
    // setup event members
    evt->target(self().lock());

    // perform capturing phase event dispatch
    evt->eventPhase(Event::CAPTURING_PHASE);
    for (int i = myTargets.size() - 1; i > 0  &&
        !evt->isPropagationStopped(); --i)
    {
        myTargets[i]->callListeners(myTargets[i]->_myCapturingEventListeners, evt);
    }

    if (!evt->isPropagationStopped()) {
        evt->eventPhase(Event::AT_TARGET);
        callListeners(_myEventListeners, evt);
    }

    if (evt->bubbles()) {
        evt->eventPhase(Event::BUBBLING_PHASE);
        for (int i = 1;  i < myTargets.size() &&
            !evt->isPropagationStopped(); ++i)
        {
            myTargets[i]->callListeners(myTargets[i]->_myEventListeners, evt);
        }
    }
    return evt->isDefaultPrevented();
    }

void
dom::Node::callListeners(EventListenerMap & theListeners, EventPtr evt) {
    evt->currentTarget(self().lock());
    DOMString type = evt->type();
    EventListenerMap::iterator myEventListeners = theListeners.find(evt->type());
    if (myEventListeners != theListeners.end()) {
        for (EventListenerSet::iterator
            myListener = myEventListeners->second.begin();
            myListener != myEventListeners->second.end()
            /*&& !evt->isImmediatePropagationStopped()*/;
            ++myListener)
        {
            myListener->second->handleEvent(evt);
        }
    }
}




std::istream &
dom::operator>>(std::istream & is, dom::Node & n) {
    String xml_data;
    std::vector<char> xml_data_vec(4096);
    while (is) {
        is.read(&(*xml_data_vec.begin()),xml_data_vec.size());

#if WIN32
        // Sane reallocation strategy. Actually, append should do this. With
        // MS stl, it doesn't.
        if (xml_data.capacity() < xml_data.length()+xml_data_vec.size()) {
            xml_data.reserve(xml_data.capacity()*2);
        }
#endif
        xml_data.append(&(*xml_data_vec.begin()),is.gcount());
    }
    //n = Node();
    n.parseAll(xml_data);
    if (n) {
        is.clear();
    } else {
        // is.clear(__STD::ios_base::badbit);
        is.clear(std::ios::badbit);
    }
    return is;
}
/*

XML - Document Object Model - Node Content:

node                  nodeName                      nodeValue                             attributes
--------------------------------------------------------------------------------------------------
Element               tagName                   null                                  NamedNodeMap
Attribute             name of attribute         value of attribute                    null
Text                  "#text"                   content of the text node              null
CDATASection          "#cdata-section"          content of the CDATA Section          null
EntityReference       name of entity referenced null                                  null
Entity                entity name               null                                  null
ProcessingInstruction target                    entire content excluding the target   null
Comment               "#comment"                content of the comment                null
Document              "#document"               null                                  null
DocumentType          document type name        null                                  null
DocumentFragment      "#document-fragment"      null                                  null
Notation notation     name                      null                                  null


XML - Document Object Model : Allowed child nodes in a Node of type:

  Document -- Element (maximum of one), ProcessingInstruction, Comment, DocumentType
  DocumentFragment -- Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
  DocumentType -- Notation, Entity
  EntityReference -- Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
  Element -- Element, Text, Comment, ProcessingInstruction, CDATASection, EntityReference
  Attribute -- Text, EntityReference
  ProcessingInstruction -- no other nodes
  Comment -- no other nodes
  Text -- no other nodes
  CDATASection -- no other nodes
  Entity -- no other nodes
  Notation -- no other nodes

See http://www.w3.org/TR/PR-DOM-Level-1/level-one-core.html

*/

