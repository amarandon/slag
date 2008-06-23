/*
 *   $Id: PatternRow.cpp,v 1.18 2007/01/28 08:12:20 rhizome Exp $
 * 
 *      Copyright (C) 2004-2008 Alex Marandon
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

#include <QButtonGroup> 
#include <QLabel> 
#include <QToolTip> 
#include <QMotifStyle>
#include <QHBoxLayout>
#include <QTabWidget>

#include "../Slag.h"
#include "../constants.h"
#include "../audio_engine/Pattern.h"
#include "../audio_engine/Pad.h"
#include "../audio_engine/Part.h"
#include "../audio_engine/Pattern.h"
#include "../audio_engine/Channel.h"
#include "../audio_engine/Song.h"
#include "PartMenu.h"
#include "PatternRow.h"
#include "RowContext.h"
#include "ChannelMenu.h"
#include "NamedButton.h"
#include "PadButton.h"
#include "Fader.h"
#include "PanFader.h"

PatternRow::PatternRow(QWidget* parent, Song* song, RowContext* row_context)
    : QWidget(parent),
      row_context(row_context),
      channel(row_context->channel), 
      pattern(row_context->pattern), 
      current_part_label(new QLabel(this)),
      new_part_button(new QPushButton(tr("N"), this)),
      part_menu(new PartMenu(this, song, row_context)),
      part_but(new QPushButton(tr("E"), this)),
      song(song)
{
    QToolTip::add(new_part_button, tr("Create a new part"));
    QToolTip::add(part_but, tr("Edit this part"));
    const int smallButtonWidth = 20;
    new_part_button->setFixedSize(smallButtonWidth, padButtonHeight);

#if defined(Q_WS_MAC)
    new_part_button->setStyle(new QMotifStyle);
    part_but->setStyle(new QMotifStyle);
#endif
    
    if (channel->audioFile() == NULL) {
        new_part_button->setEnabled(false);
        part_but->setEnabled(false);
    }
    part_but->setFixedSize(smallButtonWidth, padButtonHeight);

    connect(new_part_button, SIGNAL(clicked()),
            part_menu,       SLOT(createBlank()));

    connect(part_menu, SIGNAL(partChanged(Part*)), 
            this,      SLOT(handlePartChanged(Part*)));

    connect(part_but, SIGNAL(clicked()), 
            this,     SLOT(popup_part_menu()));

    connect(channel, SIGNAL(fileChanged()),
            this,    SLOT(handleFileChanged()));

    Part* part;
    
    if ((part = pattern->part(channel)) != NULL) {
        connect(part, SIGNAL(renamed(Part*)), 
                this, SLOT(handlePartRenamed(Part*)));
        connect(part, SIGNAL(deleted(Part*)), 
                this, SLOT(deactivate(Part*)));
        current_part_label->setText(pattern->part(channel)->name());
    } else {
        current_part_label->setText(QString("no part"));
    }

    vLayout = new QVBoxLayout;
    tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::South);
    QWidget* volumeWidget = new QWidget(this);
    QBoxLayout* volumeLayout = new QHBoxLayout;
    volumeLayout->setMargin(0);
    volumeLayout->setSpacing(0);
    volumeWidget->setLayout(volumeLayout);

    QWidget* panWidget = new QWidget(this);
    QBoxLayout* panLayout = new QHBoxLayout;
    panLayout->setMargin(0);
    panLayout->setSpacing(0);
    panWidget->setLayout(panLayout);

    tabWidget->addTab(volumeWidget, tr("Volume"));
    tabWidget->addTab(panWidget, tr("Pan"));
    tabWidget->setCurrentIndex(pattern->tabIndex(channel));
    tabWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    tabWidget->setContentsMargins(0,0,0,0);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    QBoxLayout* notesLayout = new QHBoxLayout;
    notesLayout->setSpacing(0);
    vLayout->addLayout(notesLayout, Qt::AlignTop);
    vLayout->addWidget(tabWidget, Qt::AlignTop);
    vLayout->setMargin(0);
    notesLayout->addSpacing(1);
    for (uint pad_i = 1; pad_i < song->nbPads() + 1; ++pad_i) {

        QPushButton * button = new PadButton(this);
        buttons.append(button);
        button->setFixedSize(padButtonWidth, padButtonHeight);
        button->setEnabled(0);
        notesLayout->addSpacing(buttonGap);
        notesLayout->addWidget(button);

        Fader* volumeFader = new Fader(this);
        volumeFaders.append(volumeFader);
        volumeFader->setFixedSize(padButtonWidth, padButtonHeight * 2);
        volumeLayout->addWidget(volumeFader);
        volumeLayout->addSpacing(buttonGap);

        Fader* panFader = new PanFader(this);
        panFaders.append(panFader);
        panFader->setFixedSize(padButtonWidth, padButtonHeight * 2);
        panLayout->addWidget(panFader);
        panLayout->addSpacing(buttonGap);
        if ((pad_i % 4) == 0) {
            notesLayout->addSpacing(stepGap);
            volumeLayout->addSpacing(stepGap);
            panLayout->addSpacing(stepGap);
        }
    }

    notesLayout->addWidget(new_part_button);
    notesLayout->addWidget(part_but);
    notesLayout->addWidget(current_part_label);
    notesLayout->addStretch();
    notesLayout->setMargin(0);
    notesLayout->setSizeConstraint(QLayout::SetFixedSize);
    volumeLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(vLayout);
    showDetails(channel->detailsShown());
}

PatternRow::~PatternRow() { 
    qDebug("Destruction PatternRow");
}

void PatternRow::showDetails(bool detailsShown) {
    if(detailsShown) {
        vLayout->addWidget(tabWidget, Qt::AlignTop);
        tabWidget->show();
    } else {
        tabWidget->hide();
        vLayout->removeWidget(tabWidget);
    }
}

void PatternRow::popup_part_menu() {
    part_menu->exec(part_but->mapToGlobal(QPoint(3,3)));
}

void PatternRow::clearConnections() {
    Part* part = pattern->part(channel);
    disconnect(part, SIGNAL(renamed(Part*)), this, SLOT(handlePartRenamed(Part*)));
    disconnect(part, SIGNAL(deleted(Part*)), this, SLOT(deactivate(Part*)));
    if (part == NULL) return;
    PadIterator pad_it(part->pads().begin());
    FaderIterator volumeFaderIter = volumeFaders.begin();
    FaderIterator panFaderIter = panFaders.begin();
    foreach (QPushButton* button, buttons) {
        button->disconnect(SIGNAL(toggled(bool)));
        button->disconnect(SIGNAL(volumeChanged(int)));
        disconnect(*pad_it, SIGNAL(activeStateChanged(bool)), 
                 button, SLOT(setOn(bool)));
        disconnect(*pad_it,  SIGNAL(volumeChanged(int)), 
                 *volumeFaderIter, SLOT(setVolume(int)));
        disconnect(*pad_it,  SIGNAL(balanceChanged(int)), 
                 *panFaderIter, SLOT(setVolume(int)));
        ++pad_it;
        ++volumeFaderIter;
        ++panFaderIter;
    }
}

void PatternRow::handlePartChanged(Part* new_part) {
    Part* old_part = pattern->part(channel);
    if (old_part != NULL) clearConnections();
    pattern->setPart(channel, new_part);
    if (new_part != NULL) activate();
}

void PatternRow::handleFileChanged() {
    part_but->setEnabled(true);
    new_part_button->setEnabled(true);
}

void PatternRow::handlePartRenamed(Part* part) {
    current_part_label->setText(part->name());
}

void PatternRow::activate() {
    Part* part = pattern->part(channel);
    connect(part, SIGNAL(renamed(Part*)), this, SLOT(handlePartRenamed(Part*)));
    connect(part, SIGNAL(deleted(Part*)), this, SLOT(deactivate(Part*)));

    PadIterator pad_it(pattern->part(channel)->pads().begin());
    FaderIterator volumeFaderIter = volumeFaders.begin();
    FaderIterator panFaderIter = panFaders.begin();
    foreach (QPushButton* button, buttons) {
        // on active également les pads
        button->setToggleButton(true);
        button->setEnabled(true);
        if ( (*pad_it)->isActive())
            button->setOn(1); 
        else 
            button->setOn(0); 

        ((Fader*) (*volumeFaderIter))->setVolume((*pad_it)->volumePercent());
        ((Fader*) (*panFaderIter))->setVolume((*pad_it)->balance());

        /// Le click sur le bouton modifie l'état du pad
        connect(button, SIGNAL(toggled(bool)), 
                *pad_it,  SLOT(setActive(bool)));
        connect(*volumeFaderIter,  SIGNAL(volumeChanged(int)), 
                *pad_it, SLOT(setVolume(int)));
        connect(*panFaderIter,  SIGNAL(volumeChanged(int)), 
                *pad_it, SLOT(setBalance(int)));

        /// Le changement d'état du Pad est notifié à tous les
        /// boutons associés
        connect(*pad_it,  SIGNAL(activeStateChanged(bool)), 
                button, SLOT(setOn(bool)));
        connect(*pad_it,  SIGNAL(volumeChanged(int)), 
                *volumeFaderIter, SLOT(setVolume(int)));
        connect(*pad_it,  SIGNAL(balanceChanged(int)), 
                *panFaderIter, SLOT(setVolume(int)));

        ++pad_it;
        ++volumeFaderIter;
        ++panFaderIter;
    }

    current_part_label->setText(pattern->part(channel)->name());
}

void PatternRow::deactivate(Part*) {
    foreach (QPushButton* button, buttons) {
        button->setEnabled(false);
        button->setOn(false); 
    }

    current_part_label->setText(tr("None"));
}

QString PatternRow::patternName() { 
    return pattern->name(); 
}

void PatternRow::tabChanged(int newIndex) {
    pattern->setTabIndex(channel, newIndex);
}

//EOF
