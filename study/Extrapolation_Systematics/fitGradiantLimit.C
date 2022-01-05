// Calculate pt bin mean with:
// fitter_tree->Draw("ph_et >> h", "ph_et>120 && ph_et<200 && passingCutBasedLooseHighPtIDV2")
// h->GetMean()

void fitGradiantLimit(){
    vector<int> years;
    years.push_back(2016);
    years.push_back(2017);
    years.push_back(2018);

    gStyle->SetOptFit(0000);

    for ( auto year : years ) {

        // string effFileName = "results/ReReco" + to_string(year) + "/tnpPhoID/multibin_abseta/passingCutBasedLooseHighPtIDV2//egammaEffi.txt";
        string effFileName = "./efficiency/egammaEffi_" + to_string(year) + ".txt";
        ifstream effFile(effFileName);

        const int totalEtaBins = 3;
        const int totalPtBins  = 5;
        float meanPt[totalEtaBins][totalPtBins];
        float errorPt[totalEtaBins][totalPtBins];

        string ptMeanName = "./ptMean/ptMean_" + to_string(year) + ".txt";
        ifstream ptMeanFile(ptMeanName);
        for ( int ptbin = 0; ptbin < totalPtBins; ptbin++){
            for ( int etabin = 0; etabin < totalEtaBins; etabin++){
                ptMeanFile >> meanPt[etabin][ptbin] >> errorPt[etabin][ptbin];
            }
        }

        float eff[2][totalEtaBins][totalPtBins];
        float e_eff[2][totalEtaBins][totalPtBins];
        float sf[totalEtaBins][totalPtBins];
        float e_sf[totalEtaBins][totalPtBins];

        string tmp;

        getline(effFile,tmp);
        getline(effFile,tmp);
    
        for ( int ptbin = 0; ptbin < totalPtBins; ptbin++){
            for ( int etabin = 0; etabin < totalEtaBins; etabin++){
                effFile >> tmp >> tmp >> tmp >> tmp >> eff[0][etabin][ptbin] >> e_eff[0][etabin][ptbin] >> eff[1][etabin][ptbin] >> e_eff[1][etabin][ptbin] >> tmp >> tmp >> tmp >> tmp;

                sf[etabin][ptbin] = eff[0][etabin][ptbin] / eff[1][etabin][ptbin]; // Data / MC
                e_sf[etabin][ptbin] = sqrt( pow(e_eff[0][etabin][ptbin] / eff[0][etabin][ptbin], 2) + pow(e_eff[1][etabin][ptbin] / eff[1][etabin][ptbin], 2) ) * sf[etabin][ptbin];

                std::cout << sf[etabin][ptbin] << " " << e_sf[etabin][ptbin] << std::endl;
            }
        }

        TCanvas* c1 = new TCanvas();
        TF1* fit = new TF1("fit","[0]+[1]*(x-100)",100,1000);

        string title[totalEtaBins];
        title[0] = "0 < |#eta_{SC}| < 0.8";
        title[1] = "0.8 < |#eta_{SC}| < 1.4442";
        title[2] = "1.566 < |#eta_{SC}| < 2.5";

        for ( int etabin = 0; etabin < totalEtaBins; etabin++ ) {
            fit->SetParameters(1,0);

            TGraphErrors* g = new TGraphErrors(5, meanPt[etabin],sf[etabin], errorPt[etabin], e_sf[etabin]);
            g->Draw("APE");
            g->GetXaxis()->SetTitle("pt");
            g->GetYaxis()->SetTitle("Scale Factor");
            g->SetTitle(Form("%s",title[etabin].c_str()));
            g->GetYaxis()->SetRangeUser(0.8, 1.15);
            g->Fit("fit","R");

            TPaveStats *ptstats = new TPaveStats(0.2,0.7,0.88,0.88,"brNDC");
            ptstats->SetName("stats");
            ptstats->SetBorderSize(1);
            ptstats->SetFillColor(0);
            ptstats->SetTextAlign(12);
            ptstats->SetTextSize(0.04);
            TText *ptstats_LaTex = ptstats->AddText(Form("#chi^{2} / ndf = %g / 3",fit->GetChisquare()));
            ptstats_LaTex = ptstats->AddText(Form("p0       = %g #pm %g ",fit->GetParameter(0), fit->GetParError(0)) );
            ptstats_LaTex = ptstats->AddText(Form("p1       = %g #pm %g ",fit->GetParameter(1), fit->GetParError(1)) );
            ptstats->SetOptStat(0);
            ptstats->SetOptFit(111);
            ptstats->Draw();
            c1->SaveAs(Form("plots/%d_%d.png", year, etabin));
            c1->SaveAs(Form("plots/%d_%d.pdf", year, etabin));
        }
    }
}
