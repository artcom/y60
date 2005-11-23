/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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
#include <string>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>        
#include <errno.h>
#include <libgen.h>  


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

    char * myDirectoryName = strdup(argv[0]);
    char * myProgramName = strdup(argv[0]);
    myDirectoryName = getDirectoryPart(myDirectoryName);
    myProgramName = getFilenamePart(myProgramName);
    cerr << myProgramName << " started in directory " << myDirectoryName << endl;

    string myScriptName = string("../../") + myProgramName + ".sh"; 

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
