/*
 *   $Id: LibaoWrapper.cpp,v 1.11 2007/01/26 07:39:55 rhizome Exp $
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
//
// C++ wrapper for libao <http://www.xiph.org/ao/>
//

#include <QObject>
#include <samplerate.h>
#include <math.h>
#include "LibaoWrapper.h"
#include "Exceptions.h"

LibaoWrapper::LibaoWrapper(const int bits, const int channels, 
	const int rate, const int byte_format) 
    : options(NULL)
{
    format.bits = bits;
    format.channels = channels;
    format.rate = rate;
    format.byte_format = byte_format;

    ao_initialize();
    int driver_id = ao_default_driver_id();

    if(driver_id < 0) {
	audio_IO::LibaoConnectionException e(
                "Libao error: failure to find a usable audio output device.");
	throw(e);
    }

    ao_append_option(&options, "buffer_time", "70000");
    device = ao_open_live(driver_id, &format, options);

    ao_info* driver_info = ao_driver_info(driver_id);

    std::string plugin_name = "Libao output plugin: ";
    if(driver_info != NULL) {
		plugin_name += driver_info->name;
		plugin_name += " (short name: ";
		plugin_name += driver_info->short_name;
		plugin_name += ")";
    }
    qDebug(plugin_name.c_str());

    if (device == NULL) {
        std::string err_msg = "Libao error: ";
        switch(errno) {
            case AO_ENODRIVER:
                err_msg += "No driver corresponds to driver_id";
                break;
            case AO_ENOTLIVE:
                err_msg += "This driver is not a live output device";
                break;
            case AO_EBADOPTION:
                err_msg += "A valid option key has an invalid value";
                break;
            case AO_EOPENDEVICE:
                err_msg += "Cannot open the device (for example, if /dev/dsp "
                           "cannot be opened for writing)";
                break;
            case AO_EFAIL:
                err_msg += "Any other cause of failure";
                break;
            default:
                err_msg += "Reason unknown";
                break;
        }
        err_msg += ".";
		audio_IO::LibaoConnectionException e(err_msg);
		throw(e);
    } else {
		qDebug("Libao audio output is open.");
    }
}

LibaoWrapper::~LibaoWrapper() {
    ao_close(device);
    ao_shutdown();
}

/// 
/// Fonction reprise et vaguement adpatee a partir de celle de libsamplerate
/// pour fonctionner avec des doubles.
/// Semble etre "fonctionnel" mais pas vraiment performant. En effet, les
/// doubles sont normalement plus performants que les floats car le C convertit
/// tout float en double en interne. Pourtant avec des doubles, ça consomme
/// légèrement plus de CPU.
const int CPU_CLIPS_NEGATIVE = 1;
const int CPU_CLIPS_POSITIVE = 0;
void src_double_to_short_array (const double *in, short *out, int len) {
    
    double scaled_value ;

    while (len) {
	len -- ;

	scaled_value = in [len] * (8.0 * 0x10000000) ;
	if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
	{	out [len] = 32767 ;
	    continue ;
	} ;
	if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
	{	out [len] = -32768 ;
	    continue ;
	} ;

	out [len] = ( (int) rintf (scaled_value) >> 16) ;
    };

} /* src_double_to_short_array */

void LibaoWrapper::play (sample_t* buffer, const int nb_frames)
{
    int buf_array_len = format.channels * nb_frames;

    short * short_buffer = new short[buf_array_len];

    // conversion sample_t => short
    src_sample_to_short_array(buffer, short_buffer, buf_array_len);

    ao_play(device, (char*)short_buffer, buf_array_len * sizeof(short));

    delete[] short_buffer;
}

//EOF
