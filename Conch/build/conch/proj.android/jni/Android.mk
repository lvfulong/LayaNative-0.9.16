
LOCAL_PATH:= $(call my-dir)

#liblayaair.a
include $(CLEAR_VARS)
LOCAL_MODULE := liblayaair
LOCAL_CFLAGS := \
  -fexceptions \
  -Wno-multichar \
  -DANDROID \
  -DJS_V8 \
  -DIN_LIBRARY \
  -D_GLIBCXX_PERMIT_BACKWARD_HASH \
  -DV8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR \
  -frtti \

LOCAL_CFLAGS += -std=c++11
ifeq ($(APP_PERFTEST),1)
LOCAL_CFLAGS += -DPERFTEST 
endif

LOCAL_CXXFLAGS := -O3

LOCAL_SRC_FILES := \
        ../../../../source/conch/JSWrapper/v8debug/debug-agent.cpp \
        ../../../../source/conch/JSWrapper/v8debug/V8Win32Socket.cpp \
        ../../../../source/conch/JSWrapper/v8debug/V8HeapProfiler.cpp \
        ../../../../source/conch/JSWrapper/v8debug/V8CpuProfile.cpp \
        ../../../../source/conch/JSWrapper/v8debug/V8Console.cpp \
        ../../../../source/conch/JSWrapper/JSInterface/JSInterface.cpp \
        ../../../../source/conch/JSWrapper/JSInterface/V8/JSEnv.cpp \
        ../../../../source/conch/JSWrapper/JSInterface/V8/JsBinder.cpp \
        ../../../../source/conch/JSWrapper/JSInterface/V8/JSArrayBuffer.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSAndroidEditBox.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSAppCache.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSAudio.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSConchConfig.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSConsole.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSDOMParser.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSFile.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSFileReader.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSFileSystem.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSGlobalExportCFun.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSGraphics.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSHistory.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSImage.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSInput.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSLayaContext.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSMarket.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSMeasureTextInfo.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSNode2D.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSNotify.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSParticleTemplate2D.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSRuntime.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSTextMemoryCanvas.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSWebGLContext.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSWebSocket.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSXmlAttr.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSXmlNode.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSZip.cpp\
        ../../../../source/conch/JSWrapper/LayaWrap/XMLHttpRequest.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSShaderActiveInfo.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSArrayBufferRef.cpp \
        ../../../../source/conch/Audio/android/JCAudioMp3Media.cpp \
        ../../../../source/conch/Audio/JCAudioManager.cpp \
        ../../../../source/conch/WebSocket/WebSocket.cpp \
        ../../../../source/conch/JCCmdDispatchManager.cpp \
        ../../../../source/conch/JCConch.cpp \
        ../../../../source/conch/JCConchRender.cpp \
        ../../../../source/conch/JCPublicCmdDispath.cpp \
        ../../../../source/conch/JCThreadCmdMgr.cpp \
        ../../../../source/conch/JCScrpitRuntime.cpp \
        ../../../../source/conch/JCSystemConfig.cpp \
        ../../../../source/conch/CToJavaBridge.cpp \
        ../../../../source/conch/JNIFun.cpp \
		../../../../source/conch/Bridge/JCConchBridge.cpp \


LOCAL_C_INCLUDES := ../../../../../ThirdParty/curl/include/android \
        ../../../../../ThirdParty/png/include/android \
        ../../../../../ThirdParty/jpeg/include/android \
        ../../../../../ThirdParty/freetype/include/android \
        ../../../../../ThirdParty/zip/include/android \
        ../../../../../ThirdParty/v8_4.6.9/include \
        ../../../../../ThirdParty/ogg/include/android \
        ../../../../../ThirdParty/websockets/include/android \
		../../../../../ThirdParty/openssl/include/android \
		../../../../../ThirdParty/icu/include/android \
        ../../../../../ThirdParty/harfbuzz/include/android \
        ../../../../include/common/OpenAL/include \
        ../../../../include/common/OpenAL/OpenAL32/Include \
        ../../../../include/common \
        ../../../../include/render \
        ../../../../include/msgpack/include \
        ../../../../include/render/MonkVG/include \
        ../../../../include/render/MonkVG/src/opengl \
        ../../../../include/render/MonkVG/src \
        ../../../../include/render/MonkVG/glu/include \
        ../../../../include/render/MonkVG/gles2-bc \
        

LOCAL_IS64:=armv7
ifeq ($(TARGET_ARCH),arm64)
LOCAL_IS64:=arm64
else ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_IS64:=arm
else ifeq ($(TARGET_ARCH),x86)
LOCAL_IS64:=x86
endif

LOCAL_LDLIBS    := -llog -lGLESv2 -landroid \
        ../../../../libs/android-$(LOCAL_IS64)/librender.a \
        ../../../../libs/android-$(LOCAL_IS64)/libcommon.a \
        ../../../../../ThirdParty/curl/lib/android-$(LOCAL_IS64)/libcurl.a \
        ../../../../../ThirdParty/v8_4.6.9/lib/android-$(LOCAL_IS64)/libv8_base.a \
        ../../../../../ThirdParty/v8_4.6.9/lib/android-$(LOCAL_IS64)/libv8_nosnapshot.a \
        ../../../../../ThirdParty/v8_4.6.9/lib/android-$(LOCAL_IS64)/libv8_libplatform.a \
        ../../../../../ThirdParty/v8_4.6.9/lib/android-$(LOCAL_IS64)/libv8_libbase.a \
        ../../../../../ThirdParty/websockets/lib/android-$(LOCAL_IS64)/libwebsockets.a \
        ../../../../../ThirdParty/jpeg/lib/android-$(LOCAL_IS64)/libjpeg.a \
        ../../../../../ThirdParty/png/lib/android-$(LOCAL_IS64)/libpng.a \
        ../../../../../ThirdParty/freetype/lib/android-$(LOCAL_IS64)/libfreetype.a \
        ../../../../../ThirdParty/zip/lib/android-$(LOCAL_IS64)/libzip.a \
		../../../../../ThirdParty/ogg/lib/android-$(LOCAL_IS64)/libvorbisfile.a \
		../../../../../ThirdParty/ogg/lib/android-$(LOCAL_IS64)/libvorbis.a \
		../../../../../ThirdParty/ogg/lib/android-$(LOCAL_IS64)/libogg.a \
        ../../../../../ThirdParty/openssl/lib/android-$(LOCAL_IS64)/libssl.a \
        ../../../../../ThirdParty/openssl/lib/android-$(LOCAL_IS64)/libcrypto.a \
		../../../../../ThirdParty/zlib/lib/android-$(LOCAL_IS64)/libz.a \
		../../../../../ThirdParty/icu/lib/android-$(LOCAL_IS64)/libicuuc.a \
        ../../../../../ThirdParty/harfbuzz/lib/android-$(LOCAL_IS64)/libharfbuzz.a \
			
ifeq ($(TARGET_ARCH),arm64)
LOCAL_LDLIBS += $(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/arm64-v8a/libgnustl_static.a
else ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_LDLIBS += $(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/libgnustl_static.a
else ifeq ($(TARGET_ARCH),x86)
LOCAL_LDLIBS += $(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/x86/libgnustl_static.a
else
LOCAL_LDLIBS += $(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/libgnustl_static.a
endif

include $(BUILD_SHARED_LIBRARY)

