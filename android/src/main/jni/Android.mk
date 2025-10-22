LOCAL_PATH := $(call my-dir)


LOCAL_CPP_EXTENSION := .cc

include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/mediaserver/src/openssl/build/openssl/$(TARGET_ARCH_ABI)/lib/libssl.a
LOCAL_EXPORT_CFLAGS := -I$(LOCAL_PATH)/src/mediaserver/src/openssl/build/openssl/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/mediaserver/src/openssl/build/openssl/$(TARGET_ARCH_ABI)/lib/libcrypto.a
LOCAL_EXPORT_CFLAGS := -I$(LOCAL_PATH)/src/mediaserver/src/openssl/build/openssl/$(TARGET_ARCH_ABI)/include

include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)


APP_CPPFLAGS += -std=c++20 -fexceptions 
LOCAL_CPP_FEATURES := rtti exceptions c++17

include $(CLEAR_VARS)

LOCAL_CPP_FEATURES := rtti exceptions c++17
LOCAL_CPPFLAGS += -std=c++17
LOCAL_MODULE := libMedia
LOCAL_CFLAGS :=   -Wall -Wextra -std=c++17 -fexceptions -g -DDEBUG=1
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/net/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/ \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix \
                    $(LOCAL_PATH)/src/mediaserver/src/stun/include \
                    $(LOCAL_PATH)/src/mediaserver/src/openssl/build/openssl/$(TARGET_ARCH_ABI)/include \
                    $(LOCAL_PATH)/src/mediaserver/src/json/include

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/base/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/net/src/*.cpp))  \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/libuv/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix/*.cpp))\
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/stun/src/*.cpp))


include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_MODULE := libglmark2-matrix
LOCAL_CFLAGS := -DGPULOAD_USE_GLESv2 -Wall -Wextra -fexceptions -g -DDEBUG=1 \
                -Wno-error=unused-parameter
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src \
                    $(LOCAL_PATH)/src/glad/include \
                    $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/net/include \
                    

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/libmatrix/*.cc))

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libglmark2-png
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/libpng/*.c))

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_CPP_EXTENSION := .cc
LOCAL_MODULE := libglmark2-ideas
LOCAL_CFLAGS := -DGPULOAD_DATA_PATH="" -DGPULOAD_USE_GLESv2 -Werror -Wall -Wextra -g \
                -Wnon-virtual-dtor -Wno-error=unused-parameter -DDEBUG=1
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src \
                    $(LOCAL_PATH)/src/libmatrix \
                    $(LOCAL_PATH)/src/glad/include
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/scene-ideas/*.cc))
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libglad-egl
LOCAL_CFLAGS := -Werror -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/glad/include
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/glad/src/egl.c))
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libglad-glesv2
LOCAL_CFLAGS := -Werror -Wall -g
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/glad/include
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/glad/src/gles2.c))
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CPP_FEATURES := rtti exceptions c++17
LOCAL_CPPFLAGS += -std=c++17

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libglmark2-android
LOCAL_STATIC_LIBRARIES := libMedia libssl libcrypto libglmark2-matrix libglmark2-png libglmark2-ideas  libglad-egl libglad-glesv2
LOCAL_CFLAGS := -DGPULOAD_DATA_PATH="" -DGPULOAD_VERSION="\"2021.12\"" \
                -DGPULOAD_USE_GLESv2 -Wno-error -Wall -Wextra -g  -DDEBUG=1 \
                -Wno-error=unused-parameter
LOCAL_LDLIBS := -landroid -llog -lz
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src \
                    $(LOCAL_PATH)/src/libmatrix \
                    $(LOCAL_PATH)/src/scene-ideas \
                    $(LOCAL_PATH)/src/scene-terrain \
                    $(LOCAL_PATH)/src/libpng \
                    $(LOCAL_PATH)/src/glad/include \
                    $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/net/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix \
                    $(LOCAL_PATH)/src/mediaserver/src/stun/include \
                    $(LOCAL_PATH)/src/mediaserver/src/openssl/build/openssl/$(TARGET_ARCH_ABI)/include \
                    $(LOCAL_PATH)/src/mediaserver/src/json/include

LOCAL_SRC_FILES := $(filter-out src/canvas% src/gl-state% src/native-state% src/main.cpp, \
                     $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/*.cpp))) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/scene-terrain/*.cpp)) \
                   src/canvas-android.cpp

include $(BUILD_SHARED_LIBRARY)
