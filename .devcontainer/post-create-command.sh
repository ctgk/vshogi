#!//bin/bash

sudo apt update
export DEBIAN_FRONTEND=noninteractive
sudo apt install -y --no-install-recommends cmake cpputest clang-format-13 clang-tidy-13
python -m pip install -upgrade pip
pip install -e .[develop]
pre-commit install
pre-commit install -t pre-push
