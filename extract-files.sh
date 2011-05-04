#!/bin/sh

# Copyright (C) 2010 The Android Open Source Project
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

DEVICE=morrison

mkdir -p ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libaudio.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libaudioeq.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libcamera.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libmmcamera.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libmm-qcamera-tgt.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libhpprop.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libmmjpeg.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libFMRadio.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libfmradio_jni.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/AudioFilter.csv ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/AudioPara4.csv ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libfmradioplayer.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/app/FMRadio.apk ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libloc_api.so ../../../vendor/motorola/$DEVICE/proprietary


#RIL
adb pull /system/lib/libcm.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libdsm.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libdss.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libgsdi_exp.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libgstk_exp.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libmmgsdilib.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libnv.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/liboncrpc.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libqmi.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libqueue.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libril-moto-umts-1.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libril-qc-1.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libsnd.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libwms.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libwmsts.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/qmuxd ../../../vendor/motorola/$DEVICE/proprietary

#adb pull /system/lib/libnmea.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/egl/libEGL_POWERVR_SGX530_121.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/egl/libGLES_android.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/egl/libGLES_qcom.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/hw/gralloc.msm7k.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/hw/lights.msm7k.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/akm/akmd_set.cfg ../../../vendor/motorola/$DEVICE/proprietary

#adb pull /system/app/ProgramMenuSystem.apk ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/app/ProgramMenu.apk ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/app/PhoneConfig.apk ../../../vendor/motorola/$DEVICE/proprietary

#adb pull /system/lib/libbattd.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libglslcompiler.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libHPImgApi.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libIMGegl.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libinterstitial.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libLCML.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/liblvmxipc.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libmoto_ril.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/liboemcamera.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libOMX.TI.AAC.decode.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libOMX.TI.AMR.encode.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libOMX.TI.MP3.decode.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libOMX.TI.WBAMR.decode.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libOMX.TI.WMA.decode.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_aacdec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_amrdec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_amrenc_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_arcomxcore_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_avcdec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_m4vdec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libomx_mp3dec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libomx_sharedlibrary_qc.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libomx_wmadec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libOmxCore.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libOmxH264Dec.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libOmxMp3Dec.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libOmxMpeg4Dec.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libOmxVidEnc.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libmm-adspsvc.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libomx_wmvdec_sharedlibrary.so ../../../vendor/motorola/$DEVICE/proprietary

adb pull /system/lib/libopencore_motlocal.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libopencore_motlocalreg.so ../../../vendor/motorola/$DEVICE/proprietary
#???adb pull /system/lib/libpppd_plugin-ril.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libpvr2d.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libpvrANDROID_WSEGL.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/lib/libspeech.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libsrv_um.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libVendor_ti_omx.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libVendor_ti_omx_config_parser.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/libzxing.so ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/zxing.so ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/akmd2 ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/ap_gain.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/battd ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/chat-ril ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/ftmipcd ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/mdm_panicd ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/pppd-ril ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/pvrsrvinit ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/SaveBPVer ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/tcmd_engine ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/tcmd_sql ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/01_pvplayer_mot.cfg ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/cameraCalFileDef.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/contributors.css ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/excluded-input-devices.xml ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/firmware/wl1271.bin ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/wifi/nvram.txt ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/wifi/sdio-g-cdc-reclaim-wme.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/motorola/12m/key_code_map.txt ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/ppp/peers/pppd-ril.options ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/pvplayer_mot.cfg ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/updatecmds/google_generic_update.txt ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/wifi/fw_wlan1271.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/baseimage.dof ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/conversions.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/h264vdec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/h264venc_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/jpegenc_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/m4venc_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/mp3dec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/mp4vdec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/mpeg4aacdec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/mpeg4aacenc_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/nbamrdec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/nbamrenc_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/postprocessor_dualout.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/ringio.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/usn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/wbamrdec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/wbamrenc_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/wmadec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/dsp/wmv9dec_sn.dll64P ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/act_mirred.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/act_police.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/cls_u32.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/em_u32.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/ifb.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/sch_htb.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/sch_ingress.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/lib/modules/wl127x_test.ko ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/de-DE_gl0_sg.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/de-DE_ta.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/en-GB_kh0_sg.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/en-GB_ta.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/en-US_lh0_sg.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/en-US_ta.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/es-ES_ta.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/es-ES_zl0_sg.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/fr-FR_nk0_sg.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/fr-FR_ta.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/it-IT_cm0_sg.bin ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/tts/lang_pico/it-IT_ta.bin ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/usr/keychars/adp5588_morrison.kcm.bin ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/usr/keychars/headset.kcm.bin ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/usr/keylayout/adp5588_morrison.kl ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/usr/keylayout/headset.kl ../../../vendor/motorola/$DEVICE/proprietary

