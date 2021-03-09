#!/bin/bash

for i in $(seq 41 70); do mv "file$i" "newfile$i"; done
for i in $(seq 71 100); do rm -r "file$i"; done
