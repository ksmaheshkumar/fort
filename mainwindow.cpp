/*
 * This file is part of Fort.
 *
 * Fort is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fort is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fort.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2015 Niko Rosvall <niko@ideabyte.net>
 *
 */

#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "itemdialog.h"
#include <QClipboard>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include "changemasterpassphrase.h"
#include "aboutdialog.h"
#include "preferencesdialog.h"
#include "dataexporter.h"

/* Main window constructor.
 * Setup ui and initial flag statuses.
 *
 * Once program execution is in this point, possible data
 * is already decrypted. Decrypted data is populated to the
 * view.
 */
MainWindow::MainWindow(Security *sec, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _sec = sec;
    _locked = false;
    _wantClose = false;
    _windowStateLoginDialog = NULL;

    _collection.loadItems();
    populateFromCollection(_collection);
    handleActionsState();

    //Apply settings, at the moment idle interval as well as
    //_wantClose is set.
    applySettings();

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()),this,SLOT(onTimerTick()));
    _timer->start(1000);
}

/* Deconstructor.
 * Release memory from all allocated items and delete ui.
 */
MainWindow::~MainWindow()
{
    for(int i = 0; i < ui->listWidget->count() - 1; i++)
        delete ui->listWidget->takeItem(i);

    delete ui;
    _timer->stop();
    delete _timer;

    if(_windowStateLoginDialog != NULL)
        delete _windowStateLoginDialog;
}

/* Listen main window events.
 *
 * On minimize, data is encrypted and removed from the view.
 * On maximize/normalize data is decrypted and populated back to the view.
 *
 * On encryption variable _locked is set to true.
 */
void MainWindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::WindowStateChange)
    {
        if(this->windowState() == Qt::WindowMinimized)
        {
            //Do encryption here, clear the masterpassword from memory
            //This should also clear all the data (listview and backend collection)
            if(!_locked)
            {
                if(_sec->encryptAll())
                {
                    _sec->clearMasterPassphraseHashFromMemory();
                    ui->listWidget->clear();
                    _collection.clearItems();
                    _locked = true;
                }
                else
                    QMessageBox::information(this,"Fort Password Manager",_sec->getLastErrorMessage());
            }

            //Prevent duplicate login dialogs
            if(_windowStateLoginDialog != NULL)
                _windowStateLoginDialog->close();
        }
        else if(this->windowState() == Qt::WindowNoState || this->windowState() == Qt::WindowMaximized)
        {
            if(_locked)
            {
                if (_windowStateLoginDialog == NULL)
                    _windowStateLoginDialog = new LogInDialog(_sec, this);

                if(_windowStateLoginDialog->exec() != QDialog::Accepted)
                {
                    this->setWindowState(Qt::WindowMinimized);
                }
                else
                {
                    //Upon successful decryption re-read items and populate the view
                    _collection.loadItems();
                    populateFromCollection(_collection);
                    _locked = false;
                }

                delete _windowStateLoginDialog;
                _windowStateLoginDialog = NULL;
            }
        }
    }

    QMainWindow::changeEvent(e);
}

/* Overrides QMainWindow closeEvent method.
 * Depending on preferences set, window is either
 * closed or minimized.
 *
 * Encryption is also performed if the data is
 * in decrypted state.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!_wantClose)
    {
        this->setWindowState(Qt::WindowMinimized);
        event->ignore();
    }
    else
    {
        if(!_locked)
        {
            if(_sec->encryptAll())
            {
                _sec->clearMasterPassphraseHashFromMemory();
                _locked = true;
            }
            else
                QMessageBox::information(this,"Fort Password Manager",
                                         _sec->getLastErrorMessage());
        }

        event->accept();
    }
}

/* Populate data from internal collection to
 * the view.
 */
void MainWindow::populateFromCollection(ItemCollection &collection)
{
    ui->listWidget->clear();

    for(int i = 0; i < collection.itemCount(); i++)
    {
        Item it = collection.getItem(i);
        ui->listWidget->addItem(getQListWidgetItem(it.getTitle(),
                                                   it.getIsFavorite()));
    }
}

/* This methods handles toolbar and menu buttons
 * disabled/enabled state depending on if there
 * are selected items in the view.
 */
void MainWindow::handleActionsState()
{
    if(ui->listWidget->selectedItems().count() == 0)
    {
        ui->actionCopy->setEnabled(false);
        ui->actionRemove->setEnabled(false);
        ui->actionEdit->setEnabled(false);
        ui->actionOpen_url->setEnabled(false);
        ui->actionTag->setEnabled(false);
    }
    else
    {
        ui->actionCopy->setEnabled(true);
        ui->actionRemove->setEnabled(true);
        ui->actionEdit->setEnabled(true);
        ui->actionTag->setEnabled(true);
        if(_collection.getItem(
                    ui->listWidget->currentIndex().row()).getHasUrl())
            ui->actionOpen_url->setEnabled(true);
        else
            ui->actionOpen_url->setEnabled(false);
    }
}

