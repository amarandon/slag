/*
 *   $Id: AudibleElem.h,v 1.8 2007/01/28 08:13:58 rhizome Exp $
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

#ifndef AUDIBLE_ELEM_H
#define AUDIBLE_ELEM_H

#include <qobject.h>

#include "../common.h"
#include "SafeData.h"

/**
 *  Abstract class for audible elements.
 * 
 *  It defines an interface and a default implementation for things that have
 *  volume, pan and can be muted and unmuted.
 */

class AudibleElem : public QObject {
    Q_OBJECT
    void updateVolumes();
    static inline sample_t per100toCoeff(int percent) { 
        return (sample_t)percent / 100; 
    }
protected:
    SafeData<bool> active;
    sample_t _volumeLeft;
    sample_t _volumeRight;
    int balanceFactor;
    int volumeFactor;
    AudibleElem();
    virtual ~AudibleElem();
public:
    void mute();
    void unmute();
    inline bool isActive() { return active.getValue(); }
    inline sample_t volumeLeft() { return _volumeLeft; }
    inline sample_t volumeRight() { return _volumeRight; }
    inline int volumePercent() { return volumeFactor; }
    inline int balance() { return balanceFactor; }
public slots:
    void setActive(bool);
    void setVolume(int vol);
    void setBalance(int bal);
signals:
    void activeStateChanged(bool);
    void volumeChanged(int);
    void balanceChanged(int);
    void modified();
};

#endif

//EOF
