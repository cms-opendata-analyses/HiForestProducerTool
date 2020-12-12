#!/bin/sh -l
sudo chown $USER /mnt/vol

mkdir HiForest
cd HiForest
# For the plain github action with docker, the area would be available in /mnt/vol
# git clone -b 2010 git://github.com/cms-legacy-analyses/HiForestProducerTool.git HiForestProducer
mkdir HiForestProducer
cd HiForestProducer
cp  /mnt/vol/forest2dimuon.C .

cp /mnt/vol/*.root .
root -l -b forest2dimuon.C

cp *.png /mnt/vol/
echo  ls -l /mnt/vol
ls -l /mnt/vol
