/*
** Copyright 2008, Google Inc.
** Copyright (c) 2009, Code Aurora Forum.All rights reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_AUDIO_HARDWARE_H
#define ANDROID_AUDIO_HARDWARE_H

#include <stdint.h>
#include <sys/types.h>

#include <utils/threads.h>
#include <utils/SortedVector.h>

#include <hardware_legacy/AudioHardwareBase.h>

extern "C" {
#include "msm_audio.h"
}

/* firesnatch */
#define DEVICE_OUT_TTY 0x2000
#define DEVICE_OUT_FM_HEADSET 0x0800
#define DEVICE_OUT_FM_SPEAKER 0x1000

namespace android {

// ----------------------------------------------------------------------------
// Kernel driver interface
//

#define AUDIO_IOCTL_MAGIC 'a'

#define AUDIO_START        _IOW(AUDIO_IOCTL_MAGIC, 0, unsigned)
#define AUDIO_STOP         _IOW(AUDIO_IOCTL_MAGIC, 1, unsigned)
#define AUDIO_FLUSH        _IOW(AUDIO_IOCTL_MAGIC, 2, unsigned)
#define AUDIO_GET_CONFIG   _IOR(AUDIO_IOCTL_MAGIC, 3, unsigned)
#define AUDIO_SET_CONFIG   _IOW(AUDIO_IOCTL_MAGIC, 4, unsigned)
#define AUDIO_GET_STATS    _IOR(AUDIO_IOCTL_MAGIC, 5, unsigned)
#define AUDIO_ENABLE_AUDPP _IOW(AUDIO_IOCTL_MAGIC, 6, unsigned)
#define AUDIO_SET_ADRC     _IOW(AUDIO_IOCTL_MAGIC, 7, unsigned)
#define AUDIO_SET_EQ       _IOW(AUDIO_IOCTL_MAGIC, 8, unsigned)
#define AUDIO_SET_RX_IIR   _IOW(AUDIO_IOCTL_MAGIC, 9, unsigned)

#define EQ_MAX_BAND_NUM	12

#define ADRC_ENABLE  0x0001
#define ADRC_DISABLE 0x0000
#define EQ_ENABLE    0x0002
#define EQ_DISABLE   0x0000
#define IIR_ENABLE   0x0004
#define IIR_DISABLE  0x0000

struct eq_filter_type {
    int16_t gain;
    uint16_t freq;
    uint16_t type;
    uint16_t qf;
};

struct eqalizer {
    uint16_t bands;
    uint16_t params[132];
};

struct rx_iir_filter {
    uint16_t num_bands;
    uint16_t iir_params[48];
};

struct adrc_filter {
    uint16_t adrc_params[8];
};

struct msm_audio_config {
    uint32_t buffer_size;
    uint32_t buffer_count;
    uint32_t channel_count;
    uint32_t sample_rate;
    uint32_t codec_type;
    uint32_t unused[3];
};

struct msm_audio_stats {
    uint32_t out_bytes;
    uint32_t unused[3];
};

enum tty_modes {
    TTY_OFF = 0,
    TTY_VCO = 1,
    TTY_HCO = 2,
    TTY_FULL = 3
};

#define CODEC_TYPE_PCM 0
#define AUDIO_HW_NUM_OUT_BUF 2  // Number of buffers in audio driver for output
// TODO: determine actual audio DSP and hardware latency
#define AUDIO_HW_OUT_LATENCY_MS 0  // Additionnal latency introduced by audio DSP and hardware in ms

#define AUDIO_HW_IN_SAMPLERATE 8000                 // Default audio input sample rate
#define AUDIO_HW_IN_CHANNELS (AudioSystem::CHANNEL_IN_MONO) // Default audio input channel mask
#define AUDIO_HW_IN_BUFFERSIZE 2048                 // Default audio input buffer size
#define AUDIO_HW_IN_FORMAT (AudioSystem::PCM_16_BIT)  // Default audio input sample format

// ----------------------------------------------------------------------------


class AudioHardware : public  AudioHardwareBase
{
    class AudioStreamOutMSM72xx;
    class AudioStreamInMSM72xx;

public:
                        AudioHardware();
    virtual             ~AudioHardware();
    virtual status_t    initCheck();

