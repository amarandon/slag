/*
 *   $Id: ChannelMenu.cpp,v 1.19 2007/01/26 07:39:55 rhizome Exp $
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

#include <qinputdialog.h>
#include <qlineedit.h>
#include <q3filedialog.h>
#include <qmessagebox.h>
#include <qsettings.h>
//Added by qt3to4:
#include <Q3PopupMenu>

#include "../audio_IO/AudioFile.h"
#include "../audio_IO/AudioFileExceptions.h"
#include "../audio_engine/Channel.h"
#include "../audio_engine/Song.h"
#include "../config.h"

#include "ChannelMenu.h"

const QString default_sounds_dir = CONFIG_PREFIX + QString("/share/slag/sounds");

ChannelMenu::ChannelMenu(QWidget* parent, Song* song, Channel* channel)
    : Q3PopupMenu(parent), song(song), channel(channel)
{
    insertItem( tr("Load file"), this, SLOT(loadFile()));
    insertItem( tr("Move up"), this, SLOT(moveUpChannel()));
    insertItem( tr("Move down"), this, SLOT(moveDownChannel()));
    insertItem( tr("Delete track"), this, SLOT(removeChannel()));
    insertItem( tr("Rename track"), this, SLOT(renameChannel()));
}

void ChannelMenu::removeChannel() {
    emit channelRemoved(channel);
}

void ChannelMenu::moveUpChannel() {
    if (channel->rowIndex() <= 0) return;

    // Index of the row to be replaced is incremented
    // so that indexes are permutated
    Channel* prev_channel = song->channelByIndex(channel->rowIndex() - 1);
    channel->decreaseRowIndex();
    prev_channel->increaseRowIndex();
    emit channelMoved(channel);
    emit channelMoved(prev_channel);
}

void ChannelMenu::moveDownChannel() {
    if (channel->rowIndex() >= song->channelsCount() - 1) return;
    
    // Index of the row to be replaced is decremented
    // so that indexes are permutated
    Channel* next_channel = song->channelByIndex(channel->rowIndex() + 1);
    channel->increaseRowIndex();
    next_channel->decreaseRowIndex();
    emit channelMoved(channel);
    emit channelMoved(next_channel);
}

void ChannelMenu::loadFile() {
    QSettings settings;
    QString sounds_dir = 
        settings.value("sounds_dir", default_sounds_dir).toString();
    QString filename = Q3FileDialog::getOpenFileName(
            sounds_dir,
            0,
             //tr("Wav files (*.wav *.WAV)"),
            this,
            "open file dialog",
             tr("Choose a file") );
    if (not filename.isEmpty()) {
        settings.setValue("sounds_dir", QFileInfo(filename).dirPath());
        try {
            QString candidate_name = removeWavSuffix(filename);
            bool name_already_present = false;
            while (song->hasChannel(candidate_name)) {
                candidate_name += "_";
                name_already_present = true;
            }
            channel->setFile(filename);
            if (name_already_present) {
                channel->setName(candidate_name);
            }
            emit channelRenamed(channel);
        } catch (audio_IO::OpenException& e) {
            QMessageBox::critical(
                0, tr("Slag error"),
                QString(tr("Unable to load file %1.\n%2").arg(
                                                e.filename, e.sys_msg)));
        }
    }
}

bool ChannelMenu::askForChannelName(QString& channel_name) {
    bool ok;
    channel_name = QInputDialog::getText(
            tr("Give this track a new name"),
            tr("Name of this track :"), 
            QLineEdit::Normal, 
            channel->name(),
            &ok, 
            this );

    if (ok) {
        if (channel_name.isEmpty()) {
            ok = askForChannelName(channel_name);
        } else if (channel->name() != channel_name 
                   && song->hasChannel(channel_name)) {
            QMessageBox::warning(
                0, 
                tr("Slag error"),
                tr("Name %1 already in use. Please choose another name.")
                .arg(channel_name));
            ok = askForChannelName(channel_name);
        }
    }
    return ok;
}

void ChannelMenu::renameChannel() {
	QString new_chan_name;

        if (askForChannelName(new_chan_name)) {
            song->freeChannelName(channel->name());
            channel->setName(new_chan_name);
            emit channelRenamed(channel);
        }
}

//EOF
