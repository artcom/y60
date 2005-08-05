package Makedist;
use MakedistTools;

sub runLocalMakedist($) {
    copyDirectory(".", "$_[0]/shader", (".xml", ".cg"));      
    copyDirectory("shadertex", "$_[0]/shader/shadertex", (".*"));      
}

1;