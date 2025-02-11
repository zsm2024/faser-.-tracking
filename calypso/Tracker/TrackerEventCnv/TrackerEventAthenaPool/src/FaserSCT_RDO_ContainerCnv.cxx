/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_RDO_ContainerCnv.h"

#include "TrackerIdentifier/FaserSCT_ID.h"

#include <memory>

#include <iostream>


//================================================================
namespace {
#ifdef SCT_DEBUG
  std::string shortPrint(const FaserSCT_RDO_Container* main_input_SCT, unsigned maxprint=25) {
    std::ostringstream os;
    if (main_input_SCT) {
      for (unsigned i=0; i<maxprint; i++) {
        FaserSCT_RDO_Container::const_iterator p = main_input_SCT->indexFind(i);
        if (p != main_input_SCT->end()) {
          os<<" "<< (*p)->size();
        }
        else {
          os<<" *";
        }
      }
    }
    else {
      os<<" [FaserSCT_RDO_Container==NULL]";
    }
    return os.str();
  }
#endif
}

FaserSCT_RDO_ContainerCnv::FaserSCT_RDO_ContainerCnv (ISvcLocator* svcloc)
  : FaserSCT_RDO_ContainerCnvBase(svcloc, "FaserSCT_RDO_ContainerCnv"),
    m_converter_p0(),
    m_storeGate(nullptr)
{}

//================================================================
StatusCode FaserSCT_RDO_ContainerCnv::initialize() {
  StatusCode sc = FaserSCT_RDO_ContainerCnvBase::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("FaserSCT_RDO_ContainerCnvBase::initialize() returned failure !");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("FaserSCT_RDO_ContainerCnv::initialize()");

  // get StoreGate service. This is needed only for clients
  // that register collections directly to the SG instead of adding
  // them to the container.
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  }
   
  // get DetectorStore service
  StoreGateSvc* detStore(nullptr);
  if (service("DetectorStore", detStore).isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Found DetectorStore.");
  }
   
  // Get the sct helper from the detector store
  const FaserSCT_ID* idhelper(nullptr);
  if (detStore->retrieve(idhelper, "FaserSCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get FaserSCT_ID helper !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Found the FaserSCT_ID helper.");
  }
   
  m_converter_p0.initialize(idhelper);
//   m_converter_TP1.initialize(idhelper);
//   m_converter_SCT_TP1.initialize(idhelper);
//   m_converter_SCT_TP2.initialize(idhelper);
//   m_converter_SCT_TP3.initialize(idhelper);
  m_converter_SCT_TP4.initialize(idhelper);
  m_converter_PERS.initialize(idhelper);

  //   ATH_MSG_DEBUG("Converter initialized");

  return StatusCode::SUCCESS;
}

//================================================================
FaserSCT_RDO_Container_PERS* FaserSCT_RDO_ContainerCnv::createPersistent(FaserSCT_RDO_Container* transCont) {

#ifdef SCT_DEBUG
  ATH_MSG_DEBUG("createPersistent(): main converter. TRANS = "<<shortPrint(transCont));
#endif
  // converter_num  is a switch to determine which persistent type to use
  // 1: if concrete type private data is equivalent to TrackerRawData_p1
  // 3: for cosmic/TB 
  //
  unsigned int converter_num(1);
  FaserSCT_RDO_Container::const_iterator it_Coll     = transCont->begin();
  FaserSCT_RDO_Container::const_iterator it_CollEnd  = transCont->end();
  // check one element of the container. The container can't be empty for this... 
  if (it_Coll != it_CollEnd) {
    while (it_Coll != it_CollEnd && (*it_Coll)->size() == 0) it_Coll++;
    if (it_Coll != it_CollEnd) {
      const FaserSCT_RDORawData* test((**it_Coll)[0]);
      if (dynamic_cast<const FaserSCT1_RawData*>(test) != nullptr) {
        //ATH_MSG_DEBUG("Found container with FaserSCT1_RawData concrete type objects");
        converter_num=1;
      } else if (dynamic_cast<const FaserSCT3_RawData*>(test) != nullptr) {
        //ATH_MSG_DEBUG("Found container with FaserSCT3_RawData concrete type objects");
        converter_num=3;
      } else {
        ATH_MSG_FATAL("Converter not implemented for this concrete type ");
        throw "Converter not implemented";
      }
    } else {
      ATH_MSG_WARNING("Container has only empty collections. Using TP1 converter");
    } 
  } else {
    ATH_MSG_WARNING("Empty container. Using TP1 converter");
  }
  // Switch facility depending on the concrete data type of the contained objects
  // Should do by getting the type_info of one of the contained objects
  FaserSCT_RDO_Container_PERS* persObj(nullptr);
  if (converter_num == 1 || converter_num == 3) {
    m_converter_PERS.setType(converter_num);
    persObj = m_converter_PERS.createPersistent( transCont, msg() );
  } else {
    ATH_MSG_FATAL("This shouldn't happen!! ");
  } 
#ifdef SCT_DEBUG
  ATH_MSG_DEBUG("Success");
#endif
  return persObj; 
}
    
