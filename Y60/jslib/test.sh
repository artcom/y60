#!/bin/bash
echo "Testing for undocumented code..."
grep "DOC_BEGIN(\"\")" *
RETURN_CODE=$?

if [ "$RETURN_CODE" == "0" ]; then
    echo Found `grep "DOC_BEGIN(\"\")" * | wc -l` undocumented functions!
    exit 1
fi
echo "Test succeeded"
exit 0
