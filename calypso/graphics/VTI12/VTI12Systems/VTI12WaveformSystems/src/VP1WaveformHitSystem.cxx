/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1WaveformHitSystem                       //
//                                                                     //
//  Author: Dave Casper <dcasper@uci.edu>                              //
//                                                                     //
//  Initial version: August 2021                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VTI12WaveformSystems/VP1WaveformHitSystem.h"
#include "VTI12WaveformSystems/WaveformSysController.h"

#include "VP1Base/VP1GraphicsItemCollection.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"

#include "StoreGate/StoreGateSvc.h"

#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHit.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/SbVec3f.h>

#include <QtCharts>
#include <QtWidgets>
#include <QGraphicsEllipseItem>

using namespace QtCharts;

#include <map>

class VP1WaveformHitSystem::Imp {
public:
  Imp(VP1WaveformHitSystem*tc) : theclass(tc),
			       controller(0) {}
  VP1WaveformHitSystem *theclass;
  WaveformSysController * controller;
  std::vector<QChart*> vetoCharts;
  std::vector<QChart*> vetoNuCharts;
  std::vector<QChart*> triggerCharts;
  std::vector<QChart*> preshowerCharts;
  std::vector<QChart*> calorimeterCharts;
  // std::vector<QChart*> clockCharts;
  std::vector<QChart*> testCharts;
  std::vector<QChart*> visibleCharts;
  void fillFit(const std::vector<float>& time, std::vector<float>& fit, float peak, float mean, float width, float alpha, float n);
  QChart* createChart(const std::vector<float>& time, const std::vector<float>& data, const std::vector<float>& spline, QGraphicsItem* parent = nullptr, const QString& title = "Default title");
  void createCharts(StoreGateSvc* sg, VP1GraphicsItemCollection* items, bool visible, std::vector<QChart*>& list, const std::string& collectionName);
  void getVisible(const std::vector<QChart*>& list);
  void getVisible();
  void layoutCharts();

};

//_____________________________________________________________________________________
VP1WaveformHitSystem::VP1WaveformHitSystem()
  : IVP12DSystemSimple("Waveform Hits",
		       "System displaying reconstructed waveform data",
		       "dcasper@uci.edu"), m_d(new Imp(this))
{
}


//_____________________________________________________________________________________
VP1WaveformHitSystem::~VP1WaveformHitSystem()
{
  delete m_d;
}

//_____________________________________________________________________________________
QWidget * VP1WaveformHitSystem::buildController()
{
  m_d->controller = new WaveformSysController(this);

  connect(m_d->controller,SIGNAL(vetoEnabledChanged(bool)),this,SLOT(updateVetoElements(bool)));
  connect(m_d->controller,SIGNAL(vetoNuEnabledChanged(bool)),this,SLOT(updateVetoNuElements(bool)));
  connect(m_d->controller,SIGNAL(triggerEnabledChanged(bool)),this,SLOT(updateTriggerElements(bool)));
  connect(m_d->controller,SIGNAL(preshowerEnabledChanged(bool)),this,SLOT(updatePreshowerElements(bool)));
  connect(m_d->controller,SIGNAL(calorimeterEnabledChanged(bool)),this,SLOT(updateCalorimeterElements(bool)));
  // connect(m_d->controller,SIGNAL(clockEnabledChanged(bool)),this,SLOT(updateClockElements(bool)));
  connect(m_d->controller,SIGNAL(testEnabledChanged(bool)),this,SLOT(updateTestElements(bool)));

  return m_d->controller;
}

//_____________________________________________________________________________________
void VP1WaveformHitSystem::buildEventItemCollection(StoreGateSvc* sg, VP1GraphicsItemCollection* root)
{
  if(VP1Msg::verbose() || true){
    messageVerbose("buildEventItemCollection");
  }

  m_d->vetoCharts.clear();
  m_d->vetoNuCharts.clear();
  m_d->triggerCharts.clear();
  m_d->preshowerCharts.clear();
  m_d->calorimeterCharts.clear();
  // m_d->clockCharts.clear();
  m_d->testCharts.clear();
//   m_d->nCharts = 0;

  m_d->createCharts(sg, root, m_d->controller->vetoEnabled(), m_d->vetoCharts, "VetoWaveformHits");
  m_d->createCharts(sg, root, m_d->controller->vetoNuEnabled(), m_d->vetoNuCharts, "VetoNuWaveformHits");
  m_d->createCharts(sg, root, m_d->controller->triggerEnabled(), m_d->triggerCharts, "TriggerWaveformHits");
  m_d->createCharts(sg, root, m_d->controller->preshowerEnabled(), m_d->preshowerCharts, "PreshowerWaveformHits");
  m_d->createCharts(sg, root, m_d->controller->calorimeterEnabled(), m_d->calorimeterCharts, "CaloWaveformHits");
  // m_d->createCharts(sg, root, m_d->controller->clockEnabled(), m_d->clockCharts, "ClockWaveforms");
  m_d->createCharts(sg, root, m_d->controller->testEnabled(), m_d->testCharts, "TestWaveformHits");
  
  m_d->layoutCharts();
}

