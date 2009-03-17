var t = {};
load("ape_test_simple", t);

// call free function
print( t.func() );

// create instance
var s = new t.simple_class();
// call member function
print( s.mem_func() );

print( t.simple_class.static_mem_func() );
