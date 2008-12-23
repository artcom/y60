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
//    $RCSfile: testShellScript.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test template file - change ShellScript to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifdef AC_BUILT_WITH_CMAKE
#include "aslbase_paths.h"
#endif

#include <string>
#include <iostream>
#include <cstring>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>        
#include <errno.h>
#include <libgen.h>  

#include "file_functions.h"


using namespace std;  // automatically added!



int execute(char * command) {
    int pid, status;

    if (command == 0)
        return 1;
    pid = fork();
    if (pid == -1)
        return -1;

    if (pid == 0) {
        char * const argv[4] = { "sh", "-c", command, 0};
        execve("/bin/sh", argv, 0);
        exit(127);
    }
    do {
        if (waitpid(pid, &status, 0) == -1) {
            if (errno != EINTR)
                return -1;
        } else
            return status;
    } while(1);
}

int main(int argc, char *argv[]) {

    cerr << argv[0] << " launched with argc = " << argc << endl;

    string myDirectoryName = asl::getDirectoryPart(argv[0]);
    string myProgramName = asl::getFilenamePart(argv[0]);
    cerr << myProgramName << " started in directory " << myDirectoryName << endl;

    string myScriptName =
#ifdef AC_BUILT_WITH_CMAKE
        string(CMAKE_CURRENT_SOURCE_DIR) + theDirectorySeparator
#else
        string("../../")
#endif
        + myProgramName + ".sh"; 

    cerr << myProgramName << ": executing script '" << myScriptName << "'" << endl;

    char * myCommand = strdup(myScriptName.c_str());
    int myStatus = execute(myCommand);
    free(myCommand);

    cerr << myProgramName << " finished with return code " << myStatus << endl;

    if (myStatus) {
        return -1;
    } else {
        return 0;
    }
}
