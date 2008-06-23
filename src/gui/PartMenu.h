/*
 *   $Id: PartMenu.h,v 1.7 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef PART_MENU_H
#define PART_MENU_H

#include <q3popupmenu.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>
#include <QList>

class Song;
class Part;
class Channel;
class Pattern;
class PartListMenu;
class PartEditMenu;
class RowContext;

class PartMenu : public Q3PopupMenu {
    Q_OBJECT
public:
    PartMenu(QWidget*, Song*, RowContext*);
    ~PartMenu();
public slots:
    void addPart(Part*);
    void partDeleted(Part*);
signals:
    void partChanged(Part*);
    void partAdded(Part*);
    void renamed(const QString &);
private slots:
    void partChosen(int);
    void createBlank();
    void createFromExisting(int);
    void deletePart();
    void renamePart();
private:
    bool askForPartName(QString&);
    void updateEditMenu();
    void setPart(Part*);

    Song* song;
    Channel* channel;
    Pattern* pattern;
    Part* previous_part;
    Part* current_part;
    PartListMenu* choice_menu;
    PartListMenu* create_from_menu;
    PartEditMenu* edit_menu;
    int editItemId;
    int renameItemId;
    int deleteItemId;
};

typedef QPair<QString, QString> EditFeature;
typedef Q3PtrList<EditFeature> EditFeatureList;

class PartListMenu : public Q3PopupMenu {
    Q_OBJECT
public:
    PartListMenu(QList<Part*>*);
    Part* getPartById(int);
    void addPart(Part*);
public slots:
    void partRenamed(Part*);
    void partDeleted(Part*);
private:
    int getIdByPart(Part*);

    QList<Part*>* song_parts;/// referred parts
    QMap<int, Part*> part_map;/// menu item ids and parts pointers association
};

class PartEditMenu : public Q3PopupMenu {
    Q_OBJECT
public:
    PartEditMenu(Part*);
    void appendItems();
    void setPart(Part*);
private:
    Part* part;
    EditFeatureList features;
    Q3ValueList<int> item_ids;
};
    

#endif

//EOF
