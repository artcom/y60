package Makedist;
use MakedistTools;

$PRODUCT_NAME   = "rendergirl";
$ICON_FILE      = "rendergirl.ico";
$PRODUCT_CODE   = "1cbd388d-11c1-49ae-9059-a27d6ee615d0";

sub runLocalMakedist($) {
    my $theDistDir = $_[0];

    # Install special files
    # Install special files
    copyIfMoreRecent("rendergirl.ico", $theDistDir);
    copyDirTree("GLADE", $theDistDir);
    copyDirTree("SCRIPTS", $theDistDir);

    # Install dependencies
    depends("Y60/products/acgtkshell");
}

sub runLocalMakeInstaller() {
    my $myArguments = "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]SCRIPTS;[TARGETDIR]js\" \"[TARGETDIR]SCRIPTS/rendergirl.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"";
    my $myFlighrecorderArguments = "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]SCRIPTS;[TARGETDIR]js\" \"[TARGETDIR]SCRIPTS/FlightRecorder.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"";
    addShortcut("rendergirl", $myArguments, "acgtkshell.exe");
    addFileType("rendergirl model", "x60", $myArguments, "acgtkshell.exe");
    addFileType("rendergirl binary model", "b60", $myArguments, "acgtkshell.exe");
    #addShortcut("flighrecorder", $myFlighrecorderArguments, "acgtkshell.exe");
    addFileType("rendergirl flight", "f60", $myFlighrecorderArguments, "acgtkshell.exe");
}

1;
