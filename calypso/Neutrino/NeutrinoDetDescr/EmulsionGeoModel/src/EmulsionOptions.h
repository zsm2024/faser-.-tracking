/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionOptions_H
#define EmulsionGeoModel_EmulsionOptions_H

// Class for any run time options.


class EmulsionOptions
{

public:
  EmulsionOptions();
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


#endif // EmulsionGeoModel_EmulsionOptions_H
