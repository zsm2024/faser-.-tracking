/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERALIGNGENTOOLS_ALIGNDBTOOL_H
#define TRACKERALIGNGENTOOLS_ALIGNDBTOOL_H
// TrackerAlignDBTool.h
// an AlgTool to manage the tracker alignment database classes
// Richard Hawkings, started 8/4/05
//
// This AlgTool provides utilities to manage the Tracker alignment conditions
// data in the transient store, and to read and write it in a variety of
// formats
// Tool methods provided:
//   createDB - create a 'null' set of AlignableTransforms, with no shifts
//   dispGroup - apply a pattern to displace a group of modules randomly
//               or systematically
//   printDB - printout details of all the module transforms
//   writeFile - export database contents on a text file or ntuple
//   readTextFile - import database from text file
//   readNtuple - import database from ntuple
//
// The following utility methods are also exposed
//   idToDetSet - convert an SCT/pixel Identifier to set of integers
//         specifying detector,barrel/ec,layer,ring, sector (and side for SCT)
//   dirkey - return the AlignableTransform TDS key for a given module
//            (by Identifier or set of integers)
//   setTrans - set the transformation for a particular module, identified by 
//              a given Identifier
//   tweakTrans - tweak (add to existing transform) the transformation for
//              a particular module, identified by a given Identifier

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "TrackerAlignGenTools/ITrackerAlignDBTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"


class FaserSCT_ID;
class AlignableTransform;

namespace TrackerDD {
  class SCT_DetectorManager;
}

class TrackerAlignDBTool: virtual public ITrackerAlignDBTool, public AthAlgTool {
 public:
  TrackerAlignDBTool(const std::string& type, const std::string& name,
	    const IInterface* parent);
  virtual ~TrackerAlignDBTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // tool methods
  // create null database in TDS for subsequent manipulation

  virtual StatusCode createDB() const;

  // displace a group of modules
  // modules to effect are specified by dettype (1=pixel, 2=SCT, -1 both),
  // bec (barrel=0, 2 for both endcaps, -1 for all), layer (0 to n-1, -1=all)
  // ring (-1=all) and sector (-1=all)
  // rphidisp,rdisp and zdisp specify the offsets to be used
  // syst specifies what will be done, 1=systematic displacement of affected
  // modules, 2=random (rphidisp etc are RMS), 3,4=systematic and random
  // displacements but interpreting r/phi/zdisp as x/y/z, 5 chooses the
  // same randomised numbers for all modules;
  // level specifies the level oill look for the right balance of CERN staff supervisors and ATLAS
  //USERS supervisors; f transformations affected
  // skip=n optionally skips n random numbers to enable different patterns

//   virtual void dispGroup(const int dettype, const int bec, const int layer,
//                           const int ring, const int sector,
//                           const float rphidisp, const float rdisp, const float zdisp, 
//                           const int syst, const int level, const int skip) const;

  // write database contents to flat text file or ntuple
  // for flat file, file gives filename, for ntuple, file is e.g. 
  //  /NTUPLES/FILE1
//   virtual void writeFile(const bool ntuple, const std::string file) const;

  // write GlobalFolder txt files
//   virtual void writeGlobalFolderFile( const std::string file) const;

  // read back database from text file
//   virtual void readTextFile(const std::string file) const;

  // read back database from text file
  //  void readOldTextFile(const std::string file) const;

  // read back database from ntuple
//   virtual void readNtuple(const std::string file) const;

  // set a particular transform specified by Identifier
//   virtual bool setTrans(const Identifier& ident, const int level,
//     const Amg::Transform3D& trans) const;

  // As above but takes translation and rotations alpha, beta, gamma (rotations around x,y,z axes) as input.
  // Calculates transform as HepGeom::Translate3D(translate) * HepGeom::RotateX3D(alpha) * HepGeom::RotateY3D(beta) * HepGeom::RotateZ3D(gamma)
//   virtual bool setTrans(const Identifier& ident, const int level,
// 			const Amg::Vector3D& translate, double alpha, double beta, double gamma) const;


