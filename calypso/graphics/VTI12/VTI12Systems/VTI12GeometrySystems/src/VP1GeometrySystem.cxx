/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1GeometrySystem                          //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Derived from V-atlas geometry system by Joe Boudreau.              //
//  Origins of initial version dates back to ~1996, initial VP1        //
//  version by TK (May 2007) and almost entirely rewritten Oct 2007.   //
//  Major refactoring october 2008.                                    //
//  Updates:                                                           //
//  - Aug 2019, Riccardo.Maria.Bianchi@cern.ch                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VTI12GeometrySystems/VP1GeometrySystem.h"
#include "VTI12GeometrySystems/GeoSysController.h"

#include "VTI12GeometrySystems/VP1GeoTreeView.h"
#include "VTI12GeometrySystems/VolumeHandle.h"
#include "VTI12GeometrySystems/VolumeHandleSharedData.h"
#include "VTI12GeometrySystems/VolumeTreeModel.h"
#include "VTI12GeometrySystems/VP1GeomUtils.h"
#include "VTI12GeometrySystems/VisAttributes.h"
#include "VTI12GeometrySystems/DumpShape.h"
#include "VTI12GeometrySystems/PhiSectorManager.h"

#include "VTI12Utils/VP1LinAlgUtils.h"

#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoFont.h>

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoPrintGraphAction.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"

#ifndef BUILDVP1LIGHT
#include "VTI12Utils/VP1JobConfigInfo.h"
#include "VTI12Utils/VP1SGAccessHelper.h"
#include "VTI12Utils/VP1DetInfo.h"
#endif

#include <QDebug>
#include <QRegExp>
#include <QByteArray>
#include <QTimer>
#include <QHeaderView>
#include <QApplication>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileInfo>

#include <map>

class VP1GeometrySystem::Imp {
public:
  Imp (VP1GeometrySystem*gs, const VP1GeoFlags::SubSystemFlags& ssf)
    : theclass(gs), sceneroot(0),
      detVisAttributes(0), matVisAttributes(0), volVisAttributes(0),
      initialSubSystemsTurnedOn(ssf),controller(0),phisectormanager(0),
      volumetreemodel(0),kbEvent(0), m_textSep(0)
  {
    const unsigned n_chamber_t0_sources=2;
    for (unsigned i=0;i<n_chamber_t0_sources;++i)
      chamberT0s.append(0);
  }

  VP1GeometrySystem * theclass;
  SoSeparator * sceneroot;

  std::map<SoSeparator*,VolumeHandle*> sonodesep2volhandle;

  class SubSystemInfo {
  public:
    // "geomodellocation" contains name of tree tops, 
    SubSystemInfo( QCheckBox* cb,const QRegExp& the_geomodeltreetopregexp, bool the_negatetreetopregexp,
		  const QRegExp& the_geomodelchildrenregexp, bool the_negatechildrenregexp, VP1GeoFlags::SubSystemFlag the_flag,
		  const std::string& the_matname,
		  const QRegExp& the_geomodelgrandchildrenregexp, bool the_negategrandchildrenregexp=false)
      : isbuilt(false), checkbox(cb),
        geomodeltreetopregexp(the_geomodeltreetopregexp),
        geomodelchildrenregexp(the_geomodelchildrenregexp),
        geomodelgrandchildrenregexp(the_geomodelgrandchildrenregexp),
        negatetreetopregexp(the_negatetreetopregexp),
        negatechildrenregexp(the_negatechildrenregexp),
        negategrandchildrenregexp(the_negategrandchildrenregexp),
        matname(the_matname), flag(the_flag), soswitch(0)
    {

    }

    //Info needed to define the system (along with the checkbox pointer in the map below):
    bool isbuilt;
    VolumeHandle::VolumeHandleList vollist;
    QCheckBox* checkbox;

    /* Regular Expressions for three levels of filtering: treetop, children, and grandchildren
     *
     * For example:
     *
     * - Muon                --> treetop volume
     * -    ANON             --> child volume
     * -        BAR_Toroid   --> granchild volume
     *
     */
    QRegExp geomodeltreetopregexp;      //For picking the geomodel treetops
    QRegExp geomodelchildrenregexp;     //If instead of the treetops, this system consists of volumes below the treetop, this is non-empty.
    QRegExp geomodelgrandchildrenregexp;//If instead of the treetops, this system consists of volumes below the child of a treetop, this is non-empty.
    bool negatetreetopregexp;
    bool negatechildrenregexp;
    bool negategrandchildrenregexp;


    std::string matname; //if nonempty, use this from detvisattr instead of the top volname.
    VP1GeoFlags::SubSystemFlag flag;


    //Information needed to initialise the system:
    class TreetopInfo { public:
      TreetopInfo() {}
      PVConstLink pV;
      HepGeom::Transform3D xf;// = av.getTransform();
      std::string volname;
    };
    std::vector<TreetopInfo> treetopinfo;

    //Switch associated with the system - it is initialised only if the system has info available:
    SoSwitch * soswitch;

    bool childrenRegExpNameCompatible(const std::string& volname) const {
      return negatechildrenregexp!=geomodelchildrenregexp.exactMatch(volname.c_str());
    }

    bool grandchildrenRegExpNameCompatible(const std::string& volname) const {
        if(VP1Msg::debug()){
            std::cout << "volname: " << volname << " - regexpr: " << geomodelgrandchildrenregexp.pattern().toStdString() << std::endl;
            std::cout << "negategrandchildrenregexp: " << negategrandchildrenregexp << std::endl;
        }
        return negategrandchildrenregexp!=geomodelgrandchildrenregexp.exactMatch(volname.c_str());
    }
    
    void dump() const {
      std::cout<<" SubSystemInfo @ "<<this<<"\n" 
               <<(isbuilt?"Is built.\n":"Is not built.\n"); 
      std::cout<<"Contains following volhandles: [";
      for (auto vol : vollist) std::cout<<&vol<<",";
      std::cout<<"]"<<std::endl;
      std::cout<<"Matname = "<<matname<<std::endl;
      std::cout<<"Contains following TreetopInfo: [";
      for (auto tt : treetopinfo) std::cout<<tt.volname<<",";
      std::cout<<"]"<<std::endl;
      
    }

  };

  QList<SubSystemInfo*> subsysInfoList;//We need to keep and ordered version also (since wildcards in regexp might match more than one subsystem info).
  void addSubSystem(const VP1GeoFlags::SubSystemFlag&,
		    const QString& treetopregexp, const QString& childrenregexp="",
		    const std::string& matname="", bool negatetreetopregexp = false, bool negatechildrenregexp = false,
		    const QString& grandchildrenregexp="", bool negategrandchildrenregexp = false);

  DetVisAttributes *detVisAttributes {nullptr};
  MatVisAttributes *matVisAttributes {nullptr};
  VolVisAttributes *volVisAttributes {nullptr};
  void ensureInitVisAttributes() {
    if (!detVisAttributes) detVisAttributes = new DetVisAttributes();
    if (!matVisAttributes) matVisAttributes = new MatVisAttributes();
    if (!volVisAttributes) volVisAttributes = new VolVisAttributes();
  }

