#!/usr/bin/env bash
#
# Usage: test.sh [testDir1 ... testDirN]
#

set -o errexit -o noclobber -o nounset -o pipefail

SELF_DIR="${BASH_SOURCE[0]%/*}"

: "${DEBUG:=false}"
: "${TESTS_DIR:=$SELF_DIR}"
: "${THEMES_DIR:=$SELF_DIR/../../themes}"

if [[ $# -gt 0 && "$1" == "-d" ]]; then DEBUG=true; shift; fi

function require {
  local C;
  for c in "$@"; do
    if [ -v "${c^^}" ]; then continue; fi
    C=$(command -v "$c") || { echo "Required command not found: $c" >&2; exit 1; }
    declare -gr "${c^^}"="$C"
  done
}

require diff doxlee find mktemp

$DEBUG && DIFF_ARGS=--context || DIFF_ARGS=--brief
$DEBUG && DOXLEE_ARGS=--debug

function runTest {
  local testDir="$1"
  local themeDir="$2"
  local testName="${testDir##*/}"
  local themeName="${themeDir##*/}"
  local tmpDir; tmpDir=$("$MKTEMP" --directory --tmpdir "$testName-$themeName-XXX")
  $DEBUG && echo "Test : $testName ($testDir) w/ theme $themeName ($themeDir)" >&2
  $DEBUG && echo "$DOXLEE" ${DOXLEE_ARGS:-} --force --input-dir "$themeDir/xml" --theme-dir "$THEMES_DIR/$themeName" --output-dir "$tmpDir"
  "$DOXLEE" ${DOXLEE_ARGS:-} --force --input-dir "$testDir/xml" --theme-dir "$THEMES_DIR/$themeName" --output-dir "$tmpDir"
  $DEBUG && echo "$DIFF" $DIFF_ARGS --recursive "$testDir/expected/$themeName" "$tmpDir"
  "$DIFF" $DIFF_ARGS --recursive "$testDir/expected/$themeName" "$tmpDir"
  echo "PASS : $testName($themeName)" >&2
}

function runTests {
  local testDir="$1"
  local testName="${testDir##*/}"
  $DEBUG && echo "Test : $testName ($testDir)" >&2
  while IFS= read -d '' -r themeDir; do
    runTest "$testDir" "$themeDir"
  done < <(find "$testDir/expected" -mindepth 1 -maxdepth 1 -type d -print0)
}

if [ $# -eq 0 ]; then
  while IFS= read -d '' -r testDir; do
    runTests "$testDir" < /dev/null
  done < <(find "$TESTS_DIR" -mindepth 1 -maxdepth 1 -type d -print0)
fi

while [ $# -gt 0 ]; do
  runTests "$1"
  shift
done
