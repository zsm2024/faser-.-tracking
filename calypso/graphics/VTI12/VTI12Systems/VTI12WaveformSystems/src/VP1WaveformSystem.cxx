/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1WaveformSystem                          //
//                                                                     //
//  Author: Dave Casper <dcasper@uci.edu>                              //
//                                                                     //
//  Initial version: August 2021                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VTI12WaveformSystems/VP1WaveformSystem.h"
#include "VTI12WaveformSystems/WaveformSysController.h"

#include "VP1Base/VP1GraphicsItemCollection.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"

#include "StoreGate/StoreGateSvc.h"

#include "WaveRawEvent/RawWaveformContainer.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/SbVec3f.h>

#include <QtCharts>
#include <QtWidgets>
#include <QGraphicsEllipseItem>

using namespace QtCharts;

#include <map>

class VP1WaveformSystem::Imp {
public:
  Imp(VP1WaveformSystem*tc) : theclass(tc),
			       controller(0) {}
  VP1WaveformSystem *theclass;
  WaveformSysController * controller;
  std::vector<QChart*> vetoCharts;
  std::vector<QChart*> vetoNuCharts;
  std::vector<QChart*> triggerCharts;
  std::vector<QChart*> preshowerCharts;
  std::vector<QChart*> calorimeterCharts;
  std::vector<QChart*> clockCharts;
  std::vector<QChart*> testCharts;
  std::vector<QChart*> visibleCharts;
  QChart* createChart(const std::vector<unsigned int>& data, QGraphicsItem* parent = nullptr, const QString& title = "Default title");
  void createCharts(StoreGateSvc* sg, VP1GraphicsItemCollection* items, bool visible, std::vector<QChart*>& list, const std::string& collectionName);
  void getVisible(const std::vector<QChart*>& list);
  void getVisible();
  void layoutCharts();

};

//_____________________________________________________________________________________
VP1WaveformSystem::VP1WaveformSystem()
  : IVP12DSystemSimple("Raw Waveforms",
		       "System displaying raw waveform data",
		       "dcasper@uci.edu"), m_d(new Imp(this))
{
}


//_____________________________________________________________________________________
VP1WaveformSystem::~VP1WaveformSystem()
{
  delete m_d;
}

//_____________________________________________________________________________________
QWidget * VP1WaveformSystem::buildController()
{
  m_d->controller = new WaveformSysController(this);

  connect(m_d->controller,SIGNAL(vetoEnabledChanged(bool)),this,SLOT(updateVetoElements(bool)));
  connect(m_d->controller,SIGNAL(vetoNuEnabledChanged(bool)),this,SLOT(updateVetoNuElements(bool)));
  connect(m_d->controller,SIGNAL(triggerEnabledChanged(bool)),this,SLOT(updateTriggerElements(bool)));
  connect(m_d->controller,SIGNAL(preshowerEnabledChanged(bool)),this,SLOT(updatePreshowerElements(bool)));
  connect(m_d->controller,SIGNAL(calorimeterEnabledChanged(bool)),this,SLOT(updateCalorimeterElements(bool)));
  connect(m_d->controller,SIGNAL(clockEnabledChanged(bool)),this,SLOT(updateClockElements(bool)));
  connect(m_d->controller,SIGNAL(testEnabledChanged(bool)),this,SLOT(updateTestElements(bool)));

  return m_d->controller;
}

//_____________________________________________________________________________________
void VP1WaveformSystem::buildEventItemCollection(StoreGateSvc* sg, VP1GraphicsItemCollection* root)
{
  if(VP1Msg::verbose() || true){
    messageVerbose("buildEventItemCollection");
  }

  m_d->vetoCharts.clear();
  m_d->vetoNuCharts.clear();
  m_d->triggerCharts.clear();
  m_d->preshowerCharts.clear();
  m_d->calorimeterCharts.clear();
  m_d->clockCharts.clear();
  m_d->testCharts.clear();
//   m_d->nCharts = 0;

  m_d->createCharts(sg, root, m_d->controller->vetoEnabled(), m_d->vetoCharts, "VetoWaveforms");
  m_d->createCharts(sg, root, m_d->controller->vetoNuEnabled(), m_d->vetoNuCharts, "VetoNuWaveforms");
  m_d->createCharts(sg, root, m_d->controller->triggerEnabled(), m_d->triggerCharts, "TriggerWaveforms");
  m_d->createCharts(sg, root, m_d->controller->preshowerEnabled(), m_d->preshowerCharts, "PreshowerWaveforms");
  m_d->createCharts(sg, root, m_d->controller->calorimeterEnabled(), m_d->calorimeterCharts, "CaloWaveforms");
  m_d->createCharts(sg, root, m_d->controller->clockEnabled(), m_d->clockCharts, "ClockWaveforms");
  m_d->createCharts(sg, root, m_d->controller->testEnabled(), m_d->testCharts, "TestWaveforms");
  
  m_d->layoutCharts();
}