adb pull /system/bin/touchpad ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/fmradio ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/fmradioserver ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/minipadut ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/tcmd_engine ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/fuel_gauge ../../../vendor/motorola/$DEVICE/proprietary


# BT
adb pull /system/bin/bt_downloader ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/bt_init ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/bin/bt_test_exec ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/bin/bthelp ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/BCM4325D1_004.002.004.0218.0225.hcd ../../../vendor/motorola/$DEVICE/proprietary
adb pull /system/etc/bt_init.config ../../../vendor/motorola/$DEVICE/proprietary
#adb pull /system/etc/init.qcom.bt.sh ../../../vendor/motorola/$DEVICE/proprietary




(cat << EOF) | sed s/__DEVICE__/$DEVICE/g > ../../../vendor/motorola/$DEVICE/$DEVICE-vendor-blobs.mk
# Copyright (C) 2010 The Android Open Source Project
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

# This file is generated by device/motorola/__DEVICE__/extract-files.sh

# Prebuilt libraries that are needed to build open-source libraries
PRODUCT_COPY_FILES := \\
    vendor/motorola/__DEVICE__/proprietary/libloc_api.so:obj/lib/libloc_api.so \\
    vendor/motorola/__DEVICE__/proprietary/libcamera.so:obj/lib/libcamera.so \\
    vendor/motorola/__DEVICE__/proprietary/libril-qc-1.so:obj/lib/libril-qc-1.so
#    vendor/motorola/__DEVICE__/proprietary/libaudio.so:obj/lib/libaudio.so
#    vendor/motorola/__DEVICE__/proprietary/lights.msm7k.so:obj/lib/hw/lights.msm7k.so

#PRODUCT_COPY_FILES += \\
#    vendor/motorola/__DEVICE__/proprietary/ProgramMenuSystem.apk:/system/app/ProgramMenuSystem.apk \\
#    vendor/motorola/__DEVICE__/proprietary/ProgramMenu.apk:/system/app/ProgramMenu.apk \\
#    vendor/motorola/__DEVICE__/proprietary/PhoneConfig.apk:/system/app/PhoneConfig.apk

