/*
  $Id: preferences_dialog.ui.h,v 1.9 2007/01/26 07:39:55 rhizome Exp $

      Copyright (C) 2004-2008 Alex Marandon

  This file is part of slag, a pattern-based audio sequencer.

  slag is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  slag is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with slag; see the file COPYING.  If not, write to
  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef PREFERENCES_DIALOG_UI_H
#define PREFERENCES_DIALOG_UI_H


#include "../ui_preferences_dialog.h"
#include "../../constants.h"

class PreferencesDialog : public QDialog, public Ui::PreferencesDialog {
    Q_OBJECT

public:
    PreferencesDialog(QDialog *parent = 0)
        : QDialog(parent)
    { setupUi(this); }


    bool getOpenLastSong()
    {
        return openLastSongBox->isChecked();
    }

    void setOpenLastSong(bool v)
    {
        openLastSongBox->setChecked(v);
    }

    int getAudioBufferSize()
    {
        return bufferSizeBox->currentText().toInt();
    }

    void setAudioBufferSize(int size) {
        bufferSizeBox->setCurrentText(QString("%1").arg(size));
    }

    QString getAudioDriver()
    {
        return audioDriverBox->currentText();
    }

    void setAudioDriver( QString driver )
    {
        audioDriverBox->setCurrentText(driver);
    }

    void setJackAutoConnect( bool p )
    {
        jackAutoConnectBox->setChecked(p);
    }

    bool getJackAutoConnect()
    {
        return jackAutoConnectBox->isChecked();
    }

    void setJackMulti( bool p )
    {
        jackTrackOutputsBox->setChecked(p);
    }

    bool getJackMulti()
    {
        return jackTrackOutputsBox->isChecked();
    }


    void init()
    {
#ifndef WITH_JACK
        deleteDriver( JACK_LABEL);
#endif
#ifndef WITH_LIBAO
        deleteDriver(LIBAO_LABEL);
#endif
    }

    void deleteDriver( const QString & label )
    {
        for(int i = 0; i < audioDriverBox->count(); ++i) {
            if (audioDriverBox->text(i) == label) {
                audioDriverBox->removeItem(i);
            }
        } 
    }

public slots:
    void audioDriverSelected( const QString & driver_name )
    {
        if (driver_name == JACK_LABEL) {
            jackGroupBox->setEnabled(true);
            bufferSizeLabel->setEnabled(false);
            bufferSizeBox->setEnabled(false);
        } else {
            jackGroupBox->setEnabled(false);
            bufferSizeLabel->setEnabled(true);
            bufferSizeBox->setEnabled(true);
        }

        audioDriverBox->setCurrentText(driver_name);
    }
};

#endif
//EOF
