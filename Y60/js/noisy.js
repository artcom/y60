
const HasGetter = new Components.Constructor(
    "@mozilla.org/js/xpc/test/CallJSNoisy;1",
    "nsIXPCTestCallJSNoisy");



function test() {
    var hasGetter = new HasGetter();
    var noisy = hasGetter.noisy;
    noisy.squawk();
}

test();
dump("calling gc()\n");
gc();
dump("done\n"); 

