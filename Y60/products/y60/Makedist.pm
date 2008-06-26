package Makedist;
use MakedistTools;

$PRODUCT_NAME   = "sdlengine";
$ICON_FILE      = "rendergirl.ico";
$PRODUCT_CODE   = "6e6f3569-d80d-4e2e-855c-7be2d3b980ed";
$VERSION        = "0.1"; # XXX should be revision?!?

sub runLocalMakeTarball($) {
}

sub runLocalMakedist($) {
    my $theDistDir = $_[0];

    # Install special files
    copyIfMoreRecent("rendergirl.ico", $theDistDir);

    # Add some usefull scripts
    copyIfMoreRecent("iv", $theDistDir);
    copyIfMoreRecent("sv", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/build/bin/ue", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/build/bin/show.sh", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/build/bin/make_project.sh", $theDistDir);

    if ($ourPlatform eq "WIN") {
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcp71.dll", $theDistDir);
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcr71.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libeay32.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/ssleay32.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libglib-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/iconv.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/intl.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libgobject-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libgmodule-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/freetype6.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/zlib1.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libcairo-2.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libpng12.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libfontconfig-1.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/build/Installer/uuidgen.exe", $theDistDir);
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