/* Called when user wants to create
 * new item.
 *
 * Displays add new item dialog.
 */
void MainWindow::on_actionNew_triggered()
{
    ItemDialog d(this);

    if(d.exec() == QDialog::Accepted)
    {
        Item item(d.getTitle(),d.getUser(),d.getPassword());
        item.setUrl(d.getUrl());
        item.setNotes(d.getNotes());
        item.setFavorite(d.getIsFavorite());

        _collection.addItem(item);

        //ui->listWidget->addItem(getQListWidgetItem(item.getTitle(),item.getIsFavorite()));
        //ui->listWidget->setCurrentRow(ui->listWidget->count() - 1,
                                      //QItemSelectionModel::ClearAndSelect);

        populateFromCollection(_collection);

        //Scroll to the end of the list(where the new item is) if the item is not favorite.
        //Favorites are always on top;
        if(!item.getIsFavorite())
            ui->listWidget->setCurrentRow(ui->listWidget->count() - 1,QItemSelectionModel::ClearAndSelect);
    }
}

/* Called when user wants to remove
 * an item from the view.
 *
 * Item is also removed from the internal collection.
 */
void MainWindow::on_actionRemove_triggered()
{
    int row = ui->listWidget->currentIndex().row();
    int current = _collection.getItemIndexByName
            (ui->listWidget->item(row)->text());

    QListWidgetItem *item = ui->listWidget->takeItem(row);
    delete item;

    _collection.removeItem(current);
    handleActionsState();
}

/* Copies selected plain passphrase of the item
 * to the clipboard.
 */
void MainWindow::setSelectedItemPasswordToClipboard(int itemRow)
{
    int current = _collection.getItemIndexByName
            (ui->listWidget->item(itemRow)->text());

    QClipboard *cb = QApplication::clipboard();
    cb->setText(_collection.getItem(current).getPassword());

    ui->statusBar->showMessage(tr("Password copied"),800);
}

/* When item on the view is double clicked,
 * copy the plain password to the clipboard.
 */
void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    setSelectedItemPasswordToClipboard(index.row());
}

/* Called when user clicks copy.
 * Plain password of the selected item
 * is copied to the clipboard.
 */
void MainWindow::on_actionCopy_triggered()
{
    setSelectedItemPasswordToClipboard(ui->listWidget->currentIndex().row());
}

/* Called when an item is selected in the view.
 * Handle button states.
 */
void MainWindow::on_listWidget_itemSelectionChanged()
{
    handleActionsState();
}

/* Lock action. Simply minimize the window.
 * Minimize event will handle data encryption.
 */
void MainWindow::on_actionLock_triggered()
{
    this->setWindowState(Qt::WindowMinimized);
}

/* When an item in the view is right clicked
 * show context menu with the same actions
 * found on the toolbar and in the item-menu.
 */
void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QPoint p = ui->listWidget->mapToGlobal(pos);

    QMenu menu;
    menu.addAction(ui->actionNew);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionRemove);
    menu.addSeparator();
    menu.addAction(ui->actionCopy);
    menu.addAction(ui->actionOpen_url);
    menu.exec(p);
}

/* Get a pointer to the item in the view.
 * Set item status tip and an icon depending
 * if the item is tagged as a favorite or not.
 *
 * Allocated items are deleted in the class deconstructor.
 */
QListWidgetItem* MainWindow::getQListWidgetItem(QString title,bool fav)
{
    QListWidgetItem *i = new QListWidgetItem(title);

    QIcon icon(":/icons/Tag.png");

    if(fav)
        i->setIcon(icon);
    else
        i->setIcon(QIcon(":/icons/Tag2.png"));

    i->setStatusTip(
                _collection.getItem
                (_collection.getItemIndexByName(title)).getUser());

    return i;
}

/* Called from menu action show preferences dialog.
 * Applies settings in the runtime.
 */
void MainWindow::applySettings()
{
    bool minimizeOnClose = _settingsParser.getBoolean("minimizeonclose");

    if(minimizeOnClose)
        _wantClose = false;
    else
        _wantClose = true;

    int idleValue = _settingsParser.getIntIdleTime("idleinterval");
    _idleDetector.setWantedIdleTime(idleValue * 60000);
}

/* Edit action.
 *
 * Open add item dialog and fill it with data from
 * the selected item.
 *
 * If user edits the item data, old item is removed
 * and new one is added with the edited data.
 */
