#!/bin/sh
for i in $@
do
    yes | dd  bs=1000 count=4 of=$i
done