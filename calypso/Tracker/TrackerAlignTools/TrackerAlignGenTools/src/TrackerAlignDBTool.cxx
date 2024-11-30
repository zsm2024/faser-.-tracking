/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

// TrackerAlignDBTool.cxx
// AlgTool to manage the SCT AlignableTransforms in the conditions store
// Richard Hawkings, started 8/4/05

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <set>
#include <exception>
#include <fstream>
#include <iostream>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerReadoutGeometry/TrackerDD_Defs.h"

#include "TrackerIdentifier/FaserSCT_ID.h"

#include "DetDescrConditions/AlignableTransformContainer.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "TrackerAlignDBTool.h"


// name of the folder for ID alignment information
#define TRACKER_ALIGN "/Tracker/Align"

TrackerAlignDBTool::TrackerAlignDBTool(const std::string& type,
    const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
  p_toolsvc("ToolSvc",name),
  m_sctid(nullptr),
  m_sctman(nullptr),
  m_par_scttwoside(false),
  m_par_dbroot( TRACKER_ALIGN ),
  m_par_dbkey( TRACKER_ALIGN ),
  m_dynamicDB(false),
  m_forceUserDBConfig(false)
{
  declareInterface<ITrackerAlignDBTool>(this);
  declareProperty("IToolSvc",    p_toolsvc);
  declareProperty("SCTTwoSide",  m_par_scttwoside);
  declareProperty("DBRoot",      m_par_dbroot,"name of the root folder for constants");
  declareProperty("DBKey",       m_par_dbkey,"base part of the key for loading AlignableTransforms");
  declareProperty("forceUserDBConfig",m_forceUserDBConfig, "Set to true to override any DB auto-configuration");
}

TrackerAlignDBTool::~TrackerAlignDBTool()
{}

StatusCode TrackerAlignDBTool::initialize() 
{

  ATH_MSG_DEBUG("initializing");

  m_alignobjs.clear();
  m_alignchans.clear();
  int ndet {0};

  // get storegate access to conditions store
  ATH_CHECK(detStore().retrieve());

  ATH_CHECK( p_toolsvc.retrieve() );

  // attempt to get ID helpers from detector store 
  // (relying on GeoModel to put them)

  ATH_CHECK(detStore()->retrieve(m_sctman,"SCT"));

  if (m_sctman->m_alignfoldertype == TrackerDD::static_run1 && 
      !m_forceUserDBConfig)
  {
    m_par_dbroot = "/Tracker/Align";
    m_dynamicDB = false;
  } 
  else if (m_sctman->m_alignfoldertype == TrackerDD::timedependent_run2 && 
      !m_forceUserDBConfig)
  {
    m_par_dbroot = "/Tracker/AlignL3";
    m_dynamicDB = true;
  }  
  m_par_dbkey = m_par_dbroot;

  ATH_CHECK(detStore()->retrieve(m_sctid));

  // setup list of alignable transforms from geometry
  int chan[3];
  int TransfLevel_low = 0; // depending on alignfolder sheme; 0 for old, 2 for new
  if (m_dynamicDB) TransfLevel_low = 2;

  for (int i=0;i<3;++i) chan[i]=100*i;

  std::string man_name;
  for (const TrackerDD::SiDetectorElement* element : *(m_sctman->getDetectorElementCollection())) 
  {
    if (element!=0) 
    {
      const Identifier ident=element->identify();
      int station, layer, eta, phi, side;
      if (idToDetSet(ident, station, layer, eta, phi, side)) 
      {
	std::string level[3];
	for (int i=TransfLevel_low; i<3; ++i) 
	{  
	  m_stations.insert(station);
	  level[i]=dirkey(station, layer, 1+i, phi);
	  // add this to list if not seen already
	  std::vector<std::string>::const_iterator ix =
	    find(m_alignobjs.begin(), m_alignobjs.end(), level[i]);
	  if (ix==m_alignobjs.end()) 
	  {
	    m_alignobjs.push_back(level[i]);
	    m_alignchans.push_back(chan[i]++);
	  }
	}
	++ndet;
      } 
      else 
      {
	ATH_MSG_ERROR("Error translating element identifier." );
      }
    }
  }
  ATH_CHECK(m_outputTool.retrieve());

  if (msgLvl(MSG::DEBUG)) 
  {
    ATH_MSG_DEBUG( "Database root folder " << m_par_dbroot );
    ATH_MSG_DEBUG( "Geometry initialisation sees " << ndet << 
	" SCT modules giving " << m_alignobjs.size() << " alignment keys" );
    ATH_MSG_DEBUG("Keys/channels are:");

    for (unsigned int i=0; i < m_alignobjs.size(); ++i)
      ATH_MSG_DEBUG( " " << m_alignobjs[i] << " [" << m_alignchans[i] << "]" );

  }

  return StatusCode::SUCCESS;
}

StatusCode TrackerAlignDBTool::finalize()
{
  ATH_MSG_DEBUG( "finalizing" );
  return StatusCode::SUCCESS;
}

StatusCode TrackerAlignDBTool::createDB() const
{

  ATH_MSG_DEBUG("createDB method called");
  AlignableTransform* pat = nullptr;

  // loop over all SiDetectorElements and fill corresponding
  // AlignableTransform objects with default values

  // first create the empty AlignableTransform objects in TDS
  // check object does not already exist

  if (detStore()->contains<AlignableTransformContainer>(m_par_dbroot)) 
  {
    ATH_MSG_WARNING("createDB: AlignableTransformContainer already exists" );
    return StatusCode::FAILURE;
  }

  // put them in a collection /Indet/Align
  ATH_MSG_DEBUG( "Setup database structures in AlignableTransformContainer" );
  AlignableTransformContainer* patc = new AlignableTransformContainer;

  if (m_par_scttwoside) ATH_MSG_DEBUG( "Produce separate transforms for each side of SCT modules" );
  else ATH_MSG_DEBUG( "Treat both sides of SCT module as single entity" );

  for (unsigned int i = 0; i < m_alignobjs.size(); ++i) 
  {
    pat = new AlignableTransform(m_alignobjs[i]);
    // add to collection and set corresponding channel number
    patc->push_back(pat);
    patc->add( m_alignchans[i] );
  }

  // record collection in SG

  ATH_CHECK( detStore()->record(patc, m_par_dbroot) );
  ATH_MSG_DEBUG( "Collection has size " <<  patc->size() );

  // now loop over all detector modules and add null level 3 transforms
  std::vector<std::string> level2;
  for (const TrackerDD::SiDetectorElement* element : *(m_sctman->getDetectorElementCollection()) )
  {
    if (element != 0) 
    {
      const Identifier ident = element->identify();
      std::string key = dirkey(ident,3);
      // do not produce AlignableTrasnforms for SCT side 1 if option set
      if (!(m_sctid->side(ident)==1) || m_par_scttwoside) 
      {
	if ((pat=getTransPtr(key))) 
	{
	  const auto iStation = m_sctid->station(ident);
	  const auto iLayer = m_sctid->layer(ident); 
	  const auto iModuleEta = m_sctid->eta_module(ident);
	  const auto iModulePhi = m_sctid->phi_module(ident);
	  int iModule = iModulePhi;
	  if (iModuleEta < 0) iModule +=4;

      const auto buildKey = [](auto iStation, auto iLayer, auto iModule) {
          std::stringstream ss;
          ss << iStation << iLayer << iModule; 
          return ss.str();
      };
      
      const auto key = buildKey(iStation, iLayer, iModule); 
      if (not (m_alignment.find(key) == m_alignment.end())) {
        const std::vector<double> c = m_alignment.value().find(key)->second;
        ATH_MSG_VERBOSE("Applying correction for " << key);
        ATH_MSG_VERBOSE(c[0] << " " << c[1] << " " << c[2] << " " << c[3] << " " << c[4] << " " << c[5]);
        Amg::Translation3D newtranslation(c[0], c[1], c[2]);
        Amg::Transform3D alignment = newtranslation * Amg::RotationMatrix3D::Identity();
        alignment *= Amg::AngleAxis3D(c[5], Amg::Vector3D(0.,0.,1.));
        alignment *= Amg::AngleAxis3D(c[4], Amg::Vector3D(0.,1.,0.));
        alignment *= Amg::AngleAxis3D(c[3], Amg::Vector3D(1.,0.,0.));

        pat->add(ident, Amg::EigenTransformToCLHEP(alignment));
	  } else {
        ATH_MSG_VERBOSE("No correction given for " << key);
	  }
	} 
	else 
	{
	  ATH_MSG_ERROR( "Cannot retrieve AlignableTransform for key " << key );
	}
	// add level 2 transform if needed - do this the first time a module
	// for this level 3 key is seen
	std::vector<std::string>::const_iterator ix=
	  find(level2.begin(),level2.end(),key);
	if (ix==level2.end()) 
	{
	  level2.push_back(key);
	  // construct identifier of level 2 transform
	  Identifier ident2;
	  ident2=m_sctid->wafer_id(m_sctid->station(ident),
	      m_sctid->layer(ident),
	      0, 0, 0);
	  std::string key2 = dirkey(ident, 2);
	  if ((pat = getTransPtr(key2))) 
	  {
	    auto iStation = m_sctid->station(ident);
	    auto iLayer = m_sctid->layer(ident);

	    const auto buildKey = [](auto iStation, auto iLayer) {
	      std::stringstream ss;
	      ss << iStation << iLayer; 
	      return ss.str();
	    };

	    const auto key = buildKey(iStation, iLayer); 
	    if (not (m_alignment.find(key) == m_alignment.end())) {
	      const std::vector<double> c = m_alignment.value().find(key)->second;
	      ATH_MSG_VERBOSE("Applying correction for " << key);
	      ATH_MSG_VERBOSE(c[0] << " " << c[1] << " " << c[2] << " " << c[3] << " " << c[4] << " " << c[5]);
	      Amg::Translation3D newtranslation(c[0], c[1], c[2]);
	      Amg::Transform3D alignment = newtranslation * Amg::RotationMatrix3D::Identity();
	      alignment *= Amg::AngleAxis3D(c[5], Amg::Vector3D(0.,0.,1.));
	      alignment *= Amg::AngleAxis3D(c[4], Amg::Vector3D(0.,1.,0.));
	      alignment *= Amg::AngleAxis3D(c[3], Amg::Vector3D(1.,0.,0.));

	      Amg::Translation3D newtransl(0,0,element->transform().translation()[2]);
	      Amg::Transform3D newcorr= newtransl * Amg::RotationMatrix3D::Identity();

	      pat->add(ident2, Amg::EigenTransformToCLHEP(newcorr*alignment*newcorr.inverse()));

//	      pat->add(ident2, Amg::EigenTransformToCLHEP(alignment));
	    } else {
	      ATH_MSG_VERBOSE("No correction given for " << key);
	    }
	  } 
	  else 
	  {
	    ATH_MSG_ERROR( "Cannot retrieve AlignableTransform for key " << key2 );
	  }
	}
      }
    }
  }

  // create the global object with positions for the stations
  Identifier ident1;
  std::string key1 = dirkey(ident1, 1);
  if ((pat = getTransPtr(key1))) 
  {
    Amg::Transform3D globshift;
    globshift.setIdentity();
    for (int station : m_stations)
    {
      ident1 = m_sctid->wafer_id(station, 0, 0, 0, 0);
      pat->add(ident1, Amg::EigenTransformToCLHEP(globshift));
    }
  } 
  else
  {
    ATH_MSG_ERROR( "Cannot retrieve AlignableTransform for key " << key1 );
  } 

  // sort the created objects (in case, usually come out sorted from GeoModel)
  ATH_CHECK(sortTrans());
  // list out size of all created objects
  ATH_MSG_DEBUG( "Dumping size of created AlignableTransform objects");
  for (unsigned int i = 0; i < m_alignobjs.size(); ++i)
    if ((pat = getTransPtr(m_alignobjs[i]))) pat->print();

  return StatusCode::SUCCESS;
}

bool TrackerAlignDBTool::idToDetSet(const Identifier ident, int& station,
    int& layer, int& eta, int& phi, int& side) const 
{
  // transform Identifier to list of integers specifiying station,layer
  // eta, phi, side

  station = m_sctid->station(ident);
  layer = m_sctid->layer(ident);
  eta = m_sctid->eta_module(ident);
  phi = m_sctid->phi_module(ident);
  side = m_sctid->side(ident);
  return true;
}

std::string TrackerAlignDBTool::dirkey(const Identifier& ident,
    const int level) const 
{
  // given SCT identifier, and level (1, 2 or 3) return
  // directory key name for associated alignment data
  int station, layer, eta, phi, side;
  idToDetSet(ident, station, layer, eta, phi, side);
  return dirkey(station, layer, level, phi);
}

std::string TrackerAlignDBTool::dirkey(const int station, 
    const int layer, 
    const int level, 
    const int ) const 
{
  // channel info and level (1,2 or 3) return
  // directory key name for associated alignment data
  std::ostringstream result;
  if (m_dynamicDB)
  {
    result << "/Tracker/AlignL";
    result << level;
    result << "/" ;          // new folders have L1, L2, L3 structure
  }
  else
  {
    result << m_par_dbkey << "/" ;
  }

  if (level==1) {
    result << "Stations";
  } else {
    if (level==2) result << "Planes";
    if (level==3) {
      if (station == 3 ) result << "Downstream";
      if (station == 2 ) result << "Central";
      if (station == 1 ) result << "Upstream";
      if (station == 0 ) result << "Interface";
      result << 1+layer;
    }
  }
  return result.str();
}

// void InDetAlignDBTool::dispGroup(const int dettype, const int bec, 
//                                  const int layer,const int ring, const int sector,
//                                  const float rphidisp, const float rdisp, const float zdisp, 
//                                  const int syst, const int level, const int skip) const {

//   ATH_MSG_DEBUG( "dispGroup called: level " << level << " syst " << syst);
//   int nmod=0;
//   // random number service
//   IRndmGenSvc* randsvc;
//   if (StatusCode::SUCCESS!=service("RndmGenSvc",randsvc,true))
//     ATH_MSG_ERROR("Cannot find RndmGenSvc" );
//   Rndm::Numbers gauss(randsvc,Rndm::Gauss(0.,1.));
//   if (skip>0) {
//     ATH_MSG_DEBUG("Skip random numbers " << skip ); 
//     for (int i=0;i<skip;++i) gauss();
//   }
//   // for syst 5, choose random shifts based on the input numbers
//   float rpd=0,rd=0,zd=0;
//   if (syst==5) {
//     rpd=rphidisp*gauss();
//     rd=rdisp*gauss();
//     zd=zdisp*gauss();
//   } 
//   // keep a list of level1/2 transform IDs to make sure they are only set once
//   std::vector<Identifier> lvl12id;
//   // loop over all pixel and SCT modules
//   AlignableTransform* pat;
//   for (int idet=1;idet<3;++idet) {
//     for (const InDetDD::SiDetectorElement* element: *(idet==1 ? m_pixman->getDetectorElementCollection() : m_sctman->getDetectorElementCollection())) {
//       if (element!=0) {
//         const Identifier ident=element->identify();
//         int mdet,mbec,mlayer,mring,msector,mside;
//         idToDetSet(ident,mdet,mbec,mlayer,mring,msector,mside);
//         // find matching modules - note side=1 modules never touched
//         if ((dettype==-1 || mdet==dettype) && (bec==-1 || std::abs(2*mbec)==bec) &&
//             (layer==-1 || mlayer==layer) && (ring==-1 || mring==ring) && 
//             (sector== -1 || msector==sector) && mside==0) {
//           // displace this module - first choose displacement type
//           // dont choose new displacements if seeing second side of SCT module
//           // ensures they both move together
//           // depends on the side1 module immediatly following side 0 in list
//           // which is currently the case - fragile
//           // also for syst 6 choose number only for new ring (eta) slice
//           if (dettype!=2 || mside!=1) {
//             if (syst==2 || syst==4 || (syst==6 && mring==-6)) {
//               rpd=rphidisp*gauss();
//               rd=rdisp*gauss();
//               zd=zdisp*gauss();
//               if (syst==6) ATH_MSG_DEBUG("New rndm at layer/ring " << 
//                                          mlayer << " " << mring << " z " << zd );
//             } else if (syst<5) {
//               rpd=rphidisp;
//               rd=rdisp;
//               zd=zdisp;
//             }
//           }
//           // interpretation as rphi/r or x/y
//           float xd,yd;
//           if (syst<=2 || syst==6) {
//             // rphi displacement - calculate from module position in x/y
//             const Amg::Vector3D modcent=element->center();
//             float dx=modcent.x();
//             float dy=modcent.y();
//             float dr=sqrt(dx*dx+dy*dy);
//                   xd=(rd*dx-rpd*dy)/dr;
//             yd=(rd*dy+rpd*dx)/dr;
//           } else {
//             xd=rpd;
//             yd=rd;
//           }
//           // find the corresponding AlignableTransform object
//           std::string key=dirkey(mdet,mbec,mlayer,level);
//           // first get as const as transforms might have been read in
//           const AlignableTransform* cpat=cgetTransPtr(key);
//                 pat=const_cast<AlignableTransform*>(cpat);
//           if (pat) {
//             Identifier ident2;
//             bool update=true;
//             if (level==3) {
//               ident2=ident;
//             } else if (level==2) {
//               // identifier for layer in level 2 transform
//               if (mdet==1) {
//                 ident2=m_pixid->wafer_id(m_pixid->barrel_ec(ident),
//                        m_pixid->layer_disk(ident),0,0);
//               } else {
//                 ident2=m_sctid->wafer_id(m_sctid->barrel_ec(ident),
//                  m_sctid->layer_disk(ident),0,0,0);
//               }
//               // check this identifier has not been updated before
//               std::vector<Identifier>::const_iterator ix=
//                 find(lvl12id.begin(),lvl12id.end(),ident2);
//               if (ix==lvl12id.end()) {
//                 lvl12id.push_back(ident2);
//               } else {
//                 update=false;
//               }
//             } else {
//               // identifier for ID 
//               if (mdet==1) {
//                 ident2=m_pixid->wafer_id(0,0,0,0);
//               } else {
//                 ident2=m_sctid->wafer_id(0,0,0,0,0);
//               }
//               // check this identifier has not been updated before
//               std::vector<Identifier>::const_iterator ix=
//                 find(lvl12id.begin(),lvl12id.end(),ident2);
//               if (ix==lvl12id.end()) {
//                 lvl12id.push_back(ident2);
//               } else {
//                 update=false;
//               }
//             }
//             // update, adding to any existing shift
//             if (update) {

//               Amg::Transform3D shift =   Amg::Translation3D(xd,yd,zd) * Amg::RotationMatrix3D::Identity(); 
//               pat->tweak(ident2,Amg::EigenTransformToCLHEP(shift));
//               ATH_MSG_VERBOSE( "Updated module " << mdet << "," << mbec 
//                                << "," << mlayer << "," << mring << "," << msector << " to xyz" <<
//                                xd << "," << yd << "," << zd );
//                                 ++nmod;
//             }
//           } else {
//             ATH_MSG_ERROR("Cannot find AlignableTransform for key" << key );
//           }
//         }
//       }
//     }
//   }
//  ATH_MSG_DEBUG( "Added displacement to " << nmod << " modules " << dettype << "," 
//              << bec << "," << layer << " [" << rphidisp << "," << rdisp 
//              << "," << zdisp << "]"
//              << " type " << syst );
// }


// bool InDetAlignDBTool::setTrans(const Identifier& ident, const int level,
//         const Amg::Transform3D& trans) const {

//   bool result=false;

//   // New additions for new global folder structure -- setTrans for this might need to be revisited
//   // No ATs exist for levels 1 & 2 --> need alternative
//   if (m_dynamicDB && level!=3){
//     result=tweakGlobalFolder(ident, level, trans);
//     if (!result ) ATH_MSG_ERROR( "Attempt tweak GlobalDB folder failed" );
//   }
//   else {
//     // find transform key, then set appropriate transform
//     // do storegate const retrieve, then cast to allow update of locked data
//     std::string key=dirkey(ident,level);
//     const AlignableTransform* pat;
//     AlignableTransform* pat2;
//     bool result=false;
//     if ((pat=cgetTransPtr(key))) {
//       pat2=const_cast<AlignableTransform*>(pat);
//       if (pat2!=0) {
// 	result=pat2->update(ident, Amg::EigenTransformToCLHEP(trans) );
// 	if (!result) ATH_MSG_ERROR( "Attempt to set non-existant transform" );
//       } 
//     } else {
//       ATH_MSG_ERROR( "setTrans: cannot retrieve AlignableTransform for key" << key );
//     }
//   }

//   return result;
// }

// bool InDetAlignDBTool::setTrans(const Identifier& ident, const int level,
//         const Amg::Vector3D& translate, double alpha, double beta, double gamma) const
// {

//   Amg::Translation3D  newtranslation(translate);    
//   Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
//   newtrans *= Amg::AngleAxis3D(gamma, Amg::Vector3D(0.,0.,1.));
//   newtrans *= Amg::AngleAxis3D(beta, Amg::Vector3D(0.,1.,0.));
//   newtrans *= Amg::AngleAxis3D(alpha, Amg::Vector3D(1.,0.,0.));

//   return setTrans(ident, level,  newtrans);
// }


// bool InDetAlignDBTool::tweakTrans(const Identifier& ident, const int level,
//           const Amg::Transform3D& trans) const {

//   bool result=false;

//   // New additions for new global folder structure 
//   // No ATs exist for levels 1 & 2 --> need alternative
//   if (m_dynamicDB && level!=3){
//     result=tweakGlobalFolder(ident, level, trans);
//     if (!result ) ATH_MSG_ERROR( "Attempt tweak GlobalDB folder failed" );
//   }
//   else {
//     // find transform key, then set appropriate transform
//     std::string key=dirkey(ident,level);
//     const AlignableTransform* pat;
//     AlignableTransform* pat2;
//     if ((pat=cgetTransPtr(key))) {
//       pat2=const_cast<AlignableTransform*>(pat);
//       if (pat2!=0) {
// 	result=pat2->tweak(ident,Amg::EigenTransformToCLHEP(trans));
//       if (!result) ATH_MSG_ERROR(
// 				 "Attempt to tweak non-existant transform" );
//       } else {
// 	ATH_MSG_ERROR("tweakTrans: cast fails for key " << key );
//       }
//     } else {
//       ATH_MSG_ERROR(
// 		    "tweakTrans: cannot retrieve AlignableTransform for key" << key );
//     }
//   }

//   return result;
// }

// bool InDetAlignDBTool::tweakTrans(const Identifier& ident, const int level,
//           const Amg::Vector3D& translate, double alpha, double beta, double gamma) const
// {

//   Amg::Translation3D  newtranslation(translate);    
//   Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
//   newtrans *= Amg::AngleAxis3D(gamma, Amg::Vector3D(0.,0.,1.));
//   newtrans *= Amg::AngleAxis3D(beta, Amg::Vector3D(0.,1.,0.));
//   newtrans *= Amg::AngleAxis3D(alpha, Amg::Vector3D(1.,0.,0.));

//   return tweakTrans(ident, level, newtrans);
// }

// /** convert L3 module identifier to L1 or L2 */
// Identifier InDetAlignDBTool::getL1L2fromL3Identifier( const Identifier& ident
//                   , const int& level
//                   ) const {
//   if( level == 3 ) return ident ; //!< no translation needed
//   /** check whether PIX */
//   if( m_pixid->is_pixel(ident) ) {
//     if( level == 1 ) {
//       return m_pixid->wafer_id( 0, 0, 0, 0 ) ; //!< Whole pixel det. at L1
//     }
//     if( level == 2 ) {
//       int barrel_ec  = m_pixid->barrel_ec(  ident ) ;
//       int layer_disk = m_pixid->layer_disk( ident ) ;
//       return m_pixid->wafer_id( barrel_ec, layer_disk, 0, 0 ) ;
//     }
//   }
//   /** check whether SCT */
//   if( m_sctid->is_sct(ident) ) {
//     if( level == 1 ) {
//       int barrel_ec  = m_sctid->barrel_ec(  ident ) ;
//       return m_sctid->wafer_id( barrel_ec, 0, 0, 0, 0 ) ; //!< barrel + 2 x EC at L1
//     }
//     if( level == 2 ) {
//       int barrel_ec  = m_sctid->barrel_ec(  ident ) ;
//       int layer_disk = m_sctid->layer_disk( ident ) ;
//       return m_sctid->wafer_id( barrel_ec, layer_disk, 0, 0, 0 ) ;
//     }
//   }
//   return ident ; //!< take care of the case where level != 1,2,3 or ident neither pix nor sct
// }

// /** get cumulative L1, L2, L3 trafo for (L3-) module */
// Amg::Transform3D InDetAlignDBTool::getTransL123( const Identifier& ident ) const {

//   Amg::Transform3D result ;
//   InDetDD::SiDetectorElement* element = m_pixman->getDetectorElement( ident ) ;
//   if( !element ) {
//     element = m_sctman->getDetectorElement( ident ) ;
//   }
//   if( !element ) {
//     ATH_MSG_ERROR("getTransL123(): Module not found in PIX or SCT!" );
//     return result ;
//   }
//   Amg::Transform3D trfL1 = getTrans( ident, 1 ) ;
//   Amg::Transform3D trfL2 = getTrans( ident, 2 ) ;
//   Amg::Transform3D trfL3 = getTrans( ident, 3 ) ;
//   ATH_MSG_FATAL("Code needs to corrected otherwise you will get nonsensical results-- IndetAlignDBTool:2060");
//   //const Amg::Transform3D trfNominal   ; //= element->defModuleTransform() ;
//   //result = trfNominal.inverse() * trfL1 * trfL2 * trfNominal * trfL3 ;
//   result = trfL1 * trfL2 * trfL3 ;
//   return result ;
// }

// /** return value of particular transform specified by identifier and level
//     calculates L1 and L2 identifiers automatically by getL1L2fromL3Identifier 
//     if L3 identifier passed */
// Amg::Transform3D InDetAlignDBTool::getTrans(const Identifier& ident, 
//             const int level) const {
//   const Identifier identifier = getL1L2fromL3Identifier( ident, level ) ;
//   Amg::Transform3D result;
//   const std::string key=dirkey(identifier,level);
//   const AlignableTransform* pat;
//   if ((pat=cgetTransPtr(key))) {
//     AlignableTransform::AlignTransMem_citr itr=pat->findIdent(identifier);
//     if (itr!=pat->end()) result= Amg::CLHEPTransformToEigen(itr->transform());
//   }
//   return result;
// }

StatusCode TrackerAlignDBTool::outputObjs() const {

  ATH_MSG_DEBUG( "Output AlignableTranform objects to stream" << m_outputTool );
  // get the AthenaOutputStream tool
  //   IAthenaOutputStreamTool* optool {nullptr};

  //   ATH_CHECK(p_toolsvc->retrieveTool("AthenaOutputStreamTool", m_par_condstream, optool));

  //   ATH_CHECK(optool->connectOutput("myAlignFile.pool.root"));
  ATH_CHECK(m_outputTool->connectOutput());

  // construct list of objects to be written out, either 
  // AlignableTransformContainer or several of AlignableTransforms
  int npairs=1;
  IAthenaOutputStreamTool::TypeKeyPairs typekeys(npairs);
  typekeys[0]=
    IAthenaOutputStreamTool::TypeKeyPair("AlignableTransformContainer",
	m_par_dbroot);
  if (!(detStore()->contains<AlignableTransformContainer>(m_par_dbroot)))
    ATH_MSG_ERROR(
	"Expected " << m_par_dbroot << " object not found" );
  // write objects to stream
  //   ATH_CHECK(optool->streamObjects(typekeys, "myAlignFile.pool.root"));
  ATH_CHECK(m_outputTool->streamObjects(typekeys));

  // commit output
  ATH_CHECK(m_outputTool->commitOutput(true));
  ATH_MSG_DEBUG( "Written " << typekeys.size() << " objects to stream " << m_outputTool);
  return StatusCode::SUCCESS;
}

StatusCode TrackerAlignDBTool::fillDB(const std::string tag, 
    const unsigned int run1, const unsigned int event1,
    const unsigned int run2, const unsigned int event2) const 
{

  ATH_MSG_DEBUG( "fillDB: Data tag " << tag );
  ATH_MSG_DEBUG( "Run/evt1 [" << run1 << "," << event1 << "]" );
  ATH_MSG_DEBUG( "Run/evt2 [" << run2 << "," << event2 << "]" );

  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc {nullptr};
  ATH_CHECK(service("IOVRegistrationSvc", regsvc));

  // loop over all AlignableTransform objects created earlier and save them

  ATH_CHECK(regsvc->registerIOV(
	"AlignableTransformContainer",m_par_dbroot, tag, run1, run2, event1, event2));

  ATH_MSG_DEBUG( "Stored AlignableTransform object " << m_par_dbroot );
  ATH_MSG_DEBUG( " Wrote one AlignableTransformContainer objects to conditions database" );
  return StatusCode::SUCCESS;
}

StatusCode TrackerAlignDBTool::printDB(const int level) const 
{

  ATH_MSG_DEBUG("Printout TrackerAlign database contents, detail level" << level );

  for (std::vector<std::string>::const_iterator iobj = m_alignobjs.begin();
      iobj != m_alignobjs.end();
      ++iobj) 
  {
    const AlignableTransform* pat;
    if ((pat = cgetTransPtr(*iobj))) 
    {
      ATH_MSG_DEBUG( "AlignableTransform object " << *iobj );
      int nobj = 0;
      for (AlignableTransform::AlignTransMem_citr cit = pat->begin();
	  cit!=pat->end();
	  ++cit) 
      {
	const Identifier& ident = cit->identify();
	const Amg::Transform3D& trans = Amg::CLHEPTransformToEigen( cit->transform() );
	Amg::Vector3D shift = trans.translation();
	//Amg::RotationMatrix3D rot=trans.rotation();
	int station, layer, eta, phi, side;
	if (idToDetSet(ident, station, layer, eta, phi, side)) 
	{
	  if (level > 1) 
	  {
	    double alpha, beta, gamma;
	    extractAlphaBetaGamma(trans, alpha, beta, gamma);
	    ATH_MSG_DEBUG( "Tracker [" << station << "," << layer << 
		"," << eta << "," << phi << "," << side << "] Trans:(" <<
		shift.x() << "," << shift.y() << "," << shift.z() << ") Rot:{"
		<< alpha << "," << beta << "," << gamma << "}");
	  }
	  ++nobj;
	} 
	else 
	{
	  ATH_MSG_ERROR("Unknown identifier in AlignableTransform" );
	}
      }
      ATH_MSG_DEBUG( "Object contains " << nobj << " transforms" );
    } 
    else
    {
      ATH_MSG_ERROR( "AlignableTransform " << *iobj << " not found" );
    }
  }
  return StatusCode::SUCCESS;
}

// ==========================================

AlignableTransform* TrackerAlignDBTool::getTransPtr(const std::string key) 
  const {
    // look in collection to retrieve pointer to AlignableTransform object of 
    // given key and return it, return 0 if not collection or key value not found
    AlignableTransformContainer* patc;
    AlignableTransform* pat {nullptr};

    if(detStore()->retrieve(patc,m_par_dbroot ).isFailure())
    {
      ATH_MSG_ERROR("Unable to retrieve alignment container from DetStore.");
      return nullptr;
    }

    for (DataVector<AlignableTransform>::iterator dva=patc->begin();
	dva!=patc->end();++dva) 
    {
      if ((*dva)->tag()==key) 
      {
	pat=*dva;
	break;
      }
    }
    return pat;
  }

const AlignableTransform* TrackerAlignDBTool::cgetTransPtr(const std::string key) const 
{
  // look in collection to retrieve pointer to AlignableTransform object of 
  // given key and return it, return 0 if not collection or key value not found
  // const version
  const AlignableTransformContainer* patc;
  const AlignableTransform* pat {nullptr};

  if(detStore()->retrieve(patc, m_par_dbroot ).isFailure())
  {
    ATH_MSG_ERROR("Unable to retrieve alignment container from DetStore.");
    return nullptr;
  }

  for (DataVector<AlignableTransform>::const_iterator dva=patc->begin();
      dva!=patc->end(); 
      ++dva) 
  {
    if ((*dva)->tag() == key) 
    {
      pat=*dva;
      break;
    }
  }
  return pat;
}

StatusCode TrackerAlignDBTool::sortTrans() const 
{
  // loop through all the AlignableTransform objects and sort them

  ATH_MSG_DEBUG( "Sorting all AlignableTransforms in TDS" );
  AlignableTransform* pat;
  // use cget and a const cast to allow containers that have been read in
  // (and hence are locked by StoreGate) to be sorted
  for (unsigned int i=0; i<m_alignobjs.size(); ++i)
    if ((pat = const_cast<AlignableTransform*>(cgetTransPtr(m_alignobjs[i])))) pat->sortv();

  return StatusCode::SUCCESS;
}

void TrackerAlignDBTool::extractAlphaBetaGamma(const Amg::Transform3D & trans, 
    double& alpha, double& beta, double &gamma) const 
{
  double siny = trans(0,2);
  beta = asin(siny);
  // Check if cosy = 0. This requires special treatment.
  // can check either element (1,2),(2,2) both equal zero
  // or (0,1) and (0,0)
  // Probably not likely it will be exactly 0 and may still 
  // have some problems when very close to zero. We mostly
  // deal with small rotations so its not too important.
  if ((trans(1,2) == 0) && (trans(2,2) == 0)) {
    // alpha and gamma are degenerate. We arbitrarily choose
    // gamma = 0.
    gamma = 0;
    alpha = atan2(trans(1,1),trans(2,1));
  } else {
    alpha = atan2(-trans(1,2),trans(2,2));
    gamma = atan2(-trans(0,1),trans(0,0));
    if (alpha == 0) alpha = 0; // convert -0 to 0
    if (gamma == 0) gamma = 0; // convert -0 to 0
  }
}


// bool InDetAlignDBTool::tweakGlobalFolder(const Identifier& ident, const int level,
// 					 const Amg::Transform3D& trans ) const {

//   // find transform key, then set appropriate transform           
//   const CondAttrListCollection* atrlistcol1=0;
//   CondAttrListCollection* atrlistcol2=0;
//   bool result=false;
//   std::string key=dirkey(ident,level);
//   int det,bec,layer,ring,sector,side;
//   idToDetSet(ident,det,bec,layer,ring,sector,side);
//   const unsigned int DBident=det*10000+2*bec*1000+layer*100+ring*10+sector;  
//   // so far not a very fancy DB identifier, but seems elaborate enough for this simple structure

//   if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol1,key)) {
//     // loop over objects in collection                                    
//     //atrlistcol1->dump();
//     atrlistcol2 = const_cast<CondAttrListCollection*>(atrlistcol1);
//     if (atrlistcol2!=0){
//       for (CondAttrListCollection::const_iterator citr=atrlistcol2->begin(); citr!=atrlistcol2->end();++citr) {

// 	const coral::AttributeList& atrlist=citr->second;
// 	coral::AttributeList& atrlist2  = const_cast<coral::AttributeList&>(atrlist);

// 	if(citr->first!=DBident) continue;
// 	else {
// 	  msg(MSG::DEBUG) << "Tweak Old global DB -- channel: " << citr->first
//                           << " ,det: "    << atrlist2["det"].data<int>()
//                           << " ,bec: "    << atrlist2["bec"].data<int>()
//                           << " ,layer: "  << atrlist2["layer"].data<int>()
//                           << " ,ring: "   << atrlist2["ring"].data<int>()
//                           << " ,sector: " << atrlist2["sector"].data<int>()
//                           << " ,Tx: "     << atrlist2["Tx"].data<float>()
//                           << " ,Ty: "     << atrlist2["Ty"].data<float>()
//                           << " ,Tz: "     << atrlist2["Tz"].data<float>()
//                           << " ,Rx: "     << atrlist2["Rx"].data<float>()
//                           << " ,Ry: "     << atrlist2["Ry"].data<float>()
//                           << " ,Rz: "     << atrlist2["Rz"].data<float>() << endmsg;


// 	  // Order of rotations is defined as around z, then y, then x.  
// 	  Amg::Translation3D  oldtranslation(atrlist2["Tx"].data<float>(),atrlist2["Ty"].data<float>(),atrlist2["Tz"].data<float>());
// 	  Amg::Transform3D oldtrans = oldtranslation * Amg::RotationMatrix3D::Identity();
// 	  oldtrans *= Amg::AngleAxis3D(atrlist2["Rz"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,0.,1.));
// 	  oldtrans *= Amg::AngleAxis3D(atrlist2["Ry"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,1.,0.));
// 	  oldtrans *= Amg::AngleAxis3D(atrlist2["Rx"].data<float>()*CLHEP::mrad, Amg::Vector3D(1.,0.,0.));

// 	  // get the new transform
// 	  Amg::Transform3D newtrans = trans*oldtrans;

// 	  // Extract the values we need to write to DB     
// 	  Amg::Vector3D shift=newtrans.translation();
// 	  double alpha, beta, gamma;
// 	  extractAlphaBetaGamma(newtrans, alpha, beta, gamma);

// 	  atrlist2["Tx"].data<float>() = shift.x();
// 	  atrlist2["Ty"].data<float>() = shift.y();
// 	  atrlist2["Tz"].data<float>() = shift.z();
// 	  atrlist2["Rx"].data<float>() = alpha/CLHEP::mrad ;
// 	  atrlist2["Ry"].data<float>() = beta/CLHEP::mrad ;
// 	  atrlist2["Rz"].data<float>() = gamma/CLHEP::mrad ;

// 	  result = true;
// 	  msg(MSG::DEBUG) << "Tweak New global DB -- channel: " << citr->first
//                           << " ,det: "    << atrlist2["det"].data<int>()
//                           << " ,bec: "    << atrlist2["bec"].data<int>()
//                           << " ,layer: "  << atrlist2["layer"].data<int>()
//                           << " ,ring: "   << atrlist2["ring"].data<int>()
//                           << " ,sector: " << atrlist2["sector"].data<int>()
//                           << " ,Tx: "     << atrlist2["Tx"].data<float>()
//                           << " ,Ty: "     << atrlist2["Ty"].data<float>()
//                           << " ,Tz: "     << atrlist2["Tz"].data<float>()
//                           << " ,Rx: "     << atrlist2["Rx"].data<float>()
//                           << " ,Ry: "     << atrlist2["Ry"].data<float>()
//                           << " ,Rz: "     << atrlist2["Rz"].data<float>() << endmsg;

// 	}	
//       }
//     }
//     else {
//       ATH_MSG_ERROR("tweakGlobalFolder: cast fails for DBident " << DBident );
//       return false;
//    }
//   }
//   else {
//     ATH_MSG_ERROR("tweakGlobalFolder: cannot retrieve CondAttrListCollection for key " << key );
//     return false;
//   }

//   return result;  
// }

