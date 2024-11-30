#include "FaserByteStreamCnvSvc/FaserByteStreamCnvSvc.h"
#include "FaserByteStreamCnvSvc/FaserByteStreamInputSvc.h"
#include "../FaserEventSelectorByteStream.h"
#include "../EventInfoByteStreamxAODCnv.h"
#include "../EventInfoByteStreamAuxCnv.h"
/*
#include "../ByteStreamMergeOutputSvc.h"
#include "../ByteStreamRDP_OutputSvc.h"
#include "../ByteStreamEventStorageInputSvc.h"
#include "../ByteStreamEventStorageOutputSvc.h"
#include "../ByteStreamOutputStreamCopyTool.h"
#include "../ByteStreamMetadataTool.h"
*/

DECLARE_COMPONENT( FaserByteStreamCnvSvc )
DECLARE_COMPONENT( FaserByteStreamInputSvc )
DECLARE_COMPONENT( FaserEventSelectorByteStream )
DECLARE_CONVERTER( EventInfoByteStreamxAODCnv )
DECLARE_CONVERTER( EventInfoByteStreamAuxCnv )
/*
DECLARE_COMPONENT( ByteStreamMergeOutputSvc )
DECLARE_COMPONENT( ByteStreamRDP_OutputSvc )
DECLARE_COMPONENT( ByteStreamEventStorageInputSvc )
DECLARE_COMPONENT( ByteStreamEventStorageOutputSvc )

DECLARE_COMPONENT( ByteStreamOutputStreamCopyTool )
DECLARE_COMPONENT( ByteStreamMetadataTool )
*/
