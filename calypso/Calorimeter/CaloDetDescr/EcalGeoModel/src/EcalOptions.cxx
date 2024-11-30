/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalOptions.h"

EcalOptions::EcalOptions()
  : m_gdmlFile{""}
  , m_alignable(true)
  , m_dynAlignFolders(false)
{}

void
EcalOptions::setGdmlFile(std::string fileName)
{
  m_gdmlFile = fileName;
}

std::string
EcalOptions::gdmlFile() const
{
  return m_gdmlFile;
}

void 
EcalOptions::setAlignable(bool flag)
{
  m_alignable = flag;
}

bool 
EcalOptions::alignable() const
{
  return m_alignable;
}

void EcalOptions::setDynamicAlignFolders(const bool flag)
{
  m_dynAlignFolders = flag;
}

bool EcalOptions::dynamicAlignFolders() const 
{  
  return m_dynAlignFolders;
}
