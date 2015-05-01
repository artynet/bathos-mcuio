#!/bin/bash

boards=('yun' 'one' 'chowchow' 'yunmini')

for p in "${boards[@]}"
do
	make "$p"_defconfig
	make
	mv bathos.hex bathos-$p.hex
	make clean
done