package Makedist;
use MakedistTools;

$PRODUCT_NAME   = "watchdog";
$PRODUCT_CODE   = "1435b359-ec2f-40ac-a604-80fe134e52bf";

sub runLocalMakedist($) {    
    my $theDistDir = $_[0];

    copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcp71.dll", $theDistDir);      
    copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcr71.dll", $theDistDir);      
    copyIfMoreRecent("$ENV{PRO}/lib/libglib-2.0-0.dll", $theDistDir);      
    copyIfMoreRecent("$ENV{PRO}/lib/libgmodule-2.0-0.dll", $theDistDir);      
    copyIfMoreRecent("$ENV{PRO}/lib/libgobject-2.0-0.dll", $theDistDir);      
    copyIfMoreRecent("$ENV{PRO}/lib/iconv.dll", $theDistDir);      
    copyIfMoreRecent("$ENV{PRO}/lib/intl.dll", $theDistDir);      
}

1;
