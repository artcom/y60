use("UnitTest.js");

function ape_function_test() {
    this.Constructor( this, "ape_function_test");
}

ape_function_test.prototype.Constructor = function(obj, name) {
    UnitTest.prototype.Constructor(obj, name);

    obj.run = function() {
        var t = {};

        load("ape_test_functions", t);

        ENSURE(undefined === t.void_func());
        ENSURE("const_char_ptr_func()" === t.const_char_ptr_func());
        ENSURE(23 === t.int_func_int(23));
        ENSURE(42 === t.int_func_intcr(42));
        ENSURE(undefined === t.void_func_intcr_stringcr(42,"answer: "));

        for ( var f in t ) {
            print( f );
        }
    }
}


var test_name = "functions.js";
var suite = new UnitTestSuite(test_name);

suite.addTest(new ape_function_test());
suite.run();

print(">> Finished test suite '" + test_name + "', return status = "
        + suite.returnStatus() + "");
exit( suite.returnStatus() );



/*
hello.greet2();
hello.greet_n(5);
print( hello.greet_more_args("foo", 3) );

try { hello.greet_n("fifteen"); } catch (ex) { print( ex ); }

try { hello.greet_n({foo: 5 }); } catch (ex) { print( ex ); }

try { hello.greet_more_args("foo", 5, 7, 7); } catch (ex) { print( ex ); }

var my_foo = new hello.foo();
print( my_foo.bar() );

loaded_modules( hello );
*/
