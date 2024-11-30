/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "../FaserActsExtrapolationAlg.h"
#include "../FaserActsWriteTrackingGeometry.h"
#include "../FaserActsTrackingGeometrySvc.h"
#include "../FaserActsExtrapolationTool.h"
#include "../FaserActsObjWriterTool.h"
#include "../FaserActsTrackingGeometryTool.h"
#include "../FaserActsPropStepRootWriterSvc.h"
#include "../FaserActsAlignmentCondAlg.h"
#include "../FaserNominalAlignmentCondAlg.h"
#include "../FaserActsVolumeMappingTool.h"
#include "../FaserActsMaterialJsonWriterTool.h"
#include "../FaserActsMaterialMapping.h"
#include "../FaserActsSurfaceMappingTool.h"
#include "../FaserActsMaterialTrackWriterSvc.h"
#include "../FaserActsGeometryBoundaryTestAlg.h"

DECLARE_COMPONENT( FaserActsTrackingGeometrySvc )
DECLARE_COMPONENT( FaserActsTrackingGeometryTool )
DECLARE_COMPONENT( FaserActsExtrapolationAlg )
DECLARE_COMPONENT( FaserActsExtrapolationTool )
DECLARE_COMPONENT( FaserActsPropStepRootWriterSvc )
DECLARE_COMPONENT( FaserActsObjWriterTool )
DECLARE_COMPONENT( FaserActsWriteTrackingGeometry )
DECLARE_COMPONENT( FaserActsAlignmentCondAlg )
DECLARE_COMPONENT( FaserActsVolumeMappingTool )
DECLARE_COMPONENT( FaserActsMaterialJsonWriterTool )
DECLARE_COMPONENT( FaserActsMaterialTrackWriterSvc )
DECLARE_COMPONENT( FaserActsMaterialMapping )
DECLARE_COMPONENT( FaserActsSurfaceMappingTool )
DECLARE_COMPONENT( FaserActsGeometryBoundaryTestAlg )
