#!/usr/bin/env bash
set -euo pipefail
benchmark_dir=$(cd -- "$(dirname -- "$0")" && pwd)
root_dir="$benchmark_dir/.."
sameboy_dir="${1:-$root_dir/SameBoy}"
library="${2:-$sameboy_dir/build/lib/libsameboy.a}"
output_dir="${DAC_TIMING_OUTPUT_DIR:-$root_dir/build/dac-timing-test}"
mkdir -p "$output_dir"
# Both headers and archive can be selected explicitly for immutable before/after
# builds. This runner deliberately never rebuilds a concurrently used checkout.
cc -std=gnu11 -O2 -Wall -Wextra -Werror -Wno-multichar \
    -I "$sameboy_dir" "$benchmark_dir/dac-timing-test.c" \
    "$library" -lm -ldl -o "$output_dir/dac-timing-test"
"$output_dir/dac-timing-test"
