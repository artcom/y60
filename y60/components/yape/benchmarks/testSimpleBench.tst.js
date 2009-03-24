load("ape_bench_ape_binding", this);
plug("ape_bench_classic_binding");

var num_iterations = 5000000;
//var num_iterations = 50000;

function fmt(num) {
    return num.toFixed(4);
};

var results = {};
function report( expr, duration, iterations, binding, num_args ) {
    var millions_per_second = iterations/duration/1e3;
    print(iterations.toString() + " times " + expr + " took " +
            fmt(duration) + "msec. (" +
            fmt(millions_per_second) + " mio. per sec)");
    if ( ! (num_args.toString() in results)) {
        results[num_args.toString()] = {};
    }
    results[num_args.toString()][binding] = { mps: millions_per_second}; 
}

function create_csvs() {
    var str = new String();
    for(b in results["2"]) {
        str += b + ",";
    }
    str += "\n";
    for(var num_args in results) {
        //str += num_args.toString();
        for( var b in results[num_args]) {
            str +=  results[num_args][b].mps + ", ";
        }
        str += "\n";
    }
    writeStringToFile("benchmark.csv", str);
}

gc();
//###################################
//# ape_add(i,1)

var i = 0
var n = num_iterations;

var start = millisec()

while (i < n) i=ape_add(i, 1)

var stop = millisec()
var duration = stop - start

report( "i = ape_add(i,1)", duration, n, "ape", 2);
gc();

//###################################
//# ape_add8(i,1,1,...)

i = 0
n = num_iterations;

start = millisec()

while (i < 7*n) i=ape_add8(i, 1, 1, 1, 1, 1, 1, 1);

stop = millisec()
duration = stop - start

report( "i = ape_add8(i,1,1,...)", duration, n, "ape", 8);
gc();


//###################################
//# ape_o.add(i,1)

var ape_o = new ape_adder();
ape_o.set(0);

i = 0
n = num_iterations;

start = millisec()

while (i < n) i=ape_o.add(i, 1);

stop = millisec()
duration = stop - start

report( "i = ape_o.add(i,1)", duration, n, "ape", 2);
gc();

//###################################
//# ape_o.add8(i,1,1...)

i = 0
n = num_iterations;

start = millisec()

while (i < 7*n) i=ape_o.add8(i, 1, 1, 1, 1, 1, 1, 1);

stop = millisec()
duration = stop - start

report( "i = ape_o.add8(i,1,1,...)", duration, n, "ape", 8);
gc();

//###################################
//# raw_add(i,1)

i = 0
n = num_iterations;

start = millisec()

while (i < n) i=raw_add(i, 1)

stop = millisec()
duration = stop - start

report( "i = raw_add(i,1)", duration, n, "classic", 2);
gc();

//###################################
//# raw_add8(i,1,1...)

i = 0
n = num_iterations;

start = millisec()

while (i < 7*n) i=raw_add8(i, 1, 1, 1, 1, 1, 1, 1);

stop = millisec()
duration = stop - start

report( "i = raw_add8(i,1,1,...)", duration, n, "classic", 8);
gc();

//###################################
//# jsw_o.add8(i,1,1...)

var jsw_o = new ClassicAdder();
jsw_o.set(0);

i = 0
n = num_iterations;

start = millisec()

while (i < 7*n) i=jsw_o.add8(i, 1, 1, 1, 1, 1, 1, 1);

stop = millisec()
duration = stop - start

report( "i = jsw_o.add8(i,1,1,...)", duration, n, "classic", 8);
gc();

//###################################
//# jsw_o.add(i,1)

var jsw_o = new ClassicAdder();
jsw_o.set(0);

i = 0
n = num_iterations;

start = millisec()

while (i < n) i=jsw_o.add(i, 1);

stop = millisec()
duration = stop - start

report( "i = jsw_o.add(i,1)", duration, n, "classic", 8);
gc();

//###################################
//# js_add(i,1)
i = 0
n = num_iterations;

function js_add(a, b) {
  return a + b;
};

start = millisec()

while (i < n) i=js_add(i, 1)

stop = millisec()
duration = stop - start

report( "i = js_add(i,1)", duration, n, "javascript", 2);
gc();

//###################################
//# js_add8(i,1,1...)

function js_add8(a1, a2, a3, a4, a5, a6, a7, a8) {
  return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
}

i = 0
n = num_iterations;

start = millisec()

while (i < 7*n) i=js_add8(i, 1, 1, 1, 1, 1, 1, 1);

stop = millisec()
duration = stop - start

report( "i = js_add8(i,1,1,...)", duration, n, "javascript", 8);
gc();

create_csvs();
