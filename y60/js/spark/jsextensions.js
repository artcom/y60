Object.prototype.clone = function () {
    var newObj = (this instanceof Array) ? [] : {};
    for (var i in this) {
        if (i === 'clone') {
            continue;
        }
        newObj[i] = this[i];
    }
    return newObj;
};