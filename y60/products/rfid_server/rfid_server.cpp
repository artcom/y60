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
//   $RCSfile: rfid_server.cpp,v $
//   $Author: janbo $
//   $Revision: 1.8 $
//   $Date: 2004/09/30 12:03:51 $
//
//
//  Description: A command-line tool that sends UDP packets
//  whenver it detects an RFID Tag. (Using Identec's SDK)
//
//
//=============================================================================


#include <iostream>
#include <map>

#include <asl/base/Arguments.h>
#include <asl/base/file_functions.h>
#include <asl/net/UDPSocket.h>
#include <asl/net/address.h>
#include <asl/net/net.h>
#include <asl/base/Time.h>


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <identec/TagArray.h>
#include <identec/BaseReader.h>
#include <identec/iCardIII.h>
#include <identec/BaseTag.h>

#include "TagEvaluator.h"

using namespace std;
using namespace asl;
using namespace inet;

const int DEFAULT_LOCAL_PORT = 6505;
const int DEFAULT_REMOTE_PORT = 6506;
const double DEFAULT_INTERVAL = 1.0;

void printVersion();

asl::Arguments::AllowedOption myOptions[] = {
                                             {"--pcmcia",       "%d"},
                                             {"--power",        "%d"},
                                             {"--hs",           ""  },
                                             {"--expected",     "%d"},
                                             {"--retries",      "%d"},
                                             {"--halflife",     "%f"},
                                             {"--mindb",        "%f"},
                                             {"--minload",      "%f"},
                                             {"--saturation",   "%f"},
                                             {"--localport",    "%d"},
                                             {"--remoteport",   "%d"},
                                             {"--interval",     "%f"},
                                             {"--prefix",       "%s"},
                                             {"--version",      ""  },
                                             {"--help",         ""  },
                                             {"", ""}
                                            };
asl::Arguments myArguments(myOptions);

void
printHelp() {
    // TODO: Please help to maintain this function
    myArguments.printUsage();
    cout << "Command line options:" << endl
         << "  --pcmcia     N           use pcmcia port number N (default is 1)" << endl
         << "  --power      -60 to 30   signal strength of the sender/reader in dbm (default is 6)" << endl
         << "  --hs                     enables high sensitivity" << endl
         << "  --expected   N           number of tags expected in each scan (default is 5)" << endl
         << "  --retries    N           number of retries for all tag communications (default is " << DEFAULT_READER_RETRIES_COUNT << ")"<< endl
         << "  --mindb      N           minimal signal strength in db (default is -60)" << endl
         << "  --halflife   FLOAT       half life of capacitor unloading in seconds (default is 5.0)" << endl
         << "  --minload    FLOAT       minimal capacitor load (default is 1.0)" << endl
         << "  --saturation FLOAT       saturation of virtual capacitors (default is 50.0)" << endl
         << "  --localport  N           local UDP port number (default is " << DEFAULT_LOCAL_PORT << ")"<< endl
         << "  --remoteport N           remote UDP port number (default is " << DEFAULT_REMOTE_PORT << ")"<< endl
         << "  --interval   FLOAT       interval (in seconds) between UDP packet transmissions. (default is " << DEFAULT_INTERVAL << ")"<< endl
         << "  --prefix     STRING      optional header in UDP packet before actual RFID data"<< endl
         << "  --version                print version information and exit" << endl
         << "  --help                   print this help text and exit" << endl
         << endl;
}

