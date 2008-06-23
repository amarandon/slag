/*
 *   $Id: SpinSlider.cpp,v 1.1 2007/01/27 08:03:27 rhizome Exp $
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

#include <QSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>

#include "SpinSlider.h"

SpinSlider::SpinSlider(QString legend) 
    : value(0) 
{
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setFixedWidth(100);
    spinBox = new QSpinBox;
    spinBox->setRange(0, 100);
    spinBox->setSingleStep(1);
    QHBoxLayout* layout = new QHBoxLayout;
    if (! legend.isEmpty() ) {
        QLabel* label = new QLabel;
        label->setText(legend);
        layout->addWidget(label);
    }
    layout->addWidget(spinBox);
    layout->addWidget(slider);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setMargin(0);
    setLayout(layout);

    connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), 
            this, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), 
            this, SLOT(setValue(int)));
}

void SpinSlider::setMinimum(int min) {
    spinBox->setMinimum(min);
    slider->setMinimum(min);
}

void SpinSlider::setMaximum(int max) {
    spinBox->setMaximum(max);
    slider->setMaximum(max);
}

void SpinSlider::setValue(int val) {
    if (val != value) {
        spinBox->setValue(val);
        slider->setValue(val);
        value = val;
        emit valueChanged(val);
    }
}

//EOF
