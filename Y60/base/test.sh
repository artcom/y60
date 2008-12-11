#!/bin/bash -e

xmllint --noout --schema xml.xsd --schema XMLSchema.xsd Y60.xsd

exit $?

