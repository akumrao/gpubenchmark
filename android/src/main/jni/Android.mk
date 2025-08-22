LOCAL_PATH := $(call my-dir)


LOCAL_CPP_EXTENSION := .cc

APP_CPPFLAGS += -fexceptions

include $(CLEAR_VARS)

LOCAL_MODULE := libMedia
LOCAL_CFLAGS :=   -Wall -Wextra -fexceptions -g -DDEBUG=1
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/net/include \
                    $(LOCAL_PATH)/src/mediaserver/src/json/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/ \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/base/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/net/src/*.cpp))  \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/json/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/libuv/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix/*.cpp))

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_MODULE := libgpuload-matrix
LOCAL_CFLAGS := -DGPULOAD_USE_GLESv2 -Wall -Wextra -fexceptions -g -DDEBUG=1 \
                -Wno-error=unused-parameter
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/gpu \
                    $(LOCAL_PATH)/src/glad/include \
                    $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/json/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/net/include

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/libmatrix/*.cc))

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libgpuload-png
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/libpng/*.c))
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_CPP_EXTENSION := .cc
LOCAL_MODULE := libgpuload-ideas
LOCAL_CFLAGS := -DGPULOAD_DATA_PATH="" -DGPULOAD_USE_GLESv2 -Werror -Wall -Wextra -g \
                -Wnon-virtual-dtor -Wno-error=unused-parameter -DDEBUG=1
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/gpu \
                    $(LOCAL_PATH)/src/libmatrix \
                    $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/json/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/ \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix \
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
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libgpuload-android
LOCAL_STATIC_LIBRARIES := libMedia libgpuload-png libgpuload-matrix  libgpuload-ideas  libglad-egl libglad-glesv2
LOCAL_CFLAGS := -DGPULOAD_DATA_PATH="" -DGPULOAD_VERSION="\"2024.03\"" \
                -DGPULOAD_USE_GLESv2 -Wno-error -Wall -Wextra -g  -DDEBUG=1 \
                -Wno-error=unused-parameter
LOCAL_LDLIBS := -landroid -llog -lz
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/gpu \
                    $(LOCAL_PATH)/src/libmatrix \
                    $(LOCAL_PATH)/src/scene-ideas \
                    $(LOCAL_PATH)/src/scene-terrain \
                    $(LOCAL_PATH)/src/libjpeg-turbo \
                    $(LOCAL_PATH)/src/libpng \
                    $(LOCAL_PATH)/src/glad/include \
                    $(LOCAL_PATH)/src/mediaserver/src/base/include \
                    $(LOCAL_PATH)/src/mediaserver/src/json/include \
                    $(LOCAL_PATH)/src/mediaserver/src/net/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/include \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src \
                    $(LOCAL_PATH)/src/mediaserver/src/libuv/src/unix

LOCAL_SRC_FILES := $(filter-out src/gpu/canvas% src/gpu/gl-state% src/gpu/native-state% src/gpu/main.cpp, \
                     $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/gpu/*.cpp))) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/scene-terrain/*.cpp)) \
                   src/gpu/canvas-android.cpp

include $(BUILD_SHARED_LIBRARY)
