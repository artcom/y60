#ifndef O_CLIENT_H
#define O_CLIENT_H

#include <OB/CORBA.h>
#include <OB/Util.h>
#include <stdlib.h>
#include <fstream.h>
#include <assert.h>
#include <errno.h>

template <class OBJ> 
class oclient {
public:
    oclient() : status(-1) {};
    
    bool connect(const char* iorfile = 0, int argc=0, char* argv[]=0)
    {
	object = 0;
	try
	{
	    CORBA_ORB_var orb = CORBA_ORB_init(argc, argv);
	    
	    const char* refFile;
	    if (iorfile) refFile = iorfile;
	    else refFile = "CORBA.IOR";
	    ifstream in(refFile);
	    if(in.fail())
	    {
		cerr  << "oclient:" << " can't open `" << refFile << "': "
		     << strerror(errno) << endl;
		return connected();
	    }
	    char objname[1000];
	    in >> objname;
   
//	    cerr << "Looking for object '" << objname << "'"<<endl;

	    CORBA_Object_var obj = orb -> string_to_object(objname);
	    assert(!CORBA_is_nil(obj));
//	    cerr << "got object " << (void*)obj << endl;

	    object = OBJ::_narrow(obj);
	    assert(!CORBA_is_nil(object));
//	    cerr << "oclient: object type ok" << (void*)object << endl;
//	    cerr << "oclient: connection established." << endl;
	    status = 1;
	}
	catch(CORBA_SystemException& ex)
	{
	    cerr << "oclient: a CORBA exception occured:" << endl;
	    OBPrintException(ex);
	    status = 0;
	}
	return connected();
    };
    bool connected() { return status == 1;};
    OBObjVar<OBJ> object;
protected:
    int status;
};
#endif
