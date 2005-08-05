package Makedist;
use MakedistTools;

$PRODUCT_NAME   = "acgtkshell";
$ICON_FILE      = "acgtkshell.ico";

sub runLocalMakedist($) {
    my $theDistDir = $_[0];

    # Install special files

    if ($ourPlatform eq "WIN") {
        copyIfMoreRecent("$ENV{PRO}/lib/ssleay32.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libeay32.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libatk-1.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGDK-WIN32-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGDK_PIXBUF-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGDKGLEXT-WIN32-1.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGLADE-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGLIB-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGMODULE-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGOBJECT-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGTK-WIN32-2.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBGTKGLEXT-WIN32-1.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBPANGO-1.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/iconv.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/intl.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBPANGOWIN32-1.0-0.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/LIBXML2.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/zlib1.dll", $theDistDir);
        copyIfMoreRecent("$ENV{PRO}/lib/libpng12.dll", $theDistDir);
        copyIfMoreRecent("acgtkshell.ico", $theDistDir);

        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcp71.dll", $theDistDir);
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcr71.dll", $theDistDir);
        
	    copyDirTree("$ENV{PRO}/lib/gtk-2.0", $theDistDir."lib");
	    copyDirTree("$ENV{PRO}/lib/pango", $theDistDir."lib");
	    copyDirTree("$ENV{PRO}/etc", $theDistDir);
    }

    # Install dependencies
    depends("Y60/js", "Y60/shader", "Y60/components/WindowCWFilter");

}

sub runLocalMakeInstaller() {
    addShortcut("rendergirl",
                "-I [TARGETDIR]/js [TARGETDIR]/js/sv.js \"%1\" [TARGETDIR]/shader/shaderlibrary_nocg.xml",
                "acgtkshell.exe");

    addFileType("rendergirl model", "x60;m60",
                "-I [TARGETDIR]/js [TARGETDIR]/js/sv.js \"%1\" [TARGETDIR]/shader/shaderlibrary_nocg.xml",
                "acgtkshell.exe");
}

1;