  VP1GeoFlags::SubSystemFlags initialSubSystemsTurnedOn;
  void buildSystem(SubSystemInfo*);
  GeoSysController * controller;
  PhiSectorManager * phisectormanager;
  VolumeTreeModel * volumetreemodel;

  //Helpers used for printouts://FIXME: To VolumeHandle!!
  static double exclusiveMass(const PVConstLink& pv);
  static double inclusiveMass(const PVConstLink& pv);
  static double volume(const PVConstLink& pv);

  //Basically for the zapping, and to ignore all clicks with ctrl/shift:
  static void catchKbdState(void *userData, SoEventCallback *CB);
  const SoKeyboardEvent *kbEvent;

  void changeStateOfVisibleNonStandardVolumesRecursively(VolumeHandle*,VP1GeoFlags::VOLSTATE);
  void expandVisibleVolumesRecursively(VolumeHandle*,const QRegExp&,bool bymatname);

  SoSeparator* m_textSep;//!< Separator used to hold all visible labels.

  // void showPixelModules(VolumeHandle*);
  // void showSCTBarrelModules(VolumeHandle*);
  // void showSCTEndcapModules(VolumeHandle*);

  QMap<quint32,QByteArray> restoredTopvolstates;
  void applyTopVolStates(const QMap<quint32,QByteArray>&, bool disablenotif = false);

  // Helper function for emiting a signal to the PartSpect system 
  // This function creates path entry prefixes (=Detector Factory name in G4) and extra
  // path entries (top level volumes, python envelopes) depending on the subsystem of the selected volume
  void createPathExtras(const VolumeHandle*, QString&, QStack<QString>&);
  
  QList<const std::map<GeoPVConstLink, float>*> chamberT0s;  
};

//_____________________________________________________________________________________
VP1GeometrySystem::VP1GeometrySystem( const VP1GeoFlags::SubSystemFlags& SubSystemsTurnedOn, QString name )
  : IVP13DSystemSimple(name,
		   "This system displays the geometry as defined in the GeoModel tree.",
           "Riccardo.Maria.Bianchi@cern.ch"),
    m_d(new Imp(this,SubSystemsTurnedOn))
{
}


//_____________________________________________________________________________________
void VP1GeometrySystem::systemuncreate()
{

  m_d->volumetreemodel->cleanup();
  delete m_d->matVisAttributes; m_d->matVisAttributes = 0;
  delete m_d->detVisAttributes; m_d->detVisAttributes = 0;
  delete m_d->volVisAttributes; m_d->volVisAttributes = 0;

  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList)
    delete subsys;
  m_d->subsysInfoList.clear();

}

