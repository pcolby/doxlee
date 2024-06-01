#!/usr/bin/env bash

set -o errexit -o noclobber -o nounset -o pipefail
shopt -s inherit_errexit

: "${scriptDir:=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")" || true)}"
: "${outputDir:=$scriptDir}"

[[ $# -ge 1 && $# -le 2 ]] || { echo "Usage: ${BASH_SOURCE[0]##*/} <doxygen-src-dir> [<output-dir>]" >&2; exit 1; }

while IFS= read -d '' -r fileName; do
  grep -q 'si:noNamespaceSchemaLocation=.compound.xsd.' "$1/testing/$fileName" || {
    echo "Skipping non-compound file: $fileName" >&2
    continue
  }
  dirName=$(dirname "$fileName")
  outputFileName=$(sed -Ee "s|^$dirName/$dirName|$dirName/|" -e 's|/|_|' -e 's|_+|_|g' <<< "$fileName")
  cp -v "$1/testing/$fileName" "$outputDir/$outputFileName"
done < <(find "$1/testing/" -name '*.xml' -printf '%P\0' || true)
