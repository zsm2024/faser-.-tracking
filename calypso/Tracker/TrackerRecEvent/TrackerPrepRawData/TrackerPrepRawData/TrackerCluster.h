/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerCluster.h
//   Header file for class TrackerCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Tracker
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERPREPRAWDATA_TRACKERCLUSTER_H
#define TRACKERPREPRAWDATA_TRACKERCLUSTER_H

// Base class
#include "TrkPrepRawData/PrepRawData.h"

#include "Identifier/Identifier.h"
#include "TrackerPrepRawData/FaserSiWidth.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h" // cant be forward declared

#include "TrkSurfaces/Surface.h"

#include <memory>


class MsgStream;

class FaserSCT_ClusterContainerCnv;
class FaserSCT_ClusterContainerCnv_p1;
class TrackerClusterCnv_p1;
class FaserSCT_ClusterContainerCnv_p0;

namespace Tracker 
{
	class TrackerCluster : public Trk::PrepRawData {

 		friend class ::FaserSCT_ClusterContainerCnv;
        friend class FaserSCT_ClusterContainerCnv_p1;
        friend class ::TrackerClusterCnv_p1;
        friend class ::FaserSCT_ClusterContainerCnv_p0;		

		///////////////////////////////////////////////////////////////////
		// Public methods:
		///////////////////////////////////////////////////////////////////
		public:
		
		/** Public, Copy, operator=, constructor*/
		TrackerCluster() = default;
		TrackerCluster(const TrackerCluster &) = default;
		TrackerCluster(TrackerCluster &&) = default;
		TrackerCluster &operator=(const TrackerCluster &) = default;
		TrackerCluster &operator=(TrackerCluster &&) = default;
		
		/**
		 * Constructor with parameters using ref or omitting  Amg::MatrixX.
		 * If omitted we have any empty one.
		 */
		TrackerCluster(const Identifier& RDOId,
					const Amg::Vector2D& locpos,
					const std::vector<Identifier>& rdoList,
					const FaserSiWidth& width,
					const TrackerDD::SiDetectorElement* detEl,
					const Amg::MatrixX& locErrMat);

		TrackerCluster(const Identifier& RDOId,
					const Amg::Vector2D& locpos,
					const std::vector<Identifier>& rdoList,
					const FaserSiWidth& width,
					const TrackerDD::SiDetectorElement* detEl);

		TrackerCluster(const Identifier& RDOId,
					const Amg::Vector2D& locpos,
					const Amg::Vector3D& globpos,
					const std::vector<Identifier>& rdoList,
					const FaserSiWidth& width,
					const TrackerDD::SiDetectorElement* detEl,
					const Amg::MatrixX& locErrMat);

		TrackerCluster(const Identifier& RDOId,
					const Amg::Vector2D& locpos,
					const Amg::Vector3D& globpos,
					const std::vector<Identifier>& rdoList,
					const FaserSiWidth& width,
					const TrackerDD::SiDetectorElement* detEl);

		/**
		 * Constructor with parameters using r-value reference of Amg::MatrixX.
		 * All parameters have to be given!
		 */
		TrackerCluster(const Identifier& RDOId,
					const Amg::Vector2D& locpos,
					std::vector<Identifier>&& rdoList,
					const FaserSiWidth& width,
					const TrackerDD::SiDetectorElement* detEl,
					Amg::MatrixX&& locErrMat);

		TrackerCluster(const Identifier& RDOId,
					const Amg::Vector2D& locpos,
					const Amg::Vector3D& globpos,
					std::vector<Identifier>&& rdoList,
					const FaserSiWidth& width,
					const TrackerDD::SiDetectorElement* detEl,
					Amg::MatrixX&& locErrMat);


			// Destructor:
		virtual ~TrackerCluster();
		
		///////////////////////////////////////////////////////////////////
		// Virtual methods 
		///////////////////////////////////////////////////////////////////
		
		/** return width class reference */
		virtual const FaserSiWidth&  width()  const;
		
		/** return global position reference */
		virtual const Amg::Vector3D& globalPosition() const;
		
		/** return the detector element corresponding to this PRD
		The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
		virtual const TrackerDD::SiDetectorElement* detectorElement() const override final;

		/** Interface method checking the type*/
		virtual bool type(Trk::PrepRawDataType type) const override final;

		/** dump information about the SiCluster*/
		virtual MsgStream&    dump( MsgStream&    stream) const override;
		/** dump information about the SiCluster*/
		virtual std::ostream& dump( std::ostream& stream) const override;

		/// Set the m_detEl and calculate globalPostion
		//used by TPCnv converters
		void setDetectorElement(const TrackerDD::SiDetectorElement* detEl);

		private:
	    Amg::Vector3D m_globalPosition = Amg::Vector3D::Zero();
  		/// col, row, and width in mm
		FaserSiWidth m_width; //col, row, and width in mm
		const TrackerDD::SiDetectorElement* m_detEl = nullptr;

		// Identifier m_clusId; 
		// /**see derived classes for definition of meaning of LocalPosition*/
		// // Need to force proper alignment; otherwise cling gets it wrong.
		// alignas(16) Amg::Vector2D m_localPos;
		// /**Stores the identifiers of the RDOs.*/
		// std::vector<Identifier> m_rdoList; 
		// /**See derived classes for definition of ErrorMatrix */
		// const Amg::MatrixX* m_localCovariance;

		// /**Stores its own position (index) in collection plus the hash id for the collection 
		//   (needed for the EL to IDC) */
		// IdentContIndex m_indexAndHash;

	};

	MsgStream&    operator << (MsgStream& stream,    const TrackerCluster& prd);
	std::ostream& operator << (std::ostream& stream, const TrackerCluster& prd);

}
#include "TrackerCluster.icc"
#endif // TRACKERPREPRAWDATA_TRACKERCLUSTER_H
