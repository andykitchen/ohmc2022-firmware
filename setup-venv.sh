#!/bin/bash

PYTHON=python3
VENV=venv

if [ ! -e venv ]; then
	$PYTHON -m venv $VENV
fi

source $VENV/bin/activate || exit 1

pip install -r requirements.txt

for f in deps/**/setup.py; do
	( cd $(dirname $f) && pip install -e . )
done
