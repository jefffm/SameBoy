#!/usr/bin/env bash
set -euo pipefail
benchmark_dir=$(cd -- "$(dirname -- "$0")" && pwd)
root_dir="$benchmark_dir/.."
sameboy_dir="${1:-$root_dir/SameBoy}"
output_dir="$root_dir/build/noise-feedback-test"
mkdir -p "$output_dir"
rgbasm -D RETRIGGER=0 -o "$output_dir/steady.o" "$benchmark_dir/noise-probe.asm"
rgblink -o "$output_dir/steady.gb" "$output_dir/steady.o"
rgbfix -v -p 0 -t "NOISE PROBE" -m 0x1b -r 0x04 "$output_dir/steady.gb"
make -C "$sameboy_dir" CONF=release build/lib/libsameboy.a
cc -std=gnu11 -O2 -Wall -Wextra -Werror -Wno-multichar \
    -I "$sameboy_dir" "$benchmark_dir/noise-feedback-test.c" \
    "$sameboy_dir/build/lib/libsameboy.a" -lm -ldl -o "$output_dir/noise-feedback-test"
"$output_dir/noise-feedback-test" "$output_dir/steady.gb"
