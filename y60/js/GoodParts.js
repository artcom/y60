//The Good Parts
//some code recomended by Douglas Crockford

Function.prototype.method = function (name, func) {
    if (! this.prototype.hasOwnProperty(name)) {
        this.prototype[name] = func;
    } else {
        print("function "+name+" was already defined");
    }
};

Object.method('superior', function (name) {
    var that = this,
        method = that[name];
    return function (  ) {
        return method.apply(that, arguments);
    };
});

Function.prototype.curry = function (  ) {
    var slice = Array.prototype.slice,
        args = slice.apply(arguments),
        that = this;
    return function (  ) {
        return that.apply(null, args.concat(slice.apply(arguments)));
    };
};

if (typeof Object.beget !== 'function') {
     Object.beget = function (o) {
         var F = function () {};
         F.prototype = o;
         return new F();
     };
}