void MainWindow::on_actionEdit_triggered()
{
    int row = ui->listWidget->currentIndex().row();
    int current = _collection.getItemIndexByName
            (ui->listWidget->item(row)->text());

    ItemDialog d(this);
    d.setWindowTitle("Edit item");
    d.setData(_collection.getItem(current).getTitle(),
              _collection.getItem(current).getUser(),
              _collection.getItem(current).getPassword(),
              _collection.getItem(current).getUrl(),
              _collection.getItem(current).getNotes(),
              _collection.getItem(current).getIsFavorite());

    if(d.exec() == QDialog::Accepted)
    {
        QListWidgetItem *curritem = ui->listWidget->takeItem(row);
        delete curritem;
        _collection.removeItem(current);

        Item item(d.getTitle(),d.getUser(),d.getPassword());
        item.setUrl(d.getUrl());
        item.setFavorite(d.getIsFavorite());
        item.setNotes(d.getNotes());

        _collection.addItem(item);

        populateFromCollection(_collection);
    }
}

/* If an item has url, open it with the default
 * web browser of the system.
 */
void MainWindow::on_actionOpen_url_triggered()
{
    int row = ui->listWidget->currentIndex().row();
    int current = _collection.getItemIndexByName
            (ui->listWidget->item(row)->text());

    QUrl url = _collection.getItem(current).getUrlAsQUrl();

    if(url.isValid())
    {
        setSelectedItemPasswordToClipboard(current);
        QDesktopServices::openUrl(url);
    }
}

/* Tag action.
 *
 * Tag selected item as a favorite. If the item
 * is already a favorite, favorite tag is removed
 * from the item.
 *
 * This action also clears a search if it's active.
 * Clearing search is necessary as we need to populate
 * all items again to get the right order for them
 * tagged items.
 */
void MainWindow::on_actionTag_triggered()
{
    int row = ui->listWidget->currentIndex().row();
    int current = _collection.getItemIndexByName
            (ui->listWidget->item(row)->text());

    Item item = _collection.getItem(current);

    if(item.getIsFavorite())
        item.setFavorite(false);
    else
        item.setFavorite(true);

    _collection.removeItem(current);
    _collection.addItem(item);

    if(!ui->lineEditSearch->text().isEmpty())
        ui->lineEditSearch->setText("");
    else
        populateFromCollection(_collection);
}

/* Called when users starts searching items.
 * Only items that match the search will be displayed
 * in the view.
 *
 * Copy is taken of the internal item collection
 * so we can easily restore the original view.
 */
void MainWindow::on_lineEditSearch_textChanged(const QString &arg1)
{
    _collection.createSearchView(arg1);
    populateFromCollection(_collection);
     _collection._list = _collection._backupList;

    if(this->ui->lineEditSearch->text().isEmpty())
        populateFromCollection(_collection);

}

/* Change master passphrase action.
 * Displays a dialog that allows user to change the
 * master passphrase.
 */
void MainWindow::on_actionMaster_Passphrase_triggered()
{
    changemasterpassphrase dialog(_sec, this);

    if(dialog.exec() == QDialog::Accepted)
        _sec->setMasterPassphraseHash(dialog.getNewPassphraseHash());
}

/* Called when File->Quit action is triggered.
 * _wantClose variable is set to true to allow
 * Fort to close. (User might have minimize on close property set to true)
 */
void MainWindow::on_actionQuit_triggered()
{
    _wantClose = true;
    _timer->stop();
    this->close();
}

/*Show about dialog.
 */
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

/* This method is executed everytime when the timer ticks.
 * Timer is set to tick every second.
 *
 * System idle time is detected and compared to the value user
 * wants(defaults to three minutes). If idle time is >= than the user
 * defined value application window will minimize.
 *
 * When user leaves Fort open there's a threat
 * that someone could access the machine and read passwords from Fort.
 * By minimizing the window(which will also encrypt the data, see onChangeEvent)
 * we protect the user data.
 *
 * User has the option to disable this functionality via preferences.
 */
void MainWindow::onTimerTick()
{
    int idle_value = _idleDetector.getWantedIdleValue();
    long currentIdleTime = _idleDetector.getIdleTime();

    if(currentIdleTime == -1)
        return;

    if(currentIdleTime >= idle_value)
    {
        //On idle only minimize if data is not encrypted
        if(!_locked) {
            if(this->windowState() != Qt::WindowMinimized)
                this->setWindowState(Qt::WindowMinimized);
        }
    }
}

/* Show preferences dialog.
 * If user clicks OK in the dialog, settings are applied.
 */
void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(&_settingsParser, this);

    if(dialog.exec() == QDialog::Accepted)
       applySettings();

}

/* Export all items as plain text including passwords.
 * This can be used as a backup.
 */
void MainWindow::on_actionExport_As_Plain_Text_triggered()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Fort Password Manager",
                                    "Are you sure? Exported data will be decrypted and available as plain text including your passwords.",
                                    QMessageBox::Yes|QMessageBox::No);

      if (reply == QMessageBox::Yes) {

          DataExporter exporter(&_collection);
          QString fileName = QFileDialog::getSaveFileName(this, "Save File",
                                      "",
                                      "Plain Text (*.txt)");
          if(!fileName.isEmpty())
          {
              if(!exporter.exportAll(fileName))
              {
                    QMessageBox::information(this,"Fort Password Manager",
                                             "Failed to export items. File permission problem?");
              }
          }
      }
}