# All the blobs necessary for passion
PRODUCT_COPY_FILES += \\
    vendor/motorola/__DEVICE__/proprietary/libmmcamera.so:/system/lib/libmmcamera.so \\
    vendor/motorola/__DEVICE__/proprietary/libmm-qcamera-tgt.so:/system/lib/libmm-qcamera-tgt.so \\
    vendor/motorola/__DEVICE__/proprietary/libmmjpeg.so:/system/lib/libmmjpeg.so \\
    vendor/motorola/__DEVICE__/proprietary/libhpprop.so:/system/lib/libhpprop.so \\
    vendor/motorola/__DEVICE__/proprietary/libcamera.so:/system/lib/libcamera.so \\
    vendor/motorola/__DEVICE__/proprietary/libcm.so:/system/lib/libcm.so \\
    vendor/motorola/__DEVICE__/proprietary/libdsm.so:/system/lib/libdsm.so \\
    vendor/motorola/__DEVICE__/proprietary/libdss.so:/system/lib/libdss.so \\
    vendor/motorola/__DEVICE__/proprietary/libgsdi_exp.so:/system/lib/libgsdi_exp.so \\
    vendor/motorola/__DEVICE__/proprietary/libgstk_exp.so:/system/lib/libgstk_exp.so \\
    vendor/motorola/__DEVICE__/proprietary/libmmgsdilib.so:/system/lib/libmmgsdilib.so \\
    vendor/motorola/__DEVICE__/proprietary/libnv.so:/system/lib/libnv.so \\
    vendor/motorola/__DEVICE__/proprietary/liboncrpc.so:/system/lib/liboncrpc.so \\
    vendor/motorola/__DEVICE__/proprietary/libqmi.so:/system/lib/libqmi.so \\
    vendor/motorola/__DEVICE__/proprietary/libqueue.so:/system/lib/libqueue.so \\
    vendor/motorola/__DEVICE__/proprietary/libril-moto-umts-1.so:/system/lib/libril-moto-umts-1.so \\
    vendor/motorola/__DEVICE__/proprietary/libril-qc-1.so:/system/lib/libril-qc-1.so \\
    vendor/motorola/__DEVICE__/proprietary/libsnd.so:/system/lib/libsnd.so \\
    vendor/motorola/__DEVICE__/proprietary/libwms.so:/system/lib/libwms.so \\
    vendor/motorola/__DEVICE__/proprietary/libwmsts.so:/system/lib/libwmsts.so \\
    vendor/motorola/__DEVICE__/proprietary/libGLES_qcom.so:/system/lib/egl/libGLES_qcom.so \\
    vendor/motorola/__DEVICE__/proprietary/libaudioeq.so:/system/lib/libaudioeq.so \\
    vendor/motorola/__DEVICE__/proprietary/AudioFilter.csv:/system/etc/AudioFilter.csv \\
    vendor/motorola/__DEVICE__/proprietary/AudioPara4.csv:/system/etc/AudioPara4.csv \\
    vendor/motorola/__DEVICE__/proprietary/libomx_sharedlibrary_qc.so:/system/lib/libomx_sharedlibrary_qc.so \\
    vendor/motorola/__DEVICE__/proprietary/libomx_wmadec_sharedlibrary.so:/system/lib/libomx_wmadec_sharedlibrary.so \\
    vendor/motorola/__DEVICE__/proprietary/libomx_wmvdec_sharedlibrary.so:/system/lib/libomx_wmvdec_sharedlibrary.so \\
    vendor/motorola/__DEVICE__/proprietary/libOmxH264Dec.so:/system/lib/libOmxH264Dec.so \\
    vendor/motorola/__DEVICE__/proprietary/libOmxMpeg4Dec.so:/system/lib/libOmxMpeg4Dec.so \\
    vendor/motorola/__DEVICE__/proprietary/libOmxMp3Dec.so:/system/lib/libOmxMp3Dec.so \\
    vendor/motorola/__DEVICE__/proprietary/libOmxVidEnc.so:/system/lib/libOmxVidEnc.so \\
    vendor/motorola/__DEVICE__/proprietary/libmm-adspsvc.so:/system/lib/libmm-adspsvc.so \\
    vendor/motorola/__DEVICE__/proprietary/libopencore_motlocal.so:/system/lib/libopencore_motlocal.so \\
    vendor/motorola/__DEVICE__/proprietary/libopencore_motlocalreg.so:/system/lib/libopencore_motlocalreg.so \\
    vendor/motorola/__DEVICE__/proprietary/libspeech.so:/system/lib/libspeech.so \\
    vendor/motorola/__DEVICE__/proprietary/akmd2:/system/bin/akmd2 \\
    vendor/motorola/__DEVICE__/proprietary/bthelp:/system/bin/bthelp \\
    vendor/motorola/__DEVICE__/proprietary/tcmd_engine:/system/bin/tcmd_engine \\
    vendor/motorola/__DEVICE__/proprietary/tcmd_sql:/system/bin/tcmd_sql \\
    vendor/motorola/__DEVICE__/proprietary/01_pvplayer_mot.cfg:/system/etc/01_pvplayer_mot.cfg \\
    vendor/motorola/__DEVICE__/proprietary/nvram.txt:/system/etc/wifi/nvram.txt \\
    vendor/motorola/__DEVICE__/proprietary/sdio-g-cdc-reclaim-wme.bin:/system/etc/wifi/sdio-g-cdc-reclaim-wme.bin \\
    vendor/motorola/__DEVICE__/proprietary/pvplayer_mot.cfg:/system/etc/pvplayer_mot.cfg \\
    vendor/motorola/__DEVICE__/proprietary/adp5588_morrison.kcm.bin:/system/usr/keychars/adp5588_morrison.kcm.bin \\
    vendor/motorola/__DEVICE__/proprietary/headset.kcm.bin:/system/usr/keychars/headset.kcm.bin \\
    vendor/motorola/__DEVICE__/proprietary/adp5588_morrison.kl:/system/usr/keylayout/adp5588_morrison.kl \\
    vendor/motorola/__DEVICE__/proprietary/touchpad:/system/bin/touchpad \\
    vendor/motorola/__DEVICE__/proprietary/tcmd_engine:/system/bin/tcmd_engine \\
    vendor/motorola/__DEVICE__/proprietary/fuel_gauge:/system/bin/fuel_gauge \\
    vendor/motorola/__DEVICE__/proprietary/headset.kl:/system/usr/keylayout/headset.kl \\
    vendor/motorola/__DEVICE__/proprietary/qmuxd:/system/bin/qmuxd \\
    vendor/motorola/__DEVICE__/proprietary/akmd_set.cfg:/system/etc/akm/akmd_set.cfg \\
    vendor/motorola/__DEVICE__/proprietary/BCM4325D1_004.002.004.0218.0225.hcd:/system/etc/BCM4325D1_004.002.004.0218.0225.hcd \\
    vendor/motorola/__DEVICE__/proprietary/bt_downloader:/system/bin/bt_downloader \\
    vendor/motorola/__DEVICE__/proprietary/bt_init:/system/bin/bt_init \\
    vendor/motorola/__DEVICE__/proprietary/bt_init.config:/system/etc/bt_init.config

