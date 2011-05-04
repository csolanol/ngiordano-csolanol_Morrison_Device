# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#

# WARNING: This line must come *before* including the proprietary
# variant, so that it gets overwritten by the parent (which goes
# against the traditional rules of inheritance).
USE_CAMERA_STUB := true

# inherit from the proprietary version
-include vendor/motorola/morrison/BoardConfigVendor.mk

# Hardware
TARGET_BOARD_PLATFORM := msm7k
TARGET_ARCH_VARIANT := armv6j
TARGET_CPU_ABI := armeabi-v6j
TARGET_CPU_ABI2 := armeabi
BOARD_VENDOR_USE_AKMD := akm8973
BOARD_USES_QCOM_LIBS := true
TARGET_BOOTLOADER_BOARD_NAME := morrison
TARGET_NO_BOOTLOADER := true
TARGET_OTA_ASSERT_DEVICE := morr,morrison

# Kernel
TARGET_PREBUILT_KERNEL := device/motorola/morrison/kernel
BOARD_KERNEL_CMDLINE := no_console_suspend=1 console=null quiet
BOARD_USE_USB_MASS_STORAGE_SWITCH := true

# FM Radio
BOARD_HAVE_FM_RADIO := true
TARGET_GLOBAL_CFLAGS += -DHAVE_FM_RADIO
TARGET_GLOBAL_CPPFLAGS += -DHAVE_FM_RADIO
BOARD_USE_BROADCOM_FM_VOLUME_HACK := true

# Browser JS engine
JS_ENGINE := v8

# WiFi
BOARD_WPA_SUPPLICANT_DRIVER := WEXT
WPA_SUPPLICANT_VERSION      := VER_0_5_X
BOARD_WLAN_DEVICE           := bcm4329
WIFI_DRIVER_MODULE_PATH     := "/system/lib/dhd.ko"
WIFI_DRIVER_MODULE_ARG      := "firmware_path=/system/etc/wifi/sdio-g-cdc-reclaim-wme.bin nvram_path=/system/etc/wifi/nvram.txt"
WIFI_DRIVER_MODULE_NAME     := "dhd"

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

# # dev:    size   erasesize  name
#mtd0: 00300000 00020000 "boot"
#mtd1: 0a140000 00020000 "system"
#mtd2: 0bf00000 00020000 "userdata"
#mtd3: 00040000 00020000 "misc"
#mtd4: 00500000 00020000 "recovery"
#mtd5: 06a00000 00020000 "cache"
#mtd6: 00080000 00020000 "splash"

#BOARD_BOOTIMAGE_PARTITION_SIZE := 0x00300000
#BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00500000
#BOARD_SYSTEMIMAGE_PARTITION_SIZE := 0x08400000   # limited so we enforce room to grow
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 0x0bf00000
#BOARD_FLASH_BLOCK_SIZE := 131072

# Graphics
# Use RGB565 surfaces until userspace drivers are upgraded
BOARD_NO_RGBX_8888 := true
BOARD_AVOID_DRAW_TEXTURE_EXTENSION := true
TARGET_LIBAGL_USE_GRALLOC_COPYBITS := true
BOARD_EGL_CFG := device/motorola/morrison/egl.cfg
TARGET_SF_NEEDS_REAL_DIMENSIONS := true

# GPS
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := msm7k
BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION := 3200
BOARD_VENDOR_QCOM_AMSS_VERSION := 3200
BOARD_USES_GPSSHIM := false

# Audio
BOARD_USES_GENERIC_AUDIO := false
TARGET_PROVIDES_LIBAUDIO := true

# Vibrator
BOARD_HAS_VIBRATOR_IMPLEMENTATION := ../../device/motorola/morrison/vibrator.c

# Recovery
TARGET_RECOVERY_WRITE_MISC_PART := 3
BOARD_CUSTOM_RECOVERY_KEYMAPPING:= ../../device/motorola/morrison/recovery/recovery_ui.c

TARGET_PREBUILT_RECOVERY_KERNEL := device/motorola/morrison/recovery_kernel
