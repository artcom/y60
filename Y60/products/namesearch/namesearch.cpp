/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: greplace.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: string replace utility
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <asl/file_functions.h>
#include <asl/string_functions.h>
#include <asl/Arguments.h>

#include <iostream>
#include <string>


using namespace std;  // automatically added!
using namespace asl;

bool isNameChar(char c) {
    return is_alpha(c) || is_digit(c) || is_underscore(c) || c == '.';
};


bool isQuoted(const string & myLine, int myPosInLine) {
    int myApostrophCount = 0;
    int myPos = myPosInLine;
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

bool isCppComment(const string & myLine, int myPosInLine) {
    if ((myPosInLine = myLine.rfind("//", myPosInLine)) != string::npos) {
        return !isQuoted(myLine, myPosInLine);
    }
    return false;
}

void getLine(const string & myFile, int myPosInFile, string & myLine, int & myPosInLine) {
    int myLineStart = myFile.rfind('\n',myPosInFile) + 1;
    int myLineEnd = myFile.find('\n',myPosInFile);
    myLine = myFile.substr(myLineStart, myLineEnd - myLineStart); 
    myPosInLine = myPosInFile - myLineStart;
}

int find_C_CommentToken(const string & myFile, int myPosInFile, const string & myToken) {
    int myTokenPos = myPosInFile;
    while ((myTokenPos = myFile.rfind(myToken, myTokenPos)) != string::npos) {
        string myLine;
        int myPosInLine;
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

bool isCComment(const string & myFile, int myPosInFile) {
    int myCommentStartPos = find_C_CommentToken(myFile, myPosInFile, "/*");
    if (myCommentStartPos != string::npos) {
        // we have found a real opening comment now, look for the latest closing comment
        int myCommentEndPos = find_C_CommentToken(myFile, myPosInFile, "*/");
        if (myCommentEndPos != string::npos) {
            return myCommentEndPos < myCommentStartPos;
        }
        return true;
    }
    return false;
}

bool shortForm = false;
int maxResultLen = 0;

void findWordsStartingWithEndof(const vector<string> & theDictionary, const string & theWordEnd, const string & theFullWord) {
    for (int i = 0; i < theDictionary.size(); ++i) {
        //cout << "Look for *" << theWordEnd<< " at start of " << theDictionary[i] << endl;
        if (theDictionary[i].size() > theWordEnd.size()) {
            if (read_if_string(theDictionary[i], 0, theWordEnd)) {
                string theResult = theFullWord + theDictionary[i].substr(theWordEnd.size());
                if (!maxResultLen || theResult.size() <= maxResultLen) {
                    if (!shortForm) {
                        cout << "*" << theWordEnd<< ", ";
                        cout << theFullWord<< "/" << theDictionary[i]   << " -> ";
                    }
                    cout << theResult << endl;
                }
            }
        }
    }
}

void findWordsEndingWithStartOf(const vector<string> & theDictionary, const string & theWordBegin, const string & theFullWord) {
    for (int i = 0; i < theDictionary.size(); ++i) {
        //cout << "Look for " << theWordBegin<< "* at end of " << theDictionary[i] << endl;
        if (theDictionary[i].size() > theWordBegin.size()) {
            int myNewPos = read_if_string(theDictionary[i], theDictionary[i].size() - theWordBegin.size(), theWordBegin);
            if (myNewPos == theDictionary[i].size()) {
                string theResult = theDictionary[i].substr(0, theDictionary[i].size() - theWordBegin.size()) + theFullWord;
                if (!maxResultLen || theResult.size() <= maxResultLen) {
                    if (!shortForm) {
                        cout << theWordBegin<< "*, ";
                        cout << theDictionary[i] << "/" << theFullWord << " -> ";
                    }
                    cout << theResult << endl;
                }
            }
        }
    }
}


int main(int argc, char *argv[]) {

    Arguments::AllowedOption myOptions[] = {
        {"-o", "%s"},
        {"-p", ""},
        {"-s", ""},
        {"-l", ""},
        {"-m", "%s"},
        {"-k", "%s"},
        {"", ""}
    };

    try {
        Arguments myArguments;
        myArguments.addAllowedOptions(myOptions);

        if (myArguments.parse(argc,argv) && myArguments.getCount()>=1) {
            
            string keywords;
            string dictionary;
            int minOverlap = 2;
            if (myArguments.haveOption("-o")) {
                minOverlap = asl::as<int>(myArguments.getOptionArgument("-o"));
            }
            if (myArguments.haveOption("-m")) {
                maxResultLen = asl::as<int>(myArguments.getOptionArgument("-m"));
            }
            if (myArguments.haveOption("-m")) {
                keywords = myArguments.getOptionArgument("-k");
            }
            shortForm = myArguments.haveOption("-s");
            if (readFile(myArguments.getArgument(0), dictionary)) {
                if (keywords.size() || readFile(myArguments.getArgument(1), keywords)) {
                    std::vector<string> myKeyWordList = splitString(keywords, " \n\r\t,:");
                    std::vector<string> myDictWordList = splitString(dictionary);
                    if (myArguments.haveOption("-p")) {
                        cout << "Keywords:" << endl;
                        for (int i = 0; i < myKeyWordList.size();++i) {
                            cout << "'"<<myKeyWordList[i]<<"'" << endl;
                        }
                        cout << "Dictionary:" << endl;
                        for (int i = 0; i < myDictWordList.size();++i) {
                            cout << "'"<<myDictWordList[i]<<"'" << endl;
                        }
                    }
                    if (myArguments.haveOption("-l")) {
                        cerr << "Converting dictinary and keywords to lower case" << endl;
                        for (int i = 0; i < myKeyWordList.size();++i) {
                            myKeyWordList[i] = toLowerCase(myKeyWordList[i]);
                        }
                        for (int i = 0; i < myDictWordList.size();++i) {
                            myDictWordList[i] = toLowerCase(myDictWordList[i]);
                        }
                    }
                   for (int i = 0; i < myKeyWordList.size();++i) {
                        const string & myKeyWord = myKeyWordList[i];
                        for (int l = myKeyWord.size()-1; l>=minOverlap; --l) {
                            std::string myFront = myKeyWord.substr(0, l);
                            findWordsEndingWithStartOf(myDictWordList, myFront, myKeyWord);
                            std::string myBack = myKeyWord.substr(myKeyWord.size()-l);
                            findWordsStartingWithEndof(myDictWordList, myBack, myKeyWord);
                         }
                    }
                }
                
            }
        } else {
            cerr << "usage: namesearch  [-o minoverlap]  [-p] [-l] [-s] [-m maxresultlen] [-k keyword] dictionaryfile [keywordfile]" << endl;
            cerr << "    -o minoverlap :  set minimum word overlap (default = 2)" << endl;
            cerr << "    -p :  print keywords and dictionary" << endl;
            cerr << "    -s :  print only matching words" << endl;
            cerr << "    -l :  treat dictionary and keywords as lower case" << endl;
            cerr << "    -m maxresultlen:  limit maximum result word length" << endl;
            cerr << "    -k keywords:  use this keywords from command line (comma or colon separated)" << endl;
            return -1;
        }
    }
    catch (asl::Exception & ex) {
        cerr << "### ERROR: " << ex << endl;
        return -1;
    }
    return 0;
}


#if 0                
                string myFile;
                bool hasBeenModified = false;
                if (readFile(myArguments.getArgument(i), myFile)) {
                    int matchPos = 0; 
                    while ((matchPos = myFile.find(searchForString, matchPos)) != string::npos) {
                        if (myArguments.haveOption("-w")) {
                            if (matchPos>0) {
                                if (isNameChar(myFile[matchPos-1]) || 
                                        isNameChar(myFile[matchPos+searchForString.size()])) {
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
                            int myPosInLine;
                            getLine(myFile, matchPos, myLine, myPosInLine);

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
                            myLine.replace(myPosInLine, searchForString.size(), replaceWithString);
                            cout << myLine << endl;
                        } 
                        hasBeenModified = true;
                        myFile.replace(matchPos, searchForString.size(), replaceWithString);
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
#endif
