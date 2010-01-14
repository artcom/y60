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

#include <algorithm>

#include <asl/base/UnitTest.h>

#include "xpath_api.h"
#include "XPathValue.h"

namespace {

    void testPathparser( UnitTest& unitTest
                       , bool myExpectedResult
                       , const std::string& str
                       , const std::string& cmp
                       , const char * mySourceFileName
                       , unsigned long mySourceLine )
    {
        try {
            const std::string& str_path = xpath::Path(str).toString();
            const bool equal = ( str_path == cmp );

            std::ostringstream oss;
            oss << "String: \""
                << str
                << "\" ==> Path: \""
                << str_path << "\" " << (equal ? "==" : "!=") << " \""
                << cmp
                << '\"';

            unitTest.ensure( str_path==cmp, oss.str().c_str(), mySourceFileName, mySourceLine, myExpectedResult);
        } catch( const xpath::XPathError& e ) {
            std::ostringstream oss;
            oss << "An unexpected exception occurred at " << e.where()
                << ": \"" << e.what() << "\" while parsing path \""
                << str << '\"';
            unitTest.ensure(false, oss.str().c_str(), mySourceFileName, mySourceLine, myExpectedResult);
        }
    }

    class xpath_ParserTest : public UnitTest {
        public:
            xpath_ParserTest() : UnitTest("xpath_ParserTest") {  }

#           define XPATH_TEST_PARSER(Str_,Cmp_)                                                         testPathparser(*this,true ,Str_,Cmp_,__FILE__,__LINE__)
#           define XPATH_TEST_PARSER_FAIL(Str_,Cmp_)                                                    testPathparser(*this,false,Str_,Cmp_,__FILE__,__LINE__)

