/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1PluginDialog                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VTI12Gui/VP1PluginDialog.h"
#include "ui_vp1plugindialog.h"
#include "VTI12Gui/VP1ChannelManager.h"
#include "VTI12Gui/VP1ExecutionScheduler.h"
#include "VP1Base/IVP1ChannelWidget.h"
#include <QFileInfo>
#include <cassert>

//____________________________________________________________________
class VP1PluginDialog::Imp {
public:
  VP1PluginDialog*pd;
  VP1ChannelManager* channelmanager;
  VP1ExecutionScheduler* scheduler;
  void setInfo();
  QLabel * label_selectplugin;
  QComboBox * comboBox_plugins;
  QLabel * label_fullpathtitle;
  QLabel * label_fullpath;
  QLabel * label_channels;
  QPushButton * pushButton_unload;
  QTextBrowser * textBrowser_channels;
  QMap<int,QString> index_2_fullpath;
  QString unloadfile;
};

//____________________________________________________________________
VP1PluginDialog::VP1PluginDialog(QWidget*parent, VP1ChannelManager*cm,VP1ExecutionScheduler*scheduler)
  : QDialog(parent), m_d(new Imp)
{
  m_d->pd = this;
  m_d->channelmanager=cm;
  m_d->scheduler=scheduler;
  Ui::VP1PluginDialog ui;
  ui.setupUi(this);
  ui.textBrowser_channels->viewport()->setBackgroundRole(QPalette::NoRole);
  m_d->label_selectplugin = ui.label_selectplugin;
  m_d->comboBox_plugins = ui.comboBox_plugins;
  m_d->label_fullpathtitle = ui.label_fullpathtitle;
  m_d->label_fullpath = ui.label_fullpath;
  m_d->label_channels = ui.label_channels;
  m_d->pushButton_unload = ui.pushButton_unload;
  m_d->textBrowser_channels = ui.textBrowser_channels;
  m_d->setInfo();
  setModal(true);
  connect(m_d->comboBox_plugins,SIGNAL(currentIndexChanged(int)),this,SLOT(pluginSelected(int)));
  connect(m_d->pushButton_unload,SIGNAL(clicked()),this,SLOT(unloadclicked()));
  connect(scheduler,SIGNAL(refreshingStatusChanged(bool)),this,SLOT(updateRefreshInfo()));

}

//____________________________________________________________________
void VP1PluginDialog::unloadclicked()
{
  int i = m_d->comboBox_plugins->currentIndex();
  if (i<0)
    return;
  assert(m_d->index_2_fullpath.contains(i));
  m_d->unloadfile=m_d->index_2_fullpath.value(i);
  accept();
}

//____________________________________________________________________
QString VP1PluginDialog::unloadfile()
{
  return m_d->unloadfile;
}

//____________________________________________________________________
VP1PluginDialog::~VP1PluginDialog()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
void VP1PluginDialog::Imp::setInfo()
{
  QStringList files = channelmanager->currentPluginFiles();
  comboBox_plugins->clear();
  index_2_fullpath.clear();
  if (files.empty()) {
    label_selectplugin->setEnabled(false);
    comboBox_plugins->setEnabled(false);
    label_fullpathtitle->setEnabled(false);
    label_fullpath->setEnabled(false);
    label_channels->setEnabled(false);
    pushButton_unload->setEnabled(false);
    label_fullpath->setText("n/a");
    comboBox_plugins->addItem("No plugins loaded");
    textBrowser_channels->setHtml("");
    textBrowser_channels->setEnabled(false);
  } else {
    label_selectplugin->setEnabled(true);
    comboBox_plugins->setEnabled(true);
    label_fullpathtitle->setEnabled(true);
    label_fullpath->setEnabled(true);
    label_channels->setEnabled(true);
    pushButton_unload->setEnabled(true);
    textBrowser_channels->setEnabled(true);

    int i(0);
    foreach(QString file,files) {
      QFileInfo fi(file);
      comboBox_plugins->addItem(fi.fileName());
      index_2_fullpath.insert(i++,file);
    }
    comboBox_plugins->setCurrentIndex(0);
    pd->pluginSelected(0);
  }
}

//____________________________________________________________________
void VP1PluginDialog::pluginSelected(int i)
{
  assert(i<m_d->comboBox_plugins->count());
  assert(m_d->index_2_fullpath.contains(i));
  m_d->label_fullpath->setText(m_d->index_2_fullpath.value(i));
  QStringList availablechannels = m_d->channelmanager->channelsInPluginFile(m_d->index_2_fullpath.value(i));
  if (availablechannels.empty()) {
    m_d->textBrowser_channels->setHtml("none");
    m_d->textBrowser_channels->setEnabled(false);
  } else {
    QString text;
    foreach(QString bn,availablechannels) {
      QStringList uns = m_d->channelmanager->basename2UniqueNames(bn);
      bool refreshing=false;
      foreach(QString un,uns) {
 	if (m_d->channelmanager->hasRefreshingSystem(un))
 	  refreshing=true;
	IVP1ChannelWidget* cw = m_d->channelmanager->uniqueName2Channel(un);
	assert(cw);
	connect(cw,SIGNAL(systemRefreshInfoChanged(QString,int,int)),this,SLOT(updateRefreshInfo()));
      }
      m_d->pushButton_unload->setEnabled(!m_d->scheduler->isRefreshing());
      QString icontext = m_d->channelmanager->getIconLocation(bn);
      text += (icontext.isEmpty()?"":"<img src=\""+icontext+"\">")+"<i>"
	+bn+(uns.empty()?"":" ["+QString::number(uns.count())
	     +" active"+(refreshing?" (refreshing)":"")+"]")+"</i><br/>";
    }
    m_d->textBrowser_channels->setHtml(text);
    m_d->textBrowser_channels->setEnabled(true);
  }
}

//____________________________________________________________________
void VP1PluginDialog::updateRefreshInfo() {
  int i = m_d->comboBox_plugins->currentIndex();
  if (i!=-1)
    pluginSelected(i);
}
