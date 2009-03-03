function mkns( name ) { return { namespace : name}; }
function loaded_modules( ns ) {
    print("loaded modules in namespace '" + ns.namespace + "':");
    for (var i in hello.modules) {
        print( "   ", hello.modules[i]);
    }
}

this.hello = mkns("hello");

load("y60HelloWorld", hello);

print( hello.greet() );
hello.greet2();
hello.greet_n(5);
print( hello.greet_more_args("foo", 3) );

try { hello.greet_n("fifteen"); } catch (ex) { print( ex ); }

try { hello.greet_n({foo: 5 }); } catch (ex) { print( ex ); }

try { hello.greet_more_args("foo", 5, 7, 7); } catch (ex) { print( ex ); }

loaded_modules( hello );
