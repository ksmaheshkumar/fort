#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include <QMainWindow>
#include <QModelIndex>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include "itemcollection.h"
#include "security.h"
#include "idledetector.h"
#include "settingsparser.h"
#include "logindialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(Security *sec, QWidget *parent = 0);
    ~MainWindow();
    void changeEvent(QEvent *);

protected:
    void closeEvent(QCloseEvent *);
    
private slots:
    void on_actionNew_triggered();
    void on_actionRemove_triggered();
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_actionCopy_triggered();
    void on_listWidget_itemSelectionChanged();
    void on_actionLock_triggered();
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionEdit_triggered();
    void on_actionOpen_url_triggered();
    void on_actionTag_triggered();
    void on_lineEditSearch_textChanged(const QString &arg1);
    void on_actionMaster_Passphrase_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void onTimerTick();
    void on_actionPreferences_triggered();
    void on_actionExport_As_Plain_Text_triggered();

private:
    Ui::MainWindow *ui;
    ItemCollection _collection;
    void populateFromCollection(ItemCollection &collection);
    void setSelectedItemPasswordToClipboard(int itemRow);
    void handleActionsState();
    QListWidgetItem *getQListWidgetItem(QString title,bool fav);
    Security *_sec;
    bool _locked;
    bool _wantClose;
    QTimer *_timer;
    IdleDetector _idleDetector;
    SettingsParser _settingsParser;
    void applySettings();
    LogInDialog *_windowStateLoginDialog;
};

#endif // MAINWINDOW_H