int main( int argc, char *argv[])  {
    cout << getFilenamePart(argv[0]) << " copyright (c) 2001-2004 ART+COM AG" << endl;
    string myArgDesc = string("remote_IP. See '") + string(getFilenamePart(argv[0])) +
                              " --help' for more information.";
    myArguments.setShortDescription(myArgDesc.c_str());

    if (!myArguments.parse( argc, argv )) {
        return 1;
    }

    if (myArguments.haveOption("--help")) {
        printHelp();
        return 0;
    }
    if (myArguments.haveOption("--version")) {
        printVersion();
        return 0;
    }

    if (myArguments.getCount() != 1) {
        cerr << "Please specify remote IP." << endl;
        return 1;
    }

    // ---
    // --- take values from Arguments
    // ---
    //string hostSpec = myArguments.getArgument(0);
    string remoteHostSpec = myArguments.getArgument(0);

    //u_long theHost = getHostAddress(hostSpec.c_str());
    u_long theRemoteHost = getHostAddress(remoteHostSpec.c_str());


    int localUDPPort = DEFAULT_LOCAL_PORT;
    if (myArguments.haveOption("--localport")) {
        localUDPPort = asl::as<int>(myArguments.getOptionArgument("--localport"));
    }

    int remoteUDPPort = DEFAULT_REMOTE_PORT;
    if (myArguments.haveOption("--remoteport")) {
        remoteUDPPort = asl::as<int>(myArguments.getOptionArgument("--remoteport"));
    }

    int pcmciaPort = 1;
    if (myArguments.haveOption("--pcmcia")) {
        pcmciaPort = asl::as<int>(myArguments.getOptionArgument("--pcmcia"));
    }

    int power = 6;
    if (myArguments.haveOption("--power")) {
        power = asl::as<int>(myArguments.getOptionArgument("--power"));
    }

    bool highSensitivity = false;
    if (myArguments.haveOption("--hs")) {
        highSensitivity = true;
    }

    int tagsExpected = 5;
    if (myArguments.haveOption("--expected")) {
        tagsExpected = asl::as<int>(myArguments.getOptionArgument("--expected"));
    }

    int retries = DEFAULT_READER_RETRIES_COUNT;
    if (myArguments.haveOption("--retries")) {
        retries = asl::as<int>(myArguments.getOptionArgument("--retries"));
    }

    int min_db = -60;
    if (myArguments.haveOption("--mindb")) {
        min_db = asl::as<int>(myArguments.getOptionArgument("--mindb"));
    }

    float min_load = 1.0f;
    if (myArguments.haveOption("--minload")) {
        min_load = asl::as<float>(myArguments.getOptionArgument("--minload"));
    }

    double half_life = 5.0;
    if (myArguments.haveOption("--halflife")) {
        half_life = asl::as<double>(myArguments.getOptionArgument("--halflife"));
    }

    float saturation = 50.0f;
    if (myArguments.haveOption("--saturation")) {
        saturation = asl::as<float>(myArguments.getOptionArgument("--saturation"));
    }

    double interval = DEFAULT_INTERVAL;
    if (myArguments.haveOption("--interval")) {
        interval = asl::as<double>(myArguments.getOptionArgument("--interval"));
    }

    string prefix = "";
    if (myArguments.haveOption("--prefix")) {
        prefix = asl::as<string>(myArguments.getOptionArgument("--prefix"));
    }


    try {
        // ---
        // --- create reader instance
        // ---
        CBaseReader * myReader = 0;
        // use BasReader's factory method to create an instance of
        // one of the implementation classes.
        myReader = CBaseReader::ConnectiCardPCMCIA(pcmciaPort);
        if (!myReader) {
            cerr <<  "CBaseReader::ConnectiCardPCMCIA returned 0!" << endl;
            return 1;
        }

        // ---
        // --- dump all we know about the reader
        // ---
        int rt = myReader->GetTypeOfReader();
        switch(rt) {
        case CBaseReader::ICARD2:
            cout << "iCARD II";
            break;
        case CBaseReader::ICARD3:
            cout << "iCARD III";
            break;
        default:
            cout << "unknown RFID reader!";
            break;
        }

        short ac = myReader->GetAntennaCount();
        cout << " with " << ac << " antennas." << endl;


        vector<char> buffer(1024);
        if (!myReader->GetInformation(&buffer[0], buffer.size())) {
            cerr <<  "GetInformation failed!" << endl;
            return 1;
        }
        cout << &buffer[0] << endl;

/*
        int prodYear, prodWeek, serialNbr;
        if (!myReader->GetProductionInformation(prodYear, prodWeek, serialNbr)) {
            cerr <<  "GetProductionInformation failed!" << endl;
            return 1;
        }
        cout << "Manufactured in " << prodYear << "(Week #" << prodWeek <<") Serial-No: " << serialNbr << endl;
*/

        // ---
        // --- Configure reader
        // ---

        myReader->SetFrequency(CBaseReader::EU); // returns void

        if (!myReader->SetRetries(retries)) {
            cerr << "SetRetries() failed!" << endl;
            return 1;
        }

        // NOTE: we alwys use the 'medium' profile, the indirection is unused.
        if(!myReader->SetCurrentRFPower(CBaseReader::medium)) {
            cerr <<  "SetCurrentRFPower failed!" << endl;
            return 1;

        }

        // NOTE: here we assume ID Tags are used (in contrast to IQ Tags)
        // Further we assume that we have an iCatdII reader here.
        // As RTTI is switched off in the SDK, we use a c-style cast.
        if(!((CiCardIII*)myReader)->SetiDTagRFPower(CBaseReader::medium, power, highSensitivity)) {
            cerr <<  "SetiDTagRFPower failed!" << endl;
            return 1;
        }

        // ---
        // --- Now keep scanning for tags
        // ---

        // Open the socket
        UDPSocket mySocket(INADDR_ANY, localUDPPort);

        // a ache for the tag eeprom content
        typedef std::map<unsigned, string> NameCache;
        NameCache myNameCache;
        NameCache::iterator nci;

        CTagArray tags;

        TagEvaluator myTagEvaluator(min_load, half_life, saturation);

        Time lastTransmission;
        for(;;) {
            myTagEvaluator.startScan();

            if (!myReader->ScanForTags(
                tags,
                tagsExpected,
                ID,  // tag types to scan for
                0, // options (only used for complete scans)
                CBaseReader::s_quick  // type of scan (quick or complete)
            )) {
                cerr <<  "CiCardIII::ScanForTags failed!" << endl;
            }

            // for each tag found by the scan, we check if we already know its eprom data
            for (int i=0; i<tags.GetSize(); ++i) {
                CBaseTag * myTag = tags[i];
                unsigned uid = myTag->GetID();

                string tagid = "<unknown>";
                vector<char> idbuffer(64);
                if (myTag->GetIDStyleIP(&idbuffer[0], idbuffer.size())) {
                    tagid = &idbuffer[0];
                }

                // NOTE: assumes one antenna only
                // NOTE: is this distance or signal strength?
                int db = myTag->GetLastKnownRangeData();

                db -= min_db;
                if (db > 0) {

                    cout << "Tag" << tagid << " @" << db << ": ";

                    nci = myNameCache.find(uid);
                    if (nci == myNameCache.end()) {

    /*
                        // try to write data
                        bool WriteDataWithCRCAndLength(

                            WORD wStartAddress
                            , BYTE *pbyData,
                            WORD wBytesToWrite,
                            int &iBytesWritten, int nRetries = USE_DEFAULT_RETRIES);
                        myTag->WriteDataWithCRCAndLength(
                                                16, // start address
                                                "Hello World!",
                                                12,
                                                retries);

    */


                        vector<unsigned char> buffer(1024);
                        int bytesRead = buffer.size();
#if 0   // this is because Jürgen Rieker don't want to change his code
                        if (
                            myTag->ReadDataWithCRCAndLength(
                                8, // start address
                                &buffer[0],
                                bytesRead,
                                retries
                            )
                        ) {
                            // successfully read tag data
                            // terminate string
                            buffer[bytesRead] = 0;
                            myNameCache[uid] = string((char*)&buffer[0]);
                            nci = myNameCache.find(uid);
                        }
#else
                        if (
                            myTag->ReadData(
                                8,
                                &buffer[0],
                                56,
                                bytesRead,
                                retries
                            )
                        ) {
                            if (bytesRead == 56) {
                                // successfully read tag data
                                myNameCache[uid] = string((char*)&buffer[0]);
                                nci = myNameCache.find(uid);
                            }
                        }
#endif
                    }
                    if (nci != myNameCache.end()) {
                        cout << nci->second << endl;
                        myTagEvaluator.tagFound(uid, db);
                    } else {
                        cout << "<read error>" << endl;
                    }

                }
            }
            unsigned most_attractive_uid = myTagEvaluator.getMostAttractive();
            string msg  = prefix;
            if (most_attractive_uid) {
                nci = myNameCache.find(most_attractive_uid);
                cout << "Most attractive: " << nci->second << endl;
                msg += nci->second;
            } else {
                cout << "No tag in range" << endl;
                msg += "<unknown>";
            }

            if (Time() - lastTransmission > interval) {
                // send over UDP
                lastTransmission  = Time();
                mySocket.sendTo(theRemoteHost, remoteUDPPort, &msg[0], msg.size());
            }
        }

        myReader->Disconnect();
    }
    catch (asl::Exception & e) {
        cerr << "### ERROR: " << e << endl;
        return 2;
    }
    catch (...) {
        cerr << "### ERROR: Unknown exception occured." << endl;
        return 2;
    }
    return 0;
}


void
printVersion() {
    cout << "CVS $Revision: 1.8 $ $Date: 2004/09/30 12:03:51 $." << endl;
    cout << "Build at " << __DATE__ << " " << __TIME__ << "." << endl;
}
