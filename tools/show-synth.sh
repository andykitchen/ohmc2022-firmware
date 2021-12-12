#!/bin/bash

# yosys -p "prep; opt; show -prefix out -format dot" "$@" && dot out.dot | gvpack | xdot -

yosys -p "prep; opt; show -prefix out -format dot" "$@" \
	&& gvpr 'BEGIN { graph_t g; } BEG_G { g = cloneG($G, "") } END { write(g) }' out.dot | xdot -
