set(JSTESTS
Common
Meta
Loader
Rectangle
)

foreach(TEST ${JSTESTS})
  y60_add_jstest(${TEST} spark)
endforeach(TEST)