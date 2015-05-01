#!/bin/bash

boards=('yun' 'one' 'chowchow' 'yunmini')

if -e [ .config ]
then
	rm .config
fi

rm -vf bathos-*.hex

for p in "${boards[@]}"
do
	make "$p"_defconfig
	make
	mv bathos.hex bathos-$p.hex

	echo ""
	echo "Cleaning $p"
	echo ""

	make clean
	rm -v .config
done