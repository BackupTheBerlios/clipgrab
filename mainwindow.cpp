/*
    ClipGrab³
    Copyright (C) Philipp Schmieder
    http://clipgrab.de
    feedback [at] clipgrab [dot] de

    This file is part of ClipGrab.
    ClipGrab is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    ClipGrab is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ClipGrab.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "mainwindow.h"
#include "clipgrab.h"
#include <QUrl>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    currentVideo = NULL;
}


MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
    connect(cg, SIGNAL(compatiblePortalFound(bool, video*)), this, SLOT(compatiblePortalFound(bool, video*)));
    /*
      welcome tab
    */
    changeTabMapper = new QSignalMapper(this);

    connect(ui.gotoSearch, SIGNAL(clicked()), changeTabMapper, SLOT(map()));
    changeTabMapper->setMapping(ui.gotoSearch, 2);
    connect(changeTabMapper, SIGNAL(mapped(int)), ui.mainTab, SLOT(setCurrentPage(int)));

    connect(ui.gotoDownload, SIGNAL(clicked()), changeTabMapper, SLOT(map()));
    changeTabMapper->setMapping(ui.gotoDownload, 1);
    connect(changeTabMapper, SIGNAL(mapped(int)), ui.mainTab, SLOT(setCurrentPage(int)));
    //*
    //* Download Tab
    //*
    connect(ui.downloadStart, SIGNAL(clicked()), this, SLOT(startDownload()));
    connect(ui.downloadLineEdit, SIGNAL(textChanged(QString)), cg, SLOT(determinePortal(QString)));
    ui.downloadTree->header()->setResizeMode(1, QHeaderView::Stretch);
    ui.downloadTree->header()->setStretchLastSection(false);
    ui.downloadTree->header()->setResizeMode(3, QHeaderView::ResizeToContents);
    for (int i = 0; i < this->cg->formats.size(); ++i)
    {
        this->ui.downloadComboFormat->addItem(this->cg->formats.at(i)._name);
    }

    //*
    //* Search Tab
    //*
    this->ui.mainTab->removeTab(2); //fixme!
    this->ui.mainTab->removeTab(0); //fixme!
    //this->addPortalsToSearchCombo();

    //*
    //* Settings Tab
    //*
    this->ui.settingsSavedPath->setText(cg->settings.value("savedPath", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).toString());
    this->ui.settingsSaveLastPath->setChecked(cg->settings.value("saveLastPath", true).toBool());
}

void MainWindow::addPortalsToSearchCombo()
{
    for (int i = 0; i < this->cg->portals.size(); ++i)
    {
        if (this->cg->portals.at(i)->supportsSearch())
        {
            this->ui.searchPortalCombo->addItem(this->cg->portals.at(i)->getName());
            this->ui.searchPortalCombo->setItemIcon(i, *(this->cg->portals.at(i)->getIcon()));
        }
 }
}

void MainWindow::startDownload()
{
    QTreeWidgetItem* tmpItem = new QTreeWidgetItem(QStringList() << currentVideo->getName() << currentVideo->title() << ui.downloadComboFormat->currentText());
    tmpItem->setSizeHint(0, QSize(16, 24));
    currentVideo->setTreeItem(tmpItem);
    currentVideo->setQuality(ui.downloadComboQuality->currentIndex());
    currentVideo->setConverter(cg->formats.at(ui.downloadComboFormat->currentIndex())._converter->createNewInstance(),cg->formats.at(ui.downloadComboFormat->currentIndex())._mode);
    QString title;
    title = QFileDialog::getSaveFileName(this, "Select Target", cg->settings.value("savedPath", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).toString() +"/"+currentVideo->getSaveTitle());
    cg->settings.setValue("savedPath", title.remove(title.split("/").last()));
    qDebug() << cg->settings.value("savedPath");
    if (!title.isEmpty())
    {
        currentVideo->setTargetPath(title);
        cg->addDownload(currentVideo);
        ui.downloadTree->insertTopLevelItem(0, tmpItem);
        currentVideo->_progressBar = new QProgressBar();
        currentVideo->_progressBar->setValue(0);
        currentVideo->_progressBar->setMaximum(1);
        ui.downloadTree->setItemWidget(tmpItem, 3, currentVideo->_progressBar);

        ((QProgressBar*) ui.downloadTree->itemWidget(tmpItem, 3))->setMaximum(100);
        connect(currentVideo, SIGNAL(progressChanged(int,int)),ui.downloadTree, SLOT(update()));
        connect(currentVideo, SIGNAL(downloadFinished()), currentVideo, SLOT(startConvert()));
        currentVideo = NULL;
        ui.downloadLineEdit->clear();
    }
}

void MainWindow::compatiblePortalFound(bool found, video* portal)
{
    ui.downloadStart->setDisabled(true);
    ui.downloadComboQuality->clear();;
    if (found == true)
    {
        ui.downloadLineEdit->setReadOnly(true);
        ui.downloadInfoBox->setText(tr("Please wait while ClipGrab is loading information about the video ..."));

        if (currentVideo)
        {
            delete currentVideo;
        }
        currentVideo = portal->createNewInstance();
        currentVideo->setUrl(ui.downloadLineEdit->text());
        connect(currentVideo, SIGNAL(analysingFinished()), this, SLOT(updateVideoInfo()));
        currentVideo->analyse();

    }
    else
    {
        if (ui.downloadLineEdit->text() == "")
        {
            ui.downloadInfoBox->setText(tr("Please enter the link to the video you want to download in the field below."));
        }
        else
        {
            ui.downloadInfoBox->setText(tr("The link you have entered seems to not be recognised by any of the supported portals.<br/>Now ClipGrab will check if it can download a video from that site anyway."));
        }
    }
}

void MainWindow::updateVideoInfo()
{
    if (currentVideo->title() != "")
    {
        ui.downloadInfoBox->setText("<strong>" + currentVideo->title() + "</strong>");
        ui.downloadLineEdit->setReadOnly(false);
        ui.downloadStart->setDisabled(false);
        ui.downloadComboQuality->clear();
        ui.downloadComboQuality->addItems(currentVideo->getSupportedQualities());
    }
    else
    {
        ui.downloadInfoBox->setText(tr("No downloadable video could be found. Maybe you have entered the wrong link or there is a problem with your connection."));
        ui.downloadLineEdit->setReadOnly(false);
        ui.downloadStart->setDisabled(true);
    }
}
void MainWindow::on_settingsSavedPath_textChanged(QString string)
{
    qDebug() << string;
    this->cg->settings.setValue("savedPath", string);
}
