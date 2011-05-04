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
**
**
** Customizations by _firesnatch_ for the Motorola Cliq XT
**
*/

#include <math.h>

#define LOG_NDEBUG 0 
#define LOG_NDDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_TAG "AudioHardwareMSM72XX"
#include <utils/Log.h>
#include <utils/String8.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>

#define MOT_FEATURE_PLATFORM_ANDROID 1

// hardware specific functions
// Motorola - Morrison volrange is 0-15
#define AMSS_VOL_FACTOR 15.0

#include <media/AudioSystem.h>
#include <media/AudioRecord.h>
#include "AudioHardware.h"

#define LOG_SND_RPC 1  // Set to 1 to log sound RPC's

#define TTY_MODE_KEY "tty_mode"

namespace android {

/* When this macro is non-zero, we initialize playback and recording
 * only once--when we construct the AudioHardwareMSM72xx class--and 
 * deinitialize them in this class' destructor.  When the macro is 
 * set to zero, we initialize playback and recording before we start
 * the respective operation, and deinitialize them after we stop that
 * operation.
 */
#define INIT_AUDIO_ONCE (0)
static int get_audpp_filter(void);
static int msm72xx_enable_audpp(uint16_t enable_mask, uint32_t device);

static struct rx_iir_filter iir_cfg[3];
static struct adrc_filter adrc_cfg[3];
static uint16_t adrc_flag[3];
static uint16_t eq_flag;
static uint16_t rx_iir_flag[3];
static bool audpp_filter_inited = false;


#define PCM_OUT_DEVICE "/dev/msm_pcm_out"
#define PCM_IN_DEVICE "/dev/msm_pcm_in"
#define PCM_CTL_DEVICE "/dev/msm_pcm_ctl"
#define VOICE_MEMO_DEVICE "/dev/msm_voicememo"


static int m7xSndDrvFd = -1;


const uint32_t AudioHardware::inputSamplingRates[] = {
        8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
};
// ----------------------------------------------------------------------------

AudioHardware::AudioHardware() :
    mInit(false), mMicMute(false), mBluetoothNrec(true), mBluetoothId(0),//fix
    mOutput(0), mSndEndpoints(NULL), mCurSndDevice(-1), mTtyMode(TTY_OFF),
    SND_DEVICE_CURRENT(-1),
    SND_DEVICE_HANDSET(-1),
    SND_DEVICE_SPEAKER(-1),
    SND_DEVICE_HEADSET(-1),
    SND_DEVICE_BT(-1),
    SND_DEVICE_CARKIT(-1),
    SND_DEVICE_TTY_FULL(-1),
    SND_DEVICE_TTY_VCO(-1),
    SND_DEVICE_TTY_HCO(-1),
    SND_DEVICE_NO_MIC_HEADSET(-1),
    SND_DEVICE_FM_HEADSET(-1),
    SND_DEVICE_HEADSET_AND_SPEAKER(-1),
    SND_DEVICE_FM_SPEAKER(-1),
    SND_DEVICE_BT_EC_OFF(-1),
    SND_DEVICE_DUALMIC_SPEAKER(-1),
    SND_DEVICE_TTY_HEADSET(-1),
    SND_DEVICE_HEADPHONE(-1),
    SND_DEVICE_IN_S_SADC_OUT_HANDSET(-1),
    SND_DEVICE_IN_S_SADC_OUT_SPEAKER_PHONE(-1),
    SND_DEVICE_HEADSET_MOS(-1)
{

    LOGI("libaudio Cliq XT v1.0.9 beta7 by firesnatch & turl (%s)", __DATE__);
    if (get_audpp_filter() == 0)
        audpp_filter_inited = true;
        LOGD("start AudioHardware");
#if INIT_AUDIO_ONCE
    if (msm72xx_init_record()) {
        LOGE("msm72xx_init_record failed");
        return;
    }
#endif // INIT_AUDIO_ONCE
   m7xSndDrvFd = open("/dev/msm_snd", O_RDWR);
    if (m7xSndDrvFd >= 0) {
        int rc = ioctl(m7xSndDrvFd, SND_GET_NUM_ENDPOINTS, &mNumSndEndpoints);
        if (rc >= 0) {
            mSndEndpoints = new msm_snd_endpoint[mNumSndEndpoints];
            mInit = true;
            LOGD("constructed (%d SND endpoints)", mNumSndEndpoints);
            struct msm_snd_endpoint *ept = mSndEndpoints;
            for (int cnt = 0; cnt < mNumSndEndpoints; cnt++, ept++) {
                ept->id = cnt;
                ioctl(m7xSndDrvFd, SND_GET_ENDPOINT, ept);
                LOGD("cnt = %d ept->name = %s ept->id = %d\n", cnt, ept->name, ept->id);
#define CHECK_FOR(desc) \
                if (!strcmp(ept->name, #desc)) { \
                    SND_DEVICE_##desc = ept->id; \
                    LOGD("BT MATCH " #desc); \
                } else
                    CHECK_FOR(CURRENT)
                    CHECK_FOR(HANDSET)
                    CHECK_FOR(SPEAKER)
                    CHECK_FOR(BT)
                    CHECK_FOR(BT_EC_OFF)
                    CHECK_FOR(HEADSET)
                    #ifndef MOT_FEATURE_PLATFORM_MSM7K
                    CHECK_FOR(CARKIT)
                    #endif // !MOT_FEATURE_PLATFORM_MSM7K
                    CHECK_FOR(TTY_FULL)
                    CHECK_FOR(TTY_VCO)
                    CHECK_FOR(TTY_HCO)
                    CHECK_FOR(NO_MIC_HEADSET)
                    CHECK_FOR(FM_HEADSET)
                    CHECK_FOR(FM_SPEAKER)
                    CHECK_FOR(HEADSET_AND_SPEAKER) 
                    CHECK_FOR(TTY_HEADSET)
                    CHECK_FOR(HEADPHONE)
                    CHECK_FOR(DUALMIC_SPEAKER)
                    CHECK_FOR(HEADSET_MOS)
                    CHECK_FOR(IN_S_SADC_OUT_HANDSET)
                    CHECK_FOR(IN_S_SADC_OUT_SPEAKER_PHONE)
		    {}
#undef CHECK_FOR
            }
        }
        else LOGE("Could not retrieve number of MSM SND endpoints.");
    }
    else LOGE("Could not open MSM SND driver.");
}



AudioHardware::~AudioHardware()
{
    for (size_t index = 0; index < mInputs.size(); index++) {
        closeInputStream((AudioStreamIn*)mInputs[index]);
    }
    mInputs.clear();
    closeOutputStream((AudioStreamOut*)mOutput);
    delete [] mSndEndpoints;

    LOGD("Closing the Sound driver");
    if (m7xSndDrvFd > 0) {
        close(m7xSndDrvFd);
    }

#if INIT_AUDIO_ONCE
    msm72xx_deinit_record();
#endif // INIT_AUDIO_ONCE

    mInit = false;
}

status_t AudioHardware::initCheck()
{
    LOGD("AudioHardware::initChec");
    return mInit ? NO_ERROR : NO_INIT;
}

AudioStreamOut* AudioHardware::openOutputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate, status_t *status)
{
    { // scope for the lock
        Mutex::Autolock lock(mLock);

        // only one output stream allowed
        if (mOutput) {
            if (status) {
                *status = INVALID_OPERATION;
            }
            return 0;
        }

        // create new output stream
        AudioStreamOutMSM72xx* out = new AudioStreamOutMSM72xx();
        status_t lStatus = out->set(this, devices, format, channels, sampleRate);
        if (status) {
            *status = lStatus;
        }
        if (lStatus == NO_ERROR) {
            mOutput = out;
        } else {
            delete out;
        }
    }
    return mOutput;
}

void AudioHardware::closeOutputStream(AudioStreamOut* out) {
    Mutex::Autolock lock(mLock);
    if (mOutput == 0 || mOutput != out) {
        LOGW("Attempt to close invalid output stream");
    }
    else {
        delete mOutput;
        mOutput = 0;
    }
}


AudioStreamIn* AudioHardware::openInputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate, status_t *status,
        AudioSystem::audio_in_acoustics acoustic_flags)
{
    // check for valid input source
    if (!AudioSystem::isInputDevice((AudioSystem::audio_devices)devices)) {
        LOGE("openInputStream() ERROR not a valid input source!");
        return 0;
    }

    mLock.lock();

    AudioStreamInMSM72xx* in = new AudioStreamInMSM72xx();
    status_t lStatus = in->set(this, devices, format, channels, sampleRate, acoustic_flags);
    if (status) {
        *status = lStatus;
    }
    if (lStatus != NO_ERROR) {
        mLock.unlock();
        LOGE("openInputStream() ERROR set failed!");
        delete in;
        return 0;
    }
    mInputs.add(in);
    mLock.unlock();

    return in;
}

void AudioHardware::closeInputStream(AudioStreamIn* in) {
    Mutex::Autolock lock(mLock);

    ssize_t index = mInputs.indexOf((AudioStreamInMSM72xx *)in);
    if (index < 0) {
        LOGE("closeInputStream() ERROR Attempt to close invalid input stream");
    } else {
        mLock.unlock();
        delete mInputs[index];
        mLock.lock();
        mInputs.removeAt(index);
    }
}

status_t AudioHardware::setMode(int mode)
{
    LOGI("setMode(%d)", mode);
    status_t status = AudioHardwareBase::setMode(mode);
    if (status == NO_ERROR) {
        // make sure that doAudioRouteOrMute() is called by doRouting()
        // even if the new device selected is the same as current one.
	clearCurDevice();
        if (mode == AudioSystem::MODE_NORMAL && mPrevMode == AudioSystem::MODE_RINGTONE){// && (mOutput->devices() & AudioSystem::DEVICE_OUT_WIRED_H$
                // firesnatch 3/27/2011 - work-around for sound going from 
                // headphones to speaker after missed call
                LOGI("setMode() aborted call workaround");
                doRouting(NULL);
        } // end if*/

    }
    mPrevMode = mode;
    return status;
}

bool AudioHardware::checkOutputStandby()
{
    if (mOutput)
        if (!mOutput->checkStandby())
            return false;

    return true;
}

status_t AudioHardware::setMicMute(bool state)
{
    Mutex::Autolock lock(mLock);
    return setMicMute_nosync(state);
}

// always call with mutex held
status_t AudioHardware::setMicMute_nosync(bool state)
{
    LOGI("setMicMute() mMicMute old value=%d, new=%d", mMicMute, state);
    if (mMicMute != state) {
        mMicMute = state;
        return doAudioRouteOrMute(mCurSndDevice == -1 ? SND_DEVICE_CURRENT : mCurSndDevice);
    }
    return NO_ERROR;
}

status_t AudioHardware::getMicMute(bool* state)
{
    *state = mMicMute;
    return NO_ERROR;
}

status_t AudioHardware::setParameters(const String8& keyValuePairs)
{
    static int iLastMode = AudioSystem::MODE_CURRENT;
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    String8 key;
    struct msm_snd_device_config args;
    const char BT_NREC_KEY[] = "bt_headset_nrec";
    const char BT_NAME_KEY[] = "bt_headset_name";
    const char BT_NREC_VALUE_ON[] = "on";
    const char TOGGLE_MUTE_KEY[] = "toggle";

    if (keyValuePairs.length() == 0) return BAD_VALUE;

    /* firesnatch 2/25/2011 - CliqXT mute toggle trick */
    key = String8(TOGGLE_MUTE_KEY);
    if (param.get(key, value) == NO_ERROR) {
        { // begin lock scope
        Mutex::Autolock lock(mLock);
#if 0
        switch (mMode) {
	   case -2: LOGD("[[TOGGLE MUTE]] mMode=MODE_INVALID"); break;
	   case -1: LOGD("[[TOGGLE MUTE]] mMode=MODE_CURRENT"); break;
	   case  0: LOGD("[[TOGGLE MUTE]] mMode=MODE_NORMAL"); break;
	   case  1: LOGD("[[TOGGLE MUTE]] mMode=MODE_RINGTONE"); break;
	   case  2: LOGD("[[TOGGLE MUTE]] mMode=MODE_IN_CALL"); break;
	}
#endif 
        if (mMode == AudioSystem::MODE_IN_CALL && mCurSndDevice != -1) {
            LOGI("setParameters() [[TOGGLE MUTE]] mMode=%d, mCurSndDevice=%d, mMicMute=%d", mMode, mCurSndDevice, mMicMute);

            args.device = mCurSndDevice;
            args.ear_mute = 0;
            args.mic_mute = !mMicMute;
            ioctl(m7xSndDrvFd, SND_SET_DEVICE, &args);

            usleep(2000); // slight delay, not sure if this is needed

            args.device = mCurSndDevice;
            args.ear_mute = 0;
            args.mic_mute = mMicMute;
            ioctl(m7xSndDrvFd, SND_SET_DEVICE, &args);

            iLastMode = mMode;
            return NO_ERROR;

        } // end if
        } // end lock scope

        iLastMode = mMode;
        return NO_ERROR;
    }

    LOGI("setParameters() [A] %s", keyValuePairs.string());

    key = String8(BT_NREC_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == BT_NREC_VALUE_ON) {
            mBluetoothNrec = true;
        } else {
            mBluetoothNrec = false;
            LOGD("Turning noise reduction and echo cancellation off for BT "
                 "headset");
        }
    }
    key = String8(BT_NAME_KEY);
    if (param.get(key, value) == NO_ERROR) {
        mBluetoothId = 0;
        for (int i = 0; i < mNumSndEndpoints; i++) {
            if (!strcasecmp(value.string(), mSndEndpoints[i].name)) {
                mBluetoothId = mSndEndpoints[i].id;
                LOGD("Using custom acoustic parameters for %s", value.string());
                break;
            }
        }
        if (mBluetoothId == 0) {
            LOGD("Using default acoustic parameters "
                 "(%s not in acoustic database)", value.string());
            doRouting(NULL);
        }
    }

