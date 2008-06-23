/*
 *   $Id: Slag.h,v 1.28 2007/01/27 08:03:27 rhizome Exp $
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

#ifndef SLAG_H
#define SLAG_H

#include <qapplication.h>
#include <QEvent>
#include "gui/StringListEdit.h"

class Q3ProgressDialog;
class Song;
class SongFile;
class MainWidget;
class AudioDriver;
class Config;
class SongReader;
class Channel;
class Part;
class AudioDriverFactory;


class PatternChangeEvent : public QEvent {
    QString _name;
public:
    static const QEvent::Type Type = (QEvent::Type) 1001;
    PatternChangeEvent(QString name) : QEvent(Type), _name(name) {}
    QString name() { return _name; }
};

class SongUpdateEvent : public QEvent {
public:
    static const QEvent::Type Type = (QEvent::Type) 1002;
    SongUpdateEvent() : QEvent(Type) {}
};

class Slag : public QApplication {
    Q_OBJECT
    AudioDriver* driver;
    Config* config;
    MainWidget* main_widget;
    SongReader* songReader;
    SongFile* song_file;
    AudioDriverFactory* audioDriverFactory;


    void die_usage(char**);
    void initGui(Q3ProgressDialog*);
    void addChannelsToJackMulti();
    void installTranslators();
    void showJackErrorPopup();
    void showLibaoErrorPopup();
    void showRtAudioErrorPopup();
    void initJackDriver();
    void initJackTrackDriver();
    void initLibaoDriver();
    void initNullDriver();
    void setSamplesPerPad(int);
public:
    Slag(int, char**);
    int start();
    void openSong(const QString&);
    void initAudioDriver();
    void restartAudioDriver();
    bool patternMode();
    void updatePatternList(const StringItemList&);
    Song* song();
public slots:
    void play();
    void save();
    void patternChange(const QString&);
    void setTempo(int);
    void setVolume(int);
    void setSongMode();
    void setPatternMode();
    void setLoopMode(int v);
    void deleteChannel(Channel*);
    void exportWav(const QString& filename, int format);
    void stop();
    SongFile* songFile();
private slots:
    void autoPatternChange(const QString&);
    void oneStepForward();
};

#endif

//EOF
