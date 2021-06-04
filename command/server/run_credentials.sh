#!/bin/bash

set -eou pipefail

DB_NAME="${1:-serverDB.db}"

mkdir -p bin db

make credentials

./bin/credentials -db ${DB_NAME}
