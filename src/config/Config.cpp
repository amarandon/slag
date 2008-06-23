/*   
 *   $Id: Config.cpp,v 1.23 2007/01/26 07:39:55 rhizome Exp $
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
#include <qsettings.h>

#include "../common.h"
#include "../constants.h"
#include "../audio_engine/Serialize.h"
#include "Config.h"

Config* Config::_instance = NULL;
const int default_buffer_n_samples = 1024;

Config* Config::instance() {
    if (! _instance)
        _instance = new Config;
    return _instance;

}

Config* Config::instance(int argc, char **argv) {
    if ( _instance == NULL )
        _instance = new Config(argc, argv);
    return _instance;
}

Config::Config() 
    : _loop(1), 
      _samplerate(44100),
      samples_per_pad(0)
{ }

Config::Config(int argc, char **argv) throw (BadOption) 
    : audio_driver_name("libao"),
      jack_track_output(false),
      _no_gui(false),
      _loop(true),
      _buffer_n_samples(default_buffer_n_samples),
      _samplerate(44100),
      samples_per_pad(0),
      _debug(false)
{
    BadOption bad_option;

    readSettings();          // Reading config file
    readOptions(argc, argv); // Config file options are overwritten by 
                             // command line options
    findSongFile(argc, argv);

    _instance = this;
}

void Config::readSettings() {
    QSettings settings;
    _buffer_n_samples = settings.value("audio/bufferSize", default_buffer_n_samples).toInt();
    audio_driver_name = settings.value("audio/driver").toString();
    jack_autoconnect = settings.value("jack/autoConnect").toBool();
    jack_track_output = settings.value("jack/multi").toBool();
}

void Config::setAudioDriverName(QString d) {
    audio_driver_name = d;
}

void Config::readOptions(int argc, char **argv) throw (BadOption) {
    BadOption bad_option;

    opterr = 0;
    int c;
    while ((c = getopt(argc, argv, "hajtnpv")) != -1)
        switch (c) {
            case 'h':
                throw(bad_option);
                break;
            case 'n':
                audio_driver_name = NULL_LABEL;
                break;
            case 'p':
                _no_gui = true;
                break;
            case 'j':
                audio_driver_name = JACK_LABEL;
                jack_track_output = false;
                break;
            case 't':
                audio_driver_name = JACK_LABEL;
                jack_track_output = true;
                break;
            case 'a':
                audio_driver_name = LIBAO_LABEL;
                break;
            case 'v':
                _debug = true;
                break;
            case '?':
                bad_option.msg(QString("Option `-%1' unknown.")
                                .arg((char)optopt).latin1());
                throw(bad_option);
            default:
                throw(bad_option);
        }
}

void Config::findSongFile(int argc, char **argv) {
    if ( argc - optind == 1 ) {
        _songfile = argv[optind];
    } else {
        _songfile = SongFile::defaultFileName.latin1();
    }
}

void Config::save() {
    QSettings settings;
    settings.setValue("audio/driver", audio_driver_name);
    settings.setValue("audio/bufferSize", _buffer_n_samples);
    settings.setValue("jack/multi", jack_track_output);
    settings.setValue("jack/autoConnect", jack_autoconnect);
}

//EOF
