/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VTI12PRDSystems/PRDDetTypes.h"

//______________________________________________
QString PRDDetType::typeToString(const Type& t)
{
  switch (t) {
  case SCT: return "SCT";
  case SpacePoints: return "SpacePoints";
  default: return "UNKNOWN";
  }
}

//______________________________________________
PRDDetType::Type PRDDetType::stringToType(const QString&str, bool & status)
{
  status = true;
  if (str=="SCT") return SCT;
  if (str=="SpacePoints") return SpacePoints;
  status = false;
  return SCT;//people better watch the return status!
}


//______________________________________________
qint32 PRDDetType::typeToInt(const Type& t)
{
  switch (t) {
  case SCT: return 0;
  case SpacePoints: return 1;
  default: return -1;
  }
}

//______________________________________________
PRDDetType::Type PRDDetType::intToType(const qint32&i, bool & status)
{
  status = true;
  switch (i) {
  case 0: return SCT;
  case 1: return SpacePoints;
  default:
    status = false;
    return SCT;//people better watch the return status!
  }
}
