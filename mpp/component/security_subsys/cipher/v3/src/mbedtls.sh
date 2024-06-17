MBEDTLS_DIR=$1
MBEDTLS_VER=$2
PATCH=$4
cmd=$3
if [[ $cmd == PATCH ]]; then
	if [ ! -d $MBEDTLS_DIR/$MBEDTLS_VER ]; then
		tar -zxf $MBEDTLS_DIR/$MBEDTLS_VER.tar.gz -C $MBEDTLS_DIR;
		if [ -f $PATCH ]; then
		        patch -p1 -d $MBEDTLS_DIR/$MBEDTLS_VER < $PATCH;
        	fi
	fi
else
	if [ -e $MBEDTLS_DIR/$MBEDTLS_VER.tar.gz ]; then
        	rm -rf $MBEDTLS_DIR/$MBEDTLS_VER;
	fi
fi


