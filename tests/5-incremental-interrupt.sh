#!/usr/bin/env bash

source common.sh

print_info
init

LAST_INTERRUPTED_IDX=0

make_interrupted_request() {
	local NUM=$1
	local CMD=$2
	local INTERRUPT_TIME=$NUM

	timeout "${INTERRUPT_TIME}s" bash -c "make_request '${NUM}' '${PROXY_CURL_CMD}'"
	ret=$?

	# If curl was interrupted then ignore that result file
	if [[ $ret == 124 ]]; then
		LAST_INTERRUPTED_IDX=$NUM
		echo
	fi
}

echo -e "\n${Blu}Downloading samples through proxy:${RCol}"
for i in `seq ${REQUESTS}`; do
	make_interrupted_request "${i}" "${PROXY_CURL_CMD}"
done

check_files $(( LAST_INTERRUPTED_IDX + 1))
