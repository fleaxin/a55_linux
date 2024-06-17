
#!/bin/bash
KERN_DIR=$1
KERN_MPP_DIR=$KERN_DIR/drivers/vendor/mpp
KERN_KLAD_DIR=$KERN_MPP_DIR/klad
SDK_KLAD_DIR=$(pwd)/..
if [ ! -e $KERN_MPP_DIR ]; then
	echo "Kernel mpp directory $KERN_MPP_DIR isn't exist!"
	exit -1
fi
mkdir -p $KERN_KLAD_DIR
cp -a $SDK_KLAD_DIR/include $KERN_KLAD_DIR/
cp -a $SDK_KLAD_DIR/src $KERN_KLAD_DIR/
rm -rf $KERN_KLAD_DIR/src/mpi
rm -rf $KERN_KLAD_DIR/src/linux.mak
cp Makefile $KERN_KLAD_DIR/src/
cp -r $SDK_KLAD_DIR/../copy_inc $KERN_KLAD_DIR/ext_inc
