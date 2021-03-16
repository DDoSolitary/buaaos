#!/bin/bash

find test_dir -name xfile -exec bash -c 'sed s/char/int/g "{}" > "$(dirname "{}")/output"' \;
