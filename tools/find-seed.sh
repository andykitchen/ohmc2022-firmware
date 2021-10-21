#!/bin/bash

for seed in $(seq 0 100)
do
  python3 rockling.py --seed $seed 2>&1 |
     grep "FAIL at 48.00 MHz" && continue
  echo "Working Seed is $seed"
  break
done
