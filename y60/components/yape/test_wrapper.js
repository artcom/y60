function mkns( name ) { return { namespace : name }; }

var hello = mkns("hello");

load("y60HelloWorld", hello);

print( hello.greet() );
hello.greet2();
print("with args");
hello.greet_n(5);
print("done");
