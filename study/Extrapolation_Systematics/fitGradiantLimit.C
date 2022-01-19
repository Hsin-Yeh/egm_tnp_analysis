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
        float ExtraSF[totalEtaBins];
        float ExtraSys[totalEtaBins];

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
        TF1* fit = new TF1("fit","[0]+[1]*(x-200)",125,1000);

        string title[totalEtaBins];
        title[0] = "0 < |#eta_{SC}| < 0.8";
        title[1] = "0.8 < |#eta_{SC}| < 1.4442";
        title[2] = "1.566 < |#eta_{SC}| < 2.5";
        float etaEdge[5] = {0, 0.8, 1.4442, 1.566, 2.5};

        for ( int etabin = 0; etabin < totalEtaBins; etabin++ ) {
            fit->SetParameters(1,0);

            TGraphErrors* g = new TGraphErrors(5, meanPt[etabin],sf[etabin], errorPt[etabin], e_sf[etabin]);
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
            double ey[] = {e_sf[etabin][1], sqrt(1000*ExtraSys[etabin]*1000*ExtraSys[etabin] + e_sf[etabin][1]*e_sf[etabin][1])};
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

            TGraphErrors* g = new TGraphErrors(5, meanPt[etabin],sf[etabin], errorPt[etabin], e_sf[etabin]);
            g->Draw("PEsame");

            TLegend *leg = new TLegend(0.2,0.7,0.6,0.85);
            leg->AddEntry(g,"measured SF","lep");
            leg->AddEntry(ge,"extrapolation SF + systematics", "lf");
            leg->Draw("same");

            c1->SaveAs(Form("plots/%d_%d_Check.png", year, etabin));
            c1->SaveAs(Form("plots/%d_%d_Check.pdf", year, etabin));

            std::cout << ExtraSF[etabin] << " " << sqrt(1000*ExtraSys[etabin]*1000*ExtraSys[etabin] + e_sf[etabin][1]*e_sf[etabin][1]) << std::endl;
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
            double ey[] = {e_sf[etabin][1], sqrt(1000*ExtraSys[etabin]*1000*ExtraSys[etabin] + e_sf[etabin][1]*e_sf[etabin][1])};
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
        string foutName = "Extrapolated_SF_" + to_string(year) + ".txt";
        fout.open(foutName);
        fout << "abs(eta)_min | abs(eta)_max | SF{125<pt<200} | Syst{125<pt<200} | SF{200<pt} | Syst{200<pt} | Extrapolate_Syst{200<pt}" << std::endl;
        fout << fixed << "0.0000" << "\t" << "0.8000\t" << std::setprecision(3) << sf[0][0] << "\t" << e_sf[0][0] << "\t" << ExtraSF[0] << "\t" << std::setprecision(4) << e_sf[0][1] << "\t" << scientific << std::setprecision(2) << ExtraSys[0] << std::endl;
        fout << fixed << "0.8000" << "\t" << "1.4442\t" << std::setprecision(3) << sf[1][0] << "\t" << e_sf[1][0] << "\t" << ExtraSF[1] << "\t" << std::setprecision(4) << e_sf[1][1] << "\t" << scientific << std::setprecision(2) << ExtraSys[1] << std::endl;
        fout << fixed << "1.5660" << "\t" << "2.5000\t" << std::setprecision(3) << sf[2][0] << "\t" << e_sf[2][0] << "\t" << ExtraSF[2] << "\t" << std::setprecision(4) << e_sf[2][1] << "\t" << scientific << std::setprecision(2) << ExtraSys[2] << std::endl;
    }
}
