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
    depends("Y60/js", "Y60/shader", "Y60/components", "Y60/products/GenCompressedTex", "Y60/products/GenMovie",
            "Y60/products/watchdog/app");

}

sub runLocalMakeInstaller() {
    addShortcut("rendergirl",
                "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]js\" \"[TARGETDIR]js/sv.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"",
                "acxpshell.exe");

    addFileType("rendergirl model", "x60",
                "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]js\" \"[TARGETDIR]js/sv.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"",
                "acxpshell.exe");

    addFileType("rendergirl binary model", "b60",
                "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]js\" \"[TARGETDIR]js/sv.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"",
                "acxpshell.exe");

    addFileType("rendergirl movie", "m60",
                "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]js\" \"[TARGETDIR]js/ImageViewer.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"",
                "acxpshell.exe");

    addFileType("rendergirl image", "i60",
                "--pause-on-error -I \"[TARGETDIR];[TARGETDIR]js\" \"[TARGETDIR]js/ImageViewer.js\" \"[TARGETDIR]shader/shaderlibrary.xml\" \"%1\"",
                "acxpshell.exe");
}

1;