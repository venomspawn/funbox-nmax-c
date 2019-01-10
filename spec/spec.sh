# !/bin/env sh

SCRIPT_PATH=$(dirname "$0")

NMAX_PATH="${SCRIPT_PATH}/../nmax"
if [ ! -x "${NMAX_PATH}" ]; then
	echo 'nmax binary is not found. Has `make` been invoked?'
	exit 1
fi

ARG=10
FIXTURES_PATH="${SCRIPT_PATH}/fixtures"

check_file()
{
	if [ ! -r "$1" ]; then
		echo "Required file with '$1' path is not found"
		exit 1
	fi
}

run()
{
	local name="$1"
	local input_path="${FIXTURES_PATH}/${name}.txt"

	check_file "${input_path}"

	local result=$(cat ${input_path} | ${NMAX_PATH} ${ARG})

	local check_path="${FIXTURES_PATH}/${name}_result.txt"

	check_file "${check_path}"

	local check_result=$(cat ${check_path})

	if [ "${result}" = "${check_result}" ]; then
		echo "+ test '${name}' is passed"
	else
		echo "- test '${name}' is not passed"
	fi
}

NAMES=(digits 1000 randlen)

for name in "${NAMES[@]}"; do
	run "${name}"
done