    key = String8(TTY_MODE_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == "full") {
            mTtyMode = TTY_FULL;
        } else if (value == "hco") {
            mTtyMode = TTY_HCO;
        } else if (value == "vco") {
            mTtyMode = TTY_VCO;
        } else {
            mTtyMode = TTY_OFF;
        }
        LOGD("Aks TTY mode set to %d", mTtyMode);
        doRouting(NULL);
    }

    return NO_ERROR;
}

String8 AudioHardware::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    return param.toString();
}


int AudioHardware::check_and_set_audpp_parameters(char *buf, int size)
{
    char *p, *ps;
    static const char *const seps = ",";
    int table_num;
    int i, j;
    int device_id=0;
    eq_filter_type eq[12];
    eqalizer eqalizer;
    int fd;
    void *audioeq;
    void *(*eq_cal)(int32_t, int32_t, int32_t, uint16_t, int32_t, int32_t *, int32_t *, uint16_t *);
    uint16_t numerator[6];
    uint16_t denominator[4];
    uint16_t shift[2];

    fd = open(PCM_CTL_DEVICE, O_RDWR);
    if (fd < 0) {
        LOGE("Cannot open PCM Ctl device");
        return -EPERM;
    }
    if ((buf[0] == 'A') && ((buf[1] == '1') || (buf[1] == '2') || (buf[1] == '3'))) {
        /* IIR filter */
        if(buf[1] == '1') device_id=0;
        if(buf[1] == '2') device_id=1;
        if(buf[1] == '3') device_id=2;
	if (!(p = strtok(buf, ",")))
            goto token_err;
	
	/* Table header */	
        table_num = strtol(p + 1, &ps, 10);
        if (!(p = strtok(NULL, seps)))
            goto token_err;
    	/* Table description */
    	if (!(p = strtok(NULL, seps)))
            goto token_err;

	for (i = 0; i < 48; i++) {
	    j = (i >= 40)? i : ((i % 2)? (i - 1) : (i + 1));
            iir_cfg[device_id].iir_params[j] = (uint16_t)strtol(p, &ps, 16);
	    if (!(p = strtok(NULL, seps)))
                goto token_err;
        }
        rx_iir_flag[device_id] = (uint16_t)strtol(p, &ps, 16);
	if (!(p = strtok(NULL, seps)))
            goto token_err;
        iir_cfg[device_id].num_bands = (uint16_t)strtol(p, &ps, 16);
	
        if (ioctl(fd, AUDIO_SET_RX_IIR, &iir_cfg[device_id]) < 0) {
            LOGE("set rx iir filter error.");
            return -EIO;
        }
    } else if (buf[0] == 'B' && buf[1] == '1') {
        /* This is the ADRC record we are looking for.  Tokenize it */
        if(buf[1] == '1') device_id=0;
        if(buf[1] == '2') device_id=1;
        if(buf[1] == '3') device_id=2;
        if (!(p = strtok(buf, ",")))
            goto token_err;

        /* Table header */
        table_num = strtol(p + 1, &ps, 10);
        if (!(p = strtok(NULL, seps)))
            goto token_err;

        /* Table description */
        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_flag[device_id] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[0] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[1] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[2] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[3] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[4] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[5] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[6] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;
        adrc_cfg[device_id].adrc_params[7] = (uint16_t)strtol(p, &ps, 16);

        if (!(p = strtok(NULL, seps)))
            goto token_err;

        LOGD("ADRC Filter ADRC FLAG = %02x.", adrc_flag[device_id]);
        LOGD("ADRC Filter COMP THRESHOLD = %02x.", adrc_cfg[device_id].adrc_params[0]);
        LOGD("ADRC Filter COMP SLOPE = %02x.", adrc_cfg[device_id].adrc_params[1]);
        LOGD("ADRC Filter COMP RMS TIME = %02x.", adrc_cfg[device_id].adrc_params[2]);
        LOGD("ADRC Filter COMP ATTACK[0] = %02x.", adrc_cfg[device_id].adrc_params[3]);
        LOGD("ADRC Filter COMP ATTACK[1] = %02x.", adrc_cfg[device_id].adrc_params[4]);
        LOGD("ADRC Filter COMP RELEASE[0] = %02x.", adrc_cfg[device_id].adrc_params[5]);
        LOGD("ADRC Filter COMP RELEASE[1] = %02x.", adrc_cfg[device_id].adrc_params[6]);
        LOGD("ADRC Filter COMP DELAY = %02x.", adrc_cfg[device_id].adrc_params[7]);
        LOGI("ioctl AUDIO_SET_ADRC");
        if (ioctl(fd, AUDIO_SET_ADRC, &adrc_cfg[device_id]) < 0) {
            LOGE("set adrc filter error.");
            return -EIO;
        }
    } else if (buf[0] == 'C' && buf[1] == '1') {
        /* This is the EQ record we are looking for.  Tokenize it */
        if (!(p = strtok(buf, ",")))
            goto token_err;

        /* Table header */
        table_num = strtol(p + 1, &ps, 10);
        if (!(p = strtok(NULL, seps)))
            goto token_err;
        /* Table description */
        if (!(p = strtok(NULL, seps)))
            goto token_err;

        eq_flag = (uint16_t)strtol(p, &ps, 16);
        if (!(p = strtok(NULL, seps)))
            goto token_err;
        LOGD("EQ flag = %02x.", eq_flag);

        audioeq = ::dlopen("/system/lib/libaudioeq.so", RTLD_NOW);
        if (audioeq == NULL) {
            LOGE("audioeq library open failure");
            return -1;
        }
        eq_cal = (void *(*) (int32_t, int32_t, int32_t, uint16_t, int32_t, int32_t *, int32_t *, uint16_t *))::dlsym(audioeq, "audioeq_calccoefs");
        memset(&eqalizer, 0, sizeof(eqalizer));
        /* Temp add the bands here */
        eqalizer.bands = 8;
        for (i = 0; i < eqalizer.bands; i++) {

            eq[i].gain = (uint16_t)strtol(p, &ps, 16);

            if (!(p = strtok(NULL, seps)))
                goto token_err;
            eq[i].freq = (uint16_t)strtol(p, &ps, 16);

            if (!(p = strtok(NULL, seps)))
                goto token_err;
            eq[i].type = (uint16_t)strtol(p, &ps, 16);

            if (!(p = strtok(NULL, seps)))
                goto token_err;
            eq[i].qf = (uint16_t)strtol(p, &ps, 16);

            if (!(p = strtok(NULL, seps)))
                goto token_err;
            //LOGI("gain[%d] = %d", i, eq[i].gain);
            //LOGI("freq[%d] = %d", i, eq[i].freq);
            //LOGI("type[%d] = %d", i, eq[i].type);
            //LOGI("  qf[%d] = %d", i, eq[i].qf);
            eq_cal(eq[i].gain, eq[i].freq, 48000, eq[i].type, eq[i].qf, (int32_t*)numerator, (int32_t *)denominator, shift);
            for (j = 0; j < 6; j++) {
                eqalizer.params[ ( i * 6) + j] = numerator[j];
            }
            for (j = 0; j < 4; j++) {
                eqalizer.params[(eqalizer.bands * 6) + (i * 4) + j] = denominator[j];
            }
            eqalizer.params[(eqalizer.bands * 10) + i] = shift[0];
        }
        ::dlclose(audioeq);

        if (ioctl(fd, AUDIO_SET_EQ, &eqalizer) < 0) {
            LOGE("set Equalizer error.");
            return -EIO;
        }
    }
    close(fd);
    return 0;

token_err:
    LOGE("malformatted pcm control buffer");
    return -EINVAL;
}

