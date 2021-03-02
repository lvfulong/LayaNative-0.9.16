
LOCAL_PATH:= $(call my-dir)

#libcommon.a
include $(CLEAR_VARS)
LOCAL_MODULE := libcommon
LOCAL_CFLAGS := \
  -DTHIN_COMMON_WITHOUT_DOWNLOAD \
  -fexceptions \
  -Wno-multichar \
  -DANDROID \
  -DIN_LIBRARY \
  -D_GLIBCXX_PERMIT_BACKWARD_HASH \
  -frtti

LOCAL_CFLAGS += -std=c++11

ifeq ($(APP_PERFTEST),1)
LOCAL_CFLAGS += -DPERFTEST 
endif

LOCAL_CXXFLAGS := -O3

LOCAL_SRC_FILES := \
		../../../../source/common/buffer/JCBuffer.cpp \
		../../../../source/common/downloadCache/JCFileTable.cpp \
		../../../../source/common/downloadCache/JCServerFileCache.cpp \
		../../../../source/common/downloadMgr/JCHttpHeader.cpp \
		../../../../source/common/downloadMgr/JCDownloadMgr.cpp \
		../../../../source/common/event/JCEmitter.cpp \
		../../../../source/common/event/JCEventBase.cpp \
		../../../../source/common/fileSystem/JCFileSystem.cpp \
		../../../../source/common/misc/JCGetClockExact.cpp \
		../../../../source/common/misc/JCLayaThreadPool.cpp \
		../../../../source/common/misc/JCWorkerThread.cpp \
		../../../../source/common/misc/JCWorkSemaphore.cpp \
		../../../../source/common/util/JCCommonMethed.cpp \
		../../../../source/common/util/JCLayaUrl.cpp \
		../../../../source/common/util/JCZipFile.cpp \
		../../../../source/common/util/JCZlib.cpp \
		../../../../source/common/util/Log.cpp \
		../../../../source/common/resource/JCFileResManager.cpp \
		../../../../source/common/resource/JCResManager.cpp \
		../../../../source/common/resource/JCResource.cpp \
		../../../../source/common/JNI-lib.cpp          \
		../../../../source/common/JNI-layaWebviewCacheManager.cpp	\
		../../../../source/common/laya8cache/Laya8CacheManager.cpp  \
		../../../../source/common/laya8cache/Laya8DownloadMgr.cpp  \
		../../../../source/common/util/JCCrypto.cpp \

LOCAL_C_INCLUDES := ../../../../../ThirdParty/curl-7.34.0/include \
		../../../../../ThirdParty/libzip_android/lib \
		../../../../../ThirdParty/libzip_android \


LOCAL_IS64:=32
ifeq ($(TARGET_ARCH),arm64)
LOCAL_IS64:=64
endif

LOCAL_LDLIBS    := -llog -lGLESv2 -lz -landroid \
		../../../../libs/android$(LOCAL_IS64)/libzip.a \

ifeq ($(TARGET_ARCH),arm64)
LOCAL_LDLIBS += $(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/arm64-v8a/libgnustl_static.a
#else ifeq ($(TARGET_ARCH), )
else
LOCAL_LDLIBS += $(NDK_DIR)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/libgnustl_static.a
endif

include $(BUILD_SHARED_LIBRARY)

