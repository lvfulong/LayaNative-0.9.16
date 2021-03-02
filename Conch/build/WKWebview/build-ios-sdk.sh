#!/bin/sh

rm -rf libs/libLayaWKWebview.a

#—————————————————————merge static lib————————————————————————
rm -rf armv7a
rm -rf armv64
rm -rf i386
rm -rf x86_64
mkdir armv7a
mkdir armv64
mkdir i386
mkdir x86_64


lipo -extract armv7 ../../../ThirdParty/curl/lib/ios/libcurl.a -o armv7a/libcurl.a
lipo -extract arm64 ../../../ThirdParty/curl/lib/ios/libcurl.a -o armv64/libcurl.a
lipo -extract i386 ../../../ThirdParty/curl/lib/ios/libcurl.a -o i386/libcurl.a
lipo -extract x86_64 ../../../ThirdParty/curl/lib/ios/libcurl.a -o x86_64/libcurl.a

lipo -extract armv7 ../../../ThirdParty/openssl/lib/ios/libssl.a -o armv7a/libssl.a
lipo -extract arm64 ../../../ThirdParty/openssl/lib/ios/libssl.a -o armv64/libssl.a
lipo -extract i386 ../../../ThirdParty/openssl/lib/ios/libssl.a -o i386/libssl.a
lipo -extract x86_64 ../../../ThirdParty/openssl/lib/ios/libssl.a -o x86_64/libssl.a

lipo -extract armv7 ../../../ThirdParty/openssl/lib/ios/libcrypto.a -o armv7a/libcrypto.a
lipo -extract arm64 ../../../ThirdParty/openssl/lib/ios/libcrypto.a -o armv64/libcrypto.a
lipo -extract i386 ../../../ThirdParty/openssl/lib/ios/libcrypto.a -o i386/libcrypto.a
lipo -extract x86_64 ../../../ThirdParty/openssl/lib/ios/libcrypto.a -o x86_64/libcrypto.a


lipo -extract armv7 ../../../ThirdParty/tpg/lib/ios/libTPGDec.a -o armv7a/libTPGDec.a
lipo -extract arm64 ../../../ThirdParty/tpg/lib/ios/libTPGDec.a -o armv64/libTPGDec.a
lipo -extract i386 ../../../ThirdParty/tpg/lib/ios/libTPGDec.a -o i386/libTPGDec.a
lipo -extract x86_64 ../../../ThirdParty/tpg/lib/ios/libTPGDec.a -o x86_64/libTPGDec.a

lipo -extract armv7 ../../../ThirdParty/tpg/lib/ios/liblibYUV.a -o armv7a/liblibYUV.a
lipo -extract arm64 ../../../ThirdParty/tpg/lib/ios/liblibYUV.a -o armv64/liblibYUV.a
lipo -extract i386 ../../../ThirdParty/tpg/lib/ios/liblibYUV.a -o i386/liblibYUV.a
lipo -extract x86_64 ../../../ThirdParty/tpg/lib/ios/liblibYUV.a -o x86_64/liblibYUV.a

lipo -extract armv7 ../../../Conch/libs/ios/libcommon.a -o armv7a/libcommon.a
lipo -extract arm64 ../../../Conch/libs/ios/libcommon.a -o armv64/libcommon.a
lipo -extract i386 ../../../Conch/libs/ios-sim/libcommon.a -o i386/libcommon.a
lipo -extract x86_64 ../../../Conch/libs/ios-sim/libcommon.a -o x86_64/libcommon.a

lipo -extract armv7 ../../../Conch/libs/ios/libLayaWKWebview.a -o armv7a/libLayaWKWebview.a
lipo -extract arm64 ../../../Conch/libs/ios/libLayaWKWebview.a -o armv64/libLayaWKWebview.a
lipo -extract i386 ../../../Conch/libs/ios-sim/libLayaWKWebview.a -o i386/libLayaWKWebview.a
lipo -extract x86_64 ../../../Conch/libs/ios-sim/libLayaWKWebview.a -o x86_64/libLayaWKWebview.a

cd armv7a

libtool -static *.a -o libLayaWKWebview.a

cd ..
cd armv64

libtool -static *.a -o libLayaWKWebview.a

cd ..
cd i386

libtool -static *.a -o libLayaWKWebview.a

cd ..
cd x86_64

libtool -static *.a -o libLayaWKWebview.a

cd ..

lipo -create x86_64/libLayaWKWebview.a i386/libLayaWKWebview.a armv7a/libLayaWKWebview.a armv64/libLayaWKWebview.a -output libs/libLayaWKWebview.a

rm -rf armv7a
rm -rf armv64
rm -rf i386
rm -rf x86_64

#—————————————————————copy .h————————————————————————
cp -f ../LayaWKWebview/proj.ios/LayaWKWebview/conchConfig.h libs/include/conchConfig.h
cp -f ../LayaWKWebview/proj.ios/LayaWKWebview/LayaWKWebview.h libs/include/LayaWKWebview.h



