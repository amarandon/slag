/*
 *   $Id: SndFileFormatDialog.cpp,v 1.2 2006/12/27 06:35:10 rhizome Exp $
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

#include "SndFileFormatDialog.h"
#include <qradiobutton.h>
#include <qlayout.h>
#include <Q3VButtonGroup>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

SndFileFormatDialog::SndFileFormatDialog(QWidget* parent) 
    : QDialog(parent, tr("Choose sound file format."))
{
    Q3VBoxLayout* layout = new Q3VBoxLayout(this, 11, 6);
    buttonGroup = new Q3VButtonGroup(tr("Supported formats"), this);
    buttonGroup->setRadioButtonExclusive(true); 
    layout->addWidget(buttonGroup);

    int count;
    sf_command(NULL, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof (int));
    SF_FORMAT_INFO format_info;
    for (int i = 0 ; i < count ; ++i) {
        format_info.format = i ;
        sf_command (NULL, SFC_GET_SIMPLE_FORMAT, 
                    &format_info, sizeof (format_info)) ;
        // Skip this format since it doesn't support stereo
        if (QString(format_info.name).compare("OKI Dialogic VOX ADPCM")
                == 0 ) {
            continue;
        }
        QRadioButton* radio = new QRadioButton(format_info.name, buttonGroup);
        if (QString(format_info.name).compare("WAV (Microsoft 16 bit PCM)")
                == 0 ) {
            radio->toggle();
        }
        extensionById[buttonGroup->id(radio)] = format_info;
    }

    QPushButton* okButton = new QPushButton(tr("Ok"), this);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    Q3HBoxLayout* buttons_layout = new Q3HBoxLayout(layout, 5);
    buttons_layout->insertStretch(0);
    buttons_layout->addWidget(okButton);
    buttons_layout->addWidget(cancelButton);
    buttons_layout->insertStretch(3);
}

QString SndFileFormatDialog::formatExtension() {
    return extensionById[buttonGroup->selectedId()].extension;
}

QString SndFileFormatDialog::formatName() {
    return extensionById[buttonGroup->selectedId()].name;
}

int SndFileFormatDialog::formatId() {
    return extensionById[buttonGroup->selectedId()].format;
}

//EOF