void VP1WaveformSystem::Imp::createCharts(StoreGateSvc* sg, 
                                          VP1GraphicsItemCollection* items, 
                                          bool visible, 
                                          std::vector<QChart*>& list, 
                                          const std::string& collectionName)
{
    typedef DataVector<RawWaveform>::const_iterator RawWaveformConstIterator;

    const RawWaveformContainer* p_container {nullptr};
    if(sg->retrieve(p_container, collectionName)==StatusCode::SUCCESS)
    {
      for(RawWaveformConstIterator i_wf=p_container->begin(); i_wf!=p_container->end(); ++i_wf)
      {
        if ((*i_wf)->n_samples() > 0)
        {
          QString title { collectionName.c_str() };
          title.append(" ").append(QString::number((*i_wf)->channel()));
          QChart* chart = createChart((*i_wf)->adc_counts(), nullptr, title);
          chart->setVisible(visible);
          list.push_back(chart);
          items->addItem(chart, true, true);
        }
      }
    }
    else
    {
      QString errorMsg {"Unable to retrieve "};
      theclass->message(errorMsg.append(collectionName.c_str()));
    }
}

QChart* VP1WaveformSystem::Imp::createChart(const std::vector<unsigned int>& data, QGraphicsItem* parent, const QString& title) 
{
  
  QChart* chart = new QChart(parent);
  QLineSeries *series = new QLineSeries();
  unsigned int maxEntry = 0;
  for (size_t i = 0; i < data.size(); i++)
  {
      maxEntry = std::max(maxEntry, data[i]);
      series->append(i, data[i]);
  }

  QPen pen(QRgb(0xfdb157));
  pen.setWidth(1);
  series->setPen(pen);
  chart->legend()->hide();
  chart->addSeries(series);

  QFont font;
  font.setPixelSize(24);
  chart->setTitleFont(font);
  chart->setTitleBrush(QBrush(Qt::blue));
  chart->setTitle(title);
  QValueAxis* axisX = new QValueAxis();
  QFont labelsFont;
  labelsFont.setPixelSize(8);
  QPen axisPen(QRgb(0xd18952));
  axisPen.setWidth(2);
  axisX->setRange(0, data.size());
  axisX->setTickCount(13);
  axisX->setLabelsFont(labelsFont);
  axisX->setLinePen(axisPen);
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);
  QValueAxis* axisY = new QValueAxis();
  axisY->setRange(0, 0x4000);
  axisY->setTickCount(9);
  axisY->setLabelsFont(labelsFont);
  axisY->setLinePen(axisPen);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  return chart;
}

void VP1WaveformSystem::Imp::getVisible()
{
    visibleCharts.clear();
    getVisible(vetoCharts);
    getVisible(vetoNuCharts);
    getVisible(triggerCharts);
    getVisible(preshowerCharts);
    getVisible(calorimeterCharts);
    getVisible(clockCharts);
    getVisible(testCharts);
}

void VP1WaveformSystem::Imp::getVisible(const std::vector<QChart*>& list)
{
    for (QChart* c : list)
    {
        if (c != nullptr && c->isVisible()) 
        {
            visibleCharts.push_back(c);
        }
    }
}

void VP1WaveformSystem::Imp::layoutCharts()
{
    getVisible();
    unsigned int nVisible = visibleCharts.size();
    std::cout << "Laying out " << nVisible << " raw hits" << std::endl;
    if (nVisible == 0) return;
    // unsigned int nRows = (nVisible - 1)/5 + 1;
    for (unsigned int i = 0; i < nVisible; i++)
    {
        qreal x = i%6 * 550.0;
        // qreal y = ( (i/5) - nRows ) * 550.0;
        qreal y = ( i/6 ) * 550.0;
        visibleCharts[i]->setGeometry(x, y, 500.0, 500.0);
    }
}

//_____________________________________________________________________________________
void VP1WaveformSystem::buildPermanentItemCollection(StoreGateSvc* /*detstore*/, VP1GraphicsItemCollection* /*root*/)
{
  if(VP1Msg::verbose() || true){
    messageVerbose("buildPermanentItemCollection");
  }

}

//_____________________________________________________________________________________
void VP1WaveformSystem::systemuncreate()
{
  m_d->controller=0;
}


//_____________________________________________________________________________________
QByteArray VP1WaveformSystem::saveState() {

  if (m_d->controller == nullptr) buildController();

  VP1Serialise serialise(0/*version*/,this);
  serialise.save(IVP12DSystemSimple::saveState());
  serialise.save(m_d->controller->saveSettings());
  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1WaveformSystem::restoreFromState(QByteArray ba) {

  VP1Deserialise state(ba,this);
  if (state.version() != 0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    state.disableUnrestoredChecks();//We do the testing in the controller
    return;
  }
//   if (state.version() > 0 &&state.version()<=3) {
//     message("Warning: State data in .vp1 file is in obsolete format - ignoring!");
//     state.disableUnrestoredChecks();//We do the testing in the controller
//     return;
//   }
  if (m_d->controller == nullptr) buildController();

  IVP12DSystemSimple::restoreFromState(state.restoreByteArray());
  m_d->controller->restoreSettings(state.restoreByteArray());
  state.disableUnrestoredChecks();//We do the testing in the controller
}

void VP1WaveformSystem::updateVetoElements(bool enabled)
{
    for (QChart* c : m_d->vetoCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformSystem::updateVetoNuElements(bool enabled)
{
    for (QChart* c : m_d->vetoNuCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformSystem::updateTriggerElements(bool enabled)
{
    for (QChart* c : m_d->triggerCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformSystem::updatePreshowerElements(bool enabled)
{
    for (QChart* c : m_d->preshowerCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformSystem::updateCalorimeterElements(bool enabled)
{
    for (QChart* c : m_d->calorimeterCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformSystem::updateClockElements(bool enabled)
{
    for (QChart* c : m_d->clockCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformSystem::updateTestElements(bool enabled)
{
    for (QChart* c : m_d->testCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}
