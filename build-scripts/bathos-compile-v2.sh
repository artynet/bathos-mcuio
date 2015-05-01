#!/bin/bash

boards=('yun' 'one' 'chowchow' 'yunmini')

if -e [ .config ]
then
	rm .config
fi

rm -f bathos-*.hex

for p in "${boards[@]}"
do
	make "$p"_defconfig
	make
	mv bathos.hex bathos-$p.hex
	make clean
	rm .config
done