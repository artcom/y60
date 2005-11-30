(NR == 2 && /\/---/) {
    Special = 1;
    CommentExists = 1;
    print;
  }
  
(NR == 1 && /\/\/ Copyright/) {
    Special = 1;
    print ("/*");    
    print ("/--------------------------------------------------------------------");
    print ("|");    
    print ("|      $Id: addcvsc.awk,v 1.1 1999/12/08 15:57:04 Ulrich von Zadow Exp $");
    print ("|");    
    print ("|      Copyright (c) 1996-1998 Ulrich von Zadow");    
    print ("|");    
    print ("\\--------------------------------------------------------------------");    
    print ("*/");    
  }
  
(NR == 4 && CommentExists == 1) {
    print ("|      $Id: addcvsc.awk,v 1.1 1999/12/08 15:57:04 Ulrich von Zadow Exp $");
  }

(Special != 1) { print; }

(Special == 1) { Special = 0 }
  
END {
    print ("/*");    
    print ("/--------------------------------------------------------------------");
    print ("|");    
    print ("|      $Log: addcvsc.awk,v $
    print ("|      Revision 1.1  1999/12/08 15:57:04  Ulrich von Zadow
    print ("|      no message
    print ("|");
    print ("|");    
    print ("\\--------------------------------------------------------------------");    
    print ("*/");    
  }
