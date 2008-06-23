/*
 *   $Id: ToolBar.cpp,v 1.23 2007/01/28 08:12:20 rhizome Exp $
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

#include <QMainWindow>
#include <QAction>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QSpinBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QCheckBox>

#include "../audio_engine/Song.h"
#include "../Slag.h"
#include "ToolBar.h"
#include "SpinSlider.h"

ToolBar::ToolBar(Slag* slag) : 
    slag(slag), 
    pattern_box(new QComboBox) 
{
    setLabel(tr("Playback controls"));

    // Play button
    QAction* play_action = new QAction(this);
    play_action->setAccel( Qt::Key_Space );
    play_action->setIconSet( QPixmap(":/icons/play.png") );
    addAction(play_action);
    connect(play_action, SIGNAL(activated()), slag,  SLOT(play()));

    // Stop button
    QAction* stop_action = new QAction(this);
    stop_action->setAccel( Qt::Key_Space );
    stop_action->setIconSet( QPixmap(":/icons/stop.png") );
    addAction(stop_action);
    connect(stop_action, SIGNAL(activated()), slag, SLOT(stop()));

    // Pattern choice box
    addSeparator();

    pattern_box->setMinimumWidth(100);
    addWidget(pattern_box);
    connect(pattern_box, SIGNAL(activated(const QString &)), 
            slag,        SLOT(patternChange(const QString &)));

    // Play mode: pattern or song
    QButtonGroup* mode_group = new QButtonGroup;
    mode_group->setExclusive(true);
    //mode_group->setLineWidth(0);
    //mode_group->setInsideMargin(2);

    pattern_mode_radio = new QRadioButton;
    pattern_mode_radio->setText(tr("Pattern"));
    mode_group->addButton(pattern_mode_radio);
    connect(pattern_mode_radio, SIGNAL(clicked()), 
            slag,               SLOT(setPatternMode()));
    addWidget(pattern_mode_radio);

    song_mode_radio = new QRadioButton;
    song_mode_radio->setText(tr("Song"));
    mode_group->addButton(song_mode_radio);
    connect(song_mode_radio, SIGNAL(clicked()), 
            slag,            SLOT(setSongMode()));
    addWidget(song_mode_radio);

    QCheckBox* loopCheckBox = new QCheckBox(tr("Loop"));
    loopCheckBox->setCheckState(Qt::Checked);
    connect(loopCheckBox, SIGNAL(stateChanged(int)), 
            slag,        SLOT(setLoopMode(int)));
    addWidget(loopCheckBox);

    // Tempo box
    addSeparator();
    
    tempoControl = new SpinSlider( 
            tr( "<font size=\"-1\">BPM:</font>" ) 
            );
    
    tempoControl->setMinimum(20);
    tempoControl->setMaximum(300);
    tempoControl->setValue(slag->song()->tempo());
    connect(tempoControl, SIGNAL(valueChanged(int)), slag, SLOT(setTempo(int)));
    addWidget(tempoControl);

    // Volume box
    volumeControl = new SpinSlider(tr( "<font size=\"-1\">Vol:</font>" ) );
    volumeControl->setValue(slag->song()->volumePercent());
    connect(volumeControl, SIGNAL(valueChanged(int)), 
            slag,    SLOT(setVolume(int)));
    addWidget(volumeControl);
}

void ToolBar::sync() {
    pattern_box->clear();

    QStringList patternNames = slag->song()->patternNames();
    QStringList::Iterator end = patternNames.end();

    for (QStringList::Iterator it = patternNames.begin(); it != end; ++it) {
        pattern_box->insertItem(*it);
    }
    volumeControl->setValue(slag->song()->volumePercent());
    tempoControl->setValue(slag->song()->tempo());
    if(slag->patternMode()) {
       pattern_mode_radio->setChecked(true); 
    } else {
       song_mode_radio->setChecked(true); 
    }
}

//EOF