//================================================================
FaserSCT_RDO_Container* FaserSCT_RDO_ContainerCnv::createTransient() {

  static const pool::Guid   p0_guid("F884804E-E87B-42BE-9DD0-1571A1508F25"); // with SCT1_RawData
//   static const pool::Guid   TP1_guid("DA76970C-E019-43D2-B2F9-25660DCECD9D"); // for t/p separated version with InDetRawDataContainer_p1
//   static const pool::Guid   p0_guid("B82A1D11-3F86-4F07-B380-B61BA2DAF3A9"); // with SCT1_RawData
//   static const pool::Guid   TP1_guid("DA76970C-E019-43D2-B2F9-25660DCECD9D"); // for t/p separated version with InDetRawDataContainer_p1
//   static const pool::Guid   SCT_TP1_guid("8E13963E-13E5-4D10-AA8B-73F00AFF8FA8"); // for t/p separated version with SCT_RawDataContainer_p1
//   static const pool::Guid   SCT_TP2_guid("D1258125-2CBA-476E-8578-E09D54F477E1"); // for t/p separated version with SCT_RawDataContainer_p2
//   static const pool::Guid   SCT_TP3_guid("5FBC8D4D-7B4D-433A-8487-0EA0C870CBDB"); // for t/p separated version with SCT_RawDataContainer_p3
//   static const pool::Guid   SCT_TP4_guid("6C7540BE-E85C-4777-BC1C-A9FF11460F54"); // for t/p separated version with SCT_RawDataContainer_p4
  static const pool::Guid   SCT_TP4_guid("45BE54C2-46EF-4B43-AF87-7F10B52F9487"); // for t/p separated version with SCT_RawDataContainer_p4

#ifdef SCT_DEBUG
  ATH_MSG_DEBUG("createTransient(): main converter");
#endif
  if ( compareClassGuid(SCT_TP4_guid) ) {
#ifdef SCT_DEBUG
    ATH_MSG_DEBUG("createTransient(): New TP version - TP4 branch");
#endif

    std::unique_ptr< FaserSCT_RawDataContainer_p4 >   col_vect( poolReadObject< FaserSCT_RawDataContainer_p4 >() );
    FaserSCT_RDO_Container* res = m_converter_SCT_TP4.createTransient( col_vect.get(), msg() );
#ifdef SCT_DEBUG
    ATH_MSG_DEBUG("createTransient(), TP4 branch: returns TRANS = "<<shortPrint(res));
#endif
    return res;

  }
//   else if ( compareClassGuid(SCT_TP3_guid) ) {
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(): New TP version - TP3 branch");
// #endif

//     std::unique_ptr< SCT_RawDataContainer_p3 >   col_vect( poolReadObject< SCT_RawDataContainer_p3 >() );
//     SCT_RDO_Container* res = m_converter_SCT_TP3.createTransient( col_vect.get(), msg() );
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(), TP3 branch: returns TRANS = "<<shortPrint(res));
// #endif
//     return res;

//   }
//   else if ( compareClassGuid(SCT_TP2_guid) ) {
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(): New TP version - TP2 branch");
// #endif

//     std::unique_ptr< SCT_RawDataContainer_p2 >   col_vect( poolReadObject< SCT_RawDataContainer_p2 >() );
//     SCT_RDO_Container* res = m_converter_SCT_TP2.createTransient( col_vect.get(), msg() );
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(), TP2 branch: returns TRANS = "<<shortPrint(res));
// #endif
//     return res;

//   }
//   else if ( compareClassGuid(SCT_TP1_guid) ) {
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(): New TP version - TP1 branch");
// #endif
//     std::unique_ptr< SCT_RawDataContainer_p1 >   col_vect( poolReadObject< SCT_RawDataContainer_p1 >() );
//     SCT_RDO_Container* res = m_converter_SCT_TP1.createTransient( col_vect.get(), msg() );
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(), TP1 branch: returns TRANS = "<<shortPrint(res));
// #endif
//     return res;


//   }
//   else if ( compareClassGuid(TP1_guid) ) {
//     ATH_MSG_DEBUG("createTransient(): New TP version - TP1 branch");
                                                                                                                                                             
//     std::unique_ptr< InDetRawDataContainer_p1 >   col_vect( poolReadObject< InDetRawDataContainer_p1 >() );
//     SCT_RDO_Container* res = m_converter_TP1.createTransient( col_vect.get(), msg() );
// #ifdef SCT_DEBUG
//     ATH_MSG_DEBUG("createTransient(), TP1 branch: returns TRANS = "<<shortPrint(res));
// #endif
//     return res;


//   }
  else if ( compareClassGuid(p0_guid) ) {
#ifdef SCT_DEBUG
    ATH_MSG_DEBUG("createTransient(): Old input file - p0 branch");
#endif
    std::unique_ptr< FaserSCT_RDO_Container_p0 >   col_vect( poolReadObject< FaserSCT_RDO_Container_p0 >() );
    FaserSCT_RDO_Container* res = m_converter_p0.createTransient( col_vect.get(), msg() );
#ifdef SCT_DEBUG
    ATH_MSG_DEBUG("createTransient(), p0 branch: returns TRANS = "<<shortPrint(res));
#endif
    return res;
  }

  throw std::runtime_error("Unsupported persistent version of FaserSCT_RDO_Container");
}

//================================================================
