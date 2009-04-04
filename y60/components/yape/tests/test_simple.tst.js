var t = {};
load("ape_test_simple", t);

// call free function
print( t.func() );

// create instance
var s = new t.simple_class();
// call member function
print( s.mem_func() );

s.data_member = 0;
print( s.data_member );
s.data_member = 23;
print( s.data_member );
s.read_only_member = 15;
print( s.read_only_member );

print( t.simple_class.static_data_member );
t.simple_class.static_data_member = 15;
print( t.simple_class.static_data_member );
print( t.simple_class.static_mem_func() );
print("============================");
for (p in t) {
    print(p);
}
