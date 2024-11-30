/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "CaloReadoutGeometry/CaloDetectorManager.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelKernel/GeoXF.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "CaloReadoutGeometry/CaloDetectorElementCollection.h"
#include "CaloReadoutGeometry/CaloDetectorElement.h"
#include "CaloReadoutGeometry/ExtendedAlignableTransform.h"

#include <iostream>

namespace CaloDD
{

    CaloDetectorManager::CaloDetectorManager(StoreGateSvc * detStore, const std::string & name)
        : CaloDetectorManagerBase(detStore, name),
        m_design {nullptr}
    {
  // Add default folder
        // addFolder("/Calo/Align");
    }

    const std::string& CaloDetectorManager::tag() const
    {
        return m_tag;
    }

    void CaloDetectorManager::invalidateAll() const
    {
        for (CaloDetectorElementCollection::const_iterator element_iter = getDetectorElementBegin();
        element_iter != getDetectorElementEnd();
        ++element_iter) {

            if (*element_iter) {
                (*element_iter)->invalidate();
            }
        }
    }

    void CaloDetectorManager::updateAll() const
    {
        for (CaloDetectorElementCollection::const_iterator element_iter = getDetectorElementBegin();
        element_iter != getDetectorElementEnd();
        ++element_iter) {
            if (*element_iter) {
                (*element_iter)->setAllCaches();
            }
        }
    }

    bool CaloDetectorManager::setAlignableTransformLocalDelta(ExtendedAlignableTransform * extXF, 
                                                            const Amg::Transform3D & localToGlobalXF,
                                                            const Amg::Transform3D & delta,
                                                            GeoVAlignmentStore* alignStore) const
    {
        // ATTENTION -------------------------------------------------------- (A.S.)
        // CLHEP < -- > AMG interface method
        
        // Sets the alignable transform delta when the supplied delta is in the local
        // reconstruction frame
        
        // If the default transform to the local recostruction frame is
        // T = A*B*C*D*E
        // and the alignable transform is C with delta c and the delat in the local frame is l, then
        // A*B*C*c*D*E = A*B*C*D*E*l
        //  c = (D*E) * l * (D*E).inverse()
        //  c = (A*B*C).inverse * T * l * T.inverse() * (A*B*C) 
        
        // To get default transform up and including the alignable transform,
        // we assume the next volume down is a fullphys volume and so its
        // transform is the transform we want (ie A*B*C in the above example).

        if (!extXF) return false;

        const GeoVFullPhysVol* child = extXF->child();
        if (child && extXF->alignableTransform()) {
            // the definitiv absolut transform is in CLHEP -> do the calculation in CLHEP
            const GeoTrf::Transform3D& transform = child->getDefAbsoluteTransform(alignStore);
            // calucluate the corrected delta according to the formula above
            GeoTrf::Transform3D correctedDelta = transform.inverse()*localToGlobalXF      // (A*B*C).inverse() * T
	                                         * delta                                  // l
                                                 * localToGlobalXF.inverse() * transform; // T.inverse() * (A*B*C)
            extXF->alignableTransform()->setDelta(correctedDelta, alignStore);
            return true;
        } else {
            return false;
        }
    }

    bool CaloDetectorManager::setAlignableTransformGlobalDelta(ExtendedAlignableTransform * extXF, 
                                                             const Amg::Transform3D& delta,
                                                             GeoVAlignmentStore* alignStore) const {
        // ATTENTION -------------------------------------------------------- (A.S.)
        // CLHEP < -- > AMG interface method
        
        // Sets the alignable transform delta when the supplied delta is in the global frame.
        
        // If the default transform down to the alignable transform is
        // T = A*B*C
        // and the alignable transform is C with delta c and the delta in the global frame is g, then
        // A*B*C*c = g*A*B*C
        // T*c = g*T
        //  c = T.inverse() * g * T
        
        // To get default transform up and including the alignable transform,
        // we assume the next volume down is a fullphys volume and so its
        // transform is the transform we want (ie T=A*B*C in the above example).


        if (!extXF) return false;

        const GeoVFullPhysVol * child = extXF->child();
        if (child && extXF->alignableTransform()) {
            // do the calculation in CLHEP  
            const GeoTrf::Transform3D& transform = child->getDefAbsoluteTransform(alignStore);
            extXF->alignableTransform()->setDelta(transform.inverse() * delta * transform, alignStore);
            return true;
        } else {
            return false;
        }
    }

    void CaloDetectorManager::setDesign(const CaloDetectorDesign * design)
    {
        m_design = design;
    }

    const CaloDetectorDesign* CaloDetectorManager::getDesign() const
    {
        return m_design;
    }

}// namespace CaloDD