            void run() {

                DTITLE("Testing simple children");
                XPATH_TEST_PARSER( ""                                                                           , ""                                                                                                                            );
                XPATH_TEST_PARSER( "testDoc"                                                                    , "child::testDoc"                                                                                                              );
                XPATH_TEST_PARSER( "testDoc/body/junk"                                                          , "child::testDoc/child::body/child::junk"                                                                                      );
                XPATH_TEST_PARSER( "testDoc/self/body"                                                          , "child::testDoc/self::node()/child::body"                                                                                     );
                XPATH_TEST_PARSER( "testDoc/descendant/body"                                                    , "child::testDoc/descendant::node()/child::body"                                                                               );
                XPATH_TEST_PARSER( "testDoc/body/junk/ancestor/junk"                                            , "child::testDoc/child::body/child::junk/ancestor::node()/child::junk"                                                         );
                XPATH_TEST_PARSER( "testDoc/descendant-or-self/junk"                                            , "child::testDoc/descendant-or-self::node()/child::junk"                                                                       );
                XPATH_TEST_PARSER( "*"                                                                          , "child::*"                                                                                                                    );
                XPATH_TEST_PARSER( "testDoc/body/*"                                                             , "child::testDoc/child::body/child::*"                                                                                         );
                XPATH_TEST_PARSER( "/"                                                                          , "/"                                                                                                                           );
                XPATH_TEST_PARSER( "/testDoc"                                                                   , "/child::testDoc"                                                                                                             );
                XPATH_TEST_PARSER( "/testDoc/body/junk"                                                         , "/child::testDoc/child::body/child::junk"                                                                                     );
                XPATH_TEST_PARSER( "/testDoc/self/body"                                                         , "/child::testDoc/self::node()/child::body"                                                                                    );
                XPATH_TEST_PARSER( "/testDoc/descendant/body"                                                   , "/child::testDoc/descendant::node()/child::body"                                                                              );
                XPATH_TEST_PARSER( "/testDoc/body/junk/ancestor/body"                                           , "/child::testDoc/child::body/child::junk/ancestor::node()/child::body"                                                        );
                XPATH_TEST_PARSER( "/testDoc/descendant-or-self/junk"                                           , "/child::testDoc/descendant-or-self::node()/child::junk"                                                                      );

                DTITLE("Testing attributes");
                XPATH_TEST_PARSER( "attribute::name"                                                            , "attribute::name"                                                                                                             );

                DTITLE("Testing simple predicates");
                XPATH_TEST_PARSER( "/testDoc/ancestor-or-self::junk/*[--5]"                                     , "/child::testDoc/ancestor-or-self::node()/child::junk/child::*[--5]"                                                          );
                XPATH_TEST_PARSER( "/testDoc[attribute::name]"                                                  , "/child::testDoc[attribute::name]"                                                                                            );
                XPATH_TEST_PARSER( "/testDoc[attribute::*]"                                                     , "/child::testDoc[attribute::*]"                                                                                               );
                XPATH_TEST_PARSER( "/testDoc[@name]"                                                            , "/child::testDoc[attribute::name]"                                                                                            );

                DTITLE("Testing predicate expressions");
                XPATH_TEST_PARSER( "/testDoc[@name | attribute::class]"                                         , "/child::testDoc[attribute::name | attribute::class]"                                                                         );
                XPATH_TEST_PARSER( "/testDoc//junk[attribute::name=\"more\"]"                                   , "/child::testDoc/descendant-or-self::node()/child::junk[attribute::name=\"more\"]"                                            );
                XPATH_TEST_PARSER( "testDoc/body/junk[attribute::name=\"valuable\"]/junk/@class[@name='more']"  , "child::testDoc/child::body/child::junk[attribute::name=\"valuable\"]/child::junk/attribute::class[attribute::name=\"more\"]" );

                DTITLE("Testing simple functions");
                XPATH_TEST_PARSER( "child::node()"                                                              , "child::node()"                                                                                                               );
                XPATH_TEST_PARSER( "child::processing-instruction(blah)"                                        , "child::processing-instruction(blah)"                                                                                         );
                XPATH_TEST_PARSER( "/testDoc/body/child::node()"                                                , "/child::testDoc/child::body/child::node()"                                                                                   );
                XPATH_TEST_PARSER( "testDoc/body/junk[@name=concat(\"mo\",\"re\")]"                             , "child::testDoc/child::body/child::junk[attribute::name=concat(\"mo\",\"re\")]"                                               );
                XPATH_TEST_PARSER( "testDoc/body/junk[@name=concat(\"m\",\"o\",\"r\",\"e\")]"                   , "child::testDoc/child::body/child::junk[attribute::name=concat(\"m\",\"o\",\"r\",\"e\")]"                                     );

                DTITLE("Testing abbreviated syntax");
                XPATH_TEST_PARSER( "testDoc//junk"                                                              , "child::testDoc/descendant-or-self::node()/child::junk"                                                                       );
                XPATH_TEST_PARSER( "testDoc/body/./*"                                                           , "child::testDoc/child::body/self::node()/child::*"                                                                            );
                XPATH_TEST_PARSER( "testDoc/body/../*"                                                          , "child::testDoc/child::body/parent::node()/child::*"                                                                          );
                XPATH_TEST_PARSER( "testDoc//junk[@name]"                                                       , "child::testDoc/descendant-or-self::node()/child::junk[attribute::name]"                                                      );
                XPATH_TEST_PARSER( "testDoc/junk/@name"                                                         , "child::testDoc/child::junk/attribute::name"                                                                                  );
                XPATH_TEST_PARSER( "/testDoc//junk"                                                             , "/child::testDoc/descendant-or-self::node()/child::junk"                                                                      );
                XPATH_TEST_PARSER( "/testDoc/body/./*"                                                          , "/child::testDoc/child::body/self::node()/child::*"                                                                           );
                XPATH_TEST_PARSER( "/testDoc/body/../*"                                                         , "/child::testDoc/child::body/parent::node()/child::*"                                                                         );
                XPATH_TEST_PARSER( "/testDoc//junk[@name]"                                                      , "/child::testDoc/descendant-or-self::node()/child::junk[attribute::name]"                                                     );
                XPATH_TEST_PARSER( "/testDoc/junk/@name"                                                        , "/child::testDoc/child::junk/attribute::name"                                                                                 );

                DTITLE("Tests from http://www.w3.org/TR/xpath");
                XPATH_TEST_PARSER( "child::para"                                                                , "child::para"                                                                                                                 );
                XPATH_TEST_PARSER( "child::text()"                                                              , "child::text()"                                                                                                               );
                XPATH_TEST_PARSER( "attribute::*"                                                               , "attribute::*"                                                                                                                );
                XPATH_TEST_PARSER( "descendant::para"                                                           , "descendant::node()/child::para"                                                                                              );
                XPATH_TEST_PARSER( "ancestor::div"                                                              , "ancestor::node()/child::div"                                                                                                 );
                XPATH_TEST_PARSER( "ancestor-or-self::div"                                                      , "ancestor-or-self::node()/child::div"                                                                                         );
                XPATH_TEST_PARSER( "descendant-or-self::para"                                                   , "descendant-or-self::node()/child::para"                                                                                      );
                XPATH_TEST_PARSER( "self::para"                                                                 , "self::node()/child::para"                                                                                                    );
                XPATH_TEST_PARSER( "child::chapter/descendant::para"                                            , "child::chapter/descendant::node()/child::para"                                                                               );
                XPATH_TEST_PARSER( "child::*/child::para"                                                       , "child::*/child::para"                                                                                                        );
                XPATH_TEST_PARSER( "/descendant::para"                                                          , "/descendant::node()/child::para"                                                                                             );
                XPATH_TEST_PARSER( "/descendant::olist/child::item"                                             , "/descendant::node()/child::olist/child::item"                                                                                );
                XPATH_TEST_PARSER( "child::para[position()=1]"                                                  , "child::para[position()=1]"                                                                                                   );
                XPATH_TEST_PARSER( "child::para[position()!=1]"                                                 , "child::para[position()!=1]"                                                                                                  );
                XPATH_TEST_PARSER( "child::para[position()=last()]"                                             , "child::para[position()=last()]"                                                                                              );
                XPATH_TEST_PARSER( "child::para[position()=last()-1]"                                           , "child::para[position()=last()-1]"                                                                                            );
                XPATH_TEST_PARSER( "child::para[position()>1]"                                                  , "child::para[position()>1]"                                                                                                   );
                XPATH_TEST_PARSER( "child::para[position()<1]"                                                  , "child::para[position()<1]"                                                                                                   );
                XPATH_TEST_PARSER( "child::para[position()>=1]"                                                 , "child::para[position()>=1]"                                                                                                  );
                XPATH_TEST_PARSER( "child::para[position()<=1]"                                                 , "child::para[position()<=1]"                                                                                                  );
                XPATH_TEST_PARSER( "following-sibling::chapter[position()=1]"                                   , "following-sibling::node()/child::chapter[position()=1]"                                                                      );
                XPATH_TEST_PARSER( "preceding-sibling::chapter[position()=1]"                                   , "preceding-sibling::node()/child::chapter[position()=1]"                                                                      );
                XPATH_TEST_PARSER( "/descendant::figure[position()=42]"                                         , "/descendant::node()/child::figure[position()=42]"                                                                            );
                XPATH_TEST_PARSER( "/child::myDom/child::chapter[position()=5]/child::section[position()=2]"    , "/child::myDom/child::chapter[position()=5]/child::section[position()=2]"                                                     );
                XPATH_TEST_PARSER( "child::para[attribute::type=\"warning\"]"                                   , "child::para[attribute::type=\"warning\"]"                                                                                    );
                XPATH_TEST_PARSER( "child::chapter[child::title='Introduction']"                                , "child::chapter[child::title=\"Introduction\"]"                                                                               );
                XPATH_TEST_PARSER( "child::chapter[child::title]"                                               , "child::chapter[child::title]"                                                                                                );
                XPATH_TEST_PARSER( "child::*[self::chapter or self::appendix]"                                  , "child::*[self::node()/child::chapter or self::node()/child::appendix]"                                                       );
                XPATH_TEST_PARSER( "para"                                                                       , "child::para"                                                                                                                 );
                XPATH_TEST_PARSER( "text()"                                                                     , "child::text()"                                                                                                               );
                XPATH_TEST_PARSER( "@name"                                                                      , "attribute::name"                                                                                                             );
                XPATH_TEST_PARSER( "@*"                                                                         , "attribute::*"                                                                                                                );
                XPATH_TEST_PARSER( "para[1]"                                                                    , "child::para[1]"                                                                                                              );
                XPATH_TEST_PARSER( "para[last()]"                                                               , "child::para[last()]"                                                                                                         );
                XPATH_TEST_PARSER( "*/para"                                                                     , "child::*/child::para"                                                                                                        );
                XPATH_TEST_PARSER( "/myDom/chapter[5]/section[2]"                                               , "/child::myDom/child::chapter[5]/child::section[2]"                                                                           );
                XPATH_TEST_PARSER( "chapter//para"                                                              , "child::chapter/descendant-or-self::node()/child::para"                                                                       );
                XPATH_TEST_PARSER( "//para"                                                                     , "/descendant-or-self::node()/child::para"                                                                                     );
                XPATH_TEST_PARSER( "//olist/item"                                                               , "/descendant-or-self::node()/child::olist/child::item"                                                                        );
                XPATH_TEST_PARSER( "."                                                                          , "self::node()"                                                                                                                );
                XPATH_TEST_PARSER( ".//para"                                                                    , "self::node()/descendant-or-self::node()/child::para"                                                                         );
                XPATH_TEST_PARSER( ".."                                                                         , "parent::node()"                                                                                                              );
                XPATH_TEST_PARSER( "../@lang"                                                                   , "parent::node()/attribute::lang"                                                                                              );
                XPATH_TEST_PARSER( "para[@type=\"warning\"]"                                                    , "child::para[attribute::type=\"warning\"]"                                                                                    );
                XPATH_TEST_PARSER( "chapter[title=\"Introduction\"]"                                            , "child::chapter[child::title=\"Introduction\"]"                                                                               );
                XPATH_TEST_PARSER( "chapter[title]"                                                             , "child::chapter[child::title]"                                                                                                );
                XPATH_TEST_PARSER( "employee[@secretary and @assistant]"                                        , "child::employee[attribute::secretary and attribute::assistant]"                                                              );
                XPATH_TEST_PARSER( "div/para"                                                                   , "child::div/child::para"                                                                                                      );
                XPATH_TEST_PARSER( "child::div/child::para"                                                     , "child::div/child::para"                                                                                                      );
                XPATH_TEST_PARSER( "para[@type=\"warning\"]"                                                    , "child::para[attribute::type=\"warning\"]"                                                                                    );
                XPATH_TEST_PARSER( "//"                                                                         , "/descendant-or-self::node()"                                                                                                 );
                XPATH_TEST_PARSER( "/descendant-or-self::node()/"                                               , "/descendant-or-self::node()"                                                                                                 );
                XPATH_TEST_PARSER( "div//para"                                                                  , "child::div/descendant-or-self::node()/child::para"                                                                           );
                XPATH_TEST_PARSER( "div/descendant-or-self::node()/child::para"                                 , "child::div/descendant-or-self::node()/child::para"                                                                           );
                XPATH_TEST_PARSER( "//para[1]"                                                                  , "/descendant-or-self::node()/child::para[1]"                                                                                  );
                XPATH_TEST_PARSER( "/descendant::para[1]"                                                       , "/descendant::node()/child::para[1]"                                                                                          );
                XPATH_TEST_PARSER( "self::node()"                                                               , "self::node()"                                                                                                                );
                XPATH_TEST_PARSER( "self::node()/descendant-or-self::node()/child::para"                        , "self::node()/descendant-or-self::node()/child::para"                                                                         );
                XPATH_TEST_PARSER( "parent::node()"                                                             , "parent::node()"                                                                                                              );
                XPATH_TEST_PARSER( "../title"                                                                   , "parent::node()/child::title"                                                                                                 );
                XPATH_TEST_PARSER( "parent::node()/child::title"                                                , "parent::node()/child::title"                                                                                                 );
                XPATH_TEST_PARSER( "child::*[substring(\"12345\", 1.5, 2.6)]"                                   , "child::*[substring(\"12345\",1.5,2.6)]"                                                                                      );
                XPATH_TEST_PARSER( "child::*[substring(\"12345\", 0, 3)]"                                       , "child::*[substring(\"12345\",0,3)]"                                                                                          );
                XPATH_TEST_PARSER( "child::*[substring(\"12345\", 0 div 0, 3)]"                                 , "child::*[substring(\"12345\",0 div 0,3)]"                                                                                    );
                XPATH_TEST_PARSER( "child::*[substring(\"12345\", 1, 0 div 0)]"                                 , "child::*[substring(\"12345\",1,0 div 0)]"                                                                                    );
                XPATH_TEST_PARSER( "child::*[substring(\"12345\", -42, 1 div 0)]"                               , "child::*[substring(\"12345\",-42,1 div 0)]"                                                                                  );
                XPATH_TEST_PARSER( "child::*[substring(\"12345\", -1 div 0, 1 div 0)]"                          , "child::*[substring(\"12345\",-1 div 0,1 div 0)]"                                                                             );
                XPATH_TEST_PARSER( "child::*[child::para[attribute::type='warning'][position()=5]]"             , "child::*[child::para[attribute::type=\"warning\" and position()=5]]"                                                         );
                XPATH_TEST_PARSER( "child::*[child::para[position()=5][attribute::type=\"warning\"]]"           , "child::*[child::para[position()=5 and attribute::type=\"warning\"]]"                                                         );
                XPATH_TEST_PARSER( "child::*[child::*[self::chapter or self::appendix][position()=last()]]"     , "child::*[child::*[self::node()/child::chapter or self::node()/child::appendix and position()=last()]]"                       );
                XPATH_TEST_PARSER( "para[@type=\"warning\"][5]"                                                 , "child::para[attribute::type=\"warning\" and 5]"                                                                              );
                XPATH_TEST_PARSER( "para[5][@type=\"warning\"]"                                                 , "child::para[5 and attribute::type=\"warning\"]"                                                                              );

                DTITLE("Tests from pro60 tests");
                XPATH_TEST_PARSER( "//texture[@image='B9QG8wYM2d8=']"                                           , "/descendant-or-self::node()/child::texture[attribute::image=\"B9QG8wYM2d8=\"]"                                               );
                XPATH_TEST_PARSER( "//text[@field = 'title']"                                                   , "/descendant-or-self::node()/child::text[attribute::field=\"title\"]"                                                         );

                DTITLE("Features not implemented: Path expressions, rule [19], productions #3 and #4")
                XPATH_TEST_PARSER_FAIL( "child::*[boolean(\"foo\")/child::para[position()=5]]"                  , "child::*[boolean(\"foo\")/child::para[position()=5]]"                                                                        );
                XPATH_TEST_PARSER_FAIL( "foo[$Var/blah]"                                                        , "child::foo[$Var/child::blah]"                                                                                                );
                DTITLE("Features not implemented: Path expressions, rule [19], any expression, except a LocationPath, outside of PredicateExpr")
                XPATH_TEST_PARSER_FAIL( "$Var/blah"                                                             , "$Var/child::blah"                                                                                                            );
                DTITLE("Features not implemented: Path expressions, functions not implemented")
                XPATH_TEST_PARSER_FAIL( "child::*[id(\"foo\")]"                                                 , "child::*[id(\"foo\")]"                                                                                                       );
                XPATH_TEST_PARSER_FAIL( "child::*[lang(\"foo\")]"                                               , "child::*[lang(\"foo\")]"                                                                                                     );
                XPATH_TEST_PARSER_FAIL( "child::*[sum(@class)]"                                                 , "child::*[sum(attribute::class)]"                                                                                             );
                XPATH_TEST_PARSER_FAIL( "child::*[local-name(@class)=\"blah\"]"                                 , "child::*[local-name(attribute::class)=\"blah\"]"                                                                             );
                XPATH_TEST_PARSER_FAIL( "child::*[namespace-uri(@class)=\"blah\"]"                              , "child::*[namespace-uri(attribute::class)=\"blah\"]"                                                                          );
                XPATH_TEST_PARSER_FAIL( "child::*[name(@class)=\"blah\"]"                                       , "child::*[name(attribute::class)=\"blah\"]"                                                                                   );
                XPATH_TEST_PARSER_FAIL( "child::*[name(@class)=translate(\"BLAH\",\"ABHL\",\"abhl\")]"          , "child::*[name(attribute::class)=translate(\"BLAH\",\"ABHL\",\"abhl\")]"                                                      );
            }

#       undef XPATH_TEST_PARSER
#       undef XPATH_TEST_PARSER_FAIL

    };

