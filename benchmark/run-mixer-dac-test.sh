#!/usr/bin/env bash
set -euo pipefail
benchmark_dir=$(cd -- "$(dirname -- "$0")" && pwd)
root_dir="$benchmark_dir/.."
sameboy_dir="${1:-$root_dir/SameBoy}"
output_dir="$root_dir/build/mixer-dac-test"
mkdir -p "$output_dir"
make -C "$sameboy_dir" CONF=release build/lib/libsameboy.a
cc -std=gnu11 -O2 -Wall -Wextra -Werror -Wno-multichar \
    -I "$sameboy_dir" "$benchmark_dir/mixer-dac-test.c" \
    "$sameboy_dir/build/lib/libsameboy.a" -lm -ldl -o "$output_dir/mixer-dac-test"
"$output_dir/mixer-dac-test"
