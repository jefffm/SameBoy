#!/usr/bin/env bash
set -euo pipefail
benchmark_dir=$(cd -- "$(dirname -- "$0")" && pwd)
root_dir="$benchmark_dir/.."
sameboy_dir="${1:-$root_dir/SameBoy}"
output_dir="$root_dir/build/pulse-clock-test"
mkdir -p "$output_dir"
python "$benchmark_dir/torture/build.py" --case ch2-length-domain --out "$output_dir/rom"
make -C "$sameboy_dir" CONF=release build/lib/libsameboy.a
state_flag=()
if rg -q "square_clock_active" "$sameboy_dir/Core/apu.h"; then state_flag=(-DPULSE_CLOCK_STATE); fi
cc "${state_flag[@]}" -std=gnu11 -O2 -Wall -Wextra -Werror -Wno-multichar \
    -I "$sameboy_dir" "$benchmark_dir/pulse-clock-test.c" \
    "$sameboy_dir/build/lib/libsameboy.a" -lm -ldl -o "$output_dir/pulse-clock-test"
"$output_dir/pulse-clock-test" "$output_dir/rom/audio-torture.gb"
