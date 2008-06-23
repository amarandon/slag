/*
 *   $Id: ChannelRow.cpp,v 1.15 2007/01/28 08:13:58 rhizome Exp $
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

#include <QCheckBox> 
#include <QMotifStyle>
#include <QGridLayout>
#include <QLabel>

#include "../audio_engine/Channel.h"
#include "../audio_engine/Song.h"
#include "../constants.h"

#include "ChannelRow.h"
#include "ChannelMenu.h"
#include "NamedButton.h"
#include "SpinSlider.h"

ChannelRow::ChannelRow(QWidget* parent, Song* song, Channel* channel) 
    : QWidget(parent),
      channel(channel), 
      channelName(channel->name()),
      active_box(new QCheckBox(this)),
      channel_menu(new ChannelMenu(this, song, channel)),
      channel_but(new NamedButton(this, channelName)),
      song(song),
      detailsShown(channel->detailsShown())
{
    connect(channel_but,  SIGNAL(clicked(QString)),
            this,         SLOT(popupChannelMenu()));

    connect(channel_menu, SIGNAL(channelMoved(Channel*)), 
            parent,       SLOT(syncChannelPosition(Channel*)));

    connect(channel_menu, SIGNAL(channelRenamed(Channel*)), 
            parent,       SLOT(renameChannel(Channel*)));

    connect(channel, SIGNAL(indexChanged()),
            this, SLOT(syncChannelNumber()));


    showHideButton = new QPushButton(QPixmap(":/icons/track_closed.png"), "", this);
    showHideButton->setMaximumSize(20, 20);
    connect(showHideButton, SIGNAL(clicked()), SLOT(changeDetailsVisibility()));

    SpinSlider* volumeSlider = new SpinSlider;
    QLabel* volumeLabel = new QLabel(this);
    volumeLabel->setText("<font size=\"-1\">Volume:</font>");
    volumeSlider->setValue(channel->volumePercent());
    connect(volumeSlider, SIGNAL(valueChanged(int)),
	    channel, SLOT(setVolume(int)));

    SpinSlider* balanceSlider = new SpinSlider;
    QLabel* balanceLabel = new QLabel(this);
    balanceLabel->setText("<font size=\"-1\">Pan:</font>");
    balanceSlider->setMinimum(-50);
    balanceSlider->setMaximum(50);
    balanceSlider->setValue(channel->balance());
    connect(balanceSlider, SIGNAL(valueChanged(int)),
	    channel, SLOT(setBalance(int)));

//    channel_but->setFixedSize(120, padButtonHeight);
    QFont f(font());
    f.setPointSize(10);
    channel_but->setFont(f);
#if defined(Q_WS_MAC)
    channel_but->setStyle(new QMotifStyle);
#endif

    active_box->setChecked(0);
    active_box->setEnabled(0);
    active_box->setFixedWidth(20);
    active_box->setAccel(QKeySequence(myNumber()));

    QGridLayout* layout = new QGridLayout(this);
    layout->setSpacing(5);
    layout->setMargin(0);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(showHideButton);
    hLayout->addWidget(active_box);
    layout->addLayout(hLayout, 0, 0);
    layout->addWidget(channel_but, 0, 1);
    layout->addWidget(volumeLabel, 1, 0, Qt::AlignRight);
    layout->addWidget(volumeSlider, 1, 1);
    layout->addWidget(balanceLabel, 2, 0, Qt::AlignRight);
    layout->addWidget(balanceSlider, 2, 1);
    detailsWidgets << volumeLabel;
    detailsWidgets << volumeSlider;
    detailsWidgets << balanceLabel;
    detailsWidgets << balanceSlider;
    layout->setSizeConstraint(QLayout::SetFixedSize);
    channel_but->setMinimumWidth(170);
    setLayout(layout);
    syncDetailsVisibility();

    activate();
}

void ChannelRow::syncDetailsVisibility() {
    if (detailsShown) {
        foreach (QWidget* widget, detailsWidgets) {
            widget->show();
        }
        showHideButton->setIcon(QPixmap(":/icons/track_open.png"));
    } else {
        foreach (QWidget* widget, detailsWidgets) {
            widget->hide();
        }
        showHideButton->setIcon(QPixmap(":/icons/track_closed.png"));
    }
}

void ChannelRow::changeDetailsVisibility() {
    detailsShown = detailsShown ? false : true;
    channel->showDetails(detailsShown);
    syncDetailsVisibility();
    emit showDetails(detailsShown);
}

const QString ChannelRow::myNumber() {
    return QString::number(channel->rowIndex() + 1);
}

void ChannelRow::syncChannelNumber() {
    active_box->setAccel(QKeySequence(myNumber()));
}

void ChannelRow::activate() {
    active_box->setEnabled(1);

    if (channel->isActive()) active_box->setChecked(1);

    connect(active_box, SIGNAL(toggled(bool)), 
            channel,    SLOT(setActive(bool)));
}

void ChannelRow::popupChannelMenu() {
    channel_menu->exec(channel_but->mapToGlobal(QPoint(3,3)));
}

uint ChannelRow::rowIndex() {
    return channel->rowIndex();
}

void ChannelRow::setRowIndex(uint i) {
    channel->setRowIndex(i);
}

void ChannelRow::setName(QString n) {
    channelName = n;
    channel_but->setText(channelName);
}

//EOF
