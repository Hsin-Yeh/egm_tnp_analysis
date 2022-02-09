// Calculate pt bin mean with:
// fitter_tree->Draw("ph_et >> h", "ph_et>120 && ph_et<200 && passingCutBasedLooseHighPtIDV2")
// h->GetMean()

void fitGradiantLimit(){
    vector<int> years;
    years.push_back(2016);
    years.push_back(2017);
    years.push_back(2018);
    const int totalEtaBins = 3;
    const int totalPtBins  = 5;

    gStyle->SetOptFit(0000);

    for ( auto year : years ) {


        // get pt mean value of each bin
        float meanPt[totalEtaBins][totalPtBins];
        float errorPt[totalEtaBins][totalPtBins];

        string ptMeanName = "./ptMean/ptMean_" + to_string(year) + ".txt";
        ifstream ptMeanFile(ptMeanName);
        for ( int ptbin = 0; ptbin < totalPtBins; ptbin++){
            for ( int etabin = 0; etabin < totalEtaBins; etabin++){
                ptMeanFile >> meanPt[etabin][ptbin] >> errorPt[etabin][ptbin];
            }
        }

        TFile* fmultibin = new TFile(Form("tnpOutput/egammaEffi.txt_EGM2D_%d.root",year), "read");
        TH2D* h_SF = (TH2D*)fmultibin->Get("EGamma_SF2D");
        TH2D* h_effData = (TH2D*)fmultibin->Get("EGamma_EffData2D");
        TH2D* h_effMC = (TH2D*)fmultibin->Get("EGamma_EffMC2D");
        TH2D* h_statMC = (TH2D*)fmultibin->Get("statMC");
        TH2D* h_statData = (TH2D*)fmultibin->Get("statData");

        float sf[totalEtaBins][totalPtBins];
        float error_sf[totalEtaBins][totalPtBins];  // statistical + systematics uncertainty
        float effData[totalEtaBins][totalPtBins];
        float effMC[totalEtaBins][totalPtBins];
        float statData[totalEtaBins][totalPtBins];
        float statMC[totalEtaBins][totalPtBins];
        float statError_sf[totalEtaBins][totalPtBins]; // MC + Data statistical uncertainty
        float ExtraSF[totalEtaBins];
        float ExtraSys[totalEtaBins];

        for ( int ptbin = 0; ptbin < totalPtBins; ptbin++){
            for ( int etabin = 0; etabin < totalEtaBins; etabin++){

                sf[etabin][ptbin]           = h_SF->GetBinContent(totalEtaBins-etabin, ptbin+1);
                error_sf[etabin][ptbin]     = h_SF->GetBinError(totalEtaBins-etabin, ptbin+1);

                effData[etabin][ptbin]      = h_effData->GetBinContent(totalEtaBins-etabin, ptbin+1);
                effMC[etabin][ptbin]        = h_effMC->GetBinContent(totalEtaBins-etabin, ptbin+1);
                statData[etabin][ptbin]     = h_statData->GetBinContent(totalEtaBins-etabin, ptbin+1);
                statMC[etabin][ptbin]       = h_statMC->GetBinContent(totalEtaBins-etabin, ptbin+1);

                // Using stat uncertainty only for the fit ( systematic uncertainties are correlated between pt bins and will overestimate the error of p1 )
                statError_sf[etabin][ptbin] = sqrt( pow(statData[etabin][ptbin] / effData[etabin][ptbin], 2) + pow(statMC[etabin][ptbin] / effMC[etabin][ptbin], 2) ) * sf[etabin][ptbin];

                std::cout << sf[etabin][ptbin] << " " << error_sf[etabin][ptbin] << std::endl;
            }
        }

        TCanvas* c1 = new TCanvas();
        TF1* fit = new TF1("fit","[0]+[1]*(x-200)",125,1000);

        string title[totalEtaBins];
        title[0] = "0 < |#eta_{SC}| < 0.8";
        title[1] = "0.8 < |#eta_{SC}| < 1.4442";
        title[2] = "1.566 < |#eta_{SC}| < 2.5";
        float etaEdge[5] = {0, 0.8, 1.4442, 1.566, 2.5};

        for ( int etabin = 0; etabin < totalEtaBins; etabin++ ) {
            fit->SetParameters(1,0);

            TGraphErrors* g = new TGraphErrors(5, meanPt[etabin],sf[etabin], errorPt[etabin], statError_sf[etabin]);
            g->Draw("APE");
            g->GetXaxis()->SetTitle("P_{t}^{#gamma} [GeV]");
            g->GetYaxis()->SetTitle("Scale Factor");
            g->SetTitle(Form("year:%d, etabin:%s",year, title[etabin].c_str()));
            g->GetYaxis()->SetRangeUser(0.8, 1.15);
            g->Fit("fit","R");
            ExtraSF[etabin] = fit->GetParameter(0);
            ExtraSys[etabin] = fit->GetParError(1);

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
            c1->SaveAs(Form("plots/%d_%d_Fit.png", year, etabin));
            c1->SaveAs(Form("plots/%d_%d_Fit.pdf", year, etabin));
        }


        for ( int etabin = 0; etabin < totalEtaBins; etabin++ ) {
            double x[] = {200, 1000};
            double y[] = {ExtraSF[etabin], ExtraSF[etabin]};
            double ex[] = {0., 0.};
            double ey[] = {statError_sf[etabin][1], sqrt(1000*ExtraSys[etabin]*1000*ExtraSys[etabin] + statError_sf[etabin][1]*statError_sf[etabin][1])};
            auto ge = new TGraphErrors(2, x, y, ex, ey);
            ge->SetFillColorAlpha(38, 0.35);
            ge->SetFillStyle(1001);
            ge->SetLineColor(38);
            ge->SetLineWidth(2);
            ge->Draw("al3");
            ge->GetYaxis()->SetRangeUser(0.8, 1.2);
            ge->GetXaxis()->SetRangeUser(100, 1500);
            ge->SetTitle(Form("year:%d, etabin:%s",year, title[etabin].c_str()));
            ge->GetXaxis()->SetTitle("P_{t}^{#gamma} [GeV]");
            ge->GetYaxis()->SetTitle("Scale Factor");

            TGraphErrors* g = new TGraphErrors(5, meanPt[etabin],sf[etabin], errorPt[etabin], statError_sf[etabin]);
            g->Draw("PEsame");

            TLegend *leg = new TLegend(0.2,0.7,0.6,0.85);
            leg->AddEntry(g,"measured SF","lep");
            leg->AddEntry(ge,"extrapolation SF + systematics", "lf");
            leg->Draw("same");

            c1->SaveAs(Form("plots/%d_%d_Check.png", year, etabin));
            c1->SaveAs(Form("plots/%d_%d_Check.pdf", year, etabin));
        }

        for ( int etabin = 0; etabin < totalEtaBins; etabin++ ) {
            double fixed_x[] = {200, 1000};
            double fixed_y[] = {ExtraSF[etabin], ExtraSF[etabin]};
            double fixed_ex[] = {0., 0.};
            double fixed_ey[] = {0.06, 0.06};
            auto g_fixed = new TGraphErrors(2, fixed_x, fixed_y, fixed_ex, fixed_ey);
            g_fixed->SetFillColorAlpha(kOrange+7, 0.35);
            g_fixed->SetFillStyle(1001);
            g_fixed->SetLineColor(kOrange+7);
            g_fixed->SetLineWidth(2);
            g_fixed->Draw("al3");
            g_fixed->GetYaxis()->SetRangeUser(0.8, 1.2);
            g_fixed->GetXaxis()->SetRangeUser(100, 1500);
            g_fixed->SetTitle(Form("year:%d, etabin:%s",year, title[etabin].c_str()));
            g_fixed->GetXaxis()->SetTitle("P_{t}^{#gamma} [GeV]");
            g_fixed->GetYaxis()->SetTitle("Scale Factor");

            double x[] = {200, 1000};
            double y[] = {ExtraSF[etabin], ExtraSF[etabin]};
            double ex[] = {0., 0.};
            double ey[] = {error_sf[etabin][1], sqrt(1000*ExtraSys[etabin]*1000*ExtraSys[etabin] + error_sf[etabin][1]*error_sf[etabin][1])};
            auto ge = new TGraphErrors(2, x, y, ex, ey);
            ge->SetFillColorAlpha(38, 0.35);
            ge->SetFillStyle(1001);
            ge->SetLineColor(38);
            ge->SetLineWidth(2);
            ge->Draw("l3same");

            TLegend *leg = new TLegend(0.2,0.7,0.6,0.85);
            leg->AddEntry(g_fixed,"6% fixed uncertainty", "lf");
            leg->AddEntry(ge,"extrapolation SF + uncertainty","lf");
            leg->Draw("same");

            c1->SaveAs(Form("plots/%d_%d_Compare.png", year, etabin));
            c1->SaveAs(Form("plots/%d_%d_Compare.pdf", year, etabin));
        }

        // Output SF and extrapolation uncertainty
        ofstream fout;
        string foutName = "SF/SF_" + to_string(year) + ".txt";
        fout.open(foutName);
        fout << "abs(eta)_min | abs(eta)_max | SF{125<pt<200} | Syst{125<pt<200} | SF{200<pt} | Syst{200<pt} | Extrapolate_Syst/GeV{200<pt}" << std::endl;
        fout << fixed << "0.0000" << "\t" << "0.8000\t" << std::setprecision(4) << sf[0][0] << "\t" << error_sf[0][0] << "\t" << ExtraSF[0] << "\t" << error_sf[0][1] << "\t" << scientific << std::setprecision(2) << ExtraSys[0] << std::endl;
        fout << fixed << "0.8000" << "\t" << "1.4442\t" << std::setprecision(4) << sf[1][0] << "\t" << error_sf[1][0] << "\t" << ExtraSF[1] << "\t" << error_sf[1][1] << "\t" << scientific << std::setprecision(2) << ExtraSys[1] << std::endl;
        fout << fixed << "1.5660" << "\t" << "2.5000\t" << std::setprecision(4) << sf[2][0] << "\t" << error_sf[2][0] << "\t" << ExtraSF[2] << "\t" << error_sf[2][1] << "\t" << scientific << std::setprecision(2) << ExtraSys[2] << std::endl;
    }
}
