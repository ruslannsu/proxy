#!/usr/bin/env bash

source colors.sh

export PROXY_HOST="localhost"
export PROXY_PORT="8080"

export URL="http://xcal1.vodafone.co.uk/10MB.zip"
export REQUESTS="10"

CONCURRENCY=10

CMD_TIMEOUT_S=10

POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
	case $1 in
		-p|--port)
			shift
			export PROXY_PORT="$1"
			shift
			;;
		-u|--url)
			shift
			export URL="$1"
			shift
			;;
		-r|--requests)
			shift
			export REQUESTS="$1"
			shift
			;;
		-t|--timeout)
			shift
			export CMD_TIMEOUT_S="$1"
			shift
			;;
		-c|--concurrency)
			shift
			export CONCURRENCY="$1"
			shift
			;;
		-h|--help)
			echo "Usage: $0 [-p|--port <proxy_port>] [-u|--url <http/1.0 url>] [-r|--requests <num of requests>]"
			echo "          [-t|--timeout <curl timeout in s>] [-c|--concurrency <number of parallel requests>]"
			exit 0
			;;
		--*|-*)
			echo "$0: Unknown option $1"
			echo "Usage: $0 -h"
			exit 1
			;;
		*)
			POSITIONAL_ARGS+=("$1")
			shift
			;;
	esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

export CURL_OPTIONS="--http0.9 --http1.0 --progress-bar --max-time ${CMD_TIMEOUT_S}"
export REF_CURL_OPTIONS="${CURL_OPTIONS} --location"
export PROXY_CURL_OPTIONS="${CURL_OPTIONS} --proxy1.0 ${PROXY_HOST}:${PROXY_PORT}"

REF_NUM=0

RESULTS_DIR="results"
RESULT_PREFIX="result-"
FILE_PATH="${RESULTS_DIR}/${RESULT_PREFIX}"

export REF_CURL_CMD="curl -o ${FILE_PATH}\${NUM} ${REF_CURL_OPTIONS}  ${URL}"
export PROXY_CURL_CMD="curl -o ${FILE_PATH}\${NUM} ${PROXY_CURL_OPTIONS} ${URL}"

clear_last_lines() {
	local j

	for ((j = 0; j <= "${1:-1}"; j++ )); do
		tput cuu1
	done
	tput ed
}

export -f clear_last_lines

init() {
	rm -r ${RESULTS_DIR}
	mkdir -p ${RESULTS_DIR}

	echo -e "\n${Blu}Downloading reference file:${RCol}"
	make_request "${REF_NUM}" "${REF_CURL_CMD}"
}

export -f init

print_info() {
	export NUM=0

	echo -en "${Pur}REFERENCE CURL CMD: ${RCol}\t"
	echo "$(echo ${REF_CURL_CMD} | envsubst)"

	echo -en "${Pur}    PROXY CURL CMD: ${RCol}\t"
	echo "$(echo ${PROXY_CURL_CMD} | envsubst)"
}

export -f print_info

make_request() {
	local NUM=$1
	local CMD=$2

	INTERACTIVE=${INTERACTIVE:-true}

	if $INTERACTIVE; then
		echo "${NUM}: "
	else
		CMD="${CMD} --no-progress-meter --no-progress-bar"
		echo -n "${NUM}: "
	fi

	start_time_ns=$(date +%s%N)
	# echo "executing: $(echo ${CMD} | envsubst)"
	eval "${CMD}"
	ret=$?
	duration=$(echo "scale=2; ($(date +%s%N ) - $start_time_ns) / 1000000000" | bc)

	if [[ $ret != 0 ]]; then
		return 1
	fi

	if $INTERACTIVE; then
		clear_last_lines 1
		echo "${NUM}: ${duration}s"
	else
		echo "${duration}s"
	fi

	return 0
}

export -f make_request

check_files() {
	local start=${1:-1}
	local end=${2:-$REQUESTS}
	local failed=false

	echo -e "\n${Blu}Checking results (from ${start} to ${end}):${RCol}"

	for i in `seq ${start} ${end}`; do
		REF_FILE="${FILE_PATH}${REF_NUM}"
		FILE_TO_CHECK="${FILE_PATH}${i}"

		if [ ! -f ${FILE_TO_CHECK} ]; then
			echo "File '${FILE_TO_CHECK}' not found"
			failed=true
			continue
		fi

		if ! diff ${REF_FILE} ${FILE_TO_CHECK} -q; then
			echo "Use: diff ${REF_FILE} ${FILE_TO_CHECK} --text"
			failed=true
		fi
	done

	if $failed; then
		echo -e "${BRed}FAILED${RCol}"
		return 1
	fi

	echo -e "${BGre}PASSED${RCol}"
}

export -f check_files

