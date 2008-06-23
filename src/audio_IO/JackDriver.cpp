/*
 *   $Id: JackDriver.cpp,v 1.19 2007/01/26 07:39:55 rhizome Exp $
 *
 *      Copyright (C) 2004-2008 Alex Marandon
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

#include "JackDriver.h"
#include "RingBufferDriver.h"
#include "WavFileOutput.h"
#include "Exceptions.h"
#include "../common.h"

using audio_IO::JackConnectionException;

/** 
 * Callback function called by JACK. It can't be a C++ member function since it
 * has to be called by JACK C code. Well, maybe it could, but I don't know
 * how.
 */
int jackProcessCallback(jack_nframes_t nframes, void *arg) {
    int buf_right_i = 0;
    int buf_left_i  = 0;
    JackDriver *jack_drv = (JackDriver *) arg;

    jack_default_audio_sample_t *out1 = 
        (jack_default_audio_sample_t *) jack_port_get_buffer(jack_drv->output_port_1, 
                nframes);
    jack_default_audio_sample_t *out2 = 
        (jack_default_audio_sample_t *) jack_port_get_buffer(jack_drv->output_port_2, 
                nframes);

    jack_drv->ringBufferDriver->read(jack_drv->audio_buffer);

    for ( int i = 0; i < (jack_drv->jack_buf_size * 2); ) {
        jack_drv->buffer_right[buf_right_i++] = jack_drv->audio_buffer[i++];
        jack_drv->buffer_left[buf_left_i++] = jack_drv->audio_buffer[i++];
    }

    memcpy(out1, jack_drv->buffer_right, sample_size * jack_drv->jack_buf_size);
    memcpy(out2, jack_drv->buffer_left, sample_size * jack_drv->jack_buf_size);

    QMutexLocker locker(&jack_drv->recordingMutex());
    if(jack_drv->recording()) {
        jack_drv->file_output->write(jack_drv->audio_buffer, 
                jack_drv->jack_buf_size * 2);
    }

    return(0);

}

JackDriver::JackDriver(ProcessCallback processCallback, 
        JackErrorCallback error_callback) 
throw (JackConnectionException) : 
    AudioDriver(processCallback)
{

    config = Config::instance();

    if ((client = jack_client_new ("Slag")) == 0) {
        JackConnectionException e("Impossible de se connecter au serveur JACK");
        throw (e);
    }

    jack_buf_size = jack_get_buffer_size(client);

    qDebug(QString("JACK buffer size: %1.").arg(jack_buf_size));
    config->buffer_n_samples(jack_buf_size * 2);
    ringBufferDriver = new RingBufferDriver(processCallback, 
            config->buffer_n_samples());

    int jack_samplerate = jack_get_sample_rate(client);
    qDebug(QString("JACK sample rate: %1.").arg(jack_samplerate));
    config->samplerate(jack_samplerate);

    audio_buffer = new sample_t[config->buffer_n_samples()];
    buffer_right = new sample_t[jack_buf_size];
    buffer_left  = new sample_t[jack_buf_size];

    if(jack_set_process_callback (client, jackProcessCallback, this) == 0) {
        qDebug("Assigning JACK callback function");
    } else {
        JackConnectionException e("Assigning JACK callback function failed");
        throw (e);
    }
    jack_set_error_function(error_callback);

    output_port_1 = jack_port_register (client, "output_1",
            JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_port_2 = jack_port_register (client, "output_2",
            JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

}

JackDriver::~JackDriver() {
    qDebug("JACK driver destruction.");
    jack_client_close(client);
    delete[] audio_buffer;
    delete[] buffer_right;
    delete[] buffer_left;
    delete ringBufferDriver;
}

/**
 * Connect to JACK output ports.
 */
void JackDriver::connectDriver() {
    ringBufferDriver->connectDriver();

    if( jack_activate (client) == 0 ) {
        qDebug("Activating Slag as a JACK client");
    } else {
        JackConnectionException e("Unable to activate Slag as a JACK client");
        throw (e);
    }

    jack_ports = jack_get_ports (client, NULL, NULL,
            JackPortIsPhysical|JackPortIsInput);

    if (Config::instance()->jackAutoconnect()) {
        if(jack_connect (client, jack_port_name (output_port_1), jack_ports[0]) == 0)
            qDebug("Connecting first JACK port.");
        else {
            JackConnectionException e("Unable to connect first JACK port.");
            throw (e);
        }
        if(jack_connect (client, jack_port_name (output_port_2), jack_ports[1]) == 0)
            qDebug("Connecting second JACK port.");
        else {
            JackConnectionException e("Unable to connect second JACK port.");
            throw (e);
        }
    }

    free(jack_ports);
}

void JackDriver::disconnect() {
    qDebug("JACK deconnection.");
    ringBufferDriver->disconnect();
    jack_deactivate(client);
}

//EOF
