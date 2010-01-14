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

#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Arguments.h>

#include <iostream>
#include <string>


using namespace std;  // automatically added!
using namespace asl;


void getLine(const string & myFile, int myPosInFile, string & myLine, int & myPosInLine) {
    int myLineStart = myFile.rfind('\n',myPosInFile) + 1;
    int myLineEnd = myFile.find('\n',myPosInFile);
    myLine = myFile.substr(myLineStart, myLineEnd - myLineStart);
    myPosInLine = myPosInFile - myLineStart;
}

bool shortForm = false;
unsigned int maxResultLen = 0;
unsigned int ourCounter = 0;

std::vector<string>
findWordsStartingWithEndof(const vector<string> & theDictionary, const string & theWordEnd, const string & theFullWord, bool noPrinting=false) {
    std::vector<string> myResultList;
    for (unsigned int i = 0; i < theDictionary.size(); ++i) {
        //cout << "Look for *" << theWordEnd<< " at start of " << theDictionary[i] << endl;
        if (theDictionary[i].size() > theWordEnd.size()) {
            if (read_if_string(theDictionary[i], 0, theWordEnd)) {
                string theResult = theFullWord + theDictionary[i].substr(theWordEnd.size());
                if ((maxResultLen==0) || (theResult.size() <= maxResultLen)) {
                    myResultList.push_back(theResult);
                    if (!noPrinting) {
                        cout << theResult;
                        ++ourCounter;
                        if (!shortForm) {
                            cout << " <- " << theFullWord<< "/" << theDictionary[i]   << " - ";
                            cout << "*" << theWordEnd<< ", ";
                        }
                        cout  << endl;
                    }
                }
            }
        }
    }
    return myResultList;
}

std::vector<string>
findWordsEndingWithStartOf(const vector<string> & theDictionary, const string & theWordBegin, const string & theFullWord) {
    std::vector<string> myResultList;
    for (unsigned int i = 0; i < theDictionary.size(); ++i) {
        //cout << "Look for " << theWordBegin<< "* at end of " << theDictionary[i] << endl;
        if (theDictionary[i].size() > theWordBegin.size()) {
            const std::string::size_type myNewPos = read_if_string(theDictionary[i], theDictionary[i].size() - theWordBegin.size(), theWordBegin);
            if (myNewPos == theDictionary[i].size()) {
                string theResult = theDictionary[i].substr(0, theDictionary[i].size() - theWordBegin.size()) + theFullWord;
                if ((maxResultLen==0) || (theResult.size() <= maxResultLen)) {
                    cout << theResult;
                    ++ourCounter;
                    if (!shortForm) {
                        cout << " <- " << theDictionary[i] << "/" << theFullWord << " - ";
                        cout << theWordBegin<< "*, ";
                    }
                    cout << endl;
                }
            }
        }
    }
    return myResultList;
}


