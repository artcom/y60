package Makedist;
use MakedistTools;

sub runLocalMakedist($) {
    copyDirectory(".",   "$_[0]", (".js", ".glade", ".gladep"));      
    copyDirectory("GUI", "$_[0]/GUI", (".js"));
    copyDirectory("GUI/Layers", "$_[0]/GUI/Layers", (".js"));
    copyDirectory("GUI/Animation", "$_[0]/GUI/Animation", (".js"));
}
1;
