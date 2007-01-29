#!/bin/bash
    
CheckSchema test.xsd test.xml
Result=$?
if [[ ${Result} -ne 0 ]]; then
    echo Failed
    exit ${Result}
fi

#CheckSchema broken.xsd
#Result=$?
#if [[ ${Result} -eq 0 ]]; then
#    echo Failed
#    exit ${Result}
#fi

exit 0 
