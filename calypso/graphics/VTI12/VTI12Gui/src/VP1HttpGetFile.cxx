/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1HttpGetFile                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12Gui/VP1HttpGetFile.h"
#include <QTimer>

#define VP1HttpGetFile_cxx
#include "VP1HttpGetFile_Private.h"
#undef VP1HttpGetFile_cxx

//____________________________________________________________________
class VP1HttpGetFile::Imp {
public:
  Imp() : maxDownloads(8), dead(false) {}
  unsigned maxDownloads;
  bool dead;
  QList<VP1HttpGetFile_DownloadThread *> activeDownloadThreads;
};

//____________________________________________________________________
VP1HttpGetFile::VP1HttpGetFile(QObject * parent)
  : QObject(parent), m_d(new Imp)
{
}

//____________________________________________________________________
VP1HttpGetFile::~VP1HttpGetFile()
{
  m_d->dead = true;
  foreach( VP1HttpGetFile_DownloadThread * thread, m_d->activeDownloadThreads) {
    thread->blockSignals(true);
    QString urltofile = thread->urltofile();
    QString localtargetfile = thread->localtargetfile();
    QString data = thread->data();
    emit downloadFailed( "VP1HttpGetFile object deleted before download finished", urltofile, localtargetfile, data );
    thread->terminate();
    thread->wait();
    thread->deleteLater();
  }
  delete m_d;
}

//____________________________________________________________________
void VP1HttpGetFile::setMaxNumberOfPendingDownloads(unsigned n)
{
  m_d->maxDownloads = n;
}

//____________________________________________________________________
unsigned VP1HttpGetFile::maxNumberOfPendingDownloads() const
{
  return m_d->maxDownloads;
}

//____________________________________________________________________
unsigned VP1HttpGetFile::numberOfPendingDownloads() const
{
  return m_d->activeDownloadThreads.size();
}

//____________________________________________________________________
QString VP1HttpGetFile::startDownload( const QString& urltofile,
				       const QString& localtargetfile,
				       const QString& md5sum,
				       const QString& data )
{
  if (m_d->maxDownloads>0 && unsigned(m_d->activeDownloadThreads.count()) >= m_d->maxDownloads)
    return "Too many simultaneous downloads requested";

  if (isDownloadingTo(localtargetfile))
    return "Already downloading file to location: "+localtargetfile;

  VP1HttpGetFile_DownloadThread * thread = new VP1HttpGetFile_DownloadThread( urltofile, localtargetfile, md5sum, data);
  connect(thread,SIGNAL(finished()),this,SLOT(downloadThreadFinished()));
  connect(thread,SIGNAL(terminated()),this,SLOT(downloadThreadTerminated()));
  m_d->activeDownloadThreads << thread;
  thread->start();

  return "";//Download started succesfully.
}

//____________________________________________________________________
void VP1HttpGetFile::downloadThreadFinished()
{
  if (m_d->dead)
    return;
  VP1HttpGetFile_DownloadThread * thread = dynamic_cast<VP1HttpGetFile_DownloadThread *>(sender());
  if (!thread)
    return;
  m_d->activeDownloadThreads.removeAll(thread);
  const QString urltofile = thread->urltofile();
  const QString localtargetfile = thread->localtargetfile();
  const QString data = thread->data();
  const bool errors = thread->errors();
  const QString errorString = thread->errorString();
  thread->wait();
  thread->deleteLater();
  if (errors)
    emit downloadFailed( errorString, urltofile, localtargetfile, data );
  else
    emit downloadSuccessful( urltofile, localtargetfile, data );
}

//____________________________________________________________________
void VP1HttpGetFile::downloadThreadTerminated()
{
  if (m_d->dead)
    return;
  VP1HttpGetFile_DownloadThread * thread = dynamic_cast<VP1HttpGetFile_DownloadThread *>(sender());
  if (!thread)
    return;
  m_d->activeDownloadThreads.removeAll(thread);
  const QString urltofile = thread->urltofile();
  const QString localtargetfile = thread->localtargetfile();
  const QString data = thread->data();
  thread->wait();
  thread->deleteLater();
  emit downloadFailed( "Download thread terminated", urltofile, localtargetfile, data );

  //NB: As always when threads are terminated, we might get memory
  //leaks, but it usually only happens if the VP1HttpGetFile instance
  //is deleted prematurely (e.g. at program shutdown).
}

//____________________________________________________________________
bool VP1HttpGetFile::isDownloading(const QString& urltofile) const
{
  foreach( VP1HttpGetFile_DownloadThread * thread, m_d->activeDownloadThreads)
    if (urltofile==thread->urltofile()) {
      QTimer::singleShot(0, thread, SLOT(checkForStall()));
      return true;
    }
  return false;
}

//____________________________________________________________________
bool VP1HttpGetFile::isDownloadingTo(const QString& localtargetfile) const
{
  foreach( VP1HttpGetFile_DownloadThread * thread, m_d->activeDownloadThreads)
    if (localtargetfile==thread->localtargetfile()) {
      QTimer::singleShot(0, thread, SLOT(checkForStall()));
      return true;
    }
  return false;
}
