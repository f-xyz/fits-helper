#!/usr/bin/bash
set -o pipefail

siril -s "${PATH}/stacker.ssf" 2>&1 | tee "${PATH}/stacker.log"
exit $?