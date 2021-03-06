void runProtonsFeedDownAnalysis(const char* esdAnalysisType = "Hybrid",
				const char* pidMode = "Bayesian") {
  //Macro to run the proton feed-down analysis tested for local, proof & GRID.
  //Local: Takes four arguments, the analysis mode, the type of the ESD 
  //       analysis, the PID mode and the path where the tag and ESD or 
  //       AOD files reside.
  //Interactive: Takes four arguments, the analysis mode, the type of the ESD 
  //             analysis, the PID mode and the name of the collection of tag 
  //             files.
  //Batch: Takes four arguments, the analysis mode, the type of the ESD 
  //       analysis, the PID mode and the name of the collection file with 
  //       the event list for each file.
  //Proof: Takes five arguments, the analysis level, the analysis mode in 
  //       case of ESD, the PID mode, the number of events and the dataset 
  //       name and .  
  //Analysis mode can be: "MC", "ESD", "AOD"
  //ESD analysis type can be one of the three: "TPC", "Hybrid", "Global"
  //PID mode can be one of the four: "Bayesian" (standard Bayesian approach) 
  //   "Ratio" (ratio of measured over expected/theoretical dE/dx a la STAR) 
  //   "Sigma1" (N-sigma area around the fitted dE/dx vs P band)
  //   "Sigma2" (same as previous but taking into account the No of TPC points)
  TStopwatch timer;
  timer.Start();
  
  // runLocal("ESD","TPC","Bayesian","/home/mbroz/FeedDownAnalysis/");
  //runInteractive("ESD","TPC","Bayesian","pp0901.xml");
  runBatch("ESD","TPC","Bayesian","wn.xml");  
  // runProof("ESD","TPC","Bayesian",200000,"/COMMON/COMMON/LHC09a4_run8101X");
  
  timer.Stop();
  timer.Print();
}