  // tweak a particular transform specified by Identifier (i.e. add to 
  //   already existing transformation)
//   virtual bool tweakTrans(const Identifier& ident, const int level,
//     const Amg::Transform3D& trans) const;

  // As above but takes translation and rotations alpha, beta, gamma (rotations around x,y,z axes) as input.
  // Calculates transform as HepGeom::Translate3D(translate) * HepGeom::RotateX3D(alpha) * HepGeom::RotateY3D(beta) * HepGeom::RotateZ3D(gamma)
//   virtual bool tweakTrans(const Identifier& ident, const int level,
//                         const Amg::Vector3D& translate, double alpha, double beta, double gamma) const;

  /** This is the tweak function for the GlobalFolder DB **/
//   virtual bool tweakGlobalFolder(const Identifier& ident, const int level,
// 				 const Amg::Transform3D& trans) const ;

  /** convert L3 module identifier to L1 or L2 */
//   virtual Identifier getL1L2fromL3Identifier( const Identifier& ident
//                                             , const int& level
//                                             ) const ;

  /** get cumulative L1, L2, L3 trafo for (L3-) module. Result is in local frame. */
//   virtual Amg::Transform3D getTransL123( const Identifier& ident ) const ;

  /** return value of particular transform specified by identifier and level
  calculates L1 and L2 identifiers automatically by getL1L2fromL3Identifier 
  if L3 identifier passed. L1, L2 are in global, L3 in local frame. */
//   virtual Amg::Transform3D getTrans( const Identifier& ident
//                                  , const int level
//                                  ) const ;

  // write the transforms to outputstream
  virtual StatusCode outputObjs() const;

  // write the transform IOVs to IOVDB
  virtual StatusCode fillDB(const std::string tag, 
     const unsigned int run1, const unsigned int event1,
     const unsigned int run2, const unsigned int event2) const;

  // print the transforms, level=1 lists sizes, level=2 lists all transforms
  virtual StatusCode printDB(const int level) const;

  // sort all the AlignableTransform objects so searches/inserts work properly
  virtual StatusCode sortTrans() const;
  
  
 private:

  // convert an Identifier to a set of integers specifying station
  // layer, ring (eta), sector (phi), and side (0/1)
  virtual bool idToDetSet(const Identifier ident,
          int& bec,int& layer,int& ring,int& sector,int& side) const;

  // return the AlignableTransform name where the transform for the given
  // module can be found, 3 levels corresponding to alignment hierarchy
  virtual std::string dirkey(const Identifier&, const int) const;
  virtual std::string dirkey(const int,const int, const int, const int) const;

   //calculate three rotations around locX,locY,locY = alpha,beta,gamma out of an HepGeom::Transform3D
  void extractAlphaBetaGamma(const Amg::Transform3D & trans, 
                             double& alpha, double& beta, double &gamma) const; 

  ServiceHandle < IToolSvc > p_toolsvc;

  const FaserSCT_ID* m_sctid;
  const TrackerDD::SCT_DetectorManager* m_sctman;

  std::vector<std::string> m_alignobjs;
  std::vector<int> m_alignchans;

  bool m_par_scttwoside; // create structures with separated SCT module sides
  /** name of the root folder for constants, which can be set via 
      the <key> syntax. Default: /Tracker/Align. */
  std::string m_par_dbroot;
  /** the base part of the key for loading AlignableTransform objects
      from the Transient Data Store. Default: /Tracker/Align */
  std::string m_par_dbkey;

  AlignableTransform* getTransPtr(const std::string key) const;
  const AlignableTransform* cgetTransPtr(const std::string key) const;
  bool m_dynamicDB;
  bool m_forceUserDBConfig;
  std::set<int> m_stations;
  Gaudi::Property<std::map<std::string, std::vector<double>>> m_alignment{ this, "AlignmentConstants", {}, "Alignment constants."};

  
  mutable  ToolHandle<IAthenaOutputStreamTool> m_outputTool { this, "OutputTool", "AthenaOutputStreamTool/CondStream1"} ;

};

#endif // TRACKERALIGNGENTOOLS_ALIGNDBTOOL_H
