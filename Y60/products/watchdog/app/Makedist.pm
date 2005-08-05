package Makedist;
use MakedistTools;

sub runLocalMakedist($) {    
    my $theDistDir = $_[0];
        
    if ($ourPlatform eq "WIN") {
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcp71.dll", $theDistDir);      
        copyIfMoreRecent("$ENV{SYSTEMROOT}/system32/msvcr71.dll", $theDistDir);      
    }                
}

1;