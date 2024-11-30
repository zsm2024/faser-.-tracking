#ifndef FASEREVENTSTORAGEISSUES_H
#define FASEREVENTSTORAGEISSUES_H

#include "ers/ers.h"


/**
   To report problems with configuration data base
*/



ERS_DECLARE_ISSUE(FaserEventStorage,
		  WritingIssue,
		  "FaserEventStorage writing problem" << ": " << reason,
		  ((const char*) reason)
		  )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_SingleFile,
		       FaserEventStorage::WritingIssue,
		       "ES_SingleFile" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_SingleFileAlreadyExists,
		       FaserEventStorage::WritingIssue,
		       "ES_SingleFileAlreadyExists" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )




ERS_DECLARE_ISSUE(FaserEventStorage,
		  ReadingIssue,
		  "FaserEventStorage reading problem" << ": " << reason,
		  ((const char*) reason)
		  )

ERS_DECLARE_ISSUE(FaserEventStorage,
		  RawFileNameIssue,
		  "FaserEventStorage problem with RawFileName" << ": " << reason,
		  ((const char*) reason)
		  )


ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_InternalError,
		       FaserEventStorage::ReadingIssue,
		       "ES_InternalError" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )


ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_WrongFileFormat,
		       FaserEventStorage::ReadingIssue,
		       "ES_WrongFileFormat" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_OutOfFileBoundary,
		       FaserEventStorage::ReadingIssue,
		       "ES_OutOfFileBoundary" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_NoEventFound,
		       FaserEventStorage::ReadingIssue,
		       "ES_NoEventFound" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_WrongEventSize,
		       FaserEventStorage::ReadingIssue,
		       "ES_WrongEventSize" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_NoEndOfFileRecord,
		       FaserEventStorage::ReadingIssue,
		       "ES_NoEndOfFileRecord" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_AllocatedMemoryTooLittle,
		       FaserEventStorage::ReadingIssue,
		       "ES_AllocatedMemoryTooLittle" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )

ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_AllocatingMemoryFailed,
		       FaserEventStorage::ReadingIssue,
		       "ES_AllocatingMemoryFailed" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )



ERS_DECLARE_ISSUE_BASE(FaserEventStorage,
		       ES_SquenceNextFileMissing,
		       FaserEventStorage::ReadingIssue,
		       "ES_SquenceNextFileMissing" << ": " << reason  ,
		       ((const char*) reason), ERS_EMPTY
		       )



/*
ES_InternalError

ES_WrongFileFormat

ES_OutOfFileBoundary
ES_NoEventFound
ES_WrongEventSize


ES_NoEndOfFileRecord
ES_AllocatedMemoryTooLittle



*/

#endif
