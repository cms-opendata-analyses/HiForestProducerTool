# HiForestProducerTool
Tool to produce HiForest root file from the CMS 2011 heavy-ion open data.

This repository hosts a set of simple examples that use CMSSW EDAnalyzers to extract trigger information and produce HiForest root file from CMS public heavy-ion data. Currently, this repository has two main branches, [2010](https://github.com/cms-legacydata-analyses/HiForestProducerTool/tree/2010) and [2011](https://github.com/cms-legacydata-analyses/HiForestProducerTool/tree/2011) corresponding to the CMS heavy-ion data that has been so far released.  Please choose the one you need as instructions may vary a little, and follow the instructions therein.

## Usage instructions

### Prepare and compile

First, you have to either
- create a [VM](http://opendata.cern.ch/docs/cms-virtual-machine-2010 "CMS 2010 Virtual Machines: How to install") from the CMS Open Data website. Open the `CMS shell` terminal in the VM and create and intialize the CMSSW environment with 

  ```
  cmsrel CMSSW_4_4_7
  cd CMSSW_4_4_7/src/
  cmsenv
  ```
- or, set up a [Docker container](http://opendata.cern.ch/docs/cms-guide-docker) with

  ```
  docker run --name opendata -it gitlab-registry.cern.ch/cms-cloud/cmssw-docker/cmssw_4_4_7-slc5_amd64_gcc434:latest /bin/bash
  ```

Then follow these steps:

- Create a working directory and obtain the code from git:

  ```
  mkdir HiForest
  cd HiForest
  git clone -b 2011 git://github.com/cms-opendata-analyses/HiForestProducerTool.git HiForestProducer
  cd HiForestProducer
  ```

- Compile everything:

  ```
  scram b
  ```
  
### Run the producer

- If you are using the VM, make symbolic links to the conditions database

  ```
  ln -sf /cvmfs/cms-opendata-conddb.cern.ch/GR_R_44_V15 GR_R_44_V15
  ln -sf /cvmfs/cms-opendata-conddb.cern.ch/GR_R_44_V15.db GR_R_44_V15.db
  ```

  You should now see the `cms-opendata-conddb.cern.ch` link in the `/cvmfs` area.

- If you are using the docker container, comment the line starting with  `process.GlobalTag.connect` in the configuration file `hiforestanalyzer_cfg.py`

- Set the number of events in the configuration file. The default `-1` runs over all events.

- Run the CMSSW executable in the background and dump the output in a log file with any name (full.log in this case)

  ```
  cmsRun hiforestanalyzer_cfg.py > full.log 2>&1 &
  ```

The job is configured to read the input root files from the full list of files CMS_HIRun2011_HIDiMuon_RECO_04Mar2013-v1_root_file_index.txt.

It will produce the HiForestAOD_DATAtest2011.root file as an output.

NOTE: The first time you execute the job, it will take a long time (depending on your connection speed) to the point that it looks like it is not doing anything. That is fine. This is because the database payload files will be downloaded/cached locally in the VM or container. Later attempts should be faster.

You can modify [src/Analyzer.cc](src/Analyzer.cc) file to include other object (tracks, electrons, etc) in the hiforest output. the instructions a given inside it.


### Run the analysis

[forest2dimuon.C](forest2dimuon.C) is a script to analyze the output root file. It applies a trigger filter, does some basic analysis selections and produces a histogram with the dimuon invariant mass.

Run this analysis script with
```
root -l forest2dimuon.C++
```

You can select another trigger, and you can see the name of the triggers in the producer output. Note that trigger versions may have changed during the data taking (e.g. `_v1` or `_v2` in the end of the trigger path name).

## Continuous Integration

This repository contains [a github workflow](.github/workflows/main.yml), which runs a test job on the CMS open data container using github free resources. It uses a docker container and runs a HiForest root file producer workflow defined in [commands.sh](commands.sh) and makes an example plot with [plot.sh](plot.sh). The ouput is returned as a github artifact. The workflow is triggered by a pull request. 
