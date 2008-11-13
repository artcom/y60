#!/bin/bash
FAILURES="FAILED"
for i in correct/*.js; do 
    echo $i;
    echo -n interp:' '
    INTERP=`$PRO/bin/tracemonkey -f $i`
    echo $INTERP' '
    echo -n jit:' '
    JIT=`$PRO/bin/tracemonkey -j -f $i`
    echo $JIT' '
    if [ "$INTERP" != "true" -o "$JIT" != "true" ]
    then
        FAILURES=${FAILURES}" "${i}
    fi
done

echo
if [[ "FAILED" != "${FAILURES}" ]]
then
    echo ${FAILURES}
else
    echo "PASSED"
fi