#    vendor/motorola/__DEVICE__/proprietary/lights.msm7k.so:/system/lib/hw/lights.msm7k.so
#    vendor/motorola/__DEVICE__/proprietary/FMRadio.apk:/system/app/FMRadio.apk
#    vendor/motorola/__DEVICE__/proprietary/libOmxCore.so:/system/lib/libOmxCore.so \\
#    vendor/motorola/__DEVICE__/proprietary/libOmxMp3Dec.so:/system/lib/libOmxMp3Dec.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_aacdec_sharedlibrary.so:/system/lib/libomx_aacdec_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_amrdec_sharedlibrary.so:/system/lib/libomx_amrdec_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_amrenc_sharedlibrary.so:/system/lib/libomx_amrenc_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_arcomxcore_sharedlibrary.so:/system/lib/libomx_arcomxcore_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_avcdec_sharedlibrary.so:/system/lib/libomx_avcdec_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_m4vdec_sharedlibrary.so:/system/lib/libomx_m4vdec_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/libomx_mp3dec_sharedlibrary.so:/system/lib/libomx_mp3dec_sharedlibrary.so \\
#    vendor/motorola/__DEVICE__/proprietary/fmradioserver:/system/bin/fmradioserver \\
#    vendor/motorola/__DEVICE__/proprietary/fmradio:/system/bin/fmradio \\
#    vendor/motorola/__DEVICE__/proprietary/libFMRadio.so:/system/lib/libFMRadio.so \\
#    vendor/motorola/__DEVICE__/proprietary/libfmradio_jni.so:/system/lib/libfmradio_jni.so \\
#    vendor/motorola/__DEVICE__/proprietary/libfmradioplayer.so:/system/lib/libfmradioplayer.so \\
#    vendor/motorola/__DEVICE__/proprietary/bt_downloader:/system/bin/bt_downloader \\
#    vendor/motorola/__DEVICE__/proprietary/bt_init:/system/bin/bt_init \\
#    vendor/motorola/__DEVICE__/proprietary/bt_test_exec:/system/bin/bt_test_exec \\
#    vendor/motorola/__DEVICE__/proprietary/BCM4325D1_004.002.004.0218.0225.hcd:/system/etc/BCM4325D1_004.002.004.0218.0225.hcd \\
#    vendor/motorola/__DEVICE__/proprietary/bt_init.config:/system/etc/bt_init.config \\
#    vendor/motorola/__DEVICE__/proprietary/init.qcom.bt.sh:/system/etc/init.qcom.bt.sh
#    vendor/motorola/__DEVICE__/proprietary/libaudio.so:/system/lib/libaudio.so \\


EOF

./setup-makefiles.sh
