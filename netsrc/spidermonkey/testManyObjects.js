FRAME_COUNT = 100;

function TestClass() {
    this.member = null;
    this.vector = null;
}

var ourRecursion = 1000;

function addMember(theObject) {
    ourRecursion -= 1;
    if (ourRecursion > 0) {
        theObject.member = new TestClass();
        theObject.vector = new Vector3f(Math.random(),
                                        Math.random(),
                                        Math.random());
        addMember(theObject.member);
    }
}

for (var i = 0; i < FRAME_COUNT; i++) {
    var myTestObject = new TestClass();    
    addMember(myTestObject);
}
