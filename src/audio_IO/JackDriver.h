/*
 *   $Id: JackDriver.h,v 1.10 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef JACK_DRIVER_H
#define JACK_DRIVER_H

#include <string>

#include <jack/jack.h>

#include "../config/Config.h"
#include "AudioDriver.h"
#include "Exceptions.h"

class RingBufferDriver;

class JackDriver : public AudioDriver
{
    const char **jack_ports;

    // <http://jackit.sourceforge.net/docs/reference/types_8h.html>
    jack_client_t *client;
    Config * config;
public:
    JackDriver(ProcessCallback, JackErrorCallback) 
	throw (audio_IO::JackConnectionException);
    virtual ~JackDriver();
    void connectDriver();
    void disconnect();

    /// Les données membres suivantes ne font pas partie de l'interface
    /// publique du module mais doivent être publiques pour être accédés
    /// par la fonction callback de type C
    jack_port_t *output_port_1;
    jack_port_t *output_port_2;
    // variables de configuration du serveur jack
    int jack_buf_size;   

    sample_t * audio_buffer;
    sample_t * buffer_right;
    sample_t * buffer_left;

    RingBufferDriver* ringBufferDriver;
};


#endif

//EOF