//_____________________________________________________________________________________
VP1GeometrySystem::~VP1GeometrySystem()
{
  delete m_d;
  m_d = 0;
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setGeometrySelectable(bool b) {
   ensureBuildController();
   m_d->controller->setGeometrySelectable(b);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setZoomToVolumeOnClick(bool b) {
   ensureBuildController();
   m_d->controller->setZoomToVolumeOnClick(b);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::addSubSystem(const VP1GeoFlags::SubSystemFlag& f,
					  const QString& treetopregexp, const QString& childrenregexp,
					  const std::string& matname, bool negatetreetopregexp, bool negatechildrenregexp,
					  const QString& grandchildrenregexp, bool negategrandchildrenregexp)
{
  std::stringstream sstream;
  sstream << std::hex << f;
  std::string fHex = sstream.str();
  theclass->message("VP1GeometrySystem::Imp::addSubSystem - flag: '0x" + str(fHex.c_str()) + "' - matName: '" + str(matname.c_str()) + "'." );
  
  QCheckBox * cb = controller->subSystemCheckBox(f);
  if (!cb) {
    theclass->message("Error: Problems retrieving checkbox for subsystem "+str(f));
    return;
  }
  subsysInfoList << new SubSystemInfo(cb,QRegExp(treetopregexp),negatetreetopregexp,
				      QRegExp(childrenregexp),negatechildrenregexp,f,matname,
				      QRegExp(grandchildrenregexp), negategrandchildrenregexp);
  //FIXME: DELETE!!!
}

//_____________________________________________________________________________________
QWidget * VP1GeometrySystem::buildController()
{
  message("VP1GeometrySystem::buildController");
  
  m_d->controller = new GeoSysController(this);

  m_d->phisectormanager = new PhiSectorManager(m_d->controller->phiSectionWidget(),this,this);


  connect(m_d->controller,SIGNAL(showVolumeOutLinesChanged(bool)),this,SLOT(setShowVolumeOutLines(bool)));
  setShowVolumeOutLines(m_d->controller->showVolumeOutLines());
  connect(m_d->controller,SIGNAL(saveMaterialsToFile(QString,bool)),this,SLOT(saveMaterialsToFile(QString,bool)));
  connect(m_d->controller,SIGNAL(loadMaterialsFromFile(QString)),this,SLOT(loadMaterialsFromFile(QString)));

  connect(m_d->controller,SIGNAL(transparencyChanged(float)),this,SLOT(updateTransparency()));
  connect (m_d->controller,SIGNAL(volumeStateChangeRequested(VolumeHandle*,VP1GeoFlags::VOLSTATE)),
	   this,SLOT(volumeStateChangeRequested(VolumeHandle*,VP1GeoFlags::VOLSTATE)));
  connect (m_d->controller,SIGNAL(volumeResetRequested(VolumeHandle*)),
	   this,SLOT(volumeResetRequested(VolumeHandle*)));
  connect(m_d->controller,SIGNAL(autoExpandByVolumeOrMaterialName(bool,QString)),this,SLOT(autoExpandByVolumeOrMaterialName(bool,QString)));
  connect(m_d->controller,SIGNAL(actionOnAllNonStandardVolumes(bool)),this,SLOT(actionOnAllNonStandardVolumes(bool)));
  // connect(m_d->controller,SIGNAL(autoAdaptPixelsOrSCT(bool,bool,bool,bool,bool,bool)),this,SLOT(autoAdaptPixelsOrSCT(bool,bool,bool,bool,bool,bool)));
  connect(m_d->controller,SIGNAL(resetSubSystems(VP1GeoFlags::SubSystemFlags)),this,SLOT(resetSubSystems(VP1GeoFlags::SubSystemFlags)));

  connect(m_d->controller,SIGNAL(labelsChanged(int)),this,SLOT(setLabels(int)));
  connect(m_d->controller,SIGNAL(labelPosOffsetChanged(QList<int>)),this,SLOT(setLabelPosOffsets(QList<int>)));
  setLabels(m_d->controller->labels());
  

  /* This is where we define the available different subsystems and their location in the geomodel tree.
   *
   * if the reg expr does not match any volume, the corresponding subsystem checkbox in the Geo GUI gets disabled.
   *
   * syntax: addSubSystem(VP1GeoFlags::SubSystemFlag&, // the associated system flag
		                  QString& treetopregexp,                // the regular expr for the top/root name of the main sub-detector system
		                  QString& childrenregexp="",            // the reg expr for the children of the main sub-detector
		                  std::string& matname="",               // a name we choose for displaying in VP1
		                  bool negatetreetopregexp = false,      // if we want to negate the top reg expr
		                  bool negatechildrenregexp = false);    // if we want to negate the children reg expr
                          const QString& grandchildrenregexp="", // the regex for granchildren of the main sub-detector
                          bool negategrandchildrenregexp = false // wheter we want to negate teh granchildren regex
   */
    m_d->addSubSystem( VP1GeoFlags::Emulsion, "Emulsion");
    m_d->addSubSystem( VP1GeoFlags::VetoNu,   "VetoNu");
    m_d->addSubSystem( VP1GeoFlags::Veto,     "Veto");
    m_d->addSubSystem( VP1GeoFlags::Trigger,  "Trigger");
    m_d->addSubSystem( VP1GeoFlags::Preshower,"Preshower");
    m_d->addSubSystem( VP1GeoFlags::SCT,      "SCT");
    m_d->addSubSystem( VP1GeoFlags::Dipole,   "Dipole");
    m_d->addSubSystem( VP1GeoFlags::Ecal,     "Ecal");
    m_d->addSubSystem( VP1GeoFlags::CavernInfra, "Cavern");



  //This one MUST be added last. It will get slightly special treatment in various places!
  m_d->addSubSystem( VP1GeoFlags::AllUnrecognisedVolumes,".*");


  //Setup models/views for volume tree browser and zapped volumes list:
  m_d->volumetreemodel = new VolumeTreeModel(m_d->controller->volumeTreeBrowser());
  m_d->controller->volumeTreeBrowser()->header()->hide();
  m_d->controller->volumeTreeBrowser()->uniformRowHeights();
  m_d->controller->volumeTreeBrowser()->setModel(m_d->volumetreemodel);

  return m_d->controller;
}


//_____________________________________________________________________________________
void VP1GeometrySystem::systemcreate(StoreGateSvc*)
{
  m_d->ensureInitVisAttributes();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::catchKbdState(void *address, SoEventCallback *CB) {
  VP1GeometrySystem::Imp *This=static_cast<VP1GeometrySystem::Imp*>(address);
  if (This)
    This->kbEvent = static_cast<const SoKeyboardEvent *>(CB->getEvent());
}

//_____________________________________________________________________________________
void VP1GeometrySystem::buildPermanentSceneGraph(StoreGateSvc*/*detstore*/, SoSeparator *root)
{
  m_d->sceneroot = root;

    #ifndef BUILDVP1LIGHT
      if (!VP1JobConfigInfo::hasGeoModelExperiment()) {
        message("Error: GeoModel not configured properly in job.");
        return;
      }

      //Get the world volume:
      const GeoModelExperiment * theExpt;
      if (!VP1SGAccessHelper(this,true).retrieve(theExpt,"FASER")) {
        message("Error: Could not retrieve the FASER GeoModelExperiment from detector store");
        return;
      }
    #endif

  PVConstLink world(theExpt->getPhysVol());

  if (!m_d->m_textSep) {
    // FIXME!
    //    std::cout<<"Making new Text sep"<<std::endl;
    m_d->m_textSep = new SoSeparator;
    m_d->m_textSep->setName("TextSep");
    m_d->m_textSep->ref();
  }
  m_d->sceneroot->addChild(m_d->m_textSep);
  
  // FIXME - what if font is missing?
  SoFont *myFont = new SoFont;
  myFont->name.setValue("Arial");
  myFont->size.setValue(12.0);
  m_d->m_textSep->addChild(myFont);
  
  bool save = root->enableNotify(false);

  //Catch keyboard events:
  SoEventCallback *catchEvents = new SoEventCallback();
  catchEvents->addEventCallback(SoKeyboardEvent::getClassTypeId(),Imp::catchKbdState, m_d);
  root->addChild(catchEvents);

  root->addChild(m_d->controller->drawOptions());
  root->addChild(m_d->controller->pickStyle());

  if(VP1Msg::debug()){
    qDebug() << "Configuring the default systems... - subsysInfoList len:" << (m_d->subsysInfoList).length();
  }
  // we switch on the systems flagged to be turned on at start
  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList)
  {
	VP1Msg::messageDebug("Switching on this system: " + QString::fromStdString(subsys->matname) + " - " + subsys->flag);
    bool on(m_d->initialSubSystemsTurnedOn & subsys->flag);
    subsys->checkbox->setChecked( on );
    subsys->checkbox->setEnabled(false);
    subsys->checkbox->setToolTip("This sub-system is not available");
    connect(subsys->checkbox,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
   }

  //Locate geometry info for the various subsystems, and add the info as appropriate:

  QCheckBox * checkBoxOther = m_d->controller->subSystemCheckBox(VP1GeoFlags::AllUnrecognisedVolumes);

  if(VP1Msg::debug()){
    qDebug() << "Looping on volumes from the input GeoModel...";
  }
  GeoVolumeCursor av(world);
  while (!av.atEnd()) {

	  std::string name = av.getName();
    if(VP1Msg::debug()){
      qDebug() << "volume name:" << QString::fromStdString(name);
    }

	  // DEBUG
	  VP1Msg::messageDebug("DEBUG: Found GeoModel treetop: "+QString(name.c_str()));

	  //Let us see if we recognize this volume:
	  bool found = false;
	  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList) {
		  if (subsys->negatetreetopregexp!=subsys->geomodeltreetopregexp.exactMatch(name.c_str()))
		  {
			  if (subsys->checkbox==checkBoxOther&&found) {
				  continue;//The "other" subsystem has a wildcard which matches everything - but we only want stuff that is nowhere else.
			  }

              if(VP1Msg::debug()){
                qDebug() << (subsys->geomodeltreetopregexp).pattern() << subsys->geomodeltreetopregexp.exactMatch(name.c_str()) << subsys->negatetreetopregexp;
                qDebug() << "setting 'found' to TRUE for pattern:" << (subsys->geomodeltreetopregexp).pattern();
              }
              found = true;
			  //We did... now, time to extract info:
			  subsys->treetopinfo.resize(subsys->treetopinfo.size()+1);
			  subsys->treetopinfo.back().pV = av.getVolume();
			  subsys->treetopinfo.back().xf = Amg::EigenTransformToCLHEP(av.getTransform());
			  subsys->treetopinfo.back().volname = av.getName();

			  //Add a switch for this system (turned off for now):
			  SoSwitch * sw = new SoSwitch();
			  //But add a separator on top of it (for caching):
			  subsys->soswitch = sw;
			  if (sw->whichChild.getValue() != SO_SWITCH_NONE)
				  sw->whichChild = SO_SWITCH_NONE;
			  SoSeparator * sep = new SoSeparator;
			  sep->addChild(sw);
			  root->addChild(sep);
			  //Enable the corresponding checkbox:
			  subsys->checkbox->setEnabled(true);
			  subsys->checkbox->setToolTip("Toggle the display of the "+subsys->checkbox->text()+" sub system");
			  //NB: Dont break here - several systems might share same treetop!
			  //	break;
		  }
	  }
	  if (!found) {
		  message("Warning: Found unexpected GeoModel treetop: "+QString(name.c_str()));
	  }

	  av.next(); // increment volume cursor.
  }

  //Hide other cb if not needed:
  if (!checkBoxOther->isEnabled())
    checkBoxOther->setVisible(false);

  //Build the geometry for those (available) subsystems that starts out being turned on:
  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList) {
    if (!subsys->soswitch||!subsys->checkbox->isChecked())
      continue;
    m_d->buildSystem(subsys);
    //Enable in 3D view:
    if (subsys->soswitch->whichChild.getValue() != SO_SWITCH_ALL)
      subsys->soswitch->whichChild = SO_SWITCH_ALL;
    //Enable in tree browser:
    m_d->volumetreemodel->enableSubSystem(subsys->flag);
    //     new ModelTest(m_d->volumetreemodel, m_d->treeView_volumebrowser);
  }
  if (!m_d->restoredTopvolstates.isEmpty()) {
    m_d->applyTopVolStates(m_d->restoredTopvolstates,false);
    m_d->restoredTopvolstates.clear();
  }
  m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  root->enableNotify(save);
  if (save)
    root->touch();

}

//_____________________________________________________________________________________
void VP1GeometrySystem::checkboxChanged()
{
  QCheckBox * cb = static_cast<QCheckBox*>(sender());
  Imp::SubSystemInfo * subsys(0);
  foreach (Imp::SubSystemInfo * ss, m_d->subsysInfoList) {
    if (cb==ss->checkbox) {
      subsys=ss;
      break;
    }
  }
  if (!subsys) {
    message("ERROR: Unknown checkbox");
    return;
  }
  SoSwitch * sw = subsys->soswitch;
  assert(sw);
  if (cb->isChecked()) {
    SbBool save = sw->enableNotify(false);
    if (sw->getNumChildren()==0) {
      m_d->buildSystem(subsys);
      m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();//
    }
    if (sw->whichChild.getValue() != SO_SWITCH_ALL)
      sw->whichChild = SO_SWITCH_ALL;
    sw->enableNotify(save);
    //Enable in tree browser:
    m_d->volumetreemodel->enableSubSystem(subsys->flag);
    //     new ModelTest(m_d->volumetreemodel, m_d->treeView_volumebrowser);
    if (save)
      sw->touch();
  } else {
    if (sw->whichChild.getValue() != SO_SWITCH_NONE)
      sw->whichChild = SO_SWITCH_NONE;
    m_d->volumetreemodel->disableSubSystem(subsys->flag);
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::userPickedNode(SoNode* , SoPath *pickedPath)
{

  //////////////////////////////////////////////////////////////////////////
  //  We want to find the volumehandle for the volume. To do so, we look  //
  //  for the SoSeparator identifying the actual picked shape node, and   //
  //  use it to look up the handle:                                       //
  //////////////////////////////////////////////////////////////////////////

  //Looking for the identifying "nodesep", there are three scenarios
  //for the type signatures of the final part of the path:
  //
  // 1) Most shapes:
  //    nodesep -> pickedNode (but we must pop to a group node in case of SoCylinders)
  //
  // 2) Volumes Around Z (all phi sectors enabled):
  //    nodesep -> switch -> pickedNode
  //
  // 3) Volumes Around Z (only some phi sectors enabled):
  //    nodesep -> switch -> sep -> pickedNode
  //
  // In the third scenario we also have to pop the path, in order for
  // all phi-slices of the part gets highlighted (since more than one
  // soshape node represents the volume).

  // std::cout << "Entering userPickedNode" << std::endl;
	VP1Msg::messageDebug("VP1GeometrySystem::userPickedNode()");

  if (pickedPath->getNodeFromTail(0)->getTypeId()==SoCylinder::getClassTypeId())
    pickedPath->pop();

  if (pickedPath->getLength()<5) {
    message("Path too short");
    return;
  }

  SoSeparator * nodesep(0);

  if (pickedPath->getNodeFromTail(1)->getTypeId()==SoSeparator::getClassTypeId()
      && pickedPath->getNodeFromTail(2)->getTypeId()==SoSwitch::getClassTypeId()
      && pickedPath->getNodeFromTail(3)->getTypeId()==SoSeparator::getClassTypeId()) 
  {
    //Scenario 3:
    nodesep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(3));
    pickedPath->pop();//To get highlighting of siblings also.
  } 
  else if (pickedPath->getNodeFromTail(1)->getTypeId()==SoSwitch::getClassTypeId() 
      && pickedPath->getNodeFromTail(2)->getTypeId()==SoSeparator::getClassTypeId()) 
  {
    //Scenario 2:
    nodesep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(2));
  } 
  else if (pickedPath->getNodeFromTail(1)->getTypeId()==SoSeparator::getClassTypeId()) {
    //Scenario 1 (normal):
    nodesep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(1));
  }
  if (!nodesep) {
    message("Unexpected picked path");
    return;
  }
  if ( (!(nodesep)) || (m_d->sonodesep2volhandle.find(nodesep) == m_d->sonodesep2volhandle.end()) ) {
    message("Problems finding volume handle");
    return;
  }
  VolumeHandle * volhandle = m_d->sonodesep2volhandle[nodesep];
  if (!volhandle) {
    message("Found NULL volume handle");
    return;
  }

  /////////////////////////////////////////////////////////////////////////
  //  Next thing to do is to check whether volume was clicked on with a  //
  //  modifier of SHIFT/CTRL/Z. If so, we have to change the state on    //
  //  the volume handle. Otherwise, we need to print some information:   //
  /////////////////////////////////////////////////////////////////////////

  //For focus reason, and since Qt doesn't allow standard keys such as
  //'z' as modifiers, we check for keypress states using a combination
  //of the inventor and Qt way

  bool shift_isdown = (Qt::ShiftModifier & QApplication::keyboardModifiers());
// 		       || ( m_d->kbEvent && (SO_KEY_PRESS_EVENT(m_d->kbEvent, SoKeyboardEvent::LEFT_SHIFT)||
// 					   SO_KEY_PRESS_EVENT(m_d->kbEvent, SoKeyboardEvent::RIGHT_SHIFT)) ) );

  if (shift_isdown) {
    //Parent of volume should be put in CONTRACTED state.
    deselectAll();
    if (volhandle->parent())
      volhandle->parent()->setState(VP1GeoFlags::CONTRACTED);
    m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
    return;
  }

  bool ctrl_isdown = (Qt::ControlModifier & QApplication::keyboardModifiers());
// 		       || ( m_d->kbEvent && (SO_KEY_PRESS_EVENT(m_d->kbEvent, SoKeyboardEvent::LEFT_CONTROL)||
// 					   SO_KEY_PRESS_EVENT(m_d->kbEvent, SoKeyboardEvent::RIGHT_CONTROL)) ) );

  if (ctrl_isdown) {
    //Volume should be put in EXPANDED state if it has children.
    message("Calling deselectAll");
    deselectAll();
    message("Back from deselectAll");
    if (volhandle->nChildren()>0) {
      message("Calling volhandle->setState");
      volhandle->setState(VP1GeoFlags::EXPANDED);
    }
    message("calling phisectormanager");
    m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
    message("leaving ctrl_isdown");
    return;
  }

  bool z_isdown =  m_d->kbEvent && SO_KEY_PRESS_EVENT(m_d->kbEvent,SoKeyboardEvent::Z);
  if (z_isdown) {
    //Volume should be put in ZAPPED state.
    deselectAll();
    volhandle->setState(VP1GeoFlags::ZAPPED);
    message("===> Zapping Node: "+volhandle->getName());
    // std::cout<<"Zapped VH="<<volhandle<<std::endl;
    return;
  }


  bool orientedView(false);

  //////////////////////
  //  Zoom to volume  //
  //////////////////////

  //Nb: We don't do this if we already oriented to the muon chamber above.
  if (!orientedView&&m_d->controller->zoomToVolumeOnClick()) {
    if (m_d->sceneroot&&volhandle->nodeSoSeparator()) {
      std::set<SoCamera*> cameras = getCameraList();
      std::set<SoCamera*>::iterator it,itE = cameras.end();
      for (it=cameras.begin();it!=itE;++it) {
	    VP1CameraHelper::animatedZoomToSubTree(*it,m_d->sceneroot,volhandle->nodeSoSeparator(),2.0,1.0);
      }
    }
  }


  ///////////////////////////////////
  //  Update last-select controls  //
  ///////////////////////////////////

  m_d->controller->setLastSelectedVolume(volhandle);

  /////////////////////////////////////////////////////////
  //  OK, time to print some information for the volume  //
  /////////////////////////////////////////////////////////

  message("===> Selected Node: "+volhandle->getName());
  // std::cout<<"VolHandle = "<<volhandle<<std::endl;
  if (m_d->controller->printInfoOnClick_Shape()) {
    foreach (QString str, DumpShape::shapeToStringList(volhandle->geoPVConstLink()->getLogVol()->getShape()))
      message(str);
  }

  if (m_d->controller->printInfoOnClick_Material()) {
    message("===> Material:");
    foreach (QString line, VP1GeomUtils::geoMaterialToStringList(volhandle->geoMaterial()))
      message("     "+line);
  }

  if ( m_d->controller->printInfoOnClick_CopyNumber() ) {
    int cn = volhandle->copyNumber();
    message("===> CopyNo : "+(cn>=0?QString::number(cn):QString(cn==-1?"Invalid":"Error reconstructing copynumber")));
  }

  if ( m_d->controller->printInfoOnClick_Transform() ) {

    float translation_x, translation_y, translation_z, rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians;
    VP1LinAlgUtils::decodeTransformation( volhandle->getLocalTransformToVolume(),
					  translation_x, translation_y, translation_z,
					  rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians );
    message("===> Local Translation:");
    message("        x = "+QString::number(translation_x/CLHEP::mm)+" mm");
    message("        y = "+QString::number(translation_y/CLHEP::mm)+" mm");
    message("        z = "+QString::number(translation_z/CLHEP::mm)+" mm");
    message("===> Local Rotation:");
    message("        axis x = "+QString::number(rotaxis_x));
    message("        axis y = "+QString::number(rotaxis_y));
    message("        axis z = "+QString::number(rotaxis_z));
    message("        angle = "+QString::number(rotangle_radians*180.0/M_PI)+" deg");
    VP1LinAlgUtils::decodeTransformation( volhandle->getGlobalTransformToVolume(),
					  translation_x, translation_y, translation_z,
					  rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians );
    message("===> Global Translation:");
    message("        x = "+QString::number(translation_x/CLHEP::mm)+" mm");
    message("        y = "+QString::number(translation_y/CLHEP::mm)+" mm");
    message("        z = "+QString::number(translation_z/CLHEP::mm)+" mm");
    message("===> Global Rotation:");
    message("        axis x = "+QString::number(rotaxis_x));
    message("        axis y = "+QString::number(rotaxis_y));
    message("        axis z = "+QString::number(rotaxis_z));
    message("        angle = "+QString::number(rotangle_radians*180.0/M_PI)+" deg");
  }

  if (m_d->controller->printInfoOnClick_Tree()) {
    std::ostringstream str;
    GeoPrintGraphAction pg(str);
    volhandle->geoPVConstLink()->exec(&pg);
    message("===> Tree:");
    foreach (QString line, QString(str.str().c_str()).split("\n"))
      message("     "+line);
  }

  if (m_d->controller->printInfoOnClick_Mass()) {
    //FIXME: Move the mass calculations to the volume handles, and let
    //the common data cache some of the volume information by
    //logVolume).
    message("===> Total Mass &lt;===");
    message("Inclusive "+QString::number(Imp::inclusiveMass(volhandle->geoPVConstLink())/CLHEP::kilogram)+" kg");
    message("Exclusive "+QString::number(Imp::exclusiveMass(volhandle->geoPVConstLink())/CLHEP::kilogram)+" kg");
  }


  /////////////////////////////////////////////////////////////////
  //  Emit a signal for the VP1UtilitySystems::PartSpect system  //
  /////////////////////////////////////////////////////////////////
  QStack<QString> partspectPath, extras;
  QString detFactoryName;  

  VolumeHandle *parentVH(volhandle), *childVH(volhandle);
  m_d->createPathExtras(volhandle,detFactoryName,extras);

  do {
    parentVH = parentVH->parent();
    PVConstLink parentPVLink = parentVH ? parentVH->geoPVConstLink() : childVH->geoPVConstLink()->getParent();
    if (parentPVLink) {
      int indexOfChild = parentVH ? childVH->childNumber() : (int)parentPVLink->indexOf(childVH->geoPVConstLink());
      
      std::string childPVName = parentPVLink->getNameOfChildVol(indexOfChild);
      QString pathEntry = childPVName=="ANON" ? detFactoryName+childVH->getName() : QString(childPVName.c_str());
      
      Query<int> childCopyNo = parentPVLink->getIdOfChildVol(indexOfChild);
      if(childCopyNo.isValid()) {
	    QString strCopyNo;
	    strCopyNo.setNum(childCopyNo);
	    pathEntry += ("::"+strCopyNo);
      }
      partspectPath.push(pathEntry);
      childVH = parentVH;
    }
  }while(parentVH);

  while(!extras.isEmpty())
    partspectPath.push(extras.pop());

  partspectPath.push("Atlas::Atlas");

  // Emit the signal
  //volhandle cannot be NULL here (coverity 16287)
  //int cn=(!volhandle) ?  -1 :  volhandle->copyNumber();
  int cn=volhandle->copyNumber();
  plotSpectrum(partspectPath,cn);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::buildSystem(SubSystemInfo* si)
{
  VP1Msg::messageDebug("VP1GeometrySystem::Imp::buildSystem()" );

  if (!si||si->isbuilt)
    return;
  si->isbuilt = true;
  int ichild(0);

  ensureInitVisAttributes();

//  // DEBUG
//  foreach(Imp::SubSystemInfo*si,m_d->subsysInfoList) {
//	  VP1Msg::messageDebug("vol: " + QString((si->flag).str_c()) );
//	}
  
  assert(si->soswitch);
  SoSeparator * subsystemsep = new SoSeparator;
  phisectormanager->registerSubSystemSeparator(si->flag,subsystemsep);
  phisectormanager->largeChangesBegin();

  SbBool save = si->soswitch->enableNotify(false);

  if (si->geomodelchildrenregexp.isEmpty()) {
	  //Loop over the treetop's that we previously selected:

	  std::vector<SubSystemInfo::TreetopInfo>::const_iterator it, itE = si->treetopinfo.end();
	  for (it=si->treetopinfo.begin();it!=itE;++it)
	  {
		  VP1Msg::messageDebug("toptree vol: " + QString(it->volname.c_str()) );

		  //Find material:
		  SoMaterial*m = detVisAttributes->get(it->volname);
		  if (!m) {
			  theclass->message("Warning: Did not find a predefined material for volume: "+QString(it->volname.c_str()));
		  }
		  VolumeHandleSharedData* volhandle_subsysdata = new VolumeHandleSharedData(controller,si->flag,&sonodesep2volhandle,it->pV,phisectormanager,
				  m,matVisAttributes,volVisAttributes,
				  controller->zappedVolumeListModel(),controller->volumeTreeBrowser(), m_textSep);
		  SbMatrix matr;
		  VP1LinAlgUtils::transformToMatrix(it->xf,matr);
		  VolumeHandle * vh = new VolumeHandle(volhandle_subsysdata,0,it->pV,ichild++,matr);
		  si->vollist.push_back(vh);
	  }
  } else {
	  //Loop over the children of the physical volumes of the treetops that we previously selected:
	  std::vector<SubSystemInfo::TreetopInfo>::const_iterator it, itE = si->treetopinfo.end();
	  for (it=si->treetopinfo.begin();it!=itE;++it) {

		  VP1Msg::messageDebug("group name: " + QString(si->matname.c_str()) );

		  //NB: Here we use the si->matname. Above we use the si->volname. Historical reasons!

		  //Find material for top-nodes:
		  SoMaterial*mat_top(0);
		  if (si->matname!="")
			  mat_top = detVisAttributes->get(si->matname);

		  VolumeHandleSharedData* volhandle_subsysdata = new VolumeHandleSharedData(controller,si->flag,&sonodesep2volhandle,it->pV,phisectormanager,
				  mat_top,matVisAttributes,volVisAttributes,
				  controller->zappedVolumeListModel(),controller->volumeTreeBrowser(),m_textSep);
		  volhandle_subsysdata->ref();

		  GeoVolumeCursor av(it->pV);
      //unsigned int count=0;
		  while (!av.atEnd()) {

			  // DEBUG
//			  VP1Msg::messageDebug("child vol: " + QString(av.getName().c_str()) );

			  //Use the childrenregexp to select the correct child volumes:
			  if (si->childrenRegExpNameCompatible(av.getName().c_str())) {
				  PVConstLink pVD = av.getVolume();
				  SbMatrix matr;
				  VP1LinAlgUtils::transformToMatrix(Amg::EigenTransformToCLHEP(av.getTransform()),matr);
				  VolumeHandle * vh=0;
          // std::cout<<count++<<": dump SubSystemInfo\n"<<"---"<<std::endl;
          // si->dump();
          // std::cout<<"---"<<std::endl;
            
				  vh = new VolumeHandle(volhandle_subsysdata,0,pVD,ichild++,matr);
          // std::cout<<"Does not have muon chamber (weird one) VH="<<vh<<std::endl;

				  // DEBUG
//				  VP1Msg::messageDebug("granchild vol: " + vh->getName() + " - " + vh->getDescriptiveName() );

				  if (si->geomodelgrandchildrenregexp.isEmpty()) {
					  // append the volume to the current list
					  theclass->messageDebug("grandchild inserted : " + vh->getDescriptiveName() + " - " + vh->getName() );
					  si->vollist.push_back(vh);

				  } else {
					  VP1Msg::messageDebug("filtering at grandchild level...");
					  if (si->grandchildrenRegExpNameCompatible(vh->getName().toStdString() ) ) {
						  VP1Msg::messageDebug("filtered grandchild inserted : " + vh->getDescriptiveName() + " - " + vh->getName() );
						  // append the volume to the current list
						  si->vollist.push_back(vh);
					  } else {
              theclass->message("Zapping this volumehandle because it's probably junk."+vh->getDescriptiveName() + " - " + vh->getName());
              vh->setState(VP1GeoFlags::ZAPPED); // FIXME - better solution for this? Maybe just don't create them?
              
              // std::cout<<"Not adding "<<vh->getName().toStdString()<<"["<<vh<<"] to vollist"<<std::endl;
					  }
				  }
			  }
			  av.next(); // increment volume cursor.        
		  }
		  volhandle_subsysdata->unref();//To ensure it is deleted if it was not used.
	  }
  }

  
  si->dump();

  VP1Msg::messageDebug("volumetreemodel->addSubSystem...");
  volumetreemodel->addSubSystem( si->flag, si->vollist );

  //NB: We let the destructor of volumetreemodel take care of deleting
  //our (top) volume handles, since it has to keep a list of them
  //anyway.

  
  //Perform auto expansion of all ether volumes (needed for muon dead material):
  VP1Msg::messageDebug("Perform auto expansion of all ether volumes");
  VolumeHandle::VolumeHandleListItr it, itE(si->vollist.end());
  //int idx=0; // for debug
  for (it = si->vollist.begin(); it!=itE; ++it){
    //VP1Msg::messageDebug("\nexpanding idx: " + QString::number(++idx)); 
    (*it)->expandMothersRecursivelyToNonEther();
    //VP1Msg::messageDebug("expand DONE."); 
  }
  

  VP1Msg::messageDebug("addChild...");
  phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  phisectormanager->largeChangesEnd();
  si->soswitch->addChild(subsystemsep);
  si->soswitch->enableNotify(save);
  if (save)
    si->soswitch->touch();
  VP1Msg::messageDebug("END of VP1GeometrySystem::Imp::buildSystem() " );
}

//_____________________________________________________________________________________
double VP1GeometrySystem::Imp::exclusiveMass(const PVConstLink& pv) {
  const GeoLogVol* lv       = pv->getLogVol();
  const GeoMaterial      *material = lv->getMaterial();
  double density = material->getDensity();
  return density*volume(pv);
}


//_____________________________________________________________________________________
double VP1GeometrySystem::Imp::volume(const PVConstLink& pv) {
  const GeoLogVol        * lv       = pv->getLogVol();
  const GeoShape         *shape    = lv->getShape();
  return shape->volume();
}


//_____________________________________________________________________________________
double VP1GeometrySystem::Imp::inclusiveMass(const PVConstLink& pv) {

  const GeoLogVol*        lv       = pv->getLogVol();
  const GeoMaterial      *material = lv->getMaterial();
  double density = material->getDensity();

  double mass = exclusiveMass(pv);

  GeoVolumeCursor av(pv);
  while (!av.atEnd()) {
    mass += inclusiveMass(av.getVolume());
    mass -= volume(av.getVolume())*density;
    av.next();
  }

  return mass;
}

//_____________________________________________________________________________________
QByteArray VP1GeometrySystem::saveState() {

  ensureBuildController();

  VP1Serialise serialise(7/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  //Controller:
  serialise.save(m_d->controller->saveSettings());

  //Subsystem checkboxes:
  QMap<QString,bool> subsysstate;
  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList) {
    serialise.widgetHandled(subsys->checkbox);
    subsysstate.insert(subsys->checkbox->text(),subsys->checkbox->isChecked());
  }
  serialise.save(subsysstate);

  //Volume states:
  QMap<quint32,QByteArray> topvolstates;
  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList) {
    VolumeHandle::VolumeHandleListItr it(subsys->vollist.begin()),itE(subsys->vollist.end());
    for (;it!=itE;++it)
      topvolstates.insert((*it)->hashID(),(*it)->getPersistifiableState());
  }
  serialise.save(topvolstates);

  m_d->ensureInitVisAttributes();
  serialise.save(m_d->detVisAttributes->getState());//version 7+
  serialise.save(m_d->matVisAttributes->getState());//version 7+
  serialise.save(m_d->volVisAttributes->getState());//version 7+

  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::restoreFromState(QByteArray ba) {
  VP1Deserialise state(ba,this);
  if (state.version()<0||state.version()>7) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  if (state.version()<=5) {
    message("Warning: State data in .vp1 file is in obsolete format - ignoring!");
    return;
  }

  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  //Controller:
  m_d->controller->restoreSettings(state.restoreByteArray());

  //Subsystem checkboxes:
  VP1GeoFlags::SubSystemFlags flags(0);
  QMap<QString,bool> subsysstate = state.restore<QMap<QString,bool> >();
  foreach (Imp::SubSystemInfo * subsys, m_d->subsysInfoList) {
    state.widgetHandled(subsys->checkbox);
    if (subsysstate.contains(subsys->checkbox->text())&&subsysstate[subsys->checkbox->text()])
      flags |= subsys->flag;
  }
  m_d->initialSubSystemsTurnedOn = flags;

  //Volume states:
  QMap<quint32,QByteArray> topvolstates;
  topvolstates = state.restore<QMap<quint32,QByteArray> >();
  if (m_d->sceneroot)//(for some reason) we already have been in buildPermanentScenegraph
    m_d->applyTopVolStates(topvolstates,true);
  else
    m_d->restoredTopvolstates = topvolstates;//save until buildPermanentScenegraph

  if (state.version()>=7) {
    m_d->ensureInitVisAttributes();
    m_d->detVisAttributes->applyState(state.restoreByteArray());
    m_d->matVisAttributes->applyState(state.restoreByteArray());
    m_d->volVisAttributes->applyState(state.restoreByteArray());
  }

  state.disableUnrestoredChecks();//We do the testing in the controller
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::applyTopVolStates(const QMap<quint32,QByteArray>&topvolstates,bool disablenotif)
{
  if (disablenotif)
    phisectormanager->largeChangesBegin();
  QMap<quint32,QByteArray>::const_iterator topvolstatesItr;
  foreach (Imp::SubSystemInfo * subsys, subsysInfoList) {
    VolumeHandle::VolumeHandleListItr it(subsys->vollist.begin()),itE(subsys->vollist.end());
    for (;it!=itE;++it) {
      topvolstatesItr = topvolstates.find((*it)->hashID());
      if (topvolstatesItr!=topvolstates.end())
	(*it)->applyPersistifiableState(topvolstatesItr.value());
    }
  }
  if (disablenotif) {
    phisectormanager->updateRepresentationsOfVolsAroundZAxis();
    phisectormanager->largeChangesEnd();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::createPathExtras(const VolumeHandle* volhandle, QString& prefix, QStack<QString>& entries)
{
  switch(volhandle->subsystem()){
    case VP1GeoFlags::Emulsion:{
      prefix = QString("Emulsion::");
      entries.push("NEUTRINO::NEUTRINO");
      entries.push("Emulsion::Emulsion");
      return;
    }
    case VP1GeoFlags::Veto:{
      prefix = QString("Veto::");
      entries.push("SCINT::SCINT");
      entries.push("Veto::Veto");
      return;
    }
    case VP1GeoFlags::VetoNu:{
      prefix = QString("VetoNu::");
      entries.push("SCINT::SCINT");
      entries.push("VetoNu::VetoNu");
      return;
    }
    case VP1GeoFlags::Trigger:{
      prefix = QString("Trigger::");
      entries.push("SCINT::SCINT");
      entries.push("Trigger::Trigger");
      return;
    }
    case VP1GeoFlags::Preshower:{
      prefix = QString("Preshower::");
      entries.push("SCINT::SCINT");
      entries.push("Preshower::Preshower");
      return;
    }
    case VP1GeoFlags::SCT:{
      prefix = QString("SCT::");
      entries.push("TRACKER::TRACKER");
      entries.push("SCT::SCT");
      return;
    }
    case VP1GeoFlags::Dipole:{
      prefix = QString("Dipole::");
      entries.push("TRACKER::TRACKER");
      entries.push("Dipole::Dipole");
      return;
    }
    case VP1GeoFlags::Ecal:{
      prefix = QString("Ecal::");
      entries.push("CALO::CALO");
      entries.push("Ecal::Ecal");
      return;
    }
    case VP1GeoFlags::CavernInfra:{
      prefix = QString("Cavern::");
      entries.push("CAVERN::CAVERN");
      entries.push("Trench::Trench");
      return;
    }

  case VP1GeoFlags::None:
  case VP1GeoFlags::AllUnrecognisedVolumes:
  default:{
    return;
  }
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setCurvedSurfaceRealism(int val)
{
   ensureBuildController();
   if (val<0||val>100) {
     message("setCurvedSurfaceRealism Error: Value "+str(val)+"out of range!");
     return;
   }
   m_d->controller->setComplexity(val==100?1.0:(val==0?0.0:val/100.0));
}



//_____________________________________________________________________________________
void VP1GeometrySystem::updateTransparency()
{
  ensureBuildController();

  float transparency = m_d->controller->transparency();

  VolumeHandle* lastSelVol = m_d->controller->lastSelectedVolume();
  m_d->controller->setLastSelectedVolume(0);
  m_d->ensureInitVisAttributes();
  m_d->detVisAttributes->overrideTransparencies(transparency);
  m_d->matVisAttributes->overrideTransparencies(transparency);
  m_d->volVisAttributes->overrideTransparencies(transparency);
  m_d->controller->setLastSelectedVolume(lastSelVol);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::resetSubSystems(VP1GeoFlags::SubSystemFlags f)
{
  if (!f) {
    return; 
  }

  deselectAll();
  foreach(Imp::SubSystemInfo*si,m_d->subsysInfoList) {
    if (si->flag & f) {
        if (!si->isbuilt) {
	        continue;
        }
        VolumeHandle::VolumeHandleListItr it(si->vollist.begin()),itE(si->vollist.end());
        for (;it!=itE;++it) {
            messageDebug("resetting volume --> " + (*it)->getName() );
	        (*it)->reset();
        }
    }
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::changeStateOfVisibleNonStandardVolumesRecursively(VolumeHandle*handle,VP1GeoFlags::VOLSTATE target)
{
  assert(target!=VP1GeoFlags::CONTRACTED);
  if (handle->isAttached()) {
    //The volume is visible, so ignore daughters
    if (handle->isInitialisedAndHasNonStandardShape()) {
      if (target!=VP1GeoFlags::EXPANDED||handle->nChildren()>0)
	handle->setState(target);
    }
    return;
  } else if (handle->state()==VP1GeoFlags::ZAPPED)
    return;
  //Must be expanded: Let us call on any (initialised) children instead.
  if (handle->nChildren()==0||!handle->childrenAreInitialised())
    return;
  VolumeHandle::VolumeHandleListItr it(handle->childrenBegin()), itE(handle->childrenEnd());
  for(;it!=itE;++it)
    changeStateOfVisibleNonStandardVolumesRecursively(*it,target);

}

//_____________________________________________________________________________________
void VP1GeometrySystem::actionOnAllNonStandardVolumes(bool zap)
{
  VP1GeoFlags::VOLSTATE target = zap ? VP1GeoFlags::ZAPPED : VP1GeoFlags::EXPANDED;
  messageVerbose("Action on volumes with non-standard VRML representations. Target state is "+VP1GeoFlags::toString(target));

  std::vector<std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr> > roothandles;
  m_d->volumetreemodel->getRootHandles(roothandles);
  VolumeHandle::VolumeHandleListItr it, itE;

  bool save = m_d->sceneroot->enableNotify(false);
  m_d->phisectormanager->largeChangesBegin();

  deselectAll();

  for (unsigned i = 0; i<roothandles.size();++i) {
    it = roothandles.at(i).first;
    itE = roothandles.at(i).second;
    for(;it!=itE;++it) {
      m_d->changeStateOfVisibleNonStandardVolumesRecursively(*it,target);
    }
  }

  m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  m_d->phisectormanager->largeChangesEnd();
  if (save) {
    m_d->sceneroot->enableNotify(true);
    m_d->sceneroot->touch();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::autoExpandByVolumeOrMaterialName(bool bymatname,QString targetname)
{
  if (targetname.isEmpty()) {
	  VP1Msg::messageDebug("targetname is empty.");
    return;
  }

  messageVerbose("Auto expansions of visible volumes requested. Target all volumes with "
		 +str(bymatname?"material name":"name")+" matching "+targetname);

  QRegExp selregexp(targetname,Qt::CaseSensitive,QRegExp::Wildcard);

  std::vector<std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr> > roothandles;
  m_d->volumetreemodel->getRootHandles(roothandles);
  VolumeHandle::VolumeHandleListItr it, itE;

  bool save = m_d->sceneroot->enableNotify(false);
  m_d->phisectormanager->largeChangesBegin();

  deselectAll();

  for (unsigned i = 0; i<roothandles.size();++i) {
    it = roothandles.at(i).first;
    itE = roothandles.at(i).second;
    for(;it!=itE;++it)
      m_d->expandVisibleVolumesRecursively(*it,selregexp,bymatname);
  }

  m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  m_d->phisectormanager->largeChangesEnd();
  if (save) {
    m_d->sceneroot->enableNotify(true);
    m_d->sceneroot->touch();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::expandVisibleVolumesRecursively(VolumeHandle* handle,const QRegExp& selregexp,bool bymatname)
{
  if (handle->state()==VP1GeoFlags::ZAPPED)
    return;
  if (handle->state()==VP1GeoFlags::CONTRACTED) {
    //See if we match (and have children) - if so, update state.
    if (handle->nChildren()>0
	&& selregexp.exactMatch(bymatname?QString(handle->geoMaterial()->getName().c_str()):handle->getName())) {
      handle->setState(VP1GeoFlags::EXPANDED);
    }
    return;
  }
  //Must be expanded: Let us call on any (initialised) children instead.
  if (handle->nChildren()==0||!handle->childrenAreInitialised())
    return;
  VolumeHandle::VolumeHandleListItr it(handle->childrenBegin()), itE(handle->childrenEnd());
  for(;it!=itE;++it)
    expandVisibleVolumesRecursively(*it,selregexp,bymatname);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::volumeStateChangeRequested(VolumeHandle*vh,VP1GeoFlags::VOLSTATE state)
{
  //might not use this slot presently...
  if (!vh)
    return;
  deselectAll();
  vh->setState(state);
  m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::volumeResetRequested(VolumeHandle*vh)
{
  if (!vh)
    return;
  deselectAll();
  vh->reset();
  m_d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setShowVolumeOutLines(bool b)
{
  std::map<SoSeparator*,VolumeHandle*>::iterator it,itE(m_d->sonodesep2volhandle.end());
  for (it =m_d->sonodesep2volhandle.begin();it!=itE;++it)
    VolumeHandleSharedData::setShowVolumeOutlines(it->first,b);
}


//_____________________________________________________________________________________
void VP1GeometrySystem::saveMaterialsToFile(QString filename,bool onlyChangedMaterials)
{
  if (filename.isEmpty())
    return;

  //If file exists, ask to overwrite.
  QFileInfo fi(filename);
  if (fi.exists()) {
    if (!fi.isWritable()) {
      QMessageBox::critical(0, "Error - could not save to file "+filename,
			    "Could not save to file: <i>"+filename+"</i>"
			    +"<br/><br/>Reason: File exists already and is write protected",QMessageBox::Ok,QMessageBox::Ok);
      return;
    }
  }

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(0, "Error - problems writing to file "+filename,
			    "Problems writing to file: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
  }

  m_d->ensureInitVisAttributes();
  VP1Serialise s(0/*version*/,this);
  //Save some file ID info!!
  s.save(QString("VP1GeoMaterialsBegin"));
  s.save(m_d->detVisAttributes->getState(onlyChangedMaterials));
  s.save(m_d->matVisAttributes->getState(onlyChangedMaterials));
  s.save(m_d->volVisAttributes->getState(onlyChangedMaterials));
  s.save(QString("VP1GeoMaterialsEnd"));
  s.disableUnsavedChecks();

  QDataStream outfile(&file);
  outfile<<qCompress(s.result()).toBase64();

}

//_____________________________________________________________________________________
void VP1GeometrySystem::loadMaterialsFromFile(QString filename)
{
  if (filename.isEmpty())
    return;
  QFileInfo fi(filename);
  if (!fi.exists()) {
    QMessageBox::critical(0, "Error - file does not exists: "+filename,
			  "File does not exists: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  if (!fi.isReadable()) {
    QMessageBox::critical(0, "Error - file is not readable: "+filename,
			  "File is not readable: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  //open file
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(0, "Error - problems opening file "+filename,
			    "Problems opening file: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
  }
  QByteArray byteArray64;
  QDataStream infile(&file);
  infile >> byteArray64;
  QByteArray byteArray = qUncompress(QByteArray::fromBase64(byteArray64));

  VP1Deserialise s(byteArray,this);
  if (s.version()!=0) {
    QMessageBox::critical(0, "Error - File in wrong format "+filename,
			  "File in wrong format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  QString txtbegin, txtend;
  QByteArray baDet, baMat, baVol;
  txtbegin = s.restoreString();
  baDet = s.restoreByteArray();
  baMat = s.restoreByteArray();
  baVol = s.restoreByteArray();
  txtend = s.restoreString();
  s.disableUnrestoredChecks();
  if (txtbegin!="VP1GeoMaterialsBegin"||txtend!="VP1GeoMaterialsEnd") {
    QMessageBox::critical(0, "Error - File in wrong format "+filename,
			  "File in wrong format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }

  m_d->ensureInitVisAttributes();
  m_d->detVisAttributes->applyState(baDet);
  m_d->matVisAttributes->applyState(baMat);
  m_d->volVisAttributes->applyState(baVol);

  VolumeHandle* lastsel = m_d->controller->lastSelectedVolume();
  m_d->controller->setLastSelectedVolume(0);
  m_d->controller->setLastSelectedVolume(lastsel);
}

//____________________________________________________________________
void VP1GeometrySystem::setLabelPosOffsets( QList<int> /**offsets*/)
{
  messageVerbose("setLabelPosOffsets called");
  setLabels(m_d->controller->labels());
}

void VP1GeometrySystem::setLabels(int /*i*/){

}