int main(int argc, char *argv[]) {

    Arguments::AllowedOption myOptions[] = {
        {"-o", "%s"},
        {"-p", ""},
        {"-d", ""},
        {"-s", ""},
        {"-l", ""},
        {"-e", "%s"},
        {"-b", "%s"},
        {"-E", "%s"},
        {"-B", "%s"},
        {"-m", "%s"},
        {"-O", "%s"},
        {"-k", "%s"},
        {"-K", "%s"},
        {"-x", ""},
        {"", ""}
    };

    try {
        Arguments myArguments;
        myArguments.addAllowedOptions(myOptions);

        if (myArguments.parse(argc,argv) && myArguments.getCount()>=0) {
            string keywords;
            string begin_keywords;
            string end_keywords;
            string dictionary;
            int minOverlap = 2;
            int maxOverlap = 3;
            if (myArguments.haveOption("-o")) {
                minOverlap = asl::as<int>(myArguments.getOptionArgument("-o"));
            }
            if (myArguments.haveOption("-O")) {
                maxOverlap = asl::as<int>(myArguments.getOptionArgument("-O"));
            }
            if (myArguments.haveOption("-m")) {
                maxResultLen = asl::as<int>(myArguments.getOptionArgument("-m"));
            }
            if (myArguments.haveOption("-K")) {
                keywords = readFile(myArguments.getOptionArgument("-K"), keywords);
            }
            if (myArguments.haveOption("-k")) {
                if (keywords.size()) keywords.append("\n");
                keywords.append(myArguments.getOptionArgument("-k"));
            }
            if (myArguments.haveOption("-B")) {
                if (!readFile(myArguments.getOptionArgument("-B"), begin_keywords)) {
                    cerr << "Could not read begin keywords file '" << myArguments.getOptionArgument("-B") << "'" << endl;
                    exit(1);
                }
            }
            if (myArguments.haveOption("-b")) {
                if (begin_keywords.size()) begin_keywords.append("\n");
                begin_keywords.append(myArguments.getOptionArgument("-b"));
            }
             if (myArguments.haveOption("-E")) {
                if (!readFile(myArguments.getOptionArgument("-E"), end_keywords)) {
                    cerr << "Could not read end keywords file '" << myArguments.getOptionArgument("-E") << "'" << endl;
                    exit(1);
                }
            }
            if (myArguments.haveOption("-e")) {
                if (end_keywords.size()) end_keywords.append("\n");
                end_keywords.append(myArguments.getOptionArgument("-e"));
            }
            shortForm = myArguments.haveOption("-s");
            std::vector<string> myKeyWordList = splitString(keywords, " \n\r\t,:");
            std::vector<string> myBeginKeyWordList = splitString(begin_keywords, " \n\r\t,:");
            std::vector<string> myEndKeyWordList = splitString(end_keywords, " \n\r\t,:");
            std::vector<string> myDictWordList;;
            for (int i = 0; i < myArguments.getCount();++i) {
                cerr << "reading dictionary file '" << myArguments.getArgument(i) << "'" << endl;
                if (readFile(myArguments.getArgument(i), dictionary)) {
                    std::vector<string> myNewDict = splitString(dictionary);
                    myDictWordList.insert(myDictWordList.end(), myNewDict.begin(), myNewDict.end());
                } else {
                    cerr << "Could not read dictionary file '" << myArguments.getArgument(i) << "'" << endl;
                    exit(1);
                }
            }

            if (myArguments.haveOption("-l")) {
                cerr << "Converting dictionary and keywords to lower case" << endl;
                for (unsigned int i = 0; i < myKeyWordList.size();++i) {
                    myKeyWordList[i] = toLowerCase(myKeyWordList[i]);
                }
                for (unsigned int i = 0; i < myBeginKeyWordList.size();++i) {
                    myBeginKeyWordList[i] = toLowerCase(myBeginKeyWordList[i]);
                }
                for (unsigned int i = 0; i < myEndKeyWordList.size();++i) {
                    myEndKeyWordList[i] = toLowerCase(myEndKeyWordList[i]);
                }
                for (unsigned int i = 0; i < myDictWordList.size();++i) {
                    myDictWordList[i] = toLowerCase(myDictWordList[i]);
                }
            }
            if (myArguments.haveOption("-p")) {
                cout << "Keywords:" << endl;
                for (unsigned int i = 0; i < myKeyWordList.size();++i) {
                    cout << "'"<<myKeyWordList[i]<<"'" << endl;
                }
                cout << "Begin Keywords:" << endl;
                for (unsigned int i = 0; i < myBeginKeyWordList.size();++i) {
                    cout << "'"<<myBeginKeyWordList[i]<<"'" << endl;
                }
                cout << "End Keywords:" << endl;
                for (unsigned int i = 0; i < myEndKeyWordList.size();++i) {
                    cout << "'"<<myEndKeyWordList[i]<<"'" << endl;
                }
            }
            if (myArguments.haveOption("-d")) {
                cout << "Dictionary:" << endl;
                for (unsigned int i = 0; i < myDictWordList.size();++i) {
                    cout << "'"<<myDictWordList[i]<<"'" << endl;
                }
            }
            if (myArguments.haveOption("-x")) {
                cout << "Begin Keywords:" << endl;
                for (unsigned int i = 0; i < myBeginKeyWordList.size();++i) {
                    for (unsigned int j = 0; j < myEndKeyWordList.size();++j) {
                        cout << myBeginKeyWordList[i]<<myEndKeyWordList[j] << endl;
                    }
                }
                return 0;
            }
            for (unsigned int i = 0; i < myKeyWordList.size();++i) {
                const string & myKeyWord = myKeyWordList[i];
                for (int l = asl::minimum(myKeyWord.size(), (size_t)maxOverlap); l>=minOverlap; --l) {
                    std::string myFront = myKeyWord.substr(0, l);
                    findWordsEndingWithStartOf(myDictWordList, myFront, myKeyWord);
                    std::string myBack = myKeyWord.substr(myKeyWord.size()-l);
                    findWordsStartingWithEndof(myDictWordList, myBack, myKeyWord);
                }
            }
            bool matchBeginAndEnd = myBeginKeyWordList.size() && myEndKeyWordList.size();
            std::vector<string> myBeginMatches;
            for (unsigned int i = 0; i < myBeginKeyWordList.size();++i) {
                const string & myKeyWord = myBeginKeyWordList[i];
                for (int l = asl::minimum(myKeyWord.size(), (size_t)maxOverlap); l>=minOverlap; --l) {
                    std::string myBack = myKeyWord.substr(myKeyWord.size()-l);
                    std::vector<string> myMatches = findWordsStartingWithEndof(myDictWordList, myBack, myKeyWord, matchBeginAndEnd);
                    myBeginMatches.insert(myBeginMatches.end(), myMatches.begin(), myMatches.end());
                 }
            }
            if (matchBeginAndEnd) {
                cerr << "Found " << myBeginMatches.size() << " words in first path." << endl;
                myDictWordList = myBeginMatches;
            }
            std::vector<string> myEndMatches;
            for (unsigned int i = 0; i < myEndKeyWordList.size();++i) {
                const string & myKeyWord = myEndKeyWordList[i];
                for (int l = asl::minimum(myKeyWord.size(), (size_t)maxOverlap); l>=minOverlap; --l) {
                    std::string myFront = myKeyWord.substr(0, l);
                    std::vector<string> myMatches = findWordsEndingWithStartOf(myDictWordList, myFront, myKeyWord);
                    myEndMatches.insert(myEndMatches.end(), myMatches.begin(), myMatches.end());
                }
            }
             cerr << "Found " << ourCounter << " matching words." << endl;
        } else {
            cerr << "usage: namesearch  [-o minoverlap]  [-p] [-l] [-s] [-m maxresultlen] [-k keyword] dictionaryfile [keywordfile]" << endl;
            cerr << "    -o minoverlap  ::  set minimum word overlap (default = 2)" << endl;
            cerr << "    -O maxoverlap  ::  set maximum word overlap (default = 3)" << endl;
            cerr << "    -p             :  print keywords and dictionary" << endl;
            cerr << "    -s             :  short form, print matching words without source" << endl;
            cerr << "    -l             :  treat dictionary and keywords as lower case" << endl;
            cerr << "    -x             :  just print cross product of begin and end keywords" << endl;
            cerr << "    -m maxresultlen:  limit maximum result word length" << endl;
            cerr << "    -k keywords    : use this keywords from command line (comma or colon separated)" << endl;
            cerr << "    -K keywordfile :load keywords from file" << endl;
            cerr << "    -e keywords    : keywords to match at end of word" << endl;
            cerr << "    -E keywordfile : keywords to match at end of word" << endl;
            cerr << "    -b keywords    : keywords to match at begin of word" << endl;
            cerr << "    -B keywordfile : keywords to match at begin of word" << endl;
             return -1;
        }
    }
    catch (asl::Exception & ex) {
        cerr << "### ERROR: " << ex << endl;
        return -1;
    }
    return 0;
}

