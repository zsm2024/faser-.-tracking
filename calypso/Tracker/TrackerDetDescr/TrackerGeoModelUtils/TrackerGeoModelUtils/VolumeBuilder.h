/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackerGeoModelUtils_VolumeBuilder_H
#define TrackerGeoModelUtils_VolumeBuilder_H

#include "TrackerGeoModelUtils/VolumeSplitterUtils.h"
#include "TrackerGeoModelUtils/VolumeSplitter.h"
#include "AthenaBaseComps/AthMessaging.h"

#include <string>
#include <vector>

class TrackerMaterialManager;
class GeoPhysVol;
class GeoFullPhysVol;
class GeoVPhysVol;
class GeoTransform;
class GeoShape;

namespace TrackerDD {
  class ServiceVolume;
 
  class VolumeBuilder : public AthMessaging
  {
  public:
    VolumeBuilder( const Zone & zone, const std::vector<const ServiceVolume * > & services);
    VolumeBuilder(const std::vector<const ServiceVolume * > & services);
    VolumeBuilder( const Zone & zone, const std::vector<const ServiceVolume * > & services,
		   const std::vector<const ServiceVolume * > & servEnv, const std::vector<const ServiceVolume * > & servChild );

    void setRegion(const std::string & region, double zcenter);
    void setMaterialManager(TrackerMaterialManager * matManager) {m_matManager = matManager; }
    const std::vector<const ServiceVolume *> & services();
    const std::vector<const ServiceVolume * > & servicesEnv() ;
    const std::vector<const ServiceVolume * > & servicesChild() ;

    void buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter = 0);
    void buildAndPlace(const std::string & region, GeoFullPhysVol * parent, double zcenter = 0);
    void buildAndPlaceEnvelope(const std::string & region, GeoFullPhysVol * parent, int iParent, int iElement, double zcenter = 0);
    void buildAndPlaceEnvelope(const std::string & region, GeoPhysVol * parent, int iParent, int iElement, double zcenter = 0);
    void addServices(const Zone & zone, const std::vector<const ServiceVolume * > & services);
    GeoVPhysVol* build(int iElement);
    int numCopies(int iElement);
    GeoTransform * getPlacement(int iElement, int iCopy);
    GeoTransform * getPlacementEnvelope(int iElement, int iCopy,  int iMothElement);

    bool isEnvelopeOrChild(int iElement);
    int  getEnvelopeNum(int iElement);
    int  getParentNum(int iElement);
    bool isChildService(int iElt,int iChld);
    double getZcenter(int iElt);

  private:
    //const GeoShape * getShape(const ServiceVolume & param, double & volume);
    
    std::string m_region;
    double m_zcenter;
    const std::vector<const ServiceVolume *>* m_services;
    const std::vector<const ServiceVolume *>* m_servEnvelope;
    const std::vector<const ServiceVolume *>* m_servChild;
    VolumeSplitter m_splitter;
    TrackerMaterialManager * m_matManager;

  };
 
}
#endif // TrackerGeoModelUtils_VolumeBuidler_H