//_________________________________________________//
void runLocal(const char* mode = "ESD",
	      const char* analysisType = 0x0,
	      const char* pidMode = 0x0,
	      const char* path = 0x0) {
  TString smode = mode;
  TString outputFilename = "Protons.FeedDown."; outputFilename += mode;
  if(analysisType) {
    outputFilename += "."; outputFilename += analysisType;
  }
  outputFilename += ".root";

  //____________________________________________________//
  //_____________Setting up the par files_______________//
  //____________________________________________________//
  setupPar("STEERBase");
  gSystem->Load("libSTEERBase");
  setupPar("ESD");
  gSystem->Load("libVMC");
  gSystem->Load("libESD");
  setupPar("AOD");
  gSystem->Load("libAOD");
  setupPar("ANALYSIS");
  gSystem->Load("libANALYSIS");
  setupPar("ANALYSISalice");
  gSystem->Load("libANALYSISalice");
  setupPar("CORRFW");
  gSystem->Load("libCORRFW");
  setupPar("PWG2spectra");
  gSystem->Load("libPWG2spectra");
  //____________________________________________________//  
  
  //____________________________________________//
  AliTagAnalysis *tagAnalysis = new AliTagAnalysis("ESD"); 
  tagAnalysis->ChainLocalTags(path);

  AliRunTagCuts *runCuts = new AliRunTagCuts();
  AliLHCTagCuts *lhcCuts = new AliLHCTagCuts();
  AliDetectorTagCuts *detCuts = new AliDetectorTagCuts();
  AliEventTagCuts *evCuts = new AliEventTagCuts();
  
  TChain* chain = 0x0;
  chain = tagAnalysis->QueryTags(runCuts,lhcCuts,detCuts,evCuts);
  //chain->SetBranchStatus("*Calo*",0);

  //____________________________________________//
  gROOT->LoadMacro("configProtonFeedDownAnalysis.C");
  AliProtonFeedDownAnalysis *analysis = GetProtonFeedDownAnalysisObject(mode,
									analysisType,
									pidMode);
  //____________________________________________//
  // Make the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("protonAnalysisManagerFD");
  AliVEventHandler* esdH = new AliESDInputHandler;
  mgr->SetInputEventHandler(esdH);
  AliMCEventHandler *mc = new AliMCEventHandler();
  mgr->SetMCtruthEventHandler(mc);

  //____________________________________________//
  AliProtonFeedDownAnalysisTask *taskProtons = new AliProtonFeedDownAnalysisTask("TaskProtonsFD");
  taskProtons->SetAnalysisObject(analysis);
  mgr->AddTask(taskProtons);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput1 = mgr->CreateContainer("dataChain",
							   TChain::Class(),
							   AliAnalysisManager::kInputContainer);
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("outputList",
                                                            TList::Class(),
							    AliAnalysisManager::kOutputContainer,
                                                            outputFilename.Data());

  //____________________________________________//
  mgr->ConnectInput(taskProtons,0,mgr->GetCommonInputContainer());
  mgr->ConnectOutput(taskProtons,0,coutput1);
  if (!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  mgr->StartAnalysis("local",chain);
}

//_________________________________________________//
void runInteractive(const char* mode = "ESD",
		    const char* analysisType = 0x0,
		    const char* pidMode = 0x0,
		    const char* collectionName = "tag.xml") {
  gSystem->Load("libProofPlayer");

  TString smode = mode;
  TString outputFilename = "Protons.FeedDown."; outputFilename += mode;
  if(analysisType) {
    outputFilename += "."; outputFilename += analysisType;
  }
  outputFilename += ".root";

  printf("*** Connect to AliEn ***\n");
  TGrid::Connect("alien://");
 
  //____________________________________________________//
  //_____________Setting up the par files_______________//
  //____________________________________________________//
  setupPar("STEERBase");
  gSystem->Load("libSTEERBase");
  setupPar("ESD");
  gSystem->Load("libVMC");
  gSystem->Load("libESD");
  setupPar("AOD");
  gSystem->Load("libAOD");
  setupPar("ANALYSIS");
  gSystem->Load("libANALYSIS");
  setupPar("ANALYSISalice");
  gSystem->Load("libANALYSISalice");
  setupPar("CORRFW");
  gSystem->Load("libCORRFW");
  setupPar("PWG2spectra");
  gSystem->Load("libPWG2spectra");
  //____________________________________________________//  
 
  //____________________________________________//
  AliTagAnalysis *tagAnalysis = new AliTagAnalysis("ESD");
 
  AliRunTagCuts *runCuts = new AliRunTagCuts();
  AliLHCTagCuts *lhcCuts = new AliLHCTagCuts();
  AliDetectorTagCuts *detCuts = new AliDetectorTagCuts();
  AliEventTagCuts *evCuts = new AliEventTagCuts();
 
  //grid tags
  TAlienCollection* coll = TAlienCollection::Open(collectionName);
  TGridResult* TagResult = coll->GetGridResult("",0,0);
  tagAnalysis->ChainGridTags(TagResult);
  TChain* chain = 0x0;
  chain = tagAnalysis->QueryTags(runCuts,lhcCuts,detCuts,evCuts);
  //chain->SetBranchStatus("*Calo*",0);
  
  //____________________________________________//
  gROOT->LoadMacro("configProtonFeedDownAnalysis.C");
  AliProtonFeedDownAnalysis *analysis = GetProtonFeedDownAnalysisObject(mode,
									analysisType,
									pidMode);
  //____________________________________________//
  // Make the analysis manager
  // Make the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("protonAnalysisManagerFD");
  AliVEventHandler* esdH = new AliESDInputHandler;
  mgr->SetInputEventHandler(esdH);
  AliMCEventHandler *mc = new AliMCEventHandler();
  mgr->SetMCtruthEventHandler(mc);

  //____________________________________________//
  AliProtonFeedDownAnalysisTask *taskProtons = new AliProtonFeedDownAnalysisTask("TaskProtonsFD");
  taskProtons->SetAnalysisObject(analysis);
  mgr->AddTask(taskProtons);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput1 = mgr->CreateContainer("dataChain",
							   TChain::Class(),
							   AliAnalysisManager::kInputContainer);
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("outputList",
                                                            TList::Class(),
							    AliAnalysisManager::kOutputContainer,
                                                            outputFilename.Data());
  
  //____________________________________________//
  mgr->ConnectInput(taskProtons,0,mgr->GetCommonInputContainer());
  mgr->ConnectOutput(taskProtons,0,coutput1);
  if (!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  mgr->StartAnalysis("local",chain);
}

//_________________________________________________//
void runBatch(const char* mode = "ESD",
	      const char* analysisType = 0x0,
	      const char* pidMode = 0x0,
	      const char *collectionfile = "wn.xml") {
  TString smode = mode;
  TString outputFilename = "Protons.FeedDown."; outputFilename += mode;
  if(analysisType) {
    outputFilename += "."; outputFilename += analysisType;
  }
  outputFilename += ".root";

  printf("*** Connect to AliEn ***\n");
  TGrid::Connect("alien://");
  gSystem->Load("libProofPlayer");

  //____________________________________________________//
  //_____________Setting up the par files_______________//
  //____________________________________________________//
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libANALYSIS") ;
  gSystem->Load("libANALYSISalice") ;
  gSystem->Load("libCORRFW") ;

  setupPar("PWG2spectra");
  gSystem->Load("libPWG2spectra");
  //____________________________________________________//  

  //____________________________________________//
  AliTagAnalysis *tagAnalysis = new AliTagAnalysis("ESD");
  TChain *chain = 0x0;
  chain = tagAnalysis->GetChainFromCollection(collectionfile,"esdTree");

  //____________________________________________//
  gROOT->LoadMacro("configProtonFeedDownAnalysis.C");
  AliProtonFeedDownAnalysis *analysis = GetProtonFeedDownAnalysisObject(mode,
									analysisType,
									pidMode);
  //____________________________________________//
  // Make the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("protonAnalysisManagerFD");
  AliVEventHandler* esdH = new AliESDInputHandler;
  mgr->SetInputEventHandler(esdH);
  AliMCEventHandler *mc = new AliMCEventHandler();
  mgr->SetMCtruthEventHandler(mc);

  //____________________________________________//
  AliProtonFeedDownAnalysisTask *taskProtons = new AliProtonFeedDownAnalysisTask("TaskProtonsFD");
  taskProtons->SetAnalysisObject(analysis);
  mgr->AddTask(taskProtons);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput1 = mgr->CreateContainer("dataChain",
							   TChain::Class(),
							   AliAnalysisManager::kInputContainer);
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("outputList",
                                                            TList::Class(),
							    AliAnalysisManager::kOutputContainer,
                                                            outputFilename.Data());

  //____________________________________________//
  mgr->ConnectInput(taskProtons,0,mgr->GetCommonInputContainer());
  mgr->ConnectOutput(taskProtons,0,coutput1);
  if (!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  mgr->StartAnalysis("local",chain);
}

//_________________________________________________//
void runProof(const char* mode = "ESD",
	      const char* analysisType = 0x0,
	      const char* pidMode = 0x0,
	      Int_t stats = 0, 
	      const char* dataset = 0x0) {  
  TString smode = mode;
  TString outputFilename = "Protons.FeedDown."; outputFilename += mode;
  if(analysisType) {
    outputFilename += "."; outputFilename += analysisType;
  }
  outputFilename += ".root";

  printf("****** Connect to PROOF *******\n");
  TProof::Open("alicecaf.cern.ch"); 
  //gProof->SetParallel();

  // Enable the Analysis Package
  gProof->UploadPackage("STEERBase.par");
  gProof->EnablePackage("STEERBase");
  gProof->UploadPackage("ESD.par");
  gProof->EnablePackage("ESD");
  gProof->UploadPackage("AOD.par");
  gProof->EnablePackage("AOD");
  gProof->UploadPackage("ANALYSIS.par");
  gProof->EnablePackage("ANALYSIS");
  gProof->UploadPackage("ANALYSISalice.par");
  gProof->EnablePackage("ANALYSISalice");
  gProof->UploadPackage("CORRFW.par");
  gProof->EnablePackage("CORRFW");
  gProof->UploadPackage("PWG2spectra.par");
  gProof->EnablePackage("PWG2spectra");
  
  //____________________________________________//
  gROOT->LoadMacro("configProtonFeedDownAnalysis.C");
  AliProtonFeedDownAnalysis *analysis = GetProtonFeedDownAnalysisObject(mode,
									analysisType,
									pidMode);
  //____________________________________________//

  //____________________________________________//
  // Make the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("protonAnalysisManagerFD");
  AliVEventHandler* esdH = new AliESDInputHandler;
  mgr->SetInputEventHandler(esdH);
  AliMCEventHandler *mc = new AliMCEventHandler();
  mgr->SetMCtruthEventHandler(mc);
  //____________________________________________//
  //Create the proton task
  AliProtonFeedDownAnalysisTask *taskProtons = new AliProtonFeedDownAnalysisTask("TaskProtonsFD");
  taskProtons->SetAnalysisObject(analysis);
  mgr->AddTask(taskProtons);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput1 = mgr->CreateContainer("dataChain",
							   TChain::Class(),
							   AliAnalysisManager::kInputContainer);
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("outputList",
                                                            TList::Class(),
							    AliAnalysisManager::kOutputContainer,
                                                            outputFilename.Data());

  //____________________________________________//
  mgr->ConnectInput(taskProtons,0,mgr->GetCommonInputContainer());
  mgr->ConnectOutput(taskProtons,0,coutput1);
  if (!mgr->InitAnalysis()) return;
  mgr->PrintStatus();

  if(dataset)
    mgr->StartAnalysis("proof",dataset,stats);
  else {
    // You should get this macro and the txt file from:
    // http://aliceinfo.cern.ch/Offline/Analysis/CAF/
    gROOT->LoadMacro("CreateESDChain.C");
    TChain* chain = 0x0;
    chain = CreateESDChain("ESD82XX_30K.txt",stats);
    chain->SetBranchStatus("*Calo*",0);

    mgr->StartAnalysis("proof",chain);
  }
}

//_________________________________________________//
Int_t setupPar(const char* pararchivename) {
  ///////////////////
  // Setup PAR File//
  ///////////////////
  if (pararchivename) {
    char processline[1024];
    sprintf(processline,".! tar xvzf %s.par",pararchivename);
    gROOT->ProcessLine(processline);
    const char* ocwd = gSystem->WorkingDirectory();
    gSystem->ChangeDirectory(pararchivename);
    
    // check for BUILD.sh and execute
    if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
      printf("*******************************\n");
      printf("*** Building PAR archive    ***\n");
      printf("*******************************\n");
      
      if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
        Error("runAnalysis","Cannot Build the PAR Archive! - Abort!");
        return -1;
      }
    }
    // check for SETUP.C and execute
    if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
      printf("*******************************\n");
      printf("*** Setup PAR archive       ***\n");
      printf("*******************************\n");
      gROOT->Macro("PROOF-INF/SETUP.C");
    }
    
    gSystem->ChangeDirectory("../");
  } 
  return 1;
}
