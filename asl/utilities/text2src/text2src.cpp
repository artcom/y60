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
// Description: command line utility to place long text in source code files
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

#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/Arguments.h>

#include <iostream>
#include <string>

using namespace std;
using namespace asl;

int main(int argc, char *argv[]) {

    Arguments::AllowedOptionWithDocumentation myOptions[] = {
        {"--varname", "name", " variable name, replaces every occurence of '@@@@' in prolog, concatenator or prolog"},
        {"--namespace", "name", " namespace name, replaces every occurence of '||||' in prolog, concatenator or prolog"},
        {"--newline", "pattern", " replace this pattern (default: |<- ) in prolog, concatenator or prolog with a newline"},
        {"--include", "name", " additional header to be included"},
        {"--prolog", "text", " text to be inserted at the beginning of the file"},
        {"--concatenator", "text", " text to be inserted every <maxlines> lines"},
        {"--epilog", "text", " text to be inserted at the end of the file"},
        {"--inputfile", "name", "read text from this path"},
        {"--outputfile", "name","write result to this path (default writes to stdout)"},
        {"--maxlines", "count", "split input every <count> lines with concatenator"},
        {"--help", "", "print this help text and exit"},
        {"", "[text]", "text to be put into commandline"},
        {"", ""}
    };
    std::string newLinePattern = "|<-";
    std::string myVarName("MyVar");
 	std::string myNamespace("MyNamespace");
    std::string myInclude("");

    // generate prolog
    ostringstream myPrologStream;
    myPrologStream << "#include <string>" << endl;
    myPrologStream << "namespace |||| {"<< endl;;
    myPrologStream << "const std::string @@@@ = std::string("<< endl;
    std::string myProlog = myPrologStream.str();

	std::string myConcatenator(") + std::string(");
	std::string myEpilog(");|<-}");
    unsigned int myMaxLines = 100;

    ostringstream myExample;
	myExample << "Defaults:" << endl;
	myExample << " text2cpp --prolog \"" << myProlog << '"' << endl;
	myExample << "		   --concatenator \"" << myConcatenator << '"' << endl;
	myExample << "		   --epilog \""<< myEpilog << '"' << endl;
	myExample << "		   --maxlines " << myMaxLines << endl;
	myExample << "		   --newline \"" << newLinePattern << '"' << endl;
	myExample << "Examples: text2src --namespace y60 --variable ourY60xsd --inputfile Y60.xsd --outputfile Y60xsd.cpp" << endl;
	myExample << "          text2src --prolog \"const char * revision=\" --epilog=\";\"  --outputfile revision.cpp \"Rev.12345\"" << endl;
	myExample << "    The following line will produce a C++ program that outputs the text from myFile.txt to stdout:" << endl;
    myExample << "          text2src --inputfile myFile.txt --outputfile myFile.cpp --epilog \");}|<-#include <iostream>|<-int main() {std::cout << ||||::@@@@;}\"" << endl;

    std::string myInputFilename;
	std::string myOutputFilename;
	std::string myText;
    std::vector<std::string> myInputLines;

    try {
        Arguments myArguments;
        myArguments.addAllowedOptionsWithDocumentation(myOptions);
        myArguments.setShortDescription("text2src creates src files that puts long text into a variable");
        myArguments.setLongDescription(myExample.str());

        if (myArguments.parse(argc,argv)) {
 			if (myArguments.getCount() && myArguments.haveOption("--inputfile")) {
				cerr << "### ERROR: text given both on commandline and with --inputfile option, please pick one" << myInputFilename << "'"<< endl;
				return 1;
			}
            if (myArguments.haveOption("--help")) {
                myArguments.printHelp();
                return EXIT_SUCCESS;
            }
            if (myArguments.haveOption("--prolog")) {
                myProlog = myArguments.getOptionArgument("--prolog");
            }
            if (myArguments.haveOption("--concatenator")) {
                myConcatenator = myArguments.getOptionArgument("--concatenator");
            }
            if (myArguments.haveOption("--epilog")) {
                myEpilog = myArguments.getOptionArgument("--epilog");
            }
            if (myArguments.haveOption("--varname")) {
                myVarName = myArguments.getOptionArgument("--varname");
            }
            if (myArguments.haveOption("--include")) {
                myInclude = myArguments.getOptionArgument("--include");
            }
            findAndReplace(myProlog, "@@@@", myVarName.c_str());
            findAndReplace(myConcatenator, "@@@", myVarName.c_str());
            findAndReplace(myEpilog, "@@@@", myVarName.c_str());

            if (myArguments.haveOption("--namespace")) {
                myNamespace = myArguments.getOptionArgument("--namespace");
            }
            findAndReplace(myProlog, "||||", myNamespace.c_str());
            findAndReplace(myConcatenator, "||||", myNamespace.c_str());
            findAndReplace(myEpilog, "||||", myNamespace.c_str());

            if (myArguments.haveOption("--newline")) {
                newLinePattern = myArguments.getOptionArgument("--newline");
            }
            if (newLinePattern.size()) {
                ostringstream myNewLine;
                myNewLine << endl;
                findAndReplace(myProlog, newLinePattern.c_str(), myNewLine.str().c_str());
                findAndReplace(myConcatenator, newLinePattern.c_str(), myNewLine.str().c_str());
                findAndReplace(myEpilog, newLinePattern.c_str(), myNewLine.str().c_str());
            }

            string myFileText;
            if (myArguments.haveOption("--inputfile")) {
                myInputFilename = myArguments.getOptionArgument("--inputfile");
				if (!readFile(myInputFilename, myInputLines)) {
                    cerr << "### ERROR: could not read file '" << myInputFilename << "'" << endl;
					return 1;
				}
            }
            if (myArguments.haveOption("--outputfile")) {
                myOutputFilename = myArguments.getOptionArgument("--outputfile");
            }
            if (myArguments.haveOption("--maxlines")) {
                myMaxLines =  asl::as<int>(myArguments.getOptionArgument("--maxlines"));
            }

            if (myArguments.getCount()>0) {
                for (int i = 0; i < myArguments.getCount(); ++i) {
                    myInputLines.push_back(myArguments.getArgument(i));
                }
            }

			std::ostringstream myOutput;

            myOutput << "/* --- Generated by text2src, do not modify --- */" << endl;

            if(myInclude.size() > 0) {
              myOutput <<
                "#include \"" << myInclude << "\"\n" << endl;
            }

			myOutput << myProlog;
			for (unsigned int myLine = 0; myLine < myInputLines.size();++myLine) {
				myOutput << '"';
                std::string myOutputLine = myInputLines[myLine];
                findAndReplace(myOutputLine, "\\", "\\\\");
                findAndReplace(myOutputLine, "\"", "\\\"");
				myOutput << myOutputLine;
#ifdef NO_LAST_NEWLINE
				if (myLine+1 < myInputLines.size()) {
#endif
                    myOutput << "\\n";
#ifdef NO_LAST_NEWLINE
                }
#endif
                myOutput << '"';
                myOutput << endl;
				if (myLine % myMaxLines == 0) {
					myOutput << myConcatenator << endl;
				}
			}
			myOutput << myEpilog << endl;

			if (myOutputFilename.size()) {
				if (!writeFile(myOutputFilename, myOutput.str())) {
                    cerr << "### ERROR: could not write file '" << myInputFilename << "'"<< endl;
					return 1;
				}
			} else {
				cout << myOutput.str();
			}
        }

    } catch (asl::Exception & ex) {
			cerr << "### ERROR: " << ex << endl;
        return -1;
    }
    return 0;
}