    void testEvaluator( UnitTest& unitTest
                      , bool myExpectedResult
                      , const std::string& str
                      , const std::string& cmp_str
                      , const dom::NodePtr myDom
                      , const xpath::NodeList& cmp_nodes
                      , const char * mySourceFileName
                      , unsigned long mySourceLine )
    {
        try {
            xpath::Path myPath(str);

            const std::string& str_path = myPath.toString();
            if( str_path != cmp_str ) {
                std::ostringstream oss;
                oss << "String: \""
                    << str
                    << "\" ==> Path: \""
                    << str_path << "\" " << "!=" << " \""
                    << cmp_str
                    << '\"';
                unitTest.ensure(false, oss.str().c_str(), mySourceFileName, mySourceLine, myExpectedResult);
            } else {
                xpath::NodeList result;
                xpath::findAll(myPath,myDom,result);

                std::ostringstream oss;
                if( result == cmp_nodes ) {
                    oss << '\"' << str << "\" yields the expected node set ("
                        << result.size() << " nodes).";
                } else {
                    typedef std::pair< xpath::NodeList::const_iterator  // result
                                     , xpath::NodeList::const_iterator  // cmp_nodes
                                     >                  mismatch_result_t;
                    mismatch_result_t mismatch_result( result.end(), cmp_nodes.end() );
                    if( cmp_nodes.size() < result.size() ) {
                        mismatch_result = std::mismatch( cmp_nodes.begin(), cmp_nodes.end(), result.begin() );
                        std::swap( mismatch_result.first, mismatch_result.second );
                    } else {
                        mismatch_result = std::mismatch( result.begin(), result.end(), cmp_nodes.begin() );
                    }

                    oss << '\"' << str
                        << "\" does not yield the expected node set. ("
                        << result.size() << " nodes instead of " << cmp_nodes.size() << ')';
                    if( !(result.empty() && cmp_nodes.empty()) ) {
                        oss << " First difference is at position "
                            << result.end()-mismatch_result.first
                            << "\nfound:\n";
                        if( mismatch_result.first!=result.end() ) {
                            oss << **mismatch_result.first;
                        } else {
                            oss << "[empty]\n";
                        }
                        oss << "expected: \n";
                        if( mismatch_result.second!=cmp_nodes.end() ) {
                            oss << **mismatch_result.second;
                        } else {
                            oss << "[empty]\n";
                        }
                    }
                }

                unitTest.ensure( result==cmp_nodes, oss.str().c_str(), mySourceFileName, mySourceLine, myExpectedResult);
            }
        } catch( const xpath::ParseError& e ) {
            std::ostringstream oss;
            oss << "An unexpected exception occurred at " << e.where()
                << ": \"" << e.what() << '\"';
            unitTest.ensure(false, oss.str().c_str(), mySourceFileName, mySourceLine, myExpectedResult);
        }
    }

