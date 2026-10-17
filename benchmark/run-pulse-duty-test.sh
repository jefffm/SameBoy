#!/usr/bin/env bash
set -euo pipefail
benchmark_dir=$(cd -- "$(dirname -- "$0")" && pwd)
root_dir="$benchmark_dir/.."
sameboy_dir="${1:-$root_dir/SameBoy}"
output_dir="$root_dir/build/pulse-duty-test"
mkdir -p "$output_dir"
python "$benchmark_dir/torture/build.py" --case ch1-duty-trigger --out "$output_dir/rom"
make -C "$sameboy_dir" CONF=release build/lib/libsameboy.a
state_flag=()
if rg -q "square_duty_output" "$sameboy_dir/Core/apu.h"; then state_flag=(-DPULSE_DUTY_STATE); fi
cc "${state_flag[@]}" -std=gnu11 -O2 -Wall -Wextra -Werror -Wno-multichar \
    -I "$sameboy_dir" "$benchmark_dir/pulse-duty-test.c" \
    "$sameboy_dir/build/lib/libsameboy.a" -lm -ldl -o "$output_dir/pulse-duty-test"
"$output_dir/pulse-duty-test" "$output_dir/rom/audio-torture.gb"
