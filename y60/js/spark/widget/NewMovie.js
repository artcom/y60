/**
 * A simple movie player.
 *
 * NOTE: this ClassName is deprecated use Movie instead
 */
spark.NewMovie = spark.ComponentClass("NewMovie");
spark.NewMovie.Constructor = function(Protected) {
    Logger.warning("NewMovie is deprecated use Movie instead (is the old NewMovie)");
    this.Inherit(spark.Movie);
}

