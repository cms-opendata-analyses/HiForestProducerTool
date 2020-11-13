# HiForestProducerTool
The Tool to produce HiForest .root file from the 2010(11) Heavy Ion Open Data.


Usage instructions:

in 'CMSSW_3_9_2_patch5/src/' folder:

1.
 mkdir HiForest
 
 cd HiForest
 
 mkedanlzr HiForestAnalyzer
 
 checkout the repository
 
scram b



2. To run on data:

 cmsRun hiforestanalyzer_cfg.py


This files uses the InputList.txt file as an input which contains several root files from the 2010 Data Sample.

The full list of files is CMS_HIRun2010_HIAllPhysics_ZS-v2_RECO_file_index.txt


This will produce the HiForestAOD_DATAtest.root file as an output.


3. forest2dimuon.C is analyzing this file, applying trigger filter, basic analysis selections and produces a histogram with the dimuon invariant mass.

4. one can modify src/Analyzer.cc file in order to include other object (tracks, electrons, etc in the hiforest output) the instructions a given inside it.

