#!/usr/bin/env bash

export INTERACTIVE=false

source common.sh

print_info
init

make_parallel_request() {
	local NUM=$1
	local CMD=$2

	output=$(INTERACTIVE=false make_request "${NUM}" "${CMD}" 2>&1)

	echo $output
}

echo -e "\n${Blu}Downloading samples through proxy:${RCol}"
for i in `seq ${REQUESTS}`; do
	make_parallel_request "${i}" "${PROXY_CURL_CMD}" &
done

wait

check_files