    xpath::NodeList operator+( const xpath::NodeList& lhs
                             , const dom::NodePtr&    rhs )
    {
        xpath::NodeList result(lhs);
        result.push_back(rhs);
        return result;
    }

    class xpath_EvaluationTest : public UnitTest {
        public:
            xpath_EvaluationTest() : UnitTest("xpath_EvaluationTest") {  }

#           define XPATH_TEST_EVALUATOR(Str_,CmpStr_,Dom_,CmpNodes_)         testEvaluator(*this,true ,Str_,CmpStr_,Dom_,CmpNodes_,__FILE__,__LINE__)
#           define XPATH_TEST_EVALUATOR_FAIL(Str_,CmpStr_,Dom_,CmpNodes_)    testEvaluator(*this,false,Str_,CmpStr_,Dom_,CmpNodes_,__FILE__,__LINE__)

            void run() {

                const std::string myXML =
                    "<testDoc>"
                        "<body>"
                            "<junk name='valuable'>"
                                "<junk class='more' name='valuable'>not</junk>"
                                "<junk class='less' name='valuable'>not</junk>"
                                "bar"
                            "</junk>"
                            "<junk name='valuable2'>not</junk>"
                        "</body>"
                        "<numbers>"
                            "<number>1</number>"
                            "<number>2</number>"
                            "<number>3</number>"
                            "<number>4</number>"
                            "<number>5</number>"
                            "<number>6</number>"
                            "<number>7</number>"
                            "<number>8</number>"
                            "<number>9</number>"
                            "<number>0</number>"
                        "</numbers>"
                        "<footer>"
                            "<some>"
                                "<junk>blah</junk>"
                                "<junk class='more'>blub</junk>"
                            "</some>"
                        "</footer>"
                    "</testDoc>";

                dom::NodePtr myDom( new dom::Document );
                if( static_cast<std::string::size_type>(myDom->parseAll(myXML)) != myXML.size() ) {
                    std::cerr << "XML parse error.";
                    FAILURE("Cannot load test XML");
                    return;
                }
                std::cout << "parsed document:\n" << *myDom;

                /******************************************************************************/
                DTITLE("Simple relative paths");

                XPATH_TEST_EVALUATOR( ""
                                    , ""
                                    , myDom
                                    , xpath::NodeList() );

                XPATH_TEST_EVALUATOR( "blah"
                                    , "child::blah"
                                    , myDom
                                    , xpath::NodeList() );

                XPATH_TEST_EVALUATOR( "testDoc"
                                    , "child::testDoc"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0) );

