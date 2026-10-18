#!/usr/bin/env bash
set -euo pipefail
benchmark_dir=$(cd -- "$(dirname -- "$0")" && pwd)
root_dir="$benchmark_dir/.."
sameboy_dir="${1:-$root_dir/SameBoy}"
library="${2:-$sameboy_dir/build/lib/libsameboy.a}"
output_dir="${DAC_RECONNECT_OUTPUT_DIR:-$root_dir/build/dac-reconnect-test}"
mkdir -p "$output_dir"
# An explicit archive permits testing an immutable before-build without
# rebuilding or modifying the checkout used by concurrent investigation.
if (( $# < 2 )); then
    make -C "$sameboy_dir" CONF=release build/lib/libsameboy.a
fi
cc -std=gnu11 -O2 -Wall -Wextra -Werror -Wno-multichar \
    -I "$sameboy_dir" "$benchmark_dir/dac-reconnect-test.c" \
    "$library" -lm -ldl -o "$output_dir/dac-reconnect-test"
"$output_dir/dac-reconnect-test"
