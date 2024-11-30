/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerOptions_H
#define PreshowerGeoModel_PreshowerOptions_H

// Class for any run time options.


class PreshowerOptions
{

public:
  PreshowerOptions();
  bool alignable() const;
//   bool alignAtModuleLevel() const;

  void setAlignable(bool flag = true);
//   void setAlignAtModuleLevel(bool flag = true);

  //dynamic alignment folders
  void setDynamicAlignFolders(const bool flag = true);
  bool dynamicAlignFolders() const;

private:

  bool m_alignable;
//   bool m_alignModule;
  bool m_dynAlignFolders;   //controls which set of alignment folders is used

};


#endif // PreshowerGeoModel_PreshowerOptions_H