#if 0

using namespace asl;

bool isNameChar(char c) {
    return is_alpha(c) || is_digit(c) || is_underscore(c) || c == '.';
};


bool isQuoted(const string & myLine, string::size_type myPosInLine) {
    int myApostrophCount = 0;
    string::size_type myPos = myPosInLine;
    while ((myPos = myLine.rfind('"', myPos)) != string::npos) {
        //cerr << "myPos=" << myPos << endl;
        if (myPos > 0) {
            if ((myLine[myPos-1] == '"') || (myLine[myPos-1] == '\\')) {
                --myPos;
                --myPos;
                continue;
            }
            --myPos;
        } else {
            break;
        }
        ++myApostrophCount;
    }
    return myApostrophCount % 2 != 0;
}

bool isCppComment(const string & myLine, string::size_type myPosInLine) {
    if ((myPosInLine = myLine.rfind("//", myPosInLine)) != string::npos) {
        return !isQuoted(myLine, myPosInLine);
    }
    return false;
}

void getLine(const string & myFile, string::size_type myPosInFile, string & myLine, string::size_type & myPosInLine) {
    string::size_type myLineStart = myFile.rfind('\n',myPosInFile) + 1;
    string::size_type myLineEnd = myFile.find('\n',myPosInFile);
    myLine = myFile.substr(myLineStart, myLineEnd - myLineStart);
    myPosInLine = myPosInFile - myLineStart;
}

