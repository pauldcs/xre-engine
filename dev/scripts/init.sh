#!/bin/bash

set -eu 

readonly LOGFILE="/root/logfile.log"

touch ${LOGFILE}

while true; do
    if [ -e "${LOGFILE}" ]; then
        tail -f ${LOGFILE}
    else
        echo "${LOGFILE} not found..."
        sleep 10
    fi
done
 