#!/usr/bin/env bash

export INTERACTIVE=true

source common.sh

print_info
init

echo -e "\n${Blu}Downloading samples through proxy:${RCol}"
for i in `seq ${REQUESTS}`; do
	make_request "${i}" "${PROXY_CURL_CMD}"
done

SLEEP_S=5
echo -e "\n${Blu}Waiting ${SLEEP_S}s for cache invalidation${RCol}"
sleep ${SLEEP_S}

echo -e "\n${Blu}Downloading samples through proxy:${RCol}"
for i in `seq $(( REQUESTS + 1)) $(( REQUESTS * 2))`; do
	make_request "${i}" "${PROXY_CURL_CMD}"
done

check_files 1 $(( REQUESTS * 2))
