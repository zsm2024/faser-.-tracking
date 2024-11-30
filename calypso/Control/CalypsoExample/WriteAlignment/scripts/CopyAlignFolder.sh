#!/bin/sh

rm -f *_Align.pool.root
rm -f *_ALLP200.db
rm -f PoolFileCatalog.xml
rm -f writeAlignment_*.log

python python/WriteAlignment/WriteAlignmentConfig_Faser01.py >& writeAlignment_Faser01.log
python python/WriteAlignment/WriteAlignmentConfig_Faser02.py >& writeAlignment_Faser02.log
python python/WriteAlignment/WriteAlignmentConfig_FaserTB00.py >& writeAlignment_FaserTB00.log

AtlCoolCopy "sqlite://;schema=./FASER-01_ALLP200.db;dbname=OFLP200" "sqlite://;schema=data/sqlite200/ALLP200.db;dbname=OFLP200" -fnp -ot "TRACKER-ALIGN-01"
AtlCoolCopy "sqlite://;schema=./FASER-02_ALLP200.db;dbname=OFLP200" "sqlite://;schema=data/sqlite200/ALLP200.db;dbname=OFLP200" -fnp -ot "TRACKER-ALIGN-02"
AtlCoolCopy "sqlite://;schema=./FASER-TB00_ALLP200.db;dbname=OFLP200" "sqlite://;schema=data/sqlite200/ALLP200.db;dbname=OFLP200" -fnp -ot "TRACKER-ALIGN-TB00"
mkdir data/poolcond
cp -n PoolFileCatalog.xml data/poolcond/
ln -s ${PWD}/data/poolcond/PoolFileCatalog.xml ${PWD}/data/poolcond/PoolCat_oflcond.xml
cp -n *_Align.pool.root data/poolcond/