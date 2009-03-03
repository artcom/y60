/*
=== Thoughts on Spidermonkey Wrappers =========================================

To bind a native function with spidermonkey one has to provide a function with
the following signature:

JSBool (JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval)

This wrapper function should convert the functions arguments (argv), call the
native function and may store a result in rval. With the current wrapper
infrastructure we implement these functions by hand or macro. However, it would
be nice to save the typing and to somehow create them automatically on the fly.

To bind a native function, a pointer to the wrapper function has to be stored
in a spidermonkey data structure (JSFunctionSpec). Because spidermonkey is
written in plain C, the wrapper has to be a true function and can not be
replaced by a C++ callable object, like a functor. Another option is to use the
same wrapper for all native functions and to perform some kind of dispatch. The
state for this dispatch would have to come from the JS context or some registry
because passing another runtime parameter to the wrapper is not possible, for
obvious reasons. Beside the general ugliness of this approach I think the
performance penalty will be prohibitive. In fact it would look up each function
twice: Once in the JS interpreter and a second time in the wrapper function.

What we really need is a function generator, something that emits real
functions with real addresses and with the correct wrapper signature. Because
the generated functions need to handle the argument conversion and have to call
the native function they require detailed information about the native
function's signature and address.

As it turns out such a function generator is quite simple (at least when using
preprocessor metaprogramming to compensate for missing variadic templates). The
hard part is to wrap the result in a usable interface that allows the user to
bind a native function in (hopefully) a single, human readable line of C++ code.

Let's have a look at a very simple function generator:
*/
#ifdef EX_1

#include <iostream>

template <int N>
struct generator {
    static void hello() { std::cout << "Hello " << N << std::endl; }
};

int main() {
    generator<0>::hello();
    generator<23>::hello();
    return 0;
}

#endif // EX_1
/*
Whenever the generator is instantiated with a new, unique N it emits a new
function. Because it is a static function and it has C calling conventions and
is a perfect match for our purposes. Knowing that function pointers are legal
template arguments (I suspect they count as integral constants, Hendrik is not
so sure) we can easily make the generated function call a user defined one:
*/
#ifdef EX_2

typedef void (*func_ptr_type)();

template <func_ptr_type Func>
struct invoker {
    static void invoke() { Func(); }
};

#include <iostream>

void hello() { std::cout << "Hello "; }
void world() { std::cout << "World." << std::endl; }

int main() {
    invoker<&hello>::invoke();
    invoker<&world>::invoke();
    return 0;
}
#endif // EX_2
/*
This construct has some interesting properties. First it solves the issue of
finding a unique id per function. The function pointer *is* a unique id, at
least in this compilation unit and the rest is up to the linker. Second, I
suspect that passing around a function pointer as a compile time constant
increases chances that the optimizer is able to do its job. And last but not
least, up to this point (without argument handling, that is) it has *zero*
overhead.
However, of course there is a major limitation. This version only allows
functions with signature void (). We can rectify this using another template
parameter, like this:
*/
#ifdef EX_3

template <typename FuncT, FuncT * Func>
struct invoker {
    static void invoke() { Func(); }
};

#include <iostream>

void foo() { std::cout << "foo"; }
int bar() { std::cout << "bar" << std::endl; return 23; }


int main() {
    invoker< void (), foo>::invoke();
    invoker< int (), bar>::invoke();
    
    return 0;
}
#endif // EX_3
/*
Ok, now the function signature is user definable. Please ignore the fact that
it will blow for functions with arguments. If you want to know how that works
look at the real implementation. My point is the user interface. Now that the
user can define the function signature she suddenly *has* to. 

And that is basically where I'm stuck. I'm pretty much convinced that there is
no way to pass around a function pointer as a compile time constant without
passing its type in an earlier template argument. I'm also sure there is no
standard compliant way to generate this information.
*/

/*
=== FAQ ========================================================================

Q: yape? Yet another ... ?
A: No, its pronounced y-ape, like in "big monkeys"

Q: Ah, ok ... y-ape ... ?
A: Because I dont like monkeys.

Q: Ok, wtf *is* this thing??
A: It's an attempt to get c++ to spidermonkey language bindings with a syntax
   similar to boost::python. It currently uses all kinds of practices,
   including dark ones, like preprocessor metaprogramming and uses stuff from
   the future: typeof()

Q: What does it look like?
A: Take a look at hello_world_wrapper.cpp. The file contains a full blown
   plugin that registers a couple of functions. The file test_wrapper.js loads
   the module and calls the functions. It also demonstrates a namespace aware
   replacement for plug().
*/
