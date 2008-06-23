/*
 *   $Id: ToolBar.h,v 1.12 2007/01/27 08:03:27 rhizome Exp $
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

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class QMainWindow;
class QComboBox;
class SpinSlider;
class QRadioButton;
class Slag;

class ToolBar : public QToolBar {
    Q_OBJECT
    Slag* slag;
    QComboBox* pattern_box;
    SpinSlider* tempoControl;
    SpinSlider* volumeControl;
    QRadioButton* pattern_mode_radio;
    QRadioButton* song_mode_radio;
public:
    ToolBar(Slag*);
    void sync();
};

#endif
