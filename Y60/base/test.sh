#!/bin/bash -e

xmllint --noout --schema XMLSchema.xsd Y60.xsd

exit $?