    virtual status_t    setVoiceVolume(float volume);
    virtual status_t    setMasterVolume(float volume);

    virtual status_t    setMode(int mode);

    // mic mute
    virtual status_t    setMicMute(bool state);
    virtual status_t    getMicMute(bool* state);

    virtual status_t    setParameters(const String8& keyValuePairs);
    virtual String8     getParameters(const String8& keys);

    // create I/O streams
    virtual AudioStreamOut* openOutputStream(
                                uint32_t devices,
                                int *format=0,
                                uint32_t *channels=0,
                                uint32_t *sampleRate=0,
                                status_t *status=0);

    virtual AudioStreamIn* openInputStream(

                                uint32_t devices,
                                int *format,
                                uint32_t *channels,
                                uint32_t *sampleRate,
                                status_t *status,
                                AudioSystem::audio_in_acoustics acoustics);

    virtual    void        closeOutputStream(AudioStreamOut* out);
    virtual    void        closeInputStream(AudioStreamIn* in);

            void        closeOutputStream(AudioStreamOutMSM72xx* out);
            void        closeInputStream(AudioStreamInMSM72xx* in);

    virtual size_t getInputBufferSize(uint32_t sampleRate, int format, int channelCount);    
    void        clearCurDevice() { mCurSndDevice = -1; }
protected:
    virtual status_t    dump(int fd, const Vector<String16>& args);

private:

    status_t    doAudioRouteOrMute(int device);
    status_t    setMicMute_nosync(bool state);
    status_t    checkMicMute();
    status_t    dumpInternals(int fd, const Vector<String16>& args);
    uint32_t    getInputSampleRate(uint32_t sampleRate);
    bool        checkOutputStandby();
    status_t    doRouting(AudioStreamInMSM72xx *input);
    status_t    checkInputSampleRate(uint32_t sampleRate);
#if defined(MOT_FEATURE_PLATFORM_MSM7K)
    int get_audpp_filter(void);
    int check_and_set_audpp_parameters(char *buf, int size);
    int msm72xx_enable_audpp(uint16_t enable_mask, int device);
    status_t do_route_audio_rpc(int m7xSndDrvFd, int device, bool ear_mute, bool mic_mute);
    int getHeadsetType();
#else
    status_t do_route_audio_rpc(int  device, bool ear_mute, bool mic_mute);
#endif
    AudioStreamInMSM72xx*   getActiveInput_l();

    class AudioStreamOutMSM72xx : public AudioStreamOut {
    public:
                            AudioStreamOutMSM72xx();
        virtual             ~AudioStreamOutMSM72xx();
                status_t    set(AudioHardware* mHardware,
                                uint32_t devices,
                                int *pFormat,
                                uint32_t *pChannels,
                                uint32_t *pRate);
        virtual uint32_t    sampleRate() const { return 44100; }
        // must be 32-bit aligned - driver only seems to like 4800
        virtual size_t      bufferSize() const { return 4800; }
        virtual uint32_t    channels() const { return AudioSystem::CHANNEL_OUT_STEREO; }
        virtual int         format() const { return AudioSystem::PCM_16_BIT; }
//#if defined(MOT_FEATURE_PLATFORM_MSM7K)
//        virtual uint32_t	latency() const { return 1; }
//#else
        virtual uint32_t    latency() const { return (1000*AUDIO_HW_NUM_OUT_BUF*(bufferSize()/frameSize()))/sampleRate()+AUDIO_HW_OUT_LATENCY_MS; }
//#endif
        virtual status_t    setVolume(float left, float right) { return INVALID_OPERATION; }
        virtual ssize_t     write(const void* buffer, size_t bytes);
        virtual status_t    standby();
        virtual status_t    dump(int fd, const Vector<String16>& args);
                bool        checkStandby();
        virtual status_t    setParameters(const String8& keyValuePairs);
        virtual String8     getParameters(const String8& keys);
                uint32_t    devices() { return mDevices; }
        virtual status_t    getRenderPosition(uint32_t *dspFrames);