int AudioHardware::get_audpp_filter(void)
{
    struct stat st;
    char *read_buf;
    char *next_str, *current_str;
    int csvfd;

    LOGI("get_audpp_filter");
    static const char *const path = 
        "/system/etc/AudioFilter.csv";
    csvfd = open(path, O_RDONLY);
    if (csvfd < 0) {
        /* failed to open normal acoustic file ... */
        LOGE("failed to open AUDIO_NORMAL_FILTER %s: %s (%d).",
             path, strerror(errno), errno);
        return -1;
    } else LOGI("open %s success.", path);

    if (fstat(csvfd, &st) < 0) {
        LOGE("failed to stat %s: %s (%d).",
             path, strerror(errno), errno);
        close(csvfd);
        return -1;
    }

    read_buf = (char *) mmap(0, st.st_size,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE,
                    csvfd, 0);
    
    if (read_buf == MAP_FAILED) {
        LOGE("failed to mmap parameters file: %s (%d)",
             strerror(errno), errno);
        close(csvfd);
        return -1;
    }

    current_str = read_buf;

    while (1) {
        int len;
        next_str = strchr(current_str, '\n');
        if (!next_str)
           break;
        len = next_str - current_str;
        *next_str++ = '\0';
        if (check_and_set_audpp_parameters(current_str, len)) {
            LOGE("failed to set audpp parameters, exiting.");
            munmap(read_buf, st.st_size);
	    close(csvfd);
	    return -1;
        }
        current_str = next_str;
    }

    munmap(read_buf, st.st_size);
    close(csvfd);    
    return 0;
}

