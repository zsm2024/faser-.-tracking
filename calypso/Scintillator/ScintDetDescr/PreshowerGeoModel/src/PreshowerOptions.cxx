/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerOptions.h"

PreshowerOptions::PreshowerOptions()
  : m_alignable(true)
//   , m_alignModule(true)
  , m_dynAlignFolders(false)
{}

void 
PreshowerOptions::setAlignable(bool flag)
{
  m_alignable = flag;
}

bool 
PreshowerOptions::alignable() const
{
  return m_alignable;
}

// following may eventually become useful
//
// void 
// SCT_Options::setAlignAtModuleLevel(bool flag)
// {
//   m_alignModule = flag;
// }

// bool 
// SCT_Options::alignAtModuleLevel() const
// {
//   return m_alignModule;
// }

void PreshowerOptions::setDynamicAlignFolders(const bool flag)
{
  m_dynAlignFolders = flag;
}

bool PreshowerOptions::dynamicAlignFolders() const 
{  
  return m_dynAlignFolders;
}
