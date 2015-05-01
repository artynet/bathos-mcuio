#!/bin/bash

boards=('yun' 'one' 'chowchow' 'yunmini')

if [ -e .config ]
then
	rm .config
fi

if [ -d out/ ]
then
	rm -vf out/bathos-*.hex
fi

mkdir -p out/

for p in "${boards[@]}"
do
	make "$p"_defconfig
	make
	mv bathos.hex out/bathos-$p.hex

	echo ""
	echo "Cleaning $p"
	echo ""

	make clean
	rm -v .config
done
