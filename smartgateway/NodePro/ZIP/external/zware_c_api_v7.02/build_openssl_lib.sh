#!/bin/sh
zip_base=$PWD
start_time=$(date)
external_src_dir="$zip_base/external"
openssl_lib_dir=openssl
openssl_lib_cfg=""

cd ..
openssl_lib_base="$PWD/$openssl_lib_dir"
cfg_linux_le="linux-generic32 threads no-shared  no-asm -DL_ENDIAN --prefix=$openssl_lib_base/install --openssldir=$openssl_lib_base/install/openssl"
cfg_linux_be="linux-generic32 threads no-shared  no-asm -DB_ENDIAN --prefix=$openssl_lib_base/install --openssldir=$openssl_lib_base/install/openssl"

function display_usage
{
echo "usage:source build_openssl_lib.sh [linux | beaglebone | smp86xx | smp87xx | android | osx] [debug | release]"
}

if test $# -ne 2
then
display_usage
elif [ $1 != linux -a $1 != beaglebone -a $1 != smp86xx -a $1 != smp87xx -a $1 != android -a $1 != osx ]; then
display_usage
elif [ $2 != debug -a $2 != release ]; then
display_usage
else
# Command parameters are o.k.
if [ $2 = debug ]; then
echo "Configure debug version ..."
elif [ $2 = release ]; then
echo "Configure release version ..."
fi

echo "Configuring for $1 platform ..."
cd $openssl_lib_base

case $1 in
    linux ) openssl_lib_cfg="--prefix=$openssl_lib_base/install --openssldir=$openssl_lib_base/install/openssl threads no-shared"
            ./config $openssl_lib_cfg
            ;;
    beaglebone ) openssl_lib_cfg=$cfg_linux_le
            CC=arm-linux-gnueabihf-gcc LD=arm-linux-gnueabihf-ld AR=arm-linux-gnueabihf-ar RANLIB=arm-linux-gnueabihf-ranlib NM=arm-linux-gnueabihf-nm  ./Configure $openssl_lib_cfg
            ;;
    smp86xx ) openssl_lib_cfg=$cfg_linux_le
            CC=mipsel-linux-gcc LD=mipsel-linux-ld AR=mipsel-linux-ar RANLIB=mipsel-linux-ranlib NM=mipsel-linux-nm  ./Configure $openssl_lib_cfg
	    sed -i "s/^CFLAG=\(.*\)/CFLAG=\1 -fPIC/" Makefile
            ;;
    smp87xx ) openssl_lib_cfg=$cfg_linux_le
            CC=arm-none-linux-gnueabi-gcc LD=arm-none-linux-gnueabi-ld AR=arm-none-linux-gnueabi-ar RANLIB=arm-none-linux-gnueabi-ranlib NM=arm-none-linux-gnueabi-nm  ./Configure $openssl_lib_cfg
	    sed -i "s/^CFLAG=\(.*\)/CFLAG=\1 -fPIC/" Makefile
            ;;
    android ) openssl_lib_cfg=$cfg_linux_le
            CC=arm-linux-androideabi-gcc LD=arm-linux-androideabi-ld AR=arm-linux-androideabi-ar RANLIB=arm-linux-androideabi-ranlib NM=arm-linux-androideabi-nm  ./Configure $openssl_lib_cfg
            ;;
    osx ) openssl_lib_cfg="darwin-i386-cc --prefix=$openssl_lib_base/install --openssldir=$openssl_lib_base/install/openssl threads no-shared"
            ./Configure $openssl_lib_cfg
	    sed -e "s/^CFLAG=\(.*\)/CFLAG=\1 -fPIE -arch i386/" -i .bak Makefile
            ;;
          * )   echo "Unknown platform: $1"
esac


if test $? -eq 0
then
	echo "configure done."
        # Add PURIFY flag to avoid valgrind to complain about use of uninitialized data
        sed -i "s/^CFLAG=\(.*\)/CFLAG=\1 -DPURIFY/" Makefile
	if [ $2 = debug ]; then
	    echo "Modify makefile for debug version"
	if [ $1 = osx ]; then
	    sed -e 's/-O3/-g -O0/' -i .bak Makefile
	else
	    sed -i -e 's/-O3/-g -O0/' Makefile
	fi
	fi
	echo "make clean"
	rm -rf $openssl_lib_base/install
	make clean
	echo "make"
	make  
	if test $? -eq 0
	then
	    echo "make install_sw"
	    make install_sw
	fi
else
	echo "configure failed!"
fi

fi
#Go back to initial directory
cd $zip_base
echo Build start time is $start_time
echo Build stop time is `date`


