package Makedist;
use MakedistTools;

$PRODUCT_NAME   = "watchdog";
$PRODUCT_CODE   = "1435b359-ec2f-40ac-a604-80fe134e52bf";

sub runLocalMakedist($) {    
    my $theDistDir = $_[0];
        
    if ($ourPlatform eq "WIN") {
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcp71.dll", $theDistDir);      
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcr71.dll", $theDistDir);      
    }                
}

1;