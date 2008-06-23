/*
 *  $Id: NullDriver.cpp,v 1.9 2007/01/26 07:39:55 rhizome Exp $
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

#include "NullDriver.h"
#include "WavFileOutput.h"

NullDriver::NullDriver(ProcessCallback processCallback, int buffer_n_samples) : 
    AudioDriver(processCallback, buffer_n_samples),
    active(0),
    audio_buffer(new sample_t[buffer_n_samples])
{ }

NullDriver::~NullDriver() {
    delete[] audio_buffer;
}

void NullDriver::run () { 
    while(active) {
        audioProcessCallback(audio_buffer); 
        // audioProcessCallback() must be called at least as often as with a
        // real audio driver if we want the sound objects to be deleted as if
        // there where read by a true audio thread
        // 10ms is less than 1s / (44100 / 1024)
        recording_mutex.lock();
        if(recording()) {
            file_output->write(audio_buffer, buffer_n_samples);
        }
        recording_mutex.unlock();
        // 10ms is less than 1s / (44100 / 1024)
        float time_to_wait = (static_cast<float>(buffer_n_samples) / 44100) * 1000 / 2;
        msleep(static_cast<int>(time_to_wait));
    }
}

void NullDriver::connectDriver()    { 
    active = 1; 
    start(); 
}

void NullDriver::disconnect() { 
    active = 0; 
    wait();
}

//EOF
