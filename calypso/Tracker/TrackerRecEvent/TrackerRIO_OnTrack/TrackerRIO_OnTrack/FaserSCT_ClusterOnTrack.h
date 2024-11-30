/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT_ClusterOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRIO_ONTRACK_FASERSCTCLUSTERONTRACK_H
#define TRACKERRIO_ONTRACK_FASERSCTCLUSTERONTRACK_H

// Base classes
#include "TrackerRIO_OnTrack/SiClusterOnTrack.h"

// for ElementLink to IdentifiableContainer SCT_ClusterContainer
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthLinks/ElementLink.h"


class FaserSCT_Cluster;

namespace Trk {
  class Surface;
  class LocalParameters;
  class TrkDetElementBase;
  class ITrkEventCnvTool;
}

namespace TrackerDD {
   class SiDetectorElement;
}

typedef ElementLink<Tracker::FaserSCT_ClusterContainer> ElementLinkToTrackerCFaserSCT_ClusterContainer;

namespace Tracker{

  /**@class FaserSCT_ClusterOnTrack
  Specific class to represent the SCT measurements.
  It does not currently extend the interface of Tracker::SiClusterOnTrack.
     
  @author Veronique.Boisvert@cern.ch, Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch, dcasper@uci.edu
   */
  class FaserSCT_ClusterOnTrack final : public SiClusterOnTrack {

    public:
      friend class  Trk::ITrkEventCnvTool;
      /**For POOL only. Do not use*/
      FaserSCT_ClusterOnTrack();
      /**Copy constructor*/
      FaserSCT_ClusterOnTrack(const FaserSCT_ClusterOnTrack &) = default;
     
    /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      FaserSCT_ClusterOnTrack( const Tracker::FaserSCT_Cluster* RIO, 
                               Trk::LocalParameters&& locpars, 
                               Amg::MatrixX&& locerr, 
                               const IdentifierHash& idDE,
                               bool isbroad=false); 
                        
	  /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&,
      Global Position
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      FaserSCT_ClusterOnTrack( const Tracker::FaserSCT_Cluster* RIO, 
                               Trk::LocalParameters&& locpars, 
                               Amg::MatrixX&& locerr, 
                               const IdentifierHash& idDE,
                               const Amg::Vector3D& globalPosition,
                               bool isbroad=false); 

      /*
       * Constuctor used by P->T converter.
       * The P->T converter calls the
       * setValues method to complete the object
       * e.g set/reset the DetectorElement 
       */
      FaserSCT_ClusterOnTrack( const ElementLinkToTrackerCFaserSCT_ClusterContainer& RIO,
                               const Trk::LocalParameters& locpars, 
                               const Amg::MatrixX& locerr, 
                               IdentifierHash idDE,
                               const Identifier& id,
                               bool isbroad,
                               double positionAlongStrip);

      /**Assignment operator*/
      FaserSCT_ClusterOnTrack &operator=(const FaserSCT_ClusterOnTrack &) = default;
      /**Default move assigment operator*/
      FaserSCT_ClusterOnTrack &operator=(FaserSCT_ClusterOnTrack &&) = default;

      /** Destructor */
      virtual ~FaserSCT_ClusterOnTrack() = default;
     
      /** Pseudo-constructor */
      virtual FaserSCT_ClusterOnTrack* clone() const override final;

      /** returns the surface for the local to global transformation
          - fullfills the Trk::MeasurementBase interface
      */
      virtual const Trk::Surface& associatedSurface() const override final;

      virtual bool rioType(Trk::RIO_OnTrackType::Type type) const override final
      {
        return (type == Trk::RIO_OnTrackType::SCTCluster);
      }


    /** returns the PrepRawData - is a SCT_Cluster in this scope
      - fullfills the Trk::RIO_OnTrack interface
     */
      virtual const Tracker::FaserSCT_Cluster* prepRawData() const override final;

    const ElementLinkToTrackerCFaserSCT_ClusterContainer& prepRawDataLink() const;
     
  
    /** returns the detector element, assoicated with the PRD of this class
      - fullfills the Trk::RIO_OnTrack interface
     */
      virtual const TrackerDD::SiDetectorElement* detectorElement() const override final;
    
      /**returns some information about this RIO_OnTrack.*/
      virtual MsgStream&    dump( MsgStream& out ) const override final;	
	
      /**returns some information about this RIO_OnTrack.*/
      virtual std::ostream& dump( std::ostream& out ) const override final;

      double positionAlongStrip() const;

    private:
      /** ONLY for use in custom convertor
        Allows the custom convertor to reset values when persistying/reading
        back RoTs*/
      virtual void setValues(const Trk::TrkDetElementBase* detEl,
                             const Trk::PrepRawData* prd) override final;

      /** SCT_Cluster - the RIO (PRD, PrepRawData)*/
      ElementLinkToTrackerCFaserSCT_ClusterContainer m_rio;
      /** corresponding detector element*/
      const TrackerDD::SiDetectorElement* m_detEl;

      double m_positionAlongStrip;
  };

  inline FaserSCT_ClusterOnTrack* FaserSCT_ClusterOnTrack::clone() const 
  { 
    return new FaserSCT_ClusterOnTrack(*this); 
  }
    
  inline const FaserSCT_Cluster* FaserSCT_ClusterOnTrack::prepRawData() const
  { 
    // somehow one has to ask first if it is valid ... otherwise it always returns 0 ...
    if (m_rio.isValid()) return m_rio.cachedElement();
    else return 0;
  }  
  
  inline const ElementLinkToTrackerCFaserSCT_ClusterContainer&
  FaserSCT_ClusterOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }
    
  inline const TrackerDD::SiDetectorElement* FaserSCT_ClusterOnTrack::detectorElement() const
  { 
    return m_detEl; 
  }

  inline double FaserSCT_ClusterOnTrack::positionAlongStrip() const
  {
    return m_positionAlongStrip;
  }
      

}//end of namespace definitions

#endif // TRACKERRIO_ONTRACK_FASERSCTCLUSTERONTRACK_H