int AudioHardware::msm72xx_enable_audpp(uint16_t enable_mask, int device)
{
    int fd;
    int device_id=0;

    int k=0;
    if (!audpp_filter_inited) return -EINVAL;
    if(device == SND_DEVICE_SPEAKER)
    {
            device_id = 0;
            LOGI("msm72xx_enable_audpp device=SND_DEVICE_SPEAKER device_id=0");
    }
    if(device == SND_DEVICE_HANDSET)
    {
            device_id = 1;
            LOGI("msm72xx_enable_audpp device=_DEVICE_HANDSET device_id=1");
    }
    if(device == SND_DEVICE_HEADSET || device == SND_DEVICE_HEADSET_MOS )
    {
            device_id = 2;
            LOGI("msm72xx_enable_audpp device=SND_DEVICE_HEADSET device_id=2");
    }

    fd = open(PCM_CTL_DEVICE, O_RDWR);
    if (fd < 0) {
        LOGE("Cannot open PCM Ctl device");
        return -EPERM;
    }

    if (adrc_flag[device_id] == 0 && (enable_mask & ADRC_ENABLE))
       	enable_mask &= ~ADRC_ENABLE;
    else if(enable_mask & ADRC_ENABLE)
    {
        LOGD("ADRC Filter ADRC FLAG = %02x.", adrc_flag[device_id]);
        LOGD("ADRC Filter COMP THRESHOLD = %02x.", adrc_cfg[device_id].adrc_params[0]);
        LOGD("ADRC Filter COMP SLOPE = %02x.", adrc_cfg[device_id].adrc_params[1]);
        LOGD("ADRC Filter COMP RMS TIME = %02x.", adrc_cfg[device_id].adrc_params[2]);
        LOGD("ADRC Filter COMP ATTACK[0] = %02x.", adrc_cfg[device_id].adrc_params[3]);
        LOGD("ADRC Filter COMP ATTACK[1] = %02x.", adrc_cfg[device_id].adrc_params[4]);
        LOGD("ADRC Filter COMP RELEASE[0] = %02x.", adrc_cfg[device_id].adrc_params[5]);
        LOGD("ADRC Filter COMP RELEASE[1] = %02x.", adrc_cfg[device_id].adrc_params[6]);
        LOGD("ADRC Filter COMP DELAY = %02x.", adrc_cfg[device_id].adrc_params[7]);
	LOGI("ioctl AUDIO_SET_ADRC");
         if (ioctl(fd, AUDIO_SET_ADRC, &adrc_cfg[device_id]) < 0)
        {
            LOGE("set rx iir filter error.");
            close(fd);
            return -EIO;
        }
    }
    if (eq_flag == 0 && (enable_mask & EQ_ENABLE))
	enable_mask &= ~EQ_ENABLE;
    if (rx_iir_flag[device_id] == 0 && (enable_mask & IIR_ENABLE))
        enable_mask &= ~IIR_ENABLE;   
    else if (enable_mask & IIR_ENABLE)
    {
        LOGD("IIR Filter FLAG = %02x.", rx_iir_flag[device_id]);
        LOGD("IIR NUMBER OF BANDS = %02x.", iir_cfg[device_id].num_bands);
        LOGD("IIR Filter N1 = %02x.", iir_cfg[device_id].iir_params[0]);
        LOGD("IIR Filter N2 = %02x.",  iir_cfg[device_id].iir_params[1]);
        LOGD("IIR Filter N3 = %02x.",  iir_cfg[device_id].iir_params[2]);
        LOGD("IIR Filter N4 = %02x.",  iir_cfg[device_id].iir_params[3]);
        LOGD("IIR FILTER M1 = %02x.",  iir_cfg[device_id].iir_params[24]);
        LOGD("IIR FILTER M2 = %02x.", iir_cfg[device_id].iir_params[25]);
        LOGD("IIR FILTER M3 = %02x.",  iir_cfg[device_id].iir_params[26]);
        LOGD("IIR FILTER M4 = %02x.",  iir_cfg[device_id].iir_params[27]);
        LOGD("IIR FILTER M16 = %02x.",  iir_cfg[device_id].iir_params[39]);
        LOGD("IIR FILTER SF1 = %02x.",  iir_cfg[device_id].iir_params[40]);
        LOGI("ioctl AUDIO_SET_RX_IIR");
         if (ioctl(fd, AUDIO_SET_RX_IIR, &iir_cfg[device_id]) < 0)
        {
            LOGE("set rx iir filter error.");
            close(fd);
            return -EIO;
        }
    }

    LOGI("msm72xx_enable_audpp: 0x%04x (AUDIO_ENABLE_AUDPP)", enable_mask);
    if (ioctl(fd, AUDIO_ENABLE_AUDPP, &enable_mask) < 0) {
        LOGE("enable audpp error");
        close(fd);
        return -EPERM;
    }
    
    close(fd);
    return 0;
}


static status_t set_volume_rpc(int m7xSndDrvFd,
                               uint32_t device,
                               uint32_t method,
                               uint32_t volume)
{
#if LOG_SND_RPC
    LOGD("rpc_snd_set_volume(device=%d, method=%d, volume=%d)\n", device, method, volume);
#endif

    if (device == -1UL) return NO_ERROR;

        if (m7xSndDrvFd < 0) {
        LOGE("Can not open snd device");
        return -EPERM;
    }
    struct msm_snd_volume_config args;
    args.device = device;
    args.method = method;
    args.volume = volume;

    if (ioctl(m7xSndDrvFd, SND_SET_VOLUME, &args) < 0) {
        LOGE("snd_set_volume error.");
        return -EIO;
    }
    return NO_ERROR;
}

status_t AudioHardware::setVoiceVolume(float v)
{
    if (v < 0.01) {
        LOGW("setVoiceVolume(%f) under 0.01, assuming 0.01\n", v);
        v = 0.01;
    } else if (v > 1.0) {
        LOGW("setVoiceVolume(%f) over 1.0, assuming 1.0\n", v);
        v = 1.0;
    }
    // Set all devices the same volume to maintain consistency.
    setMasterVolume(v);
    return NO_ERROR;
}

