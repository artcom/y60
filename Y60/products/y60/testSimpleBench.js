print("Pavel's trivial javascript benchmark V.0.1");

function my_add(a, b) {
  return a + b;
};

function fullMatMult(a, b) {
  c = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
  c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
  c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
  c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
  c[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

  c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
  c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
  c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
  c[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

  c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
  c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
  c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
  c[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

  c[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
  c[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
  c[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
  c[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
  return c;
}
function fullMatMult2(a, b) {
    c = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]];
    for (var i = 0; i < 4; ++i) {
        for (var j = 0; j < 4; ++j) {
            c[j][i] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
        }
    }
    return c;
}

//###################################
//# i = i + 1

var i = 0
var n = 5000000

var start = millisec() 

for (i = 0; i < n; i+=1) {}

//while (i < n) i+=1;

var stop = millisec()
var duration = stop - start

print(n.toString() + " x {i+=1} took "+ duration +"msec. ("+ n/duration/1e3 +" mio. per sec)");

//###################################
//# my_add(i,1)
i = 0
n = 5000000

start = millisec()

while (i < n) i=my_add(i, 1)

stop = millisec()
duration = stop - start

print(n.toString() + " i = my_add(i,1) took "+ duration +"msec. ("+ n/duration/1e3 +" mio. per sec)");

//###################################
//# a = fullMatMult(b, c)
i = 0
n = 50000

var a = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
var b = [[2,0,0,0],[0,2,0,0],[0,0,2,0],[0,0,0,2]]

start = millisec()

for (i = 0; i < n; i+=1) {
    var c = fullMatMult2(a, b)
}
stop = millisec()
duration = stop - start

print(n.toString() + " j = fullMatMult took "+ duration +"msec. ("+ n/duration * 1000+" per sec)");

