#!/bin/bash
    
APP=CheckSchema
if [ "$DEBUG" = "1" ] ; then
    APP=$PRO/bin/CheckSchemaDBG
fi

echo Running $APP
$APP test.xsd test.xml
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