void getLine(const string & myFile, string::size_type myPosInFile, string::size_type myMatchSize, string & myLine, string::size_type & myPosInLine) {
    string::size_type myLineStart = myFile.rfind('\n',myPosInFile) + 1;
    string::size_type myLineEnd = myFile.find('\n',myPosInFile+myMatchSize);
    myLine = myFile.substr(myLineStart, myLineEnd - myLineStart);
    myPosInLine = myPosInFile - myLineStart;
}

string::size_type find_C_CommentToken(const string & myFile, string::size_type myPosInFile, const string & myToken) {
    string::size_type myTokenPos = myPosInFile;
    while ((myTokenPos = myFile.rfind(myToken, myTokenPos)) != string::npos) {
        string myLine;
        string::size_type myPosInLine;
        getLine(myFile, myTokenPos, myLine, myPosInLine);
        if (!isCppComment(myLine, myPosInLine) && !isQuoted(myLine, myPosInLine)) {
            // this is our normal positive exit
            break;
        };
        // try again
        --myTokenPos;
    }
    return myTokenPos;
}

bool isCComment(const string & myFile, string::size_type myPosInFile) {
    string::size_type myCommentStartPos = find_C_CommentToken(myFile, myPosInFile, "/*");
    if (myCommentStartPos != string::npos) {
        // we have found a real opening comment now, look for the latest closing comment
        string::size_type myCommentEndPos = find_C_CommentToken(myFile, myPosInFile, "*/");
        if (myCommentEndPos != string::npos) {
            return myCommentEndPos < myCommentStartPos;
        }
        return true;
    }
    return false;
}

