/*
 *   $Id: StringListEdit.cpp,v 1.8 2007/01/26 07:39:55 rhizome Exp $
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

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qinputdialog.h>
#include <qfontmetrics.h>
#include <qlayout.h>
#include <q3listbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include "StringListEdit.h"
#include "../audio_engine/UntitledList.h"

StringItem::StringItem() :
        Q3ListBoxText(QString("")),
        originalText(QString("")),
        isNew(false),
        isRenamed(false)
{ }

StringItem::StringItem(QString text) :
        Q3ListBoxText(text),
        originalText(text),
        isNew(false),
        isRenamed(false)
{ }

StringItem::StringItem(StringItem* item) :
    Q3ListBoxText(item->text()),
    originalText(item->originalText),
    isNew(item->isNew),
    isRenamed(item->isRenamed)
{ }

StringItem::StringItem(const StringItem& item) :
    Q3ListBoxText(item.text()),
    originalText(item.originalText),
    isNew(item.isNew),
    isRenamed(item.isRenamed)
{ }

void StringItem::setText(const QString & text) {
    Q3ListBoxText::setText(text);
}

StringListEdit::StringListEdit(const QStringList &list, 
                               UntitledList* unamedList, 
                               QWidget *parent)
    : QDialog(parent), 
      ask(false), 
      duplicatesOk(false),
      _suggestedStringsList(unamedList)
{
    addCaption = "Add String";
    addLabel = "String:";
    editCaption = "Edit String";
    editLabel = addLabel;

    Q3HBoxLayout *hbox = new Q3HBoxLayout(this, 5, 5);
    Q3VBoxLayout *vbox = new Q3VBoxLayout;
    listBox = new Q3ListBox(this);
    connect(listBox, SIGNAL(currentChanged(Q3ListBoxItem*)),
            this, SLOT(updateButtons()));
    hbox->addWidget(listBox, 1);
    QPushButton *button = new QPushButton(tr("&Add..."), this);
    connect(button, SIGNAL(clicked()), this, SLOT(addString()));
    vbox->addWidget(button);
    editButton = new QPushButton(tr("&Edit..."), this);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editString()));
    vbox->addWidget(editButton);
    removeButton = new QPushButton(tr("&Remove..."), this);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeString()));
    vbox->addWidget(removeButton);
    upButton = new QPushButton(tr("&Up"), this);
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
    vbox->addWidget(upButton);
    downButton = new QPushButton(tr("&Down"), this);
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
    vbox->addWidget(downButton);
    vbox->addStretch(1);
    button = new QPushButton(tr("OK"), this);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    vbox->addWidget(button);
    button = new QPushButton(tr("Cancel"), this);
    connect(button, SIGNAL(clicked()), this, SLOT(reject()));
    vbox->addWidget(button);
    hbox->addLayout(vbox);

    setList(list);
}

void StringListEdit::setTexts(
        const QString &addCaption, const QString &addLabel,
        const QString &editCaption, const QString &editLabel)
{
    this->addCaption = addCaption;
    this->addLabel = addLabel;
    this->editCaption = editCaption;
    this->editLabel = editLabel.isEmpty() ? addLabel : editLabel;
}

void StringListEdit::setList(const QStringList &list)
{
    listBox->clear();
    for (QStringList::const_iterator it = list.begin(); it != list.end(); ++it)
        listBox->insertItem(new StringItem(*it));
    QFontMetrics fm(listBox->font());
    int width = 0;
    for (int i = 0; i < list.count(); ++i) {
        int w = fm.width(list[i]);
        if (w > width)
            width = w;
    }
    if (listBox->verticalScrollBar())
        width += listBox->verticalScrollBar()->width();
    listBox->setMinimumWidth(
        QMIN(width, qApp->desktop()->screenGeometry().width() * 4 / 5));
    updateButtons();
}

StringItemList StringListEdit::list() const
{
    StringItemList lst;
    for (uint i = 0; i < listBox->count(); ++i) {
        StringItem* listItem = (StringItem*) listBox->item(i);
        StringItem item(listItem);
        lst.append(item);
    }
    return lst;
}

void StringListEdit::addString()
{
    bool ok;
    QString suggested_string = _suggestedStringsList->getNewName();
    QString string = QInputDialog::getText(addCaption, addLabel,
                            QLineEdit::Normal, suggested_string,
                            &ok, this);
    if (string != suggested_string) {
        _suggestedStringsList->remove(suggested_string);
    }
    if (ok && !string.isEmpty()) {
        if (!duplicatesOk && listBox->findItem(string,
                               Qt::CaseSensitive | Q3ListBox::ExactMatch))
            return;
        StringItem* newItem = new StringItem(string);
        newItem->isNew = true;
        listBox->insertItem(newItem);
        listBox->setCurrentItem(listBox->count() - 1);
        listBox->ensureCurrentVisible();
        updateButtons();
    }
}

void StringListEdit::editString()
{
    bool ok;
    QString original = listBox->currentText();
    if (!original.isEmpty()) {
        QString string = QInputDialog::getText(editCaption, editLabel,
                                QLineEdit::Normal, original, &ok, this);
        if (ok && !string.isEmpty()) {
            if (!duplicatesOk && listBox->findItem(string,
                               Qt::CaseSensitive | Q3ListBox::ExactMatch))
                return;
            StringItem* stringItem = 
                static_cast<StringItem*>(listBox->item(listBox->currentItem()));
            stringItem->setText(string);
            stringItem->isRenamed = true;
            updateButtons();
            _suggestedStringsList->remove(original);
        }
    }
    else {
        addString();
    }
}

void StringListEdit::removeString()
{
    QString original = listBox->currentText();
    if (original.isEmpty() ||
        (ask && QMessageBox::question(this, tr("Remove"),
                        tr("Remove '%1'?").arg(original),
                        QMessageBox::Yes|QMessageBox::Default,
                        QMessageBox::No|QMessageBox::Escape) ==
                QMessageBox::No))
        return;
    _suggestedStringsList->remove(listBox->currentText());
    listBox->removeItem(listBox->currentItem());
    updateButtons();
}

void StringListEdit::moveUp()
{
    int i = listBox->currentItem();
    if (i > 0) {
        StringItem* movedItem = 
            new StringItem( (StringItem*) listBox->item(i - 1) );
        StringItem* replacedItem = 
            new StringItem( (StringItem*) listBox->item(i) );
        listBox->changeItem(replacedItem, i - 1);
        listBox->changeItem(movedItem, i);
        listBox->setCurrentItem(i - 1);
        updateButtons();
    }
}

void StringListEdit::moveDown()
{
    uint i = listBox->currentItem();
    if (i < listBox->count() - 1) {
        StringItem* movedItem = 
            new StringItem( (StringItem*) listBox->item(i + 1) );
        StringItem* replacedItem = 
            new StringItem( (StringItem*) listBox->item(i) );
        listBox->changeItem(replacedItem, i + 1);
        listBox->changeItem(movedItem, i);
        listBox->setCurrentItem(i + 1);
        updateButtons();
    }
}

void StringListEdit::updateButtons()
{
    bool hasItems = (bool)listBox->count();
    editButton->setEnabled(hasItems);
    // We need at least one pattern
    removeButton->setEnabled(listBox->count() > 1);
    int i = listBox->currentItem();
    upButton->setEnabled(hasItems && i > 0);
    downButton->setEnabled(hasItems && i < (int)listBox->count() - 1);
}

//EOF
