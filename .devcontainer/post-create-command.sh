#!//bin/bash

sudo apt update
export DEBIAN_FRONTEND=noninteractive
sudo apt install -y --no-install-recommends cmake cpputest clang-format-15 clang-tidy-17
python -m pip install -upgrade pip
pip install -e .[develop]
pre-commit install
pre-commit install -t pre-push
