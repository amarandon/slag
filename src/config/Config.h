/*
 *   $Id: Config.h,v 1.16 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef CONFIG_H
#define CONFIG_H

#include <unistd.h>
#include <string>
#include <QString>

class BadOption {
    std::string _msg;
public:
    inline void msg(std::string s) { _msg = s; }
    inline std::string msg() { return _msg; }
};

class Config {
     QString audio_driver_name;
     bool jack_track_output;
     bool jack_autoconnect;
     bool _no_gui;
     bool _loop;
     int  _buffer_n_samples;
     int  _samplerate;
     unsigned int samples_per_pad;
     bool _debug;
     const char* _songfile;
     static Config * _instance;
     Config();
     Config(int argc, char **argv) throw (BadOption);
     void readSettings();
     void readOptions(int argc, char **argv) throw (BadOption);
     void findDebugLevel() throw (BadOption);
     void findSongFile(int argc, char **argv);
public:
     inline bool debug() { return _debug; }

     inline QString audioDriverName() const { return audio_driver_name; }
     void setAudioDriverName(QString d);

     inline bool jackTrackOutput() const { return jack_track_output; }
     inline void setJackTrackOutput(bool v) { jack_track_output = v; }

     inline bool jackAutoconnect() const { return jack_autoconnect; }
     inline void setJackAutoconnect(bool b) { jack_autoconnect = b; }

     inline bool no_gui() { return _no_gui; }
     inline const char* songfile() { return _songfile; }
     inline int buffer_n_samples() const { return _buffer_n_samples; }
     inline void buffer_n_samples(int i) { _buffer_n_samples = i; }
     inline bool loop() const { return _loop; }
     inline void loop(bool v) { _loop = v; }
     inline int samplerate() { return _samplerate; }
     inline void samplerate(int v) { _samplerate = v; }
     inline unsigned int samplesPerPad() { return samples_per_pad; }
     inline void samplesPerPad(unsigned int v) { samples_per_pad = v; }
     static Config * instance();
     static Config * instance(int argc, char **argv);
     void save();
};

#endif

//EOF
