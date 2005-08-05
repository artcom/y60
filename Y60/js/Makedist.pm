package Makedist;
use MakedistTools;

sub runLocalMakedist($) {
    copyDirectory(".", "$_[0]/js", (".js", ".glade", ".gladep"));      
}

1;