status_t AudioHardware::setMasterVolume(float v)
{
    Mutex::Autolock lock(mLock);
    int vol = ceil(v * AMSS_VOL_FACTOR);
    LOGI("Set master volume to %d.\n", vol);

// Motorola - Morrison makes these RPC calls while other platforms have them commented.

    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_HANDSET, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_SPEAKER, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_BT,      SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_HEADSET, SND_METHOD_VOICE, vol);
/*mot:cvk011c set  volume for other devices */ 
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_HEADSET_MOS, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_HEADPHONE, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_TTY_HEADSET, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_TTY_VCO, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_TTY_HCO, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_IN_S_SADC_OUT_HANDSET, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_IN_S_SADC_OUT_SPEAKER_PHONE, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_FM_HEADSET, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_FM_SPEAKER, SND_METHOD_VOICE, vol);
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_DUALMIC_SPEAKER, SND_METHOD_VOICE, vol);
    // This is a redundant setting.
    set_volume_rpc(m7xSndDrvFd, SND_DEVICE_CURRENT, SND_METHOD_VOICE, vol);

    // We return an error code here to let the audioflinger do in-software
    // volume on top of the maximum volume that we set through the SND API.
    // return error - software mixer will handle it
    return -1;
}


status_t AudioHardware::do_route_audio_rpc(int m7xSndDrvFd, int device,
                                   bool ear_mute, bool mic_mute)

{
    if (device == -1L)
        return NO_ERROR;

    int fd;

    fd = m7xSndDrvFd;

    if (fd < 0) {
        LOGE("Can not open snd device");
        return -EPERM;
    }
    // RPC call to switch audio path
    /* rpc_snd_set_device(
     *     device,            # Hardware device enum to use
     *     ear_mute,          # Set mute for outgoing voice audio
     *                        # this should only be unmuted when in-call
     *     mic_mute,          # Set mute for incoming voice audio
     *                        # this should only be unmuted when in-call or
     *                        # recording.
     */  
    struct msm_snd_device_config args;
    args.device = device;
    args.ear_mute = ear_mute ? SND_MUTE_MUTED : SND_MUTE_UNMUTED;
    args.mic_mute = mic_mute ? SND_MUTE_MUTED : SND_MUTE_UNMUTED;
    LOGI("do_route_audio_rpc(device=%d, ear_mute=%d, mic_mute=%d)", args.device, args.ear_mute, args.mic_mute);
    if (ioctl(fd, SND_SET_DEVICE, &args) < 0) {
        LOGE("snd_set_device error."); 
        return -EIO;
    }

    return NO_ERROR;
}

// always call with mutex held
status_t AudioHardware::doAudioRouteOrMute(int device)
{
    bool ear_mute = 1;
    if (device == SND_DEVICE_BT || device == SND_DEVICE_CARKIT) {
        if (mBluetoothId) {
            device = mBluetoothId;
        } else if (!mBluetoothNrec) {
#ifdef MOT_FEATURE_PLATFORM_MSM7K
          // NOTE: bug7550; when modem side echo cancellation is fixed,
          //                please revert to "device = SND_DEVICE_BT_EC_OFF"
          // Note2: bug20360; echo cancellation on the AG was disabled for BT in the modem side
          // for all headsets for device SND_DEVICE_BT.  SND_DEVICE_BT_EC_OFF is not defined or supported
	      // device = SND_DEVICE_BT_EC_OFF;
	      device = SND_DEVICE_BT;   // this is just temporary until modem side EC is fixed.
#else
          device = SND_DEVICE_BT_EC_OFF;
#endif
        }
    }
    // Motorola Morrison - two new audio devices for FM
    // unmute ear path for fm device
    if(( device == SND_DEVICE_FM_HEADSET ) || ( device == SND_DEVICE_FM_SPEAKER ))
    {
    	ear_mute = 0;
    }
    else
    {
    	ear_mute = (mMode != AudioSystem::MODE_IN_CALL);
    }

    return do_route_audio_rpc(m7xSndDrvFd,
                              device, ear_mute, mMicMute);

}
//
// MOT_LV
// MORRISON/MOTUS - new function
int AudioHardware::getHeadsetType()
{

  int   fd1, val1;
  size_t nbytes;
  char buf[20];
  char buf1[20];
  int  hs_type = SND_DEVICE_HEADSET;
 

    /* firesnatch 12/30/2010 */
    /* On the Cliq XT, the file seems to be in a different location */
    fd1 = open("/sys/devices/virtual/switch/hs/state", O_RDONLY);
    if (fd1 < 0) {
      	LOGE("getHeadsetType() Cannot open state file");
       	return hs_type;
    }

    if(read(fd1 , buf1 , sizeof(buf1)) > 0)
    {
      val1 = strtol( buf1 , NULL,0);
      LOGD("getHeadsetType() type=%d", val1);
      switch (val1) {
      case 5:	 // MOS Headset
              hs_type = SND_DEVICE_HEADSET_MOS;
              break;
      case 7:	 // TV OUT  
      case 3:    // Normal headphones
      default :     
              hs_type = SND_DEVICE_HEADSET;       
      }     
    }

    close(fd1);
    return (hs_type );
}


