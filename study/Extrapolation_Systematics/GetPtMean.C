void GetPtMean(){

  TCanvas* c1 = new TCanvas();

  vector<int> years;
  years.push_back(2016);
  years.push_back(2017);
  years.push_back(2018);

  const int totalptbins = 5;
  const int totaletabins = 3;
  float ptEdge[totalptbins+1] = {125, 200, 300, 400, 500, 1000};
  float etaEdge[totaletabins+1] = {0, 0.8, 1.566, 2.5};

  for (auto year : years) {
    std::cout << "year: " << year << std::endl;
    string inFileName = "/eos/home-h/hsinyeh/store/Crab1102_ReRecoDY/merged_" +  to_string(year) + "_data.root";
    TFile* inFile = new TFile( inFileName.c_str(), "read" );
    TTree* fitter_tree = (TTree*)inFile->Get("tnpPhoIDs/fitter_tree");

    string outFileName = "./ptMean/ptMean_" + to_string(year) + ".txt";
    fstream outFile;
    outFile.open(outFileName, ios::out);

    for (int ptbin = 0; ptbin < totalptbins; ptbin++) {
      for (int etabin = 0; etabin < totaletabins; etabin++) {
        TH1F *h = new TH1F("h","", 500, ptEdge[ptbin], ptEdge[ptbin+1]);
        fitter_tree->Draw("ph_et >> h", Form("ph_et>%f && ph_et<%f && abs(ph_sc_eta)>%f && abs(ph_sc_eta)<%f && passingCutBasedLooseHighPtIDV2",ptEdge[ptbin], ptEdge[ptbin+1], etaEdge[etabin], etaEdge[etabin+1]) );
        std::cout << "ptbin: " << ptbin << " etabin: " << etabin << " Mean: " << h->GetMean() << std::endl;
        outFile << h->GetMean() << " " << h->GetRMS() << " ";
        delete h;
      }
      outFile << "\n";
    }

  }
}
