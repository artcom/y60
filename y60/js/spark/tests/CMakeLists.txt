set(JSTESTS
Common
)

foreach(TEST ${JSTESTS})
  y60_add_jstest(${TEST} spark)
endforeach(TEST)