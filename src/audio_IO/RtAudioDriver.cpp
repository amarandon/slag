/*
 *   $Id: RtAudioDriver.cpp,v 1.3 2007/01/26 07:39:55 rhizome Exp $
 *
 *      Copyright (C) 2004-2007 Alex Marandon
 *
 *  This file is part of slag, a pattern-based audio sequencer.
 *
 *  slag is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  slag is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with slag; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <samplerate.h>

#include "Exceptions.h"
#include "RtAudioDriver.h"
#include "RtAudio.h"
#include "../config/Config.h"

using audio_IO::RtAudioConnectionException;

int processCallbackWrapper(void *buffer, void*, uint nFrames, double, RtAudioStreamStatus, void* data) {
    RtAudioDriver* rtAudioDriver = static_cast<RtAudioDriver*>(data);

    int nSamples = 2 * nFrames;

    sample_t* float_buffer = new sample_t[nSamples];
    rtAudioDriver->audioProcessCallback(float_buffer);

    // conversion sample_t => short
    src_float_to_short_array(float_buffer, (short*)buffer, nSamples);
    delete float_buffer;
    return 0;
}

RtAudioDriver::RtAudioDriver(ProcessCallback processCallback, 
        const int buffer_n_samples) :
    AudioDriver(processCallback, buffer_n_samples)
{
    Config* config = Config::instance();
    uint bufferSize = config->buffer_n_samples() / 2;  // sample frames
    try {
        rtAudio = new RtAudio();
        if ( rtAudio->getDeviceCount() < 1 ) {
            delete rtAudio;
            RtAudioConnectionException e("No audio devices found!");
            throw e;
        }
        int deviceNumber = rtAudio->getDefaultOutputDevice();
        RtAudio::DeviceInfo info = rtAudio->getDeviceInfo(deviceNumber);
        qDebug(QString("Using audio device number %1 with API %2, name %3").arg(
                    deviceNumber).arg(rtAudio->getCurrentApi()).arg(QString(info.name.c_str())));
        qDebug(QString("sample size is %1").arg(sample_size));
        uint sampleRate = 48000;
        std::vector<uint> sampleRateList = info.sampleRates;
        for(uint i = 0; i < sampleRateList.size(); ++i) {
            qWarning(QString("found samplerate %1").arg(sampleRateList[i]));
            if (sampleRateList[i] >= sampleRate) {
                sampleRate = sampleRateList[i];
                break;
            }
        }
        qWarning(QString("samplerate will be %1").arg(sampleRate));
        config->samplerate(sampleRate);
        RtAudio::StreamParameters* parameters = new RtAudio::StreamParameters;
        parameters->deviceId = deviceNumber;
        parameters->nChannels = 2;
        parameters->firstChannel = 0;

        rtAudio->openStream(parameters, NULL, RTAUDIO_SINT16,
                sampleRate, &bufferSize, processCallbackWrapper, this);

    } catch (RtError &error) {
        delete rtAudio;
        RtAudioConnectionException e(error.getMessage());
        throw e;
    }
    Q_ASSERT(rtAudio != NULL);
}

RtAudioDriver::~RtAudioDriver() {
    delete rtAudio;
}

void RtAudioDriver::connectDriver() {
    Q_ASSERT(rtAudio != NULL);
    rtAudio->startStream();
}

void RtAudioDriver::disconnect() {
    rtAudio->stopStream();
}

//EOF
