/*
 *   $Id: ChannelRow.h,v 1.4 2006/12/27 06:35:10 rhizome Exp $
 * 
 *      Copyright (C) 2004, 2005 Alex Marandon
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

#ifndef CHANNEL_ROW_H
#define CHANNEL_ROW_H

#include <QWidget>

class QCheckBox;

class Song;
class Channel;
class NamedButton;
class ChannelMenu;
class QPushButton;

class ChannelRow : public QWidget {
    Q_OBJECT
public:
    ChannelRow(QWidget*, Song*, Channel*);
    QString name() { return channelName; }
    void setName (QString);
    void activate();
    uint rowIndex();
    void setRowIndex(uint);
    inline ChannelMenu* channelMenu() { return channel_menu; }
public slots:
    void syncChannelNumber();
private slots:
    void popupChannelMenu();
    void changeDetailsVisibility();
signals:
    void showDetails(bool);
private:
    Channel* channel;
    QString channelName;
    QCheckBox* active_box;
    ChannelMenu* channel_menu;
    NamedButton* channel_but;
    Song* song;
    const QString myNumber();
    QList<QWidget*> detailsWidgets;
    bool detailsShown;
    QPushButton* showHideButton;
    void syncDetailsVisibility();
};

#endif

//EOF
