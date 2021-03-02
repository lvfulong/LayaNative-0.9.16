GCC=arm-linux-androideabi-g++
SYSROOT=/home/layabox/Android/android-ndk-r10e/platforms/android-9/arch-arm
CFLAGS=-I../../../../ThirdParty/boost_1.58/include/boost-1_58 \
-I/home/layabox/Android/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/include \
-I/home/layabox/Android/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include \
-I/home/layabox/Android/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/include/backward \
-I/home/layabox/Android/android-ndk-r10e/platforms/android-9/arch-arm/usr/include \
-I../../../include/common \
-DANDROID \
-DLINUX \
-D_TEST_ \
-DJS_V8 \
-DV8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR \
-std=c++11 \
-fPIE \
-O3 

#LOCAL_SRC_FILES=
LDLIB=-llog -lGLESv2 -lz -landroid -fPIE -pie -lgcc -march=armv7-a\
-L${SYSROOT}/usr/lib \
--sysroot=${SYSROOT} \
-L../../../libs/android32 \
-lcommon \
$(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/libgnustl_static.a

all: buildtest install
    

buildtest: ConchTest.o JCTestManager.o testDiskRW.o
	${GCC} ConchTest.o JCTestManager.o testDiskRW.o ${LDLIB} -o testConch
    
ConchTest.o: ConchTest.cpp
	${GCC} -c ${CFLAGS} ConchTest.cpp
    
JCTestManager.o : ../../../source/conch/Test/JCTestManager.cpp ../../../source/conch/Test/JCTestManager.h
	${GCC} -c ${CFLAGS} ../../../source/conch/Test/JCTestManager.cpp

testDiskRW.o : ../../../source/conch/Test/Cases/testDiskRW.cpp 
	${GCC} -c ${CFLAGS} ../../../source/conch/Test/Cases/testDiskRW.cpp

clean:
	rm *.o
install:
	adb push testConch /data/temp/
