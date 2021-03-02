
LOCAL_PATH:= $(call my-dir)

#liblaya.a
include $(CLEAR_VARS)
LOCAL_MODULE := liblaya
LOCAL_CFLAGS := \
  -fexceptions \
  -Wno-multichar \
  -DANDROID \
  -DJS_SM \
  -DIN_LIBRARY \
  -D_GLIBCXX_PERMIT_BACKWARD_HASH \
  -DV8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR \
  -DOS_ANDROID \
  -frtti \
  -DNEWJSI

LOCAL_CFLAGS += -std=gnu++11
ifeq ($(APP_PERFTEST),1)
LOCAL_CFLAGS += -DPERFTEST 
endif

LOCAL_CXXFLAGS := -O3

LOCAL_SRC_FILES := \
		../../../../source/conch/JSWrapper/JSInterface/JSInterface.cpp \
		../../../../source/conch/JSWrapper/JSInterface/SpiderMonkey/SMEnv.cpp \
		../../../../source/conch/JSWrapper/JSInterface/SpiderMonkey/SMBinder.cpp \
		../../../../source/conch/JSWrapper/JSInterface/SpiderMonkey/SMArrayBuffer.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSCanvas.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSConsole.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSCookie.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSDocument.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSErrorHandle.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSError.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSFunctionInfo.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSMarket.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSGlobalExportCFun.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSImage.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSInput.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSLayaContext.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSLayaTextWord.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSTextWordImage.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSLocation.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSMeasureTextInfo.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSNavigator.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSNode.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSNotify.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSZip.cpp\
		../../../../source/conch/JSWrapper/LayaWrap/JSPattern.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSScreen.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSAndroidEditBox.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSAudio.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSVBitmap.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSArrayTextWord.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSAppCache.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSTextureGroup.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSBoneAnimTemplate.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSBoneAnimation.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSBoneInfo.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSEJTextureData.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSSocket.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSWebSocket.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSDOMParser.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSXmlAttr.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSXmlNode.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/XMLHttpRequest.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSFileSystem.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSMask.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSTextStr.cpp \
        ../../../../source/conch/JSWrapper/LayaWrap/JSTextCanvas.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSArrayWordStr.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSConchConfig.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSLayaWebGLContext.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSGlExt_Mesh.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSProcess.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSFile.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSFileReader.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSConchAndroidBridge.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSObjectTestGC.cpp \
		../../../../source/conch/JSWrapper/v8debug/debug-agent.cc \
		../../../../source/conch/JSWrapper/v8debug/V8Win32Socket.cc \
		../../../../source/conch/JSWrapper/v8debug/V8HeapProfiler.cpp \
		../../../../source/conch/JSWrapper/v8debug/V8CpuProfile.cpp \
		../../../../source/conch/JSWrapper/v8debug/V8Console.cpp \
		../../../../source/conch/network/connect.cpp \
		../../../../source/conch/network/connectmanage.cpp \
		../../../../source/conch/network/network.cpp \
		../../../../source/conch/network/serviceguard.cpp \
		../../../../source/conch/process/process.cpp \
		../../../../source/conch/system/SystemConfig.cpp \
		../../../../source/conch/JSWrapper/LayaWrap/JSMemoryCanvas.cpp \
		../../../../source/conch/WebGL/GLRenderCammands.cpp \
		../../../../source/conch/WebSocket/WebSocket.cpp \
		../../../../source/conch/conch.cpp \
		../../../../source/conch/Render2D.cpp \
		../../../../source/conch/ConchRender.cpp \
		../../../../source/conch/CToJavaBridge.cpp \
		../../../../source/conch/JNIFun.cpp \
		../../../../source/conch/Audio/android/JCAudioMp3Media.cpp \
		../../../../source/conch/Audio/JCAudioManager.cpp \


LOCAL_C_INCLUDES := ../../../../../ThirdParty/curl-7.34.0/include \
		../../../../../ThirdParty/libpng \
		../../../../../ThirdParty/jpeg-9 \
		../../../../../ThirdParty/freetype-2.5.4/include \
		../../../../../ThirdParty/libuv/include \
		../../../../../ThirdParty/libzip_android/lib \
		../../../../../ThirdParty/libzip_android \
		../../../../../ThirdParty/spidermonkey/include/android \
		../../../../../ThirdParty/ogg/include \
		../../../../../ThirdParty/libWebSocket/include/android \
		../../../../include/common/OpenAL/include \
		../../../../include/common/OpenAL/OpenAL32/Include \
		../../../../include/common \
		../../../../include/render2D \
		../../../../include/render3D \
        ../../../../include/msgpack/include 

LOCAL_IS64:=32
ifeq ($(TARGET_ARCH),arm64)
LOCAL_IS64:=64
endif

LOCAL_LDLIBS    := -llog -lGLESv2 -lstdc++ -lz -landroid \
		../../../../libs/android$(LOCAL_IS64)/librender3d.a \
		../../../../libs/android$(LOCAL_IS64)/librender2d.a \
		../../../../libs/android$(LOCAL_IS64)/libcommon.a \
		../../../../../ThirdParty/spidermonkey/lib/android/armeabi-v7a/libjs_static.a \
		../../../../libs/android$(LOCAL_IS64)/libcurl.a \
		../../../../libs/android$(LOCAL_IS64)/libgnustl_static.a \
		../../../../libs/android$(LOCAL_IS64)/libwebsockets.a \
		../../../../libs/android$(LOCAL_IS64)/libjpeg.a \
		../../../../libs/android$(LOCAL_IS64)/libpng.a \
		../../../../libs/android$(LOCAL_IS64)/libfreetype.a \
		../../../../libs/android$(LOCAL_IS64)/libuv.a \
		../../../../libs/android$(LOCAL_IS64)/libzip.a \
		../../../../libs/android$(LOCAL_IS64)/libogg.a \
		../../../../libs/android$(LOCAL_IS64)/libvorbis.a \
		../../../../libs/android$(LOCAL_IS64)/libvorbis-jni.a \

include $(BUILD_SHARED_LIBRARY)

