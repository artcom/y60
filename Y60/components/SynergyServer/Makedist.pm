package Makedist;
use MakedistTools;

$VERSION = substr(`svn info | grep Revision`,10,-1);
print( "Setting dist-version to svn revision '$VERSION'.\n" );

$DEB_POSTINST_SCRIPT = "$ENV{'PRO'}/src/Y60/components/SynergyServer/deb_postinst_script";
$PACKAGE_TYPE = 1;
$PRODUCT_NAME = "SynergyServer";

sub runLocalMakedist($) {

    my $theDistDir = $_[0];

# Install special files
    copyIfMoreRecent("settings.xml", $theDistDir);

    copyIfMoreRecent("$ENV{PRO}/lib/libASSEventSourceOPT.so", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/lib/liby60ASSCoreOPT.so", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/lib/libEventLoopOPT.so", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/lib/libSynergyServerOPT.so", $theDistDir);
    copyIfMoreRecent("$ENV{PRO}/lib/liboscpackOPT.so", $theDistDir);
    copyIfMoreRecent("run_nowindow.js", $theDistDir);
    copyIfMoreRecent("start_dist.sh", $theDistDir);
    depends("Y60/products/y60");
}

sub runLocalMakeInstaller() {
}
1;