int main(int argc, char *argv[]) {

    Arguments::AllowedOption myOptions[] = {
        {"-n", ""},
        {"-v", ""},
        {"-s", ""},
        {"-w", ""},
        {"-q", ""},
        {"-Q", ""},
        {"-c", ""},
        {"-C", ""},
        {"-2", ""},
        {"-p", "%d"},
        {"", ""}
    };

    try {
        Arguments myArguments;
        myArguments.addAllowedOptions(myOptions);

        if (myArguments.parse(argc,argv) && myArguments.getCount()>=3) {
            string searchForString = myArguments.getArgument(0);
            string replaceUntilString;
            unsigned firstFileArgument = 2;
            if (myArguments.haveOption("-2")) {
                replaceUntilString = myArguments.getArgument(1);
                ++firstFileArgument;
            }
            string::size_type matchOnlyPos = string::npos;
            if (myArguments.haveOption("-p")) {
                matchOnlyPos = asl::as<int>(myArguments.getOptionArgument("-p"));
            }


            string replaceWithString = myArguments.getArgument(firstFileArgument-1);
            for (int i = firstFileArgument; i < myArguments.getCount(); ++i) {
                string myFile;
                bool hasBeenModified = false;
                if (readFile(myArguments.getArgument(i), myFile)) {
                    string::size_type matchPos = 0;
                    string::size_type matchLen = searchForString.size();
                    while ((matchPos = myFile.find(searchForString, matchPos)) != string::npos) {
                        if (matchOnlyPos != string::npos) {
                            if (matchPos > matchOnlyPos) {
                               break;
                            }
                            if (matchPos < matchOnlyPos) {
                               continue;
                            }
                       }

                        string::size_type matchPos2 = 0;
                        if (myArguments.haveOption("-2")) {
                            matchPos2 = myFile.find(replaceUntilString, matchPos+searchForString.size());
                            if (matchPos2 == string::npos) {
                                break;
                            }
                            matchLen = matchPos2 - matchPos + replaceUntilString.size();
                        }

                        if (myArguments.haveOption("-w")) {
                            if (matchPos>0) {
                                if (isNameChar(myFile[matchPos-1]) ||
                                        isNameChar(myFile[matchPos+matchLen])) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                        }
                        if (!myArguments.haveOption("-s") ||
                                myArguments.haveOption("-q") ||
                                myArguments.haveOption("-Q") ||
                                myArguments.haveOption("-c") ||
                                myArguments.haveOption("-C"))
                        {

                            string myLine;
                            string::size_type myPosInLine;
                            getLine(myFile, matchPos, matchLen, myLine, myPosInLine);

                            if (myArguments.haveOption("-q")) {
                                if (isQuoted(myLine, myPosInLine)) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                            if (myArguments.haveOption("-Q")) {
                                if (!isQuoted(myLine, myPosInLine)) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                            if (myArguments.haveOption("-c")) {
                                if (isCppComment(myLine, myPosInLine) || isCComment(myFile, matchPos)) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                            if (myArguments.haveOption("-C")) {
                                if (!isCppComment(myLine, myPosInLine) && !isCComment(myFile, matchPos)) {
                                    ++matchPos;
                                    continue;
                                }
                            }

                            if (myArguments.haveOption("-v")) {
                                cerr << "Match in file '" << myArguments.getArgument(i) << "', pos "
                                    << matchPos << ":" << endl;
                                cerr << "Replacing :" << myLine << endl;
                                cerr << "     with :";
                            }
                            myLine.replace(myPosInLine, matchLen, replaceWithString);
                            cout << myLine << endl;
                        }
                        hasBeenModified = true;
                        myFile.replace(matchPos, matchLen, replaceWithString);
                        matchPos+=replaceWithString.size();
                    };
                } else {
                    cerr << "### WARNING: could not read file '" << myArguments.getArgument(i) << endl;
                }
                if (!myArguments.haveOption("-n") && hasBeenModified) {
                    if (!writeFile(myArguments.getArgument(i), myFile)) {
                        cerr << "### WARNING: could not write file '" << myArguments.getArgument(i) << endl;
                    }
                }
            }
        } else {
            cerr << "usage: greplace [options] search-string [until-string] replace-string file [file2...fileN]" << endl;
            cerr << "    -s : enable silent mode " << endl;
            cerr << "    -v : enable verbose mode " << endl;
            cerr << "    -n : do not write back file " << endl;
            cerr << "    -w : match whole words only (no adjacent [A-F,a-f,0-9,_] char" << endl;
            cerr << "    -q : do not match strings between quotes ("")" << endl;
            cerr << "    -Q : do only match strings between quotes ("")" << endl;
            cerr << "    -c : do not match strings in c/c++ comments" << endl;
            cerr << "    -C : do only match strings in c/c++ comments" << endl;
            cerr << "    -2 : replace all characters from <search-string> up to (including) <until-string>" << endl;
            cerr << "    -p <charpos>: replace only match at specified character position" << endl;
            return -1;
        }
    }
    catch (asl::Exception & ex) {
        cerr << "### ERROR: " << ex << endl;
        return -1;
    }
    return 0;
}
#endif
