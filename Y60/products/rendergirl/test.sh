#!/bin/bash -e
PASSED=0
FAILED=0
FAILED_IMAGES=""

ulimit -c unlimited

compareSceneImages() {
    local BASELINE_IMAGE=$1
    local BASENAME=`basename $BASELINE_IMAGE`
    local TEST_IMAGE=TEST_IMAGES/${BASENAME}

    echo "Comparing screenshot $TEST_IMAGE with baseline image $BASELINE_IMAGE"
    $PRO/bin/CompareImage --tolerance 3 --threshold 20 $TEST_IMAGE $BASELINE_IMAGE
    RESULT=$?
    if [ $RESULT -ne 0 ]; then
        echo "### FAILED: $TEST_IMAGE does not match $BASELINE_IMAGE."
        FAILED_IMAGES="${FAILED_IMAGES}, $BASELINE_IMAGE"
        ((FAILED++));
    else
        echo Test succeeded
        ((PASSED++));
    fi

    return 0
}

if [ "$MAYA_SDK" == "" ] ; then
    echo "MAYA_SDK not set - skipping tests"
    exit 0
fi

rm -f TEST_IMAGES/*
rm -f TESTMODELS/test_save.x60
rm -f TESTMODELS/test_save_as.x60

# export maya file
(cd $PRO/src/Y60/products/maya-xport && ./install_maya)
maya2x60 TESTMODELS/testcube.mb
cp TESTMODELS/testcube.x60 TESTMODELS/test_save.x60

# run acgtkshell with test
APP="acgtkshell"
if [ "X$DEBUG" = "X1" ]; then
    APP="${APP}DBG"
fi
if [ ! -z $VG ]; then
    TOOLARGS=""
    case "$VG" in
    heap*)
        TOOL="massif"
        ;;
    call*)
        TOOL="callgrind"
        ;;
    *)
        TOOL="memcheck"
        TOOLARGS="--leak-check=yes --show-reachable=yes"
        ;;
    esac
    APP="valgrind --tool=${TOOL} ${TOOLARGS} ${APP}"
    echo "VALGRINDING:${APP}"
fi
CMD="${APP} -I 'SCRIPTS;$PRO/src/Y60/js;$PRO/src/Y60/shader;$PRO/lib' ./SCRIPTS/rendertest.js TESTMODELS/test_save.x60 shaderlibrary.xml"
echo $CMD
$CMD

# compare resulting images
for BASELINE_IMAGE in BASELINE_IMAGES/* ; do
    compareSceneImages $BASELINE_IMAGE
done

echo "=================   TEST RESULTS   ======================"
echo "PASSED: $PASSED"
echo "FAILED: ${FAILED}$FAILED_TUTORIALS"

EXITCODE=1
if [ $FAILED == 0 ] ; then
    if [ $PASSED == 0 ] ; then
        echo "### WARNING: No tests executed"
        EXITCODE=1
    else
        echo "### SUCCESS: All tests passed"
        EXITCODE=0
    fi
else
    echo "### FAILED: $((FAILED)) test failed."
    EXITCODE=2
fi

echo "========================================================="
exit $EXITCODE