status_t AudioHardware::doRouting(AudioStreamInMSM72xx *input)
{
 
    Mutex::Autolock lock(mLock);
    uint32_t outputDevices = mOutput->devices();
    status_t ret = NO_ERROR;

    int audProcess = (ADRC_DISABLE | EQ_DISABLE | IIR_DISABLE);
    int sndDevice = -1;

    if (input != NULL) {
        uint32_t inputDevice = input->devices();
        LOGI("Aks [input] doRouting, mTtyMode=%d inputdevices=0x%x outputdevices=0x%x mode=%d", mTtyMode, inputDevice, outputDevices, mMode);
        if (inputDevice != 0) {
            if (inputDevice & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET) {
                LOGI("Routing audio to Bluetooth PCM");
                sndDevice = SND_DEVICE_BT;
            } else if (inputDevice & AudioSystem::DEVICE_IN_WIRED_HEADSET) {
                if ((outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET) &&
                    (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER)) {
                    LOGI("Routing audio to Wired Headset and Speaker");
                    sndDevice = SND_DEVICE_HEADSET_AND_SPEAKER;
                    audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
                } else {
                    LOGI("Routing audio to Wired Headset");
                    sndDevice = getHeadsetType();
                }
            } else {
                if (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) {
                    if(mMode != AudioSystem::MODE_IN_CALL) {
                        LOGI("out-of-call: Routing audio to SND_DEVICE_SPEAKER");
                        sndDevice = SND_DEVICE_SPEAKER;
                    }
                    else {
                        LOGI("in-call: Routing audio to SND_DEVICE_DUALMIC_SPEAKER");
                        sndDevice = SND_DEVICE_DUALMIC_SPEAKER;
                        audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
                    }
                } else {
                    if(mMode != AudioSystem::MODE_IN_CALL) {
                        LOGI("out-of-call: Routing audio to SND_DEVICE_HANDSET");
                        sndDevice = SND_DEVICE_HANDSET;
                    }
                    else {
                        LOGI("in-call: Routing audio to dualmic SND_DEVICE_IN_S_SADC_OUT_HANDSET");
                        sndDevice = SND_DEVICE_IN_S_SADC_OUT_HANDSET;
                        audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
                    }
                }
            }
        }
        // if inputDevice == 0, restore output routing
    }

    if (sndDevice == -1) {
        if (outputDevices & (outputDevices - 1)) {
            if ((outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) == 0) {
                LOGI("Hardware does not support requested route combination (%#X),"
                     " picking closest possible route...", outputDevices);
            }
        }
        LOGI("Aks [output] doRouting, mTtyMode=%d outputdevices=0x%x mode=%d", mTtyMode, outputDevices, mMode);

        if ((mTtyMode != TTY_OFF) /* && (mMode == AudioSystem::MODE_IN_CALL) */ &&
            (outputDevices & (DEVICE_OUT_TTY | AudioSystem::DEVICE_OUT_WIRED_HEADSET | AudioSystem::DEVICE_OUT_WIRED_HEADPHONE))) {
            if (mTtyMode == TTY_FULL) {
                LOGI("Routing audio to TTY FULL Mode\n");
                sndDevice = SND_DEVICE_TTY_HEADSET; // Motorola , a24159, IKMORRISON-2009: Using correct SND_DEVICE for TTY_FULL mode
            } else if (mTtyMode == TTY_VCO) {
                LOGI("Routing audio to TTY VCO Mode\n");
                sndDevice = SND_DEVICE_TTY_VCO;
            } else if (mTtyMode == TTY_HCO) {
                LOGI("Routing audio to TTY HCO Mode\n");
                sndDevice = SND_DEVICE_TTY_HCO;
            }
        } else if (outputDevices &
            (AudioSystem::DEVICE_OUT_BLUETOOTH_SCO | AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET)) {
            LOGI("Routing audio to Bluetooth PCM\n");
            sndDevice = SND_DEVICE_BT;
        } else if (outputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT) {
            LOGI("Routing audio to Bluetooth PCM\n");
            sndDevice = SND_DEVICE_CARKIT;
        } else if ((outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET) &&
                   (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER)) {
            LOGI("Routing audio to Wired Headset and Speaker\n");
            sndDevice = SND_DEVICE_HEADSET_AND_SPEAKER;
            audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
        } else if (outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE) {
            if (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) {
                LOGI("Routing audio to No microphone Wired Headset and Speaker (%d,%x)\n", mMode, outputDevices);
                sndDevice = SND_DEVICE_HEADSET_AND_SPEAKER;
                audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
            } else {
                LOGI("Routing audio to No microphone Wired Headset (%d,%x)\n", mMode, outputDevices);
                sndDevice = SND_DEVICE_HEADPHONE;
            }
        } else if (outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET) {
	    if (mMode != AudioSystem::MODE_IN_CALL) {
                LOGI("out-of-call: Routing audio to Wired Headset\n");
	    } else {	
	        LOGI("in-call: Routing audio to Wired Headset\n");
	    }
            sndDevice = getHeadsetType();
	    /* firesnatch 01/01/2011 */
            audProcess = (ADRC_ENABLE | EQ_DISABLE | IIR_ENABLE);
        } else if (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) {
            if (mMode != AudioSystem::MODE_IN_CALL) {
                LOGI("out-of-call: Routing audio to SND_DEVICE_SPEAKER\n");
		sndDevice = SND_DEVICE_SPEAKER;
            }
            else {
                LOGI("in-call: Routing audio to SND_DEVICE_DUALMIC_SPEAKER\n");
		sndDevice = SND_DEVICE_DUALMIC_SPEAKER;
                audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
            }
            audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
   	} else if (outputDevices == DEVICE_OUT_FM_HEADSET) {
            /* firesnatch 01/01/2011 - Added route for FM radio */
    	    LOGI("Routing audio to FM Wired Headset without MIC\n");
            sndDevice = SND_DEVICE_FM_HEADSET;
            audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
        } else if (outputDevices == DEVICE_OUT_FM_SPEAKER) {
            /* firesnatch 01/01/2011 - Added route for FM radio */
            LOGI("Routing audio to FM Speakerphone\n");
	    sndDevice = SND_DEVICE_FM_SPEAKER;
            audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
    	} else {
            if (mMode != AudioSystem::MODE_IN_CALL) {
                LOGI("out-of-call: Routing audio to SND_DEVICE_HANDSET\n");
                sndDevice  = SND_DEVICE_HANDSET;
            }
            else {
                LOGI("in-call: Routing audio to dualmic SND_DEVICE_IN_S_SADC_OUT_HANDSET\n");
                sndDevice = SND_DEVICE_IN_S_SADC_OUT_HANDSET;
	    }
            audProcess = (ADRC_ENABLE | EQ_ENABLE | IIR_ENABLE);
        }
    }

    if (sndDevice != -1 && sndDevice != mCurSndDevice) {
        ret = doAudioRouteOrMute(sndDevice);
        msm72xx_enable_audpp(audProcess,sndDevice);
        mCurSndDevice = sndDevice;
    }

    return ret;
}

status_t AudioHardware::checkMicMute()
{
    Mutex::Autolock lock(mLock);
    if (mMode != AudioSystem::MODE_IN_CALL) {
        setMicMute_nosync(true);
    }
    return NO_ERROR;
}

// w30216 bug6623
size_t AudioHardware::getInputBufferSize(uint32_t sampleRate, int format, int channelCount)
{
    if (checkInputSampleRate(sampleRate) != NO_ERROR) {
        LOGW("getInputBufferSize bad sampling rate: %d", sampleRate);
        return 0;
    }
    if (format != AudioSystem::PCM_16_BIT) {
        LOGW("getInputBufferSize bad format: %d", format);
        return 0;
    }
    if (channelCount < 1 || channelCount > 2) {
        LOGW("getInputBufferSize bad channel count: %d", channelCount);
        return 0;
    }

    return 2048*channelCount;
}

// w30216 bug6623
status_t AudioHardware::checkInputSampleRate(uint32_t sampleRate)
{
    for (uint32_t i = 0; i < sizeof(inputSamplingRates)/sizeof(uint32_t); i++) {
        if (sampleRate == inputSamplingRates[i]) {
            return NO_ERROR;
        } 
    }
    return BAD_VALUE;
}

