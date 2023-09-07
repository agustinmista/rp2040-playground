#!/bin/bash

set -e

ELF_FILE=$1

if [ -z "$ELF_FILE" ]; then
  echo "USAGE: $0 blink.elf"
  exit
fi

if ! test -f "$ELF_FILE"; then
  echo "$ELF_FILE does not exist!"
  exit
fi

openocd \
  -f interface/cmsis-dap.cfg \
  -f target/rp2040.cfg \
  -c "adapter speed 5000" \
  -c "program $ELF_FILE verify reset exit"