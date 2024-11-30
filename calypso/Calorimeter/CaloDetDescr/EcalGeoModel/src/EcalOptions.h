/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EcalGeoModel_EcalOptions_H
#define EcalGeoModel_EcalOptions_H

// Class for any run time options.
#include <string>

class EcalOptions
{

public:
  EcalOptions();
  bool alignable() const;
  void setAlignable(bool flag = true);

  //dynamic alignment folders
  void setDynamicAlignFolders(const bool flag = true);
  bool dynamicAlignFolders() const;

  void setGdmlFile(std::string fileName);
  std::string gdmlFile() const;
private:
  std::string m_gdmlFile;
  bool m_alignable;
  bool m_dynAlignFolders;   //controls which set of alignment folders is used

};


#endif // EcalGeoModel_EcalOptions_H
