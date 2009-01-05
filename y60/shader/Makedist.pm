package Makedist;
use MakedistTools;

sub runLocalMakedist($) {
    copyDirectory(".", "$_[0]", (".xml", ".cg"));      
    copyDirectory("shadertex", "$_[0]/shadertex", (".*"));      
}

1;
