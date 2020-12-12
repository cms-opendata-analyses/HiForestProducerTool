#!/bin/sh -l
# parameters: $1 number of events (default: 100), $2 configuration file (default: hiforestanalyzer_cfg.py)
sudo chown $USER /mnt/vol

mkdir HiForest
cd HiForest
# For the plain github action with docker, the repository is available in /mnt/vol
# git clone -b 2011 git://github.com/cms-legacy-analyses/HiForestProducerTool.git HiForestProducer
cp -av /mnt/vol HiForestProducer
cd HiForestProducer

scram b -j8

if [ -z "$1" ]; then nev=100; else nev=$1; fi
if [ -z "$2" ]; then config=hiforestanalyzer_cfg.py; else config=$2; fi
eventline=$(grep maxEvents $config)
sed -i "s/$eventline/process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32($nev) )/g" $config
sed -i "s/process.GlobalTag.connect/#process.GlobalTag.connect/g" $config
cmsRun $config

cp *.root /mnt/vol/
echo  ls -l /mnt/vol
ls -l /mnt/vol