status_t AudioHardware::dumpInternals(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioHardware::dumpInternals\n");
    snprintf(buffer, SIZE, "\tmInit: %s\n", mInit? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmStandby: %s\n", mStandby? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmOutputStandby: %s\n", mOutputStandby? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmMicMute: %s\n", mMicMute? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmBluetoothNrec: %s\n", mBluetoothNrec? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmBluetoothId: %d\n", mBluetoothId);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardware::dump(int fd, const Vector<String16>& args)
{
    dumpInternals(fd, args);
    for (size_t index = 0; index < mInputs.size(); index++) {
        mInputs[index]->dump(fd, args);
    }

    if (mOutput) {
        mOutput->dump(fd, args);
    }
    return NO_ERROR;
}

uint32_t AudioHardware::getInputSampleRate(uint32_t sampleRate)
{
    uint32_t i;
    uint32_t prevDelta;
    uint32_t delta;

    for (i = 0, prevDelta = 0xFFFFFFFF; i < sizeof(inputSamplingRates)/sizeof(uint32_t); i++, prevDelta = delta) {
        delta = abs(sampleRate - inputSamplingRates[i]);
        if (delta > prevDelta) break;
    }
    // i is always > 0 here
    return inputSamplingRates[i-1];
}

// ----------------------------------------------------------------------------

AudioHardware::AudioStreamOutMSM72xx::AudioStreamOutMSM72xx() :
    mHardware(0), mFd(-1), mStartCount(0), mRetryCount(0), mStandby(true), mDevices(0)
{
}

status_t AudioHardware::AudioStreamOutMSM72xx::set(
        AudioHardware* hw, uint32_t devices, int *pFormat, uint32_t *pChannels, uint32_t *pRate)
{
    int lFormat = pFormat ? *pFormat : 0;
    uint32_t lChannels = pChannels ? *pChannels : 0;
    uint32_t lRate = pRate ? *pRate : 0;

    mHardware = hw;

    // fix up defaults
    if (lFormat == 0) lFormat = format();
    if (lChannels == 0) lChannels = channels();
    if (lRate == 0) lRate = sampleRate();

    // check values
    if ((lFormat != format()) ||
        (lChannels != channels()) ||
        (lRate != sampleRate())) {
        if (pFormat) *pFormat = format();
        if (pChannels) *pChannels = channels();
        if (pRate) *pRate = sampleRate();
        return BAD_VALUE;
    }

    if (pFormat) *pFormat = lFormat;
    if (pChannels) *pChannels = lChannels;
    if (pRate) *pRate = lRate;

    mDevices = devices;

    return NO_ERROR;
}

AudioHardware::AudioStreamOutMSM72xx::~AudioStreamOutMSM72xx()
{
    if (!mHardware->mStandby) {
        if (mFd > 0) close(mFd);
    }
//    mHardware->closeOutputStream(this);
}

ssize_t AudioHardware::AudioStreamOutMSM72xx::write(const void* buffer, size_t bytes)
{
    //LOGI("AudioStreamOutMSM72xx::write(%p, %u)", buffer, bytes);
    status_t status = NO_INIT;
    size_t count = bytes;
    const uint8_t* p = static_cast<const uint8_t*>(buffer);

    if (mStandby) {

        // open driver
        LOGD("open driver mFd %d stb %d", mFd, mStandby);
#if defined(MOT_FEATURE_PLATFORM_ANDROID)
        status = ::open(PCM_OUT_DEVICE, O_RDWR);
#else
        status = ::open("/dev/msm_pcm_out", O_RDWR);
#endif // MOT_FEATURE_PLATFORM_ANDROID
        if (status < 0) {
            LOGE("Cannot open /dev/msm_pcm_out errno: %d", errno);
            goto Error;
        }
        mFd = status;

        // configuration
        LOGD("get config");
        struct msm_audio_config config;
        status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
        if (status < 0) {
            LOGE("Cannot read config");
            goto Error;
        }

        LOGD("set config");
        config.channel_count = AudioSystem::popCount(channels());
        config.sample_rate = sampleRate();
        config.buffer_size = bufferSize();
        config.buffer_count = AUDIO_HW_NUM_OUT_BUF;
        config.codec_type = CODEC_TYPE_PCM;
        status = ioctl(mFd, AUDIO_SET_CONFIG, &config);
        if (status < 0) {
            LOGE("Cannot set config");
            goto Error;
        }

        LOGD("buffer_size: %u", config.buffer_size);
        LOGD("buffer_count: %u", config.buffer_count);
        LOGD("channel_count: %u", config.channel_count);
        LOGD("sample_rate: %u", config.sample_rate);

        // fill 2 buffers before AUDIO_START
        mStartCount = AUDIO_HW_NUM_OUT_BUF;
        mStandby = false;
    }
    while (count) {
        ssize_t written = ::write(mFd, p, count);
        if (written >= 0) {
            count -= written;
            p += written;
        } else {
            if (errno != EAGAIN) return written;
            mRetryCount++;
            LOGW("EAGAIN - retry");
        }
    }
    // start audio after we fill 2 buffers
    if (mStartCount) {
        if (--mStartCount == 0) {
            ioctl(mFd, AUDIO_START, 0);
        }
    }
    return bytes;

Error:
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    LOGE("write() ERROR");
    // Simulate audio output timing in case of error
    usleep(bytes * 1000000 / sizeof(int16_t) / frameSize() / sampleRate());

    return status;
}

status_t AudioHardware::AudioStreamOutMSM72xx::standby()
{
    status_t status = NO_ERROR;
    if (!mStandby && mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    mStandby = true;
    return status;
}

status_t AudioHardware::AudioStreamOutMSM72xx::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioStreamOutMSM72xx::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannels: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmHardware: %p\n", mHardware);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmFd: %d\n", mFd);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmStartCount: %d\n", mStartCount);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmRetryCount: %d\n", mRetryCount);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

bool AudioHardware::AudioStreamOutMSM72xx::checkStandby()
{
    return mStandby;
}


status_t AudioHardware::AudioStreamOutMSM72xx::setParameters(const String8& keyValuePairs)
{
    const char FM_ON_KEY[] = "fm_on";
    const char FM_OFF_KEY[] = "fm_off";
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    int tprevWasFM;
    LOGI("AudioStreamOutMSM72xx::setParameters() [B] %s", keyValuePairs.string());

    tprevWasFM = prevWasFM;
    prevWasFM = 0;

    if (param.getInt(key, device) == NO_ERROR) {
        mDevices = device;
        LOGI("set output routing 0x%x", mDevices);
        status = mHardware->doRouting(NULL);
        param.remove(key);
        mDevicesOld = mDevices;
    }
    /* firesnatch 02/19/2011 - Android 2.3 FM Routing */
    key = String8(FM_ON_KEY);
    if (param.getInt(key, device) == NO_ERROR) {
        /*all of this is buggy.*/
        if (device & AudioSystem::DEVICE_OUT_SPEAKER) {
            mDevices = DEVICE_OUT_FM_SPEAKER;
            LOGE("Chose speakers");
	} else if (device & AudioSystem::DEVICE_OUT_WIRED_HEADSET || device & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE) {
            mDevices = DEVICE_OUT_FM_HEADSET;
            LOGE("Chose headphones");
        }
        //bug fixing
        if (!tprevWasFM) {
            LOGE("device: 0x%x",mDevicesOld);
            mDevices = (mDevicesOld == AudioSystem::DEVICE_OUT_SPEAKER ? DEVICE_OUT_FM_SPEAKER : DEVICE_OUT_FM_HEADSET);
        }
        LOGI("fm_on set output routing %x", mDevices);
        status = mHardware->doRouting(NULL);
        param.remove(key);
        prevWasFM = 1;
    }
    /* firesnatch 02/19/2011 - Android 2.3 FM Routing */
    key = String8(FM_OFF_KEY);
    if (param.getInt(key, device) == NO_ERROR) {
        if (device & AudioSystem::DEVICE_OUT_SPEAKER) {
            mDevices = DEVICE_OUT_FM_SPEAKER;
        } else if (device & AudioSystem::DEVICE_OUT_WIRED_HEADSET || device & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE) {
            mDevices = DEVICE_OUT_FM_HEADSET;
        } else {
            mDevices = AudioSystem::DEVICE_OUT_WIRED_HEADSET;
        }
        LOGI("fm_off set output routing %x", mDevices);
        status = mHardware->doRouting(NULL);
        param.remove(key);
        //prevWasFM = 1;
    }

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioHardware::AudioStreamOutMSM72xx::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        LOGI("get routing %x", mDevices);
        param.addInt(key, (int)mDevices);
    }

    LOGI("AudioStreamOutMSM72xx::getParameters() %s", param.toString().string());
    return param.toString();
}

