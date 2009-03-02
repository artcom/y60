function mkns( name ) { return { namespace : name }; }

this.hello = mkns("hello");

load("y60HelloWorld", hello);

print( hello.greet() );
hello.greet2();
hello.greet_n(5);
print( hello.greet_more_args("foo", 3) );

try {
    hello.greet_n("fifteen");
} catch (ex) { print( ex ); }

try {
    hello.greet_n({foo: 5 });
} catch (ex) { print( ex ); }

try {
    hello.greet_n( 5, 7, 8);
} catch (ex) { print( ex ); }

for ( var p in this ) {
    print(p, " = ", this[p]);
}

print( this.all_modules );
