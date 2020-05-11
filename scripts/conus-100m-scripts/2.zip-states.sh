#!/bin/bash

for i in ?? ; do
	echo $1
	cd $1
	zip -9 ../USNG_$1_100m.zip *
	echo $1 done.
done
