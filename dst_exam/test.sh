#!/bin/bash

out_file=lab0_exam.c
in_dir=dir
grep -Rn 'hello OS lab0' $in_dir > $out_file
find $in_dir -name lab0_x >> $out_file
