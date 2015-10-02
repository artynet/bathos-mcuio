#!/bin/bash

build () {

	make "$1"_defconfig
	make

	[ $? -ne 0 ] && echo "Error ! ! !" && exit 0 

	mv bathos.hex out/bathos-$1.hex

	echo ""
	echo "Cleaning $1"
	echo ""

	make clean
	rm -v .config

}

finalise () {

cd out/

HEX="bathos-yunmini.hex"

if [ -e $HEX ]
then
	echo ""
	echo "Renaming bathos for yun-mini..."
	echo ""

	mv -v $HEX bathos-yun-mini.hex
fi

cd ../

echo ""
echo "Finished ! ! !"
echo ""

}

boards=('yun' 'one' 'lei' 'yunmini' 'chiwawa')

if [ -e .config ]
then
	rm -v .config
fi

if [ -d out/ ]
then
	echo ""
	rm -vf out/bathos-*.hex
fi

echo ""
echo "Cleaning Done !"
echo ""

sleep 2

mkdir -p out/

for p in "${boards[@]}"
do
	if [ $p != 'yun' ]
	then
		echo ""

		cp configs/yun_defconfig configs/"$p"_defconfig
		cd configs/
		patch -p0 < ../patches/$p.patch
		cd ..

		build $p
		rm -v configs/"$p"_defconfig
	else
		build $p
	fi

done

finalise