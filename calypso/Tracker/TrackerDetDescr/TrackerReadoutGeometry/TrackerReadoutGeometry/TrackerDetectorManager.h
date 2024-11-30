/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerDectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERREADOUTGEOMETRY_TRACKERDETECTORMANAGER_H
#define TRACKERREADOUTGEOMETRY_TRACKERDETECTORMANAGER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// GeoModel stuff
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "TrackerReadoutGeometry/TrackerDD_Defs.h"
#include "TrackerReadoutGeometry/Version.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "AthenaBaseComps/AthMessaging.h"

// IOV SVC for alignment:
#include "AthenaKernel/IIOVSvc.h"

#include "DetDescrConditions/AlignableTransformContainer.h"

#include "CxxUtils/checker_macros.h"

#include <atomic>
#include <string>
#include <map>
#include <set>
#include <list>

class StoreGateSvc;
class AlignableTransform;
class Identifier; 
class FaserDetectorID;
class GeoVAlignmentStore;
class CondAttrListCollection;

namespace TrackerDD {

  typedef std::map<std::string, const void*> RawAlignmentObjects;

    /** @class TrackerDetectorManager
    
        Virtual base class for all Tracker detector managers.
        
        It implements the processKey() method for alingment
        which calls the setAlignableTransformDelta() method which
        is specified in the extended classes. This method supports both,
        local and global delta's in the frame and translates it to the 
        underlying GeoModel transform. As GeoModel (CLHEP) and tracking
        (Amg) use different geo libraries, these are the methods that
        act as the CLHEP <--> Amg interface
        
        @author: Grant Gorfine
        - modified & maintained: Nick Styles & Andreas Salzburger 
    */
    class TrackerDetectorManager : public GeoVDetectorManager, public AthMessaging  {
    
    public:
    
      // Constructor
      TrackerDetectorManager(StoreGateSvc * detStore, const std::string & name);
     
      // Destructor
      virtual ~TrackerDetectorManager();
    
      
      /** Get version information */
      const Version & getVersion() const; 
      const std::string & getLayout() const; // eg Initial, Final, TestBeam 
      void setVersion(const Version & version); 
    
      /** Alignment access */
      void addChannel(const std::string & key, int level, FrameType frame);
      void addFolder(const std::string & key);
      void addSpecialFolder(const std::string & key);
      void addGlobalFolder(const std::string & key); 
      void addAlignFolderType(const AlignFolderType alignfolder);

      StatusCode align( IOVSVC_CALLBACK_ARGS ) const;

      StatusCode align(const RawAlignmentObjects& alignObjects, GeoVAlignmentStore* alignStore) const;
    
      /** Invalidate cache for all detector elements */
      virtual void invalidateAll() const = 0;
    
      /** Update all caches */
      virtual void updateAll() const = 0;
    
      /** Check identifier is for this detector */
      virtual bool identifierBelongs(const Identifier & id) const = 0;
    
      AlignFolderType                           m_alignfoldertype;
    
    protected:
      StoreGateSvc * m_detStore;
    
    private:
      /** @class LevelInfo
         Private helper class definition.
         */
      class LevelInfo {

        private:
          int m_level;
          FrameType m_type;
        
        public:
          LevelInfo(): m_level(-1), m_type(TrackerDD::global) {};
          LevelInfo(int level, FrameType frame): m_level(level), m_type(frame) {};
        
          int level() const {return m_level;}
          FrameType frame() const {return m_type;} 
          bool isGlobalDelta() const {return m_type == TrackerDD::global;}
          bool isLocalDelta() const {return m_type == TrackerDD::local;} 
          bool isValid() const {return (m_level >= 0);}
        
      };

      class AlignInfo {

        private:
        AlignFolderType m_aligntype;

        public:
          AlignInfo(): m_aligntype(TrackerDD::none) {};
          AlignInfo(AlignFolderType alignfolder): m_aligntype(alignfolder) {};
          AlignFolderType AlignFolder() const {return m_aligntype;}
          bool isValidAlign() const {return (m_aligntype != TrackerDD::none);}

      };

    
      /** Retrieve level information */
      const LevelInfo & getLevel(const std::string & key) const;

      /** return align folder string to use **/
      //      TrackerDD::AlignFolderType getAlignInfo();

      /** Process the alignment container, calls processKey */
      bool processAlignmentContainer(const std::string & key) const;
      bool processAlignmentContainer(const AlignableTransformContainer* container, GeoVAlignmentStore* alignStore) const;

      /** Called by processAlignmentContainer, 
          applies only one key on the transform Collections */
      bool processKey(const std::string key, 
                      const AlignableTransform* transformCollection,
                      GeoVAlignmentStore* alignStore=nullptr) const;
    
      /** Set method applying the delta transform (in global or local frame)
          onto the geoModel transform : CLHEP <--> Amg interface */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore=nullptr) const = 0;

      virtual bool processSpecialAlignment(const std::string & key,
                                           TrackerDD::AlignFolderType alignfolder) const = 0;

      virtual bool processSpecialAlignment(const std::string& key,
                                           const CondAttrListCollection* obj=nullptr,
                                           GeoVAlignmentStore* alignStore=nullptr) const = 0;

      bool processGlobalAlignmentContainer(const std::string & key,
                                           const CondAttrListCollection* obj=nullptr,
                                           GeoVAlignmentStore* alignStore=nullptr) const;

      virtual bool processGlobalAlignment(const std::string & key, int level, FrameType frame,
                                          const CondAttrListCollection* obj=nullptr,
                                          GeoVAlignmentStore* alignStore=nullptr) const;
      
      virtual const FaserDetectorID* getIdHelper() const = 0;
         
      Version                                   m_version;
      std::map<std::string, LevelInfo>          m_keys;
      std::set<std::string>                     m_folders;
      std::set<std::string>                     m_specialFolders;
      std::set<std::string>                     m_globalFolders; // new time-dependent global folders

      static const LevelInfo s_invalidLevel;
    };

} // namespace TrackerDD

#endif // TRACKERREADOUTGEOMETRY_TRACKERDETECTORMANAGER_H