    private:
                AudioHardware* mHardware;
                int         mFd;
                int         mStartCount;
                int         mRetryCount;
                bool        mStandby;
                uint32_t    mDevices;
                uint32_t    mDevicesOld;
                int         prevWasFM;
    };

    class AudioStreamInMSM72xx : public AudioStreamIn {
    public:
        enum input_state {
            AUDIO_INPUT_CLOSED,
            AUDIO_INPUT_OPENED,
            AUDIO_INPUT_STARTED
        };

                            AudioStreamInMSM72xx();
        virtual             ~AudioStreamInMSM72xx();
                status_t    set(AudioHardware* mHardware,
                                uint32_t devices,
                                int *pFormat,
                                uint32_t *pChannels,
                                uint32_t *pRate,
                                AudioSystem::audio_in_acoustics acoustics);
        virtual size_t      bufferSize() const { return mBufferSize; }
        virtual uint32_t    channels() const { return mChannels; }
        virtual int         format() const { return mFormat; }
        virtual uint32_t    sampleRate() const { return mSampleRate; }
        virtual status_t    setGain(float gain) { return INVALID_OPERATION; }
        virtual ssize_t     read(void* buffer, ssize_t bytes);
        virtual status_t    dump(int fd, const Vector<String16>& args);
        virtual status_t    standby();
        virtual status_t    setParameters(const String8& keyValuePairs);
        virtual String8     getParameters(const String8& keys);
        virtual unsigned int  getInputFramesLost() const { return 0; }
                uint32_t    devices() { return mDevices; }
                int         state() const { return mState; }

    private:
                AudioHardware* mHardware;
                int         mFd;
                int         mState;
                int         mRetryCount;
                int         mFormat;
                uint32_t    mChannels;
                uint32_t    mSampleRate;
                size_t      mBufferSize;
                AudioSystem::audio_in_acoustics mAcoustics;
                uint32_t    mDevices;
    };
    // w30216 bug6623
    static const uint32_t inputSamplingRates[];

            bool        mInit;
            bool        mStandby;
            bool        mOutputStandby;
            bool        mMicMute;
            bool        mBluetoothNrec;
            uint32_t    mBluetoothId;
            AudioStreamOutMSM72xx*  mOutput;
            SortedVector <AudioStreamInMSM72xx*>   mInputs;

            msm_snd_endpoint *mSndEndpoints;
            int mNumSndEndpoints;
            int mCurSndDevice;
            int mTtyMode;
            int mPrevMode;

     friend class AudioStreamInMSM72xx;
            Mutex       mLock;

            int SND_DEVICE_CURRENT;
            int SND_DEVICE_HANDSET;
            int SND_DEVICE_SPEAKER;
            int SND_DEVICE_HEADSET;
            int SND_DEVICE_BT;
            int SND_DEVICE_CARKIT;
            int SND_DEVICE_TTY_FULL;
            int SND_DEVICE_TTY_VCO;
            int SND_DEVICE_TTY_HCO;
            int SND_DEVICE_NO_MIC_HEADSET;
            int SND_DEVICE_FM_HEADSET;
            int SND_DEVICE_HEADSET_AND_SPEAKER;
            int SND_DEVICE_FM_SPEAKER;
            int SND_DEVICE_BT_EC_OFF;
#if defined(MOT_FEATURE_PLATFORM_MSM7K)
            int SND_DEVICE_DUALMIC_SPEAKER;
            int SND_DEVICE_TTY_HEADSET;
            int SND_DEVICE_HEADPHONE;
	    int SND_DEVICE_IN_S_SADC_OUT_HANDSET;
            int SND_DEVICE_IN_S_SADC_OUT_SPEAKER_PHONE;
            int SND_DEVICE_HEADSET_MOS;
#endif
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif // ANDROID_AUDIO_HARDWARE_MSM72XX_H
