/*
 *   $Id: ChannelJackDriver.h,v 1.12 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef ChannelJackDriver_H
#define ChannelJackDriver_H

#include <q3ptrlist.h>

#include "../audio_engine/Channel.h"
#include "AudioDriver.h"
#include "JackDriver.h"

typedef void (* JackChannelProcessCallback)(Channel*, sample_t*);
class Song;

/** 
 * Les channels de slag sont stéréos, cette classe représente donc un couple de
 * ports JACK.
 */
class ChannelJackPort : public QObject {
    Q_OBJECT
private:
    jack_client_t* client;
    const char** jack_ports;
    bool connected;
public:
    Channel* channel;
    jack_port_t* jack_port_L;
    jack_port_t* jack_port_R;

    ChannelJackPort(Channel* channel, jack_client_t* client);
    ~ChannelJackPort();
    void connect();
public slots:
    void syncName();
};

class ChannelJackDriver : public QObject, public AudioDriver {
    Q_OBJECT
    Config* config;
    jack_client_t* client;

public:
    int jack_buf_size;   
    sample_t * audio_buffer;
    sample_t * buffer_right;
    sample_t * buffer_left;
    Q3PtrList<ChannelJackPort> ports;
    JackChannelProcessCallback audioProcessChannelCallback;

    ChannelJackDriver(JackChannelProcessCallback, JackErrorCallback)
        throw (audio_IO::JackConnectionException);
    virtual ~ChannelJackDriver();
    void updateChannels(Song*);
    void clear();
    void activate();

public slots:
    void addChannel(Channel*);
    void deleteChannel(Channel*);
    void connectDriver();
    void disconnect();
};

#endif

//EOF 

