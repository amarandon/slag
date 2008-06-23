#   $Id: test_gui_1.pro,v 1.5 2005/03/05 18:58:34 rhizome Exp $
#
#       Copyright (C) 2004, 2005 Alex Marandon
# 
#   This file is part of slag, a pattern-based audio sequencer.
# 
#   slag is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
# 
#   slag is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with slag; see the file COPYING.  If not, write to
#   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#
########################################################################
# 
# 
# Configurer la variable AUDIO_OUTPUT pour compiler avec JACK et/ou libao
#
# Exemples :
#
#   Compilation avec JACK seul
#   AUDIO_OUTPUT = jack
#
#   Compilation avec libao seul
#   AUDIO_OUTPUT = libao
#
#   Compilation avec les deux
#   AUDIO_OUTPUT = jack libao
#
#   Si on n'en configure aucun, seul le NullDriver (qui ne fait rien) sera
#   compilé. Cela peut permettre de voir la GUI si on a ni libao ni JACK.
#
#   Compilation uniquement avec le NullDriver
#   AUDIO_OUTPUT = 

AUDIO_OUTPUT = libao jack

TEMPLATE = app
INCLUDEPATH += .
CONFIG += thread debug
LIBS += -lsndfile -lsamplerate

# Input
HEADERS += ../../src/audio_engine/Song.h ../../src/audio_engine/Pad.h \
	   ../../src/audio_engine/Timer.h  ../../src/gui/MainWidget.h \
           ../../src/gui/PatternWidget.h ../../src/Slag.h
SOURCES += main.cpp \
           ../../src/logger/Logger.cpp \
           ../../src/config/Config.cpp \
           ../../src/audio_engine/Song.cpp \
           ../../src/audio_engine/Pad.cpp \
           ../../src/audio_engine/Serialize.cpp \
           ../../src/audio_engine/List.cpp \
           ../../src/audio_engine/ListInstances.cpp \
           ../../src/audio_IO/WavFile.cpp \
           ../../src/audio_IO/NullDriver.cpp \
	   ../../src/gui/MainWidget.cpp \
	   ../../src/gui/ToolBar.cpp \
	   ../../src/gui/PatternWidget.cpp \
	   ../../src/Slag.cpp 

contains( AUDIO_OUTPUT, jack ) {
    LIBS += `pkg-config --cflags --libs jack`
    DEFINES += WITH_JACK
    SOURCES += ../../src/audio_IO/JackDriver.cpp
}

contains( AUDIO_OUTPUT, libao ) {
    LIBS += -lao
    DEFINES += WITH_LIBAO
    SOURCES += ../../src/audio_IO/LibaoDriver.cpp \
               ../../src/audio_IO/LibaoWrapper.cpp
} 

#EOF
