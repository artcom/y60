/*jslint*/
/*globals spark*/

spark.Stage = spark.AbstractClass("Stage");
spark.Stage.Constructor = function (Protected) {
    var Public = this;
    Public.Inherit(spark.Widget);
};