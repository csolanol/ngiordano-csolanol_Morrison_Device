ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),morrison)

ifneq ($(BUILD_TINY_ANDROID),true)

$(warning Enabling QCOM audio HW )

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=               \
    AudioPolicyManager.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libmedia

LOCAL_STATIC_LIBRARIES := libaudiopolicybase
LOCAL_MODULE:= libaudiopolicy
LOCAL_CFLAGS += -DWITH_A2DP
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libaudio

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libmedia \
    libhardware_legacy

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
    LOCAL_LDLIBS += -ldl
endif

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

#LOCAL_LDLIBS += -lpthread

#ifeq ($(strip $(BOARD_USES_QCOM_7x_CHIPSET)), true)
    LOCAL_CFLAGS += -DSURF
#else ifeq ($(strip $(BOARD_USES_QCOM_8x_CHIPSET)), true)
#    LOCAL_CFLAGS += -DSURF8K
#endif
  
LOCAL_SRC_FILES += AudioHardware.cpp

LOCAL_CFLAGS += -fno-short-enums

ifeq ($(strip $(BOARD_USES_QCOM_HARDWARE)), true)
LOCAL_CFLAGS += -DMSM72XX_AUDIO
LOCAL_CFLAGS += -DVOC_CODEC_DEFAULT=0
endif

LOCAL_CFLAGS += -DMOT_FEATURE_PLATFORM_MSM7K=1

LOCAL_STATIC_LIBRARIES += libaudiointerface
#ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_SHARED_LIBRARIES += liba2dp
#endif

include $(BUILD_SHARED_LIBRARY)

endif # not BUILD_TINY_ANDROID

endif
