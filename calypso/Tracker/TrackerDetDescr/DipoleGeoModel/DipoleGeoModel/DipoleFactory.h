#ifndef DIPOLEGEOMODEL_DIPOLEFACTORY_H
#define DIPOLEGEOMODEL_DIPOLEFACTORY_H


#include "TrackerGeoModelUtils/TrackerDetectorFactoryBase.h"
#include "DipoleGeoModel/DipoleManager.h"

namespace TrackerDD {
  class AthenaComps;
}

class DipoleDataBase;

class DipoleFactory : public TrackerDD::DetectorFactoryBase  {

 public:
  
  // Constructor:
  DipoleFactory(const TrackerDD::AthenaComps * athenaComps);

  // Destructor:
  ~DipoleFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  // manager
  virtual const TrackerDD::DipoleManager* getDetectorManager () const;

private:  
  
  // Illegal operations:
  const DipoleFactory & operator=(const DipoleFactory &right);
  DipoleFactory(const DipoleFactory &right);

  // private data
  TrackerDD::DipoleManager   *m_manager;
  DipoleDataBase*             m_db;
};

#endif //  DIPOLEGEOMODEL_DIPOLEFACTORY_H


