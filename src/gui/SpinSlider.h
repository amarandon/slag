/*
 *   $Id: SpinSlider.h,v 1.1 2007/01/27 08:03:27 rhizome Exp $
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

#ifndef SPINSLIDER_H
#define SPINSLIDER_H

#include <QWidget>

class QSpinBox;
class QSlider;

class SpinSlider : public QWidget {
    int value;
    Q_OBJECT
    QSpinBox* spinBox;
    QSlider* slider;
public:
    SpinSlider(QString = "");
    void setMinimum(int);
    void setMaximum(int);
public slots:
    void setValue(int);
signals:
    void valueChanged(int);
};

#endif
