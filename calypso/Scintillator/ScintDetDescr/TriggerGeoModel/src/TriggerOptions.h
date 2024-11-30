/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerGeoModel_TriggerOptions_H
#define TriggerGeoModel_TriggerOptions_H

// Class for any run time options.


class TriggerOptions
{

public:
  TriggerOptions();
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


#endif // TriggerGeoModel_TriggerOptions_H
