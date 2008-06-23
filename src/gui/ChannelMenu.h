/*
 *   $Id: ChannelMenu.h,v 1.10 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef CHANNEL_MENU_H
#define CHANNEL_MENU_H

#include <qstring.h>
#include <q3popupmenu.h>

class Channel;
class Song;

class ChannelMenu : public Q3PopupMenu {
    Q_OBJECT
public:
    ChannelMenu(QWidget*, Song*, Channel*);
signals:
    void channelMoved(Channel*);
    void channelRenamed(Channel*);
    void channelRemoved(Channel*);
private slots:
    void moveUpChannel();
    void moveDownChannel();
    void renameChannel();
    void removeChannel();
    void loadFile();
private:
    Song* song;
    Channel* channel;
    bool askForChannelName(QString&);
};

#endif

//EOF
