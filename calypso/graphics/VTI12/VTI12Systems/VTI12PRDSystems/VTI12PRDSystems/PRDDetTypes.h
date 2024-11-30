/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/

#ifndef PRDDETTYPES_H
#define PRDDETTYPES_H

#include <QString>

class PRDDetType {
public:
  enum Type {
    SCT, SpacePoints
  };
  static QString typeToString(const Type&);
  static Type stringToType(const QString&str, bool & status);
  static qint32 typeToInt(const Type& t);
  static Type intToType(const qint32&i, bool & status);

private:
  PRDDetType();
};


#endif
