/*
 *   $Id: AudibleElem.cpp,v 1.1 2007/01/28 08:13:58 rhizome Exp $
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

#include "AudibleElem.h"

AudibleElem::AudibleElem() : 
    _volumeLeft(1), 
    _volumeRight(1),
    balanceFactor(0),
    volumeFactor(100)
{}

void AudibleElem::setVolume(int vol) { 
    emit volumeChanged(vol);
    emit modified();
    volumeFactor = vol;
    updateVolumes();
}

void AudibleElem::setBalance(int bal) { 
    emit balanceChanged(bal);
    emit modified();
    balanceFactor = bal;
    updateVolumes();
}

void AudibleElem::updateVolumes() {
    if (balanceFactor > 0) {
        _volumeLeft = per100toCoeff(volumeFactor)
            * (1.0 - (balanceFactor / 50.0));
        _volumeRight = per100toCoeff(volumeFactor);
    } else if (balanceFactor < 0) {
        _volumeLeft = per100toCoeff(volumeFactor);
        _volumeRight = per100toCoeff(volumeFactor)
            * (1.0 - ((-balanceFactor) / 50.0));
    } else {
        _volumeLeft = per100toCoeff(volumeFactor);
        _volumeRight = per100toCoeff(volumeFactor);
    }
}

void AudibleElem::setActive(bool val) { 
    active.setValue(val); 
    emit modified();
    emit activeStateChanged(val); 
}

void AudibleElem::mute() { 
    active.setValue(0); 
    emit modified();
}
void AudibleElem::unmute() { 
    active.setValue(1); 
    emit modified();
}

//EOF
