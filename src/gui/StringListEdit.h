/*
 *   $Id: StringListEdit.h,v 1.6 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef STRINGLISTEDIT_H
#define STRINGLISTEDIT_H

#include <qdialog.h>
#include <q3listbox.h>
//Added by qt3to4:
#include <Q3ValueList>

class Q3ListBox;
class QPushButton;
class QStringList;
class UntitledList;

struct StringItem : public Q3ListBoxText {
    StringItem();
    StringItem(QString);
    StringItem(const StringItem&);
    StringItem(StringItem*);
    void setText(const QString&);
    QString originalText;
    bool isNew;
    bool isRenamed;
};

typedef Q3ValueList<StringItem> StringItemList;

class StringListEdit : public QDialog
{
    Q_OBJECT

public:
    StringListEdit(const QStringList &list, 
                   UntitledList* unamedList,
                   QWidget *parent = 0);

    void setTexts(const QString &addCaption, const QString &addLabel,
                  const QString &editCaption, const QString &editLabel = "");
    bool askBeforeRemoving() const { return ask; }
    void setAskBeforeRemoving(bool a) { ask = a; }
    bool allowDuplicates() const { return duplicatesOk; }
    void setAllowDuplicates(bool allow) { duplicatesOk = allow; }
    StringItemList list() const;
    void setList(const QStringList &list);

protected slots:
    virtual void addString();
    virtual void editString();
    virtual void removeString();
    void updateButtons();

private slots:
    void moveUp();
    void moveDown();

protected:
    QString addCaption;
    QString addLabel;
    QString editCaption;
    QString editLabel;
    bool ask;
    bool duplicatesOk;
    Q3ListBox *listBox;

private:
    QPushButton *editButton;
    QPushButton *removeButton;
    QPushButton *upButton;
    QPushButton *downButton;
    UntitledList* _suggestedStringsList;
};

#endif
