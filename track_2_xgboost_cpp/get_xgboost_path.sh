#!/usr/bin/env bash

CONDA_SH="/usr/conda/etc/profile.d/conda.sh"
if [ -f "$CONDA_SH" ] ; then
    source $CONDA_SH
    conda activate
fi
python3 -c 'import xgboost; print(xgboost.__path__[0])'
