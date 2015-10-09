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

HASH=`git rev-parse --short HEAD`

cd out/

HEX="bathos-yunmini.hex"

if [ -e $HEX ]
then
	echo ""
	echo "Renaming bathos for yun-mini..."
	echo ""

	mv -v $HEX bathos-yun-mini.hex
fi

echo ""

md5sum *.hex >> bathos-"$HASH".md5

zip -r ../bathos-arty-$HASH.zip .

cd ../

md5sum bathos-arty-$HASH.zip > bathos-arty-$HASH.zip.md5

echo ""
echo "Finished ! ! !"
echo ""

}

boards=('yun' 'one' 'lei' 'yunmini' 'chiwawa')

echo ""
rm -v *.zip *.md5 2> /dev/null

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
