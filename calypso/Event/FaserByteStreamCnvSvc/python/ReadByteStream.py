# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# module for reading EventStorage BS input file.
#
#  DEPRECATED - do not use or copy as an example
#==============================================================

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

# Load ByteStreamEventStorageInputSvc
if not hasattr (svcMgr, 'FaserByteStreamInputSvc'):
    svcMgr += CfgMgr.FaserByteStreamInputSvc ("FaserByteStreamInputSvc")

# Load ROBDataProviderSvc
if not hasattr (svcMgr, 'FaserROBDataProviderSvc'):
    svcMgr += CfgMgr.FaserROBDataProviderSvc ("FaserROBDataProviderSvc")

# Load EventSelectorByteStream
if not hasattr (svcMgr, 'FaserEventSelector'):
    svcMgr += CfgMgr.FaserEventSelectorByteStream ("FaserEventSelector")
theApp.EvtSel = "FaserEventSelector"

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoSelectorTool 
xconv = xAODMaker__EventInfoSelectorTool()
svcMgr.FaserEventSelector.HelperTools += [xconv]

# Load ByteStreamCnvSvc
if not hasattr (svcMgr, 'FaserByteStreamCnvSvc'):
    svcMgr += CfgMgr.FaserByteStreamCnvSvc ("FaserByteStreamCnvSvc")

# Properties
svcMgr.FaserEventSelector.ByteStreamInputSvc = "FaserByteStreamInputSvc"

svcMgr.EventPersistencySvc.CnvServices += [ "FaserByteStreamCnvSvc" ]

# Load ProxyProviderSvc
if not hasattr (svcMgr, 'ProxyProviderSvc'):
    svcMgr += CfgMgr.ProxyProviderSvc()

# Add in ByteStreamAddressProviderSvc
if not hasattr (svcMgr, 'FaserByteStreamAddressProviderSvc'):
    svcMgr += CfgMgr.FaserByteStreamAddressProviderSvc ("FaserByteStreamAddressProviderSvc")
svcMgr.ProxyProviderSvc.ProviderNames += [ "FaserByteStreamAddressProviderSvc" ]

# Add in MetaDataSvc
#if not hasattr (svcMgr, 'MetaDataSvc'):
#    svcMgr += CfgMgr.MetaDataSvc ("MetaDataSvc")
#svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

# Add in MetaData Stores
# from StoreGate.StoreGateConf import StoreGateSvc

#if not hasattr (svcMgr, 'InputMetaDataStore'):
#    svcMgr += StoreGateSvc( "InputMetaDataStore" )
#if not hasattr (svcMgr, 'MetaDataStore'):
#    svcMgr += StoreGateSvc( "MetaDataStore" )

# enable IOVDbSvc to read metadata
#svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
#svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]

#if not hasattr (svcMgr.ToolSvc, 'IOVDbMetaDataTool'):
#    svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()

# Enable ByteStream to read MetaData
#svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]
#if not hasattr (svcMgr.ToolSvc, 'ByteStreamMetadataTool'):
#    svcMgr.ToolSvc += CfgMgr.ByteStreamMetadataTool()