void VP1WaveformHitSystem::Imp::createCharts(StoreGateSvc* sg, 
                                             VP1GraphicsItemCollection* items, 
                                             bool visible, 
                                             std::vector<QChart*>& list, 
                                             const std::string& collectionName)
{
    const xAOD::WaveformHitContainer* p_container {nullptr};
    if(sg->retrieve(p_container, collectionName)==StatusCode::SUCCESS)
    {
      for(xAOD::WaveformHitContainer::const_iterator i_wf=p_container->begin(); i_wf!=p_container->end(); ++i_wf)
      {
        const xAOD::WaveformHit* hit = (*i_wf);
        if (hit->time_vector().size() > 0 && hit->wave_vector().size() == hit->time_vector().size())
        {
          QString title { collectionName.c_str() };
          title.append(" ").append(QString::number(hit->channel()));

          std::vector<float> cb;
          if (!hit->status_bit(xAOD::WaveformStatus::CBFIT_FAILED) && hit->width() > 0 && hit->alpha() != 0 && hit->nval() != 0)
          {
            fillFit(hit->time_vector(), cb, hit->peak(), hit->mean(), hit->width(), hit->alpha(), hit->nval());
          }
          QChart* chart = createChart((*i_wf)->time_vector(), (*i_wf)->wave_vector(), cb, nullptr, title);

          QFont fitFont;
          fitFont.setPixelSize(18);

          QGraphicsTextItem* meanText = new QGraphicsTextItem("Mean: " + QString::number(hit->mean(),'f',1), chart);
          meanText->setFont(fitFont);
          meanText->setDefaultTextColor(QRgb(0xb157fd));
          meanText->setPos(290, 75);

          QGraphicsTextItem* peakText = new QGraphicsTextItem("Peak: " + QString::number(hit->peak(),'f',1), chart);
          peakText->setFont(fitFont);
          peakText->setDefaultTextColor(QRgb(0xb157fd));
          peakText->setPos(290, 100);

          QGraphicsTextItem* intText = new QGraphicsTextItem("Integral: " + QString::number(hit->integral(),'f',1), chart);
          intText->setFont(fitFont);
          intText->setDefaultTextColor(QRgb(0xb157fd));
          intText->setPos(290, 125);


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

void VP1WaveformHitSystem::Imp::fillFit(const std::vector<float>& time, 
                                        std::vector<float>& fit, 
                                        float peak, 
                                        float mean, 
                                        float width, 
                                        float alpha, 
                                        float nval)
{
    for (size_t i = 0; i < time.size(); i++)
    {
      float z = (time[i] - mean)/width;
      if (alpha < 0) z = -z;
      if (z > -fabs(alpha))
      {
        fit.push_back(peak * std::exp(-z*z/2));
      }
      else
      {
        fit.push_back(peak * std::pow((nval/fabs(alpha))/(((nval/fabs(alpha)-fabs(alpha)) - z)), nval) * std::exp(-alpha*alpha/2));
      }
    }
}

QChart* VP1WaveformHitSystem::Imp::createChart(const std::vector<float>& times, 
                                               const std::vector<float>& data, 
                                               const std::vector<float>& spline,
                                               QGraphicsItem* parent, const QString& title) 
{
  
  QChart* chart = new QChart(parent);
  QLineSeries *series = new QLineSeries();
  // unsigned int maxEntry = 0;
  for (size_t i = 0; i < data.size(); i++)
  {
      // maxEntry = std::max(maxEntry, data[i]);
      series->append(times[i], data[i]);
  }

  QPen pen(QRgb(0xfdb157));
  pen.setWidth(1);
  series->setPen(pen);
  chart->addSeries(series);

  if (spline.size() == times.size())
  {
    QSplineSeries* fit = new QSplineSeries();
    for (size_t i = 0; i < spline.size(); i++)
    {
      fit->append(times[i], spline[i]);
    }
    QPen fitPen(QRgb(0xb157fd));
    fitPen.setWidth(2);
    fit->setPen(fitPen);
    chart->addSeries(fit);
  }

  chart->legend()->hide();

  chart->createDefaultAxes();

  QFont font;
  font.setPixelSize(24);
  chart->setTitleFont(font);
  chart->setTitleBrush(QBrush(Qt::blue));
  chart->setTitle(title);
  // QValueAxis* axisX = new QValueAxis();
  QAbstractAxis* axisX = chart->axes(Qt::Horizontal)[0];
  QFont labelsFont;
  labelsFont.setPixelSize(8);
  QPen axisPen(QRgb(0xd18952));
  axisPen.setWidth(2);
  // axisX->setRange(times[0]-2.0, times[times.size()-1]+2.0);
  // axisX->setTickCount(13);
  axisX->setLabelsFont(labelsFont);
  axisX->setLinePen(axisPen);
  // chart->addAxis(axisX, Qt::AlignBottom);
  // series->attachAxis(axisX);
  // QValueAxis* axisY = new QValueAxis();
  QAbstractAxis* axisY = chart->axes(Qt::Vertical)[0];
  // axisY->setRange(-256, 0x4000);
  // axisY->setTickCount(17);
  axisY->setLabelsFont(labelsFont);
  axisY->setLinePen(axisPen);
  // chart->addAxis(axisY, Qt::AlignLeft);
  // series->attachAxis(axisY);

  return chart;
}

void VP1WaveformHitSystem::Imp::getVisible()
{
    visibleCharts.clear();
    getVisible(vetoNuCharts);
    getVisible(vetoCharts);
    getVisible(triggerCharts);
    getVisible(preshowerCharts);
    getVisible(calorimeterCharts);
    // getVisible(clockCharts);
    getVisible(testCharts);
}

void VP1WaveformHitSystem::Imp::getVisible(const std::vector<QChart*>& list)
{
    for (QChart* c : list)
    {
        if (c != nullptr && c->isVisible()) 
        {
            visibleCharts.push_back(c);
        }
    }
}

void VP1WaveformHitSystem::Imp::layoutCharts()
{
    getVisible();
    unsigned int nVisible = visibleCharts.size();
    if (nVisible == 0) return;
    // unsigned int nRows = (nVisible - 1)/5 + 1;
    for (unsigned int i = 0; i < nVisible; i++)
    {
        qreal x = i%6 * 550.0;
        // qreal y = ( (i/5) - nRows ) * 550.0;
        qreal y = ( i/6 ) * 550.0 + 550.0*2;
        visibleCharts[i]->setGeometry(x, y, 500.0, 500.0);
    }
}

//_____________________________________________________________________________________
void VP1WaveformHitSystem::buildPermanentItemCollection(StoreGateSvc* /*detstore*/, VP1GraphicsItemCollection* /*root*/)
{
  if(VP1Msg::verbose() || true){
    messageVerbose("buildPermanentItemCollection");
  }

}

//_____________________________________________________________________________________
void VP1WaveformHitSystem::systemuncreate()
{
  m_d->controller=0;
}


//_____________________________________________________________________________________
QByteArray VP1WaveformHitSystem::saveState() {

  if (m_d->controller == nullptr) buildController();

  VP1Serialise serialise(0/*version*/,this);
  serialise.save(IVP12DSystemSimple::saveState());
  serialise.save(m_d->controller->saveSettings());
  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1WaveformHitSystem::restoreFromState(QByteArray ba) {

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

void VP1WaveformHitSystem::updateVetoElements(bool enabled)
{
    for (QChart* c : m_d->vetoCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformHitSystem::updateVetoNuElements(bool enabled)
{
    for (QChart* c : m_d->vetoNuCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}


void VP1WaveformHitSystem::updateTriggerElements(bool enabled)
{
    for (QChart* c : m_d->triggerCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformHitSystem::updatePreshowerElements(bool enabled)
{
    for (QChart* c : m_d->preshowerCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

void VP1WaveformHitSystem::updateCalorimeterElements(bool enabled)
{
    for (QChart* c : m_d->calorimeterCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}

// void VP1WaveformHitSystem::updateClockElements(bool enabled)
// {
//     for (QChart* c : m_d->clockCharts)
//     {
//         c->setVisible(enabled);
//     }
//     m_d->layoutCharts();
// }

void VP1WaveformHitSystem::updateTestElements(bool enabled)
{
    for (QChart* c : m_d->testCharts)
    {
        c->setVisible(enabled);
    }
    m_d->layoutCharts();
}
