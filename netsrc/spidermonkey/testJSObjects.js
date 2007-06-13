const FRAME_COUNT = 100;

function TestClass() {
    this.member = null;
    this.number = 0.0;
    this.string = null;
}

var ourRecursion = 1000;

function addMember(theObject) {
    ourRecursion -= 1;
    if (ourRecursion > 0) {
        theObject.member = new TestClass();
        theObject.number = Math.random();
        theObject.string = "teststring" + Math.random();
        addMember(theObject.member);
    }
}

for (var i = 0; i < FRAME_COUNT; i++) {
    var myTestObject = new TestClass();    
    addMember(myTestObject);
    gc();
}



