var t = {};
load("ape_test_simple", t);

print( t.func() );

var s = new t.simple_class();
print( s.mem_func() );
