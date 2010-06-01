/* This is a text rendering facility for spark
 * It supports autosizing: the quad has the appropriate size after
 * setting the 'text' property.
 * NOTE: this className is deprecated use 'Text' spark class
 */
spark.NewText = spark.ComponentClass("NewText");

spark.NewText.Constructor = function(Protected) {
    Logger.warning("'NewText' is deprecated use 'Text' (is the old NewText)");
    this.Inherit(spark.Text);
}

