#!bin/bash

awk "/$2/ { print NR }" $1 > $3
