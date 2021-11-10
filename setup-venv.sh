#!/bin/bash

PYTHON=python3
VENV=venv

if [ ! -e venv ]; then
	$PYTHON -m venv $VENV
fi

source $VENV/bin/activate || exit 1

pip install -r requirements.txt

find deps -maxdepth 2 -name "setup.py" | while read line; do
	(
		echo install "$line"
		cd `dirname "$line"` &&
		pip install -e .
	)
done
