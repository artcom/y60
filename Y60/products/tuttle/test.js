
function testA() {
   print("Test A");
}

function testB() {
   print("Test B");
}

function testC(theArgument) {
   print("Test C: " + theArgument);
}

function trapDebugger() {
   debugger;
}

testA();
testB();
testC(true);
testC(false);

