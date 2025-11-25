#!/usr/bin/env bash

source common.sh

print_info
init

echo -e "\n${Blu}Downloading samples through proxy:${RCol}"
for i in `seq ${REQUESTS}`; do
	make_request "${i}" "${PROXY_CURL_CMD}"
done

check_files
