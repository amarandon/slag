/*
 *   $Id: ChannelJackDriver.cpp,v 1.18 2007/01/26 07:39:55 rhizome Exp $
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

#include "ChannelJackDriver.h"
#include "JackDriver.h"
#include "../common.h"
#include "../audio_engine/Song.h"
//Added by qt3to4:
#include <Q3PtrList>

using audio_IO::JackConnectionException;

const QString left_suffix = QObject::tr("_L");
const QString right_suffix = QObject::tr("_R");

ChannelJackPort::ChannelJackPort(Channel* channel, jack_client_t* client) : 
    client(client),
    jack_ports(jack_get_ports(client, NULL, NULL, 
                JackPortIsPhysical|JackPortIsInput)),
    connected(false),
    channel(channel),  
    jack_port_L(jack_port_register (client, channel->name() + left_suffix, 
                JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0)),
    jack_port_R(jack_port_register (client, channel->name() + right_suffix, 
                JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0))
{ 
    QObject::connect(channel, SIGNAL(renamed()), this, SLOT(syncName()));
}

ChannelJackPort::~ChannelJackPort() {
    if((jack_port_unregister(client, jack_port_L)) != 0) {
        qWarning("Unable to unregister left JACK port.");
    }
    if((jack_port_unregister(client, jack_port_R)) != 0) {
        qWarning("Unable to unregister right JACK port.");
    }
}

//TODO factoriser connect() et disconnect() dans JackDriver
void ChannelJackPort::connect() {
    if (connected || ( ! Config::instance()->jackAutoconnect() ) )
        return;
    if(jack_connect (client, jack_port_name (jack_port_L), jack_ports[0]) == 0)
        qDebug("Connecting first JACK port.");
    else {
        JackConnectionException e("Unable to connect first JACK port.");
        throw (e);
    }
    if(jack_connect (client, jack_port_name (jack_port_R), jack_ports[1]) == 0)
        qDebug("Connecting second JACK port.");
    else {
        JackConnectionException e("Unable to connect second JACK port.");
        throw (e);
    }
    connected = true;
}

void ChannelJackPort::syncName() {
    jack_port_set_name(jack_port_L, channel->name() + left_suffix);
    jack_port_set_name(jack_port_R, channel->name() + right_suffix);
}

int jackProcessChannelCallback(jack_nframes_t nframes, void *arg) {
    ChannelJackDriver* jack_drv = (ChannelJackDriver*) arg;
    for (Q3PtrList<ChannelJackPort>::iterator it(jack_drv->ports.begin()); 
            it != jack_drv->ports.end(); ++it) {
        int buf_right_i = 0;
        int buf_left_i  = 0;

        jack_default_audio_sample_t *out1 = 
          (jack_default_audio_sample_t *) jack_port_get_buffer(
                                                (*it)->jack_port_L, nframes);
        jack_default_audio_sample_t *out2 = 
          (jack_default_audio_sample_t *) jack_port_get_buffer(
                                                (*it)->jack_port_R, nframes);

        jack_drv->audioProcessChannelCallback((*it)->channel, 
                                              jack_drv->audio_buffer);

        for ( int i = 0; i < (jack_drv->jack_buf_size * 2); ) {
            jack_drv->buffer_right[buf_right_i++] = jack_drv->audio_buffer[i++];
            jack_drv->buffer_left[buf_left_i++] = jack_drv->audio_buffer[i++];
        }

        memcpy(out1, jack_drv->buffer_right, sample_size * jack_drv->jack_buf_size);
        memcpy(out2, jack_drv->buffer_left, sample_size * jack_drv->jack_buf_size);
    }
    return (0);
}

ChannelJackDriver::ChannelJackDriver(JackChannelProcessCallback processCallback,
        JackErrorCallback error_function) 
    throw (JackConnectionException) :
        AudioDriver(NULL)
{
    ports.setAutoDelete(true);

    audioProcessChannelCallback = processCallback;

    config = Config::instance();

    if ((client = jack_client_new ("Slag")) == 0) {
	JackConnectionException e(
                "Unable to connect JACK server. Is jackd running?");
	throw (e);
    }

    jack_set_error_function(error_function);

    jack_buf_size = jack_get_buffer_size(client);

    qDebug(QString("JACK buffer size: %1.").arg(jack_buf_size));
    config->buffer_n_samples(jack_buf_size * 2);

    int jack_samplerate = jack_get_sample_rate(client);
    qDebug(QString("JACK sample rate: %1.").arg(jack_samplerate));
    config->samplerate(jack_samplerate);

    audio_buffer = new sample_t[config->buffer_n_samples()];
    buffer_right = new sample_t[jack_buf_size];
    buffer_left  = new sample_t[jack_buf_size];

    if(jack_set_process_callback (client, jackProcessChannelCallback, this) == 0) {
	qDebug("Assigning JACK callback function");
    } else {
	JackConnectionException e("Assigning JACK callback function failed");
	throw (e);
    }


}

void ChannelJackDriver::addChannel(Channel* channel) {
    ChannelJackPort* chan_port = new ChannelJackPort(channel, client);
    if (Config::instance()->jackAutoconnect())
        chan_port->connect();
    ports.append(chan_port);
}

void ChannelJackDriver::deleteChannel(Channel* channel) {
    for (Q3PtrList<ChannelJackPort>::iterator it(ports.begin()); 
            it != ports.end();) {
        if ((*it)->channel == channel) {
            ports.remove(*(it++)); // XXX
        } else {
            ++it;
        }
    }
}

void ChannelJackDriver::clear() {
    ports.clear();
}

void ChannelJackDriver::activate() {
    // Activation de slag en tant que client Jack
    if( jack_activate (client) == 0 ) {
        qDebug("Slag activated as a JACK client");
    } else {
        JackConnectionException e("Unable to activate Slag as a JACK client");
        throw (e);
    }
}

void ChannelJackDriver::connectDriver() {
    // Connection des ports
    Q3PtrList<ChannelJackPort>::iterator ports_end = ports.end();
    if (Config::instance()->jackAutoconnect()) {
        for (Q3PtrList<ChannelJackPort>::iterator it(ports.begin()); 
                it != ports.end(); ++it) {
            (*it)->connect();
        }
    }
}

void ChannelJackDriver::disconnect() {
    jack_deactivate(client);
}

ChannelJackDriver::~ChannelJackDriver() {
    ports.clear();
    jack_client_close(client);
    delete[] audio_buffer;
    delete[] buffer_right;
    delete[] buffer_left;
}

void ChannelJackDriver::updateChannels(Song* song) {
    clear();
    activate();
    if (config->jackTrackOutput()) {
        connect(song, SIGNAL(channelAdded(Channel*)), 
                this, SLOT(addChannel(Channel*)));
        connect(song, SIGNAL(channelRemoved(Channel*)), 
                this, SLOT(deleteChannel(Channel*)));

        foreach (Channel* channel_ptr, song->channels()) {
            addChannel(channel_ptr);
        }
    }
}

//EOF
