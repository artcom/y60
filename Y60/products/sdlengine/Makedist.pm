package Makedist;
use MakedistTools;

$PRODUCT_NAME   = "Y60-SDK";
$ICON_FILE      = "rendergirl.ico";
$PRODUCT_CODE   = "6e6f3569-d80d-4e2e-855c-7be2d3b980ed";

sub runLocalMakedist($) {
    my $theDistDir = $_[0];

    # Install special files
    copyIfMoreRecent("rendergirl.ico", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/lib/libeay32.dll", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/lib/ssleay32.dll", $theDistDir);

    if ($ourPlatform eq "WIN") {
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcp71.dll", $theDistDir);
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcr71.dll", $theDistDir);
    }

    # Install dependencies
    depends("Y60/js", "Y60/shader", "y60/components", "Y60/products/watchdog/app");

}

sub runLocalMakeInstaller() {
    my $mySceneViewerArguments = "--pause-on-error -I \"[TARGETDIR];\" \"[TARGETDIR]/sv.js\" \"[TARGETDIR]/shaderlibrary.xml\" \"%1%\"";
    my $myImageViewerArguments = "--pause-on-error -I \"[TARGETDIR];\" \"[TARGETDIR]/ImageViewer.js\" \"[TARGETDIR]/shaderlibrary.xml\" \"%1%\"";

    addShortcut("Y60 Viewer", $mySceneViewerArguments, "y60.exe");
    addFileType("Rendergirl Model", "x60", $mySceneViewerArguments, "y60.exe");
    addFileType("Rendergirl Binary Model", "b60", $mySceneViewerArguments, "y60.exe");
    addFileType("Rendergirl Movie", "m60", $myImageViewerArguments, "y60.exe");
    addFileType("Rendergirl Image", "i60", $myImageViewerArguments,  "y60.exe");
}

1;