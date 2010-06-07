/*remove offending file*/
/*
    The Object.prototype should NEVER be extended!!
    clone should be a function if this is required at all.
*/
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