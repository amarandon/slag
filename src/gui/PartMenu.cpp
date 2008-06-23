/*
 *   $Id: PartMenu.cpp,v 1.15 2007/01/26 07:39:55 rhizome Exp $
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

#include <qinputdialog.h>
#include <qlineedit.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <Q3PopupMenu>

#include "../audio_engine/Song.h"
#include "../audio_engine/Pattern.h"
#include "../audio_engine/Channel.h"
#include "../audio_engine/Part.h"
#include "../audio_engine/Pad.h"
#include "RowContext.h"
#include "PartMenu.h"

PartMenu::PartMenu(QWidget* parent, Song* song, RowContext* row_context) : 
    Q3PopupMenu(parent, "part menu"), 
    song(song),
    channel(row_context->channel),
    pattern(row_context->pattern), 
    previous_part(NULL),
    current_part(pattern->part(channel)), 
    choice_menu(new PartListMenu(&song->parts())), 
    create_from_menu(new PartListMenu(&song->parts())),
    edit_menu(new PartEditMenu(current_part))
{
    insertItem( tr("Link to"), choice_menu);

    connect(choice_menu, SIGNAL(activated(int)), this, SLOT(partChosen(int)));
    connect(create_from_menu, SIGNAL(activated(int)), 
            this,             SLOT(createFromExisting(int)));

    // partAdded signal is transmitted to the top level object which dispatch it
    // among all PartMenu's
    connect(this, SIGNAL(partAdded(Part*)), song, SIGNAL(partAdded(Part*)));
    connect(song, SIGNAL(partAdded(Part*)), this, SLOT(addPart(Part*)));

    if (current_part != NULL) {
        connect(current_part, SIGNAL(deleted(Part*)), 
                this,         SLOT(partDeleted(Part*)));
    }

    insertItem( tr("Copy"), create_from_menu);

    editItemId = insertItem( tr("Edit"), edit_menu);
    renameItemId = insertItem( tr("Rename"), this, SLOT(renamePart()) );
    deleteItemId = insertItem( tr("Delete"), this, SLOT(deletePart()) );
    if (current_part == NULL) {
        setItemVisible(editItemId, false);
        setItemVisible(renameItemId, false);
        setItemVisible(deleteItemId, false);
    }
}

PartMenu::~PartMenu() {
    qDebug("Destruction PartMenu");
}

void PartMenu::setPart(Part* new_part) {
    //if (previous_part != NULL && new_part == NULL) {
    if (new_part == NULL) {
        setItemVisible(editItemId, false);
        setItemVisible(renameItemId, false);
        setItemVisible(deleteItemId, false);
        disconnect(current_part, SIGNAL(deleted(Part*)), 
                   this,         SLOT(partDeleted(Part*)));
    } else if (new_part != NULL) {
        setItemVisible(editItemId, true);
        setItemVisible(renameItemId, true);
        setItemVisible(deleteItemId, true);
        connect(new_part, SIGNAL(deleted(Part*)), 
                this,     SLOT(partDeleted(Part*)));
    }
    previous_part = current_part;
    current_part = new_part;
    edit_menu->setPart(current_part);
    if (new_part != NULL) emit partChanged(new_part);
}

void PartMenu::partDeleted(Part*) {
    setPart(NULL);
}

void PartMenu::partChosen(int id) {
    setPart(choice_menu->getPartById(id));
}

void PartMenu::addPart(Part* new_part) {
    choice_menu->addPart(new_part);
    create_from_menu->addPart(new_part);
}

void PartMenu::createBlank() {
    QString new_part_name;
    if (askForPartName(new_part_name)) {
        Part* new_part = new Part;
        for (uint pad_i = 1; pad_i < 32 + 1; ++pad_i) {
            Pad * new_pad = new Pad;
            new_pad->mute();
            new_pad->setVolume(100);
            new_part->addPad(new_pad);
        }
        new_part->setName(new_part_name); 
        setPart(new_part);
        emit partAdded(new_part);
        song->parts().append(new_part);
    }
}

void PartMenu::createFromExisting(int id) {
    QString new_part_name;
    if (askForPartName(new_part_name)) {
        Part* new_part = new Part(*create_from_menu->getPartById(id));
        new_part->setName(new_part_name); 
        setPart(new_part);
        emit partAdded(new_part);
        song->parts().append(new_part);
    }
}

void PartMenu::renamePart() {
    bool ok;
    QString original_name = current_part->name();
    QString text = QInputDialog::getText(
            tr("Rename part"), 
            tr("Enter new name:"), 
            QLineEdit::Normal,
            original_name, 
            &ok, 
            this );
    if ( ok && !text.isEmpty() ) {
        song->freePartName(original_name);
        current_part->setName(text);
    }
    emit renamed(text);
}

void PartMenu::deletePart() {
    int result = QMessageBox::question(
        this, 
        tr("Slag - Delete part ?"),
        tr("Are you sure you want to delete %1 ?").arg(current_part->name()),
        QMessageBox::Ok, 
        QMessageBox::Cancel);

    if (result == QMessageBox::Ok) {
        song->freePartName(current_part->name());
        current_part->scheduleDelete();
    }
}

bool PartMenu::askForPartName(QString& part_name) {
    bool ok;
    QString suggested_name = song->getNewPartName();
    part_name = QInputDialog::getText(
            tr("Give this new part a name"), 
            tr("Name of new part :"), 
            QLineEdit::Normal, 
            suggested_name, 
            &ok, 
            this );
    if (part_name != suggested_name) {
        song->freePartName(suggested_name);
    }
    // Si le user a cliqué ok sans rien mettre, on renvoie le popup
    if (ok) {
        if(part_name.isEmpty()) {
            ok = askForPartName(part_name);
        }
    }
    return ok;
}


PartListMenu::PartListMenu(PartList* song_parts)
    : song_parts(song_parts) 
{
    foreach (Part* part, *song_parts) {
        addPart(part);
    }
}

Part* PartListMenu::getPartById(int id) {
    return part_map[id];
}

int PartListMenu::getIdByPart(Part* part) {
    QMap<int, Part*>::Iterator it;
    int result = 0;
    for ( it = part_map.begin(); it != part_map.end(); ++it ) {
        if (it.data() == part) {
            result = it.key();
            break;
        }
    }
    return result; // We should
}

void PartListMenu::addPart(Part* new_part) {
    part_map[insertItem(new_part->name())] = new_part;
    connect(new_part, SIGNAL(renamed(Part*)), this, SLOT(partRenamed(Part*)));
    connect(new_part, SIGNAL(deleted(Part*)), this, SLOT(partDeleted(Part*)));
}

void PartListMenu::partRenamed(Part* part) {
    changeItem(getIdByPart(part), part->name());
}

void PartListMenu::partDeleted(Part* part) {
    removeItem(getIdByPart(part));
}

PartEditMenu::PartEditMenu(Part* part) 
    : Q3PopupMenu(), part(part)
{
    features.setAutoDelete(true);
    features.append(new EditFeature( tr("1/2"), SLOT(active1for2())));
    features.append(new EditFeature( tr("1/4"), SLOT(active1for4())));
    features.append(new EditFeature( tr("1/8"), SLOT(active1for8())));
    features.append(new EditFeature( tr("Blank"), SLOT(unactiveAll())));
    features.append(new EditFeature( tr("Fill"), SLOT(activeAll())));
    features.append(new EditFeature( tr("Invert"), SLOT(invert())));
    features.append(new EditFeature( tr("Shift"), SLOT(shiftRight())));
    features.append(new EditFeature( tr("Randomize"), SLOT(randomize())));
    appendItems();
}

void PartEditMenu::appendItems() {
    if (part != NULL)
        for (EditFeature* feature = features.first(); feature; 
                feature = features.next())
            item_ids.append(insertItem(feature->first, part, feature->second));
}

void PartEditMenu::setPart(Part* new_part) {
    if (part != NULL) {
        for (uint i = 0; i < features.count(); ++i) {
            disconnectItem(item_ids[i], part, features.at(i)->second);
        }
    }

    part = new_part;

    if (part != NULL) {
        if (item_ids.size() == 0) {
            appendItems();
        } else {
            for (uint i = 0; i < features.count(); ++i) {
                connectItem(item_ids[i], part, features.at(i)->second);
            }
        }
    }
}

//EOF