status_t AudioHardware::AudioStreamOutMSM72xx::getRenderPosition(uint32_t *dspFrames) {
//TODO: enable when supported by driver
    return INVALID_OPERATION;
}
 

// ----------------------------------------------------------------------------

AudioHardware::AudioStreamInMSM72xx::AudioStreamInMSM72xx() :
    mHardware(0), mFd(-1), mState(AUDIO_INPUT_CLOSED), mRetryCount(0),
    mFormat(AUDIO_HW_IN_FORMAT), mChannels(AUDIO_HW_IN_CHANNELS),
    mSampleRate(AUDIO_HW_IN_SAMPLERATE), mBufferSize(AUDIO_HW_IN_BUFFERSIZE),
    mAcoustics((AudioSystem::audio_in_acoustics)0), mDevices(0) 
{
}

status_t AudioHardware::AudioStreamInMSM72xx::set(
        AudioHardware* hw, uint32_t devices, int *pFormat, uint32_t *pChannels, uint32_t *pRate,
        AudioSystem::audio_in_acoustics acoustic_flags)
{
    if (pFormat == 0 || *pFormat != AUDIO_HW_IN_FORMAT) {
        *pFormat = AUDIO_HW_IN_FORMAT;
        return BAD_VALUE;
    }
    if (pRate == 0) {
        return BAD_VALUE;
    }
    uint32_t rate = hw->getInputSampleRate(*pRate);
    if (rate != *pRate) {
        *pRate = rate;
        return BAD_VALUE;
    }

    if (pChannels == 0 || (*pChannels != AudioSystem::CHANNEL_IN_MONO &&
        *pChannels != AudioSystem::CHANNEL_IN_STEREO)) {
        *pChannels = AUDIO_HW_IN_CHANNELS;
        return BAD_VALUE;
    }

    mHardware = hw;

    LOGI("AudioStreamInMSM72xx::set(%d, %d, %u)", *pFormat, *pChannels, *pRate);
    if (mFd >= 0) {
        LOGE("Audio record already open");
        return -EPERM;
    }

    // open audio input device

    status_t status = ::open(PCM_IN_DEVICE, O_RDWR);

    if (status < 0) {
        LOGE("Cannot open %s error: %d", PCM_IN_DEVICE, errno);
        goto Error;
    }
    mFd = status;

    // configuration
//    LOGV("get config");
    struct msm_audio_config config;
    status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
    if (status < 0) {
        LOGE("Cannot read config");
        goto Error;
    }

    config.channel_count = AudioSystem::popCount(*pChannels);
    config.sample_rate = *pRate;
    config.buffer_size = bufferSize();
    config.buffer_count = 2;
    config.codec_type = CODEC_TYPE_PCM;
    status = ioctl(mFd, AUDIO_SET_CONFIG, &config);
    if (status < 0) {
        LOGE("Cannot set config");
        if (ioctl(mFd, AUDIO_GET_CONFIG, &config) == 0) {
            if (config.channel_count == 1) {
                *pChannels = AudioSystem::CHANNEL_IN_MONO;
            } else {
                *pChannels = AudioSystem::CHANNEL_IN_STEREO;
            }
            *pRate = config.sample_rate;
        }
        goto Error;
    }

    status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
    if (status < 0) {
        LOGE("Cannot read config");
        goto Error;
    }
    LOGI("buffer_size: %u", config.buffer_size);
    LOGI("buffer_count: %u", config.buffer_count);
    LOGI("channel_count: %u", config.channel_count);
    LOGI("sample_rate: %u", config.sample_rate);

    mDevices = devices;
    mFormat = AUDIO_HW_IN_FORMAT;
    mChannels = *pChannels;
    mSampleRate = config.sample_rate;
    mBufferSize = config.buffer_size;

    //mHardware->setMicMute_nosync(false);
    mState = AUDIO_INPUT_OPENED;


    return NO_ERROR;

Error:
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    return status;
}

AudioHardware::AudioStreamInMSM72xx::~AudioStreamInMSM72xx()
{
    LOGD("AudioStreamInMSM72xx destructor");
    standby();
}

ssize_t AudioHardware::AudioStreamInMSM72xx::read( void* buffer, ssize_t bytes)
{
     if (!mHardware) return -1;

    size_t count = bytes;
    uint8_t* p = static_cast<uint8_t*>(buffer);

    if (mState < AUDIO_INPUT_OPENED) {
        Mutex::Autolock lock(mHardware->mLock);
        if (set(mHardware, mDevices, &mFormat, &mChannels, &mSampleRate, mAcoustics) != NO_ERROR) {
            LOGE("read() ERROR calling set()");
            return -1;
        }
    }

    if (mState < AUDIO_INPUT_STARTED) {
        if (ioctl(mFd, AUDIO_START, 0)) {
            LOGE("resd() Error starting record");
            return -1;
        }
        mState = AUDIO_INPUT_STARTED;
    }

    // Resetting the bytes value, to return the appropriate read value
    bytes = 0;

    while (count) {
        ssize_t bytesRead = ::read(mFd, p, count);
        if (bytesRead >= 0) {
            count -= bytesRead;
            p += bytesRead;

            bytes += bytesRead;

        } else {
            if (errno != EAGAIN) return bytesRead;
            mRetryCount++;
            LOGW("EAGAIN - retrying");
        }
    }
    return bytes;
}

status_t AudioHardware::AudioStreamInMSM72xx::standby()
{
    LOGI("standby()");
    if (!mHardware) return -1;
    if (mState > AUDIO_INPUT_CLOSED) {
        if (mFd >= 0) {
            ::close(mFd);
            mFd = -1;
        }
        //mHardware->checkMicMute();
        mState = AUDIO_INPUT_CLOSED;
    }
    return NO_ERROR;
}

status_t AudioHardware::AudioStreamInMSM72xx::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioStreamInMSM72xx::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannels: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmHardware: %p\n", mHardware);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmFd count: %d\n", mFd);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmState: %d\n", mState);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmRetryCount: %d\n", mRetryCount);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardware::AudioStreamInMSM72xx::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    LOGI("AudioStreamInMSM72xx::setParameters() [C] %s", keyValuePairs.string());
    if (param.getInt(key, device) == NO_ERROR) {
        LOGI("set input routing %x", device);
        if (device & (device - 1)) {
            LOGE("setParameters() BAD_VALUE");
            status = BAD_VALUE;
        } else {
            mDevices = device;
            status = mHardware->doRouting(this);
        }
        param.remove(key);
    }
    /* firesnatch 2/26/2011 */
    key = String8("vr_mode");
    if (param.getInt(key, device) == NO_ERROR) {
        /* remove voice recognition parm */
        param.remove(key);
    }

    /* firesnatch 3/13/2011 */
    key = String8("input_source");
    if (param.getInt(key, device) == NO_ERROR) {
        /* remove input source parm */
        param.remove(key);
    }

    if (param.size()) {
        LOGE("setParameters() BAD_VALUE");
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioHardware::AudioStreamInMSM72xx::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        LOGD("get routing %x", mDevices);
        param.addInt(key, (int)mDevices);
    }

    LOGI("AudioStreamInMSM72xx::getParameters() %s", param.toString().string());
    return param.toString();
}

// ----------------------------------------------------------------------------

extern "C" AudioHardwareInterface* createAudioHardware(void) {
    return new AudioHardware();
}

}; // namespace android