                XPATH_TEST_EVALUATOR( "testDoc/body"
                                    , "child::testDoc/child::body"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0) );

                XPATH_TEST_EVALUATOR( "testDoc/body/junk"
                                    , "child::testDoc/child::body/child::junk"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( ".//junk"
                                    , "self::node()/descendant-or-self::node()/child::junk"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( ".//junk"
                                    , "self::node()/descendant-or-self::node()/child::junk"
                                    , myDom->childNode(0)->childNode(2)
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "body"
                                    , "child::body"
                                    , myDom->childNode(0)
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0) );

                XPATH_TEST_EVALUATOR( "testDoc/body/.."
                                    , "child::testDoc/child::body/parent::node()"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0) );

                XPATH_TEST_EVALUATOR( ".."
                                    , "parent::node()"
                                    , myDom->childNode(0)->childNode(0)
                                    , xpath::NodeList() + myDom->childNode(0) );

                /******************************************************************************/
                DTITLE("Simple absolute paths");

                XPATH_TEST_EVALUATOR( "/testDoc"
                                    , "/child::testDoc"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0) );

                XPATH_TEST_EVALUATOR( "/testDoc/body"
                                    , "/child::testDoc/child::body"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0) );

                XPATH_TEST_EVALUATOR( "/testDoc/body/junk"
                                    , "/child::testDoc/child::body/child::junk"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk"
                                    , "/descendant-or-self::node()/child::junk"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk"
                                    , "/descendant-or-self::node()/child::junk"
                                    , myDom->childNode(0)->childNode(2)
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/body"
                                    , "/child::testDoc/child::body"
                                    , myDom->childNode(0)
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0) );

                /******************************************************************************/
                DTITLE("Paths with predicates");

                XPATH_TEST_EVALUATOR( "testDoc/body/junk[attribute::name=\"valuable\"]"
                                    , "child::testDoc/child::body/child::junk[attribute::name=\"valuable\"]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0) );

                XPATH_TEST_EVALUATOR( "testDoc/body/junk[attribute::name=\"valuable2\"]"
                                    , "child::testDoc/child::body/child::junk[attribute::name=\"valuable2\"]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@name=\"valuable\"]/junk[@name=\"valuable\"][@class=\"more\"]/@class"
                                    , "/descendant-or-self::node()/child::junk[attribute::name=\"valuable\"]/child::junk[attribute::name=\"valuable\" and attribute::class=\"more\"]/attribute::class"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)->getAttribute(0) );

                XPATH_TEST_EVALUATOR( "//junk[@class=\"more\"]/*"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=\"more\"]/child::*"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1)->childNode(0) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[last()]"
                                    , "/child::testDoc/child::*[last()]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[last()-1]"
                                    , "/child::testDoc/child::*[last()-1]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[2]"
                                    , "/child::testDoc/child::*[2]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[position()=2]"
                                    , "/child::testDoc/child::*[position()=2]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[not(position()=1) and not(position()=3)]"
                                    , "/child::testDoc/child::*[not(position()=1) and not(position()=3)]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[not(position()=1)][not(position()=3)]"
                                    , "/child::testDoc/child::*[not(position()=1) and not(position()=3)]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[round(last()-0.4)]"
                                    , "/child::testDoc/child::*[round(last()-0.4)]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[true()]"
                                    , "/child::testDoc/child::*[true()]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[false()]"
                                    , "/child::testDoc/child::*[false()]"
                                    , myDom
                                    , xpath::NodeList() );

                XPATH_TEST_EVALUATOR( "/testDoc/*[boolean(\"dummy\")]"
                                    , "/child::testDoc/child::*[boolean(\"dummy\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[not(\"dummy\")]"
                                    , "/child::testDoc/child::*[not(\"dummy\")]"
                                    , myDom
                                    , xpath::NodeList() );

                XPATH_TEST_EVALUATOR( "/testDoc/*[contains(\"dummy\",\"mm\")]"
                                    , "/child::testDoc/child::*[contains(\"dummy\",\"mm\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[contains(\"dummy\",\"nm\")]"
                                    , "/child::testDoc/child::*[contains(\"dummy\",\"nm\")]"
                                    , myDom
                                    , xpath::NodeList()  );

                XPATH_TEST_EVALUATOR( "/testDoc/*[starts-with(\"dummy\",\"du\")]"
                                    , "/child::testDoc/child::*[starts-with(\"dummy\",\"du\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(1)
                                                        + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[starts-with(\"dummy\",\"umm\")]"
                                    , "/child::testDoc/child::*[starts-with(\"dummy\",\"umm\")]"
                                    , myDom
                                    , xpath::NodeList()  );

                XPATH_TEST_EVALUATOR( "/testDoc/*[count(/testDoc/*) div 2]"
                                    , "/child::testDoc/child::*[count(/child::testDoc/child::*) div 2]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[count(/testDoc/*) div 2 + 0.7]"
                                    , "/child::testDoc/child::*[count(/child::testDoc/child::*) div 2+0.7]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[string-length(\"1\")]"
                                    , "/child::testDoc/child::*[string-length(\"1\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[string-length(\"12\")]"
                                    , "/child::testDoc/child::*[string-length(\"12\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "/testDoc/*[string-length(\"123\")]"
                                    , "/child::testDoc/child::*[string-length(\"123\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(2) );

                XPATH_TEST_EVALUATOR( "//junk[@class=substring-before('moreless','less')]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=substring-before(\"moreless\",\"less\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@class=substring-after('moreless','more')]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=substring-after(\"moreless\",\"more\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@class=substring('lessmoreless',string-length('less'),string-length('more'))]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=substring(\"lessmoreless\",string-length(\"less\"),string-length(\"more\"))]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@class=concat(\"mo\",\"re\")]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=concat(\"mo\",\"re\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@class=concat(\"m\",\"o\",\"r\",\"e\")]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=concat(\"m\",\"o\",\"r\",\"e\")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@class=substring-before(normalize-space(' more\tless\n'),' ')]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=substring-before(normalize-space(\" more\tless\n\"),\" \")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(0)
                                                        + myDom->childNode(0)->childNode(2)->childNode(0)->childNode(1) );

                XPATH_TEST_EVALUATOR( "//junk[@class=substring-after(normalize-space(' more\tless\n'),' ')]"
                                    , "/descendant-or-self::node()/child::junk[attribute::class=substring-after(normalize-space(\" more\tless\n\"),\" \")]"
                                    , myDom
                                    , xpath::NodeList() + myDom->childNode(0)->childNode(0)->childNode(0)->childNode(1) );

                /******************************************************************************/

            }
#           undef XPATH_TEST_EVALUATOR
#           undef XPATH_TEST_EVALUATOR_FAIL
    };

    class MyTestSuite : public UnitTestSuite {
        public:
            MyTestSuite(const char * myName, int argc, char *argv[]) :
                UnitTestSuite(myName, argc, argv) {}
            void setup() {
                UnitTestSuite::setup(); // called to print a launch message
                addTest(new xpath_ParserTest);
                addTest(new xpath_EvaluationTest);
            }
    };

}

int main(int argc, char **argv) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
    std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << std::endl;

    return mySuite.returnStatus();
}

