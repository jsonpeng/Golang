#!/bin/sh
zip_base=$PWD
external_src_dir="$zip_base/external"
openssl_lib_src=openssl-1.0.1r.tar.gz
openssl_lib_untar_dir=openssl-1.0.1r
openssl_lib_dir=openssl
echo "Z/IP base directory: " $zip_base

echo "Unzip openssl lib: $external_src_dir/$openssl_lib_src"
cd ..
tar zxf "$external_src_dir/$openssl_lib_src"

if test $? -eq 0
then
    echo "Unzip openssl lib done."

    if [ -L $openssl_lib_dir ] ; then
    echo "Soft link $openssl_lib_dir exist."
    rm $openssl_lib_dir
    fi

    ln -s $openssl_lib_untar_dir $openssl_lib_dir
    echo "Patching openssl lib ..."
    sed  -i -r "s%unsigned char iv\[EVP_MAX_IV_LENGTH\].*%unsigned char iv\[EVP_MAX_IV_LENGTH\] = {0}; /* to prevent Valgrind uninitialized error*/%" \
    $openssl_lib_dir/ssl/d1_srvr.c
    sed  -i -r "s%unsigned char key_name\[16\].*%unsigned char key_name\[16\] = {0}; /* to prevent Valgrind uninitialized error*/%" \
    $openssl_lib_dir/ssl/d1_srvr.c

    echo "Done."

else
    echo "Unzip openssl lib failed!"
fi

#Go back to initial directory
cd $zip_base

