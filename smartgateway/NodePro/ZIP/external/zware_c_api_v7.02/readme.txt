How to build CAPI

# 1. only need to run once for all platforms
source install_openssl_lib.sh 
#2. build either debug or release library version
source build_openssl_lib.sh linux [debug | release] 

#3. make library
make TARGET_PLATFORM=LINUX_ZIP2 [DEBUG=1]