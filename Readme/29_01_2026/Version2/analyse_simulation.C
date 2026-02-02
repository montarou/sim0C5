//============================================================================
// analyse_simulation.C
// Script ROOT pour l'analyse des données de simulation Geant4
// 
// Usage: root -l -b -q 'analyse_simulation.C("output.root")'
//============================================================================

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TMath.h"
#include "TColor.h"
#include "TROOT.h"

#include <iostream>
#include <vector>
#include <string>

void analyse_simulation(const char* filename = "output.root")
{
    //==========================================================================
    // Configuration du style
    //==========================================================================
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
    gStyle->SetTitleFontSize(0.04);
    gStyle->SetLabelSize(0.035, "XY");
    gStyle->SetTitleSize(0.04, "XY");
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPalette(kRainBow);
    
    //==========================================================================
    // Ouverture du fichier
    //==========================================================================
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Erreur: impossible d'ouvrir " << filename << std::endl;
        return;
    }
    std::cout << "Fichier ouvert: " << filename << std::endl;
    
    //==========================================================================
    // Liste des plans de scoring
    //==========================================================================
    std::vector<std::string> planeNames = {
        "plane_passages",
        "ScorePlane2_passages", 
        "ScorePlane3_passages",
        "ScorePlane5_passages"
    };
    std::vector<std::string> planeLabels = {
        "z = 18 mm",
        "z = 30 mm",
        "z = 50 mm", 
        "z = 70 mm"
    };
    std::vector<int> planeColors = {kBlue, kGreen+2, kOrange+1, kRed};
    
    //==========================================================================
    // 1. HISTOGRAMMES 2D : Distribution XY des particules
    //==========================================================================
    std::cout << "\n=== Création des histogrammes 2D (X,Y) ===" << std::endl;
    
    TCanvas *c1 = new TCanvas("c1", "Distribution 2D - Primaires", 1400, 1000);
    c1->Divide(2, 2);
    
    TCanvas *c2 = new TCanvas("c2", "Distribution 2D - Secondaires", 1400, 1000);
    c2->Divide(2, 2);
    
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) {
            std::cout << "  Ntuple " << planeNames[i] << " non trouvé" << std::endl;
            continue;
        }
        
        std::cout << "  " << planeNames[i] << ": " << tree->GetEntries() << " entrées" << std::endl;
        
        // Histogramme 2D primaires
        c1->cd(i+1);
        gPad->SetLogz();
        TH2D *h2d_prim = new TH2D(Form("h2d_prim_%d", i), 
            Form("Primaires - %s;X (mm);Y (mm)", planeLabels[i].c_str()),
            100, -5, 5, 100, -5, 5);
        tree->Draw(Form("y_mm:x_mm>>h2d_prim_%d", i), "is_secondary==0 || parentID==0", "COLZ");
        h2d_prim->SetMinimum(1);
        
        // Histogramme 2D secondaires
        c2->cd(i+1);
        gPad->SetLogz();
        TH2D *h2d_sec = new TH2D(Form("h2d_sec_%d", i),
            Form("Secondaires - %s;X (mm);Y (mm)", planeLabels[i].c_str()),
            100, -5, 5, 100, -5, 5);
        tree->Draw(Form("y_mm:x_mm>>h2d_sec_%d", i), "is_secondary==1 && parentID!=0", "COLZ");
        h2d_sec->SetMinimum(1);
    }
    
    c1->SaveAs("distribution_2D_primaires.png");
    c1->SaveAs("distribution_2D_primaires.pdf");
    c2->SaveAs("distribution_2D_secondaires.png");
    c2->SaveAs("distribution_2D_secondaires.pdf");
    
    //==========================================================================
    // 2. DISTRIBUTION RADIALE (échelle log)
    //==========================================================================
    std::cout << "\n=== Création des distributions radiales ===" << std::endl;
    
    // Canvas pour primaires
    TCanvas *c3 = new TCanvas("c3", "Distribution radiale - Primaires", 1200, 800);
    c3->SetLogy();
    c3->SetGridx();
    c3->SetGridy();
    
    TLegend *leg3 = new TLegend(0.65, 0.65, 0.88, 0.88);
    leg3->SetBorderSize(1);
    leg3->SetFillColor(kWhite);
    
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_r_prim = new TH1D(Form("h_r_prim_%d", i),
            "Distribution radiale - Particules primaires;Rayon (mm);Counts",
            100, 0, 10);
        h_r_prim->SetLineColor(planeColors[i]);
        h_r_prim->SetLineWidth(2);
        
        tree->Draw(Form("sqrt(x_mm*x_mm + y_mm*y_mm)>>h_r_prim_%d", i), 
                   "is_secondary==0 || parentID==0", i==0 ? "" : "SAME");
        
        leg3->AddEntry(h_r_prim, planeLabels[i].c_str(), "l");
    }
    leg3->Draw();
    c3->SaveAs("distribution_radiale_primaires.png");
    c3->SaveAs("distribution_radiale_primaires.pdf");
    
    // Canvas pour secondaires
    TCanvas *c4 = new TCanvas("c4", "Distribution radiale - Secondaires", 1200, 800);
    c4->SetLogy();
    c4->SetGridx();
    c4->SetGridy();
    
    TLegend *leg4 = new TLegend(0.65, 0.65, 0.88, 0.88);
    leg4->SetBorderSize(1);
    leg4->SetFillColor(kWhite);
    
    bool firstSec = true;
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_r_sec = new TH1D(Form("h_r_sec_%d", i),
            "Distribution radiale - Particules secondaires;Rayon (mm);Counts",
            100, 0, 10);
        h_r_sec->SetLineColor(planeColors[i]);
        h_r_sec->SetLineWidth(2);
        
        tree->Draw(Form("sqrt(x_mm*x_mm + y_mm*y_mm)>>h_r_sec_%d", i),
                   "is_secondary==1 && parentID!=0", firstSec ? "" : "SAME");
        
        if (h_r_sec->GetEntries() > 0) {
            leg4->AddEntry(h_r_sec, planeLabels[i].c_str(), "l");
            firstSec = false;
        }
    }
    leg4->Draw();
    c4->SaveAs("distribution_radiale_secondaires.png");
    c4->SaveAs("distribution_radiale_secondaires.pdf");
    
    // Canvas pour toutes les particules
    TCanvas *c5 = new TCanvas("c5", "Distribution radiale - Toutes particules", 1200, 800);
    c5->SetLogy();
    c5->SetGridx();
    c5->SetGridy();
    
    TLegend *leg5 = new TLegend(0.65, 0.65, 0.88, 0.88);
    leg5->SetBorderSize(1);
    leg5->SetFillColor(kWhite);
    
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_r_all = new TH1D(Form("h_r_all_%d", i),
            "Distribution radiale - Toutes particules;Rayon (mm);Counts",
            100, 0, 10);
        h_r_all->SetLineColor(planeColors[i]);
        h_r_all->SetLineWidth(2);
        
        tree->Draw(Form("sqrt(x_mm*x_mm + y_mm*y_mm)>>h_r_all_%d", i), "", i==0 ? "" : "SAME");
        
        leg5->AddEntry(h_r_all, planeLabels[i].c_str(), "l");
    }
    leg5->Draw();
    c5->SaveAs("distribution_radiale_toutes.png");
    c5->SaveAs("distribution_radiale_toutes.pdf");
    
    //==========================================================================
    // 3. DISTRIBUTION EN ÉNERGIE
    //==========================================================================
    std::cout << "\n=== Création des distributions en énergie ===" << std::endl;
    
    // Toutes les particules
    TCanvas *c6 = new TCanvas("c6", "Distribution en énergie - Toutes", 1200, 800);
    c6->SetLogy();
    c6->SetGridx();
    c6->SetGridy();
    
    TLegend *leg6 = new TLegend(0.65, 0.65, 0.88, 0.88);
    leg6->SetBorderSize(1);
    leg6->SetFillColor(kWhite);
    
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_E_all = new TH1D(Form("h_E_all_%d", i),
            "Distribution en énergie - Toutes particules;Énergie (keV);Counts",
            100, 0, 50);
        h_E_all->SetLineColor(planeColors[i]);
        h_E_all->SetLineWidth(2);
        
        tree->Draw(Form("ekin_keV>>h_E_all_%d", i), "", i==0 ? "" : "SAME");
        
        leg6->AddEntry(h_E_all, planeLabels[i].c_str(), "l");
    }
    leg6->Draw();
    c6->SaveAs("distribution_energie_toutes.png");
    c6->SaveAs("distribution_energie_toutes.pdf");
    
    // Particules primaires
    TCanvas *c7 = new TCanvas("c7", "Distribution en énergie - Primaires", 1200, 800);
    c7->SetLogy();
    c7->SetGridx();
    c7->SetGridy();
    
    TLegend *leg7 = new TLegend(0.65, 0.65, 0.88, 0.88);
    leg7->SetBorderSize(1);
    leg7->SetFillColor(kWhite);
    
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_E_prim = new TH1D(Form("h_E_prim_%d", i),
            "Distribution en énergie - Particules primaires;Énergie (keV);Counts",
            100, 0, 50);
        h_E_prim->SetLineColor(planeColors[i]);
        h_E_prim->SetLineWidth(2);
        
        tree->Draw(Form("ekin_keV>>h_E_prim_%d", i), "is_secondary==0 || parentID==0", i==0 ? "" : "SAME");
        
        leg7->AddEntry(h_E_prim, planeLabels[i].c_str(), "l");
    }
    leg7->Draw();
    c7->SaveAs("distribution_energie_primaires.png");
    c7->SaveAs("distribution_energie_primaires.pdf");
    
    // Particules secondaires
    TCanvas *c8 = new TCanvas("c8", "Distribution en énergie - Secondaires", 1200, 800);
    c8->SetLogy();
    c8->SetGridx();
    c8->SetGridy();
    
    TLegend *leg8 = new TLegend(0.65, 0.65, 0.88, 0.88);
    leg8->SetBorderSize(1);
    leg8->SetFillColor(kWhite);
    
    bool firstSecE = true;
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_E_sec = new TH1D(Form("h_E_sec_%d", i),
            "Distribution en énergie - Particules secondaires;Énergie (keV);Counts",
            100, 0, 50);
        h_E_sec->SetLineColor(planeColors[i]);
        h_E_sec->SetLineWidth(2);
        
        tree->Draw(Form("ekin_keV>>h_E_sec_%d", i), "is_secondary==1 && parentID!=0", 
                   firstSecE ? "" : "SAME");
        
        if (h_E_sec->GetEntries() > 0) {
            leg8->AddEntry(h_E_sec, planeLabels[i].c_str(), "l");
            firstSecE = false;
        }
    }
    leg8->Draw();
    c8->SaveAs("distribution_energie_secondaires.png");
    c8->SaveAs("distribution_energie_secondaires.pdf");
    
    //==========================================================================
    // 4. HISTOGRAMMES DE DOSE PAR 1000 ÉVÉNEMENTS
    //==========================================================================
    std::cout << "\n=== Récupération des histogrammes de dose ===" << std::endl;
    
    TCanvas *c9 = new TCanvas("c9", "Dose par 1000 événements", 1400, 900);
    c9->Divide(3, 2);
    
    // Dose totale
    c9->cd(1);
    TH1D *h_dose_total = (TH1D*)f->Get("Dose_total_1000evt");
    if (h_dose_total) {
        h_dose_total->SetTitle("Dose totale par 1000 evt;Dose (#muGy);Counts");
        h_dose_total->SetLineColor(kBlack);
        h_dose_total->SetLineWidth(2);
        h_dose_total->SetFillColor(kGray);
        h_dose_total->Draw();
        std::cout << "  Dose_total_1000evt: " << h_dose_total->GetEntries() << " entrées" << std::endl;
    }
    
    // Dose par anneau
    int ringColors[5] = {kRed, kOrange+1, kYellow+1, kGreen+1, kCyan+1};
    for (int i = 0; i < 5; i++) {
        c9->cd(i+2);
        TH1D *h_dose_ring = (TH1D*)f->Get(Form("Dose_ring%d_1000evt", i));
        if (h_dose_ring) {
            h_dose_ring->SetTitle(Form("Dose anneau %d (r=%d-%d mm) par 1000 evt;Dose (#muGy);Counts", 
                                       i, 2*i, 2*(i+1)));
            h_dose_ring->SetLineColor(ringColors[i]);
            h_dose_ring->SetLineWidth(2);
            h_dose_ring->SetFillColor(ringColors[i]-9);
            h_dose_ring->Draw();
            std::cout << "  Dose_ring" << i << "_1000evt: " << h_dose_ring->GetEntries() << " entrées" << std::endl;
        }
    }
    
    c9->SaveAs("dose_par_1000evt.png");
    c9->SaveAs("dose_par_1000evt.pdf");
    
    //==========================================================================
    // 5. DOSE TOTALE PAR ANNEAU (avec pavé dose totale)
    //==========================================================================
    std::cout << "\n=== Création de l'histogramme dose vs anneau ===" << std::endl;
    
    TCanvas *c10 = new TCanvas("c10", "Dose par anneau", 1000, 700);
    c10->SetGridy();
    
    // Récupérer les doses depuis les histogrammes _run
    TH1D *h_dose_vs_ring = new TH1D("h_dose_vs_ring", 
        "Dose déposée par anneau;Numéro d'anneau;Dose (#muGy)", 5, -0.5, 4.5);
    
    double dose_totale = 0.0;
    double doses[5] = {0, 0, 0, 0, 0};
    
    // Récupérer la dose totale du run
    TH1D *h_total_run = (TH1D*)f->Get("Dose_total_run");
    if (h_total_run && h_total_run->GetEntries() > 0) {
        dose_totale = h_total_run->GetMean();
    }
    
    // Récupérer les doses par anneau
    for (int i = 0; i < 5; i++) {
        TH1D *h_ring = (TH1D*)f->Get(Form("Dose_ring%d_run", i));
        if (h_ring && h_ring->GetEntries() > 0) {
            doses[i] = h_ring->GetMean();
            h_dose_vs_ring->SetBinContent(i+1, doses[i]);
            std::cout << "  Anneau " << i << " (r=" << 2*i << "-" << 2*(i+1) 
                      << " mm): " << doses[i] << " µGy" << std::endl;
        }
    }
    
    // Style de l'histogramme
    h_dose_vs_ring->SetFillColor(kAzure-4);
    h_dose_vs_ring->SetLineColor(kAzure+2);
    h_dose_vs_ring->SetLineWidth(2);
    h_dose_vs_ring->SetBarWidth(0.8);
    h_dose_vs_ring->SetBarOffset(0.1);
    h_dose_vs_ring->GetYaxis()->SetRangeUser(0, h_dose_vs_ring->GetMaximum() * 1.3);
    
    // Labels personnalisés pour l'axe X
    h_dose_vs_ring->GetXaxis()->SetBinLabel(1, "0 (0-2mm)");
    h_dose_vs_ring->GetXaxis()->SetBinLabel(2, "1 (2-4mm)");
    h_dose_vs_ring->GetXaxis()->SetBinLabel(3, "2 (4-6mm)");
    h_dose_vs_ring->GetXaxis()->SetBinLabel(4, "3 (6-8mm)");
    h_dose_vs_ring->GetXaxis()->SetBinLabel(5, "4 (8-10mm)");
    h_dose_vs_ring->GetXaxis()->SetLabelSize(0.045);
    
    h_dose_vs_ring->Draw("BAR");
    
    // Ajouter les valeurs au-dessus des barres
    TLatex latex;
    latex.SetTextSize(0.035);
    latex.SetTextAlign(21);
    for (int i = 0; i < 5; i++) {
        latex.DrawLatex(i, doses[i] + h_dose_vs_ring->GetMaximum()*0.03, 
                        Form("%.4f", doses[i]));
    }
    
    // Pavé avec la dose totale
    TPaveText *pave = new TPaveText(0.55, 0.70, 0.88, 0.88, "NDC");
    pave->SetBorderSize(2);
    pave->SetFillColor(kWhite);
    pave->SetFillStyle(1001);
    pave->SetTextAlign(12);
    pave->SetTextFont(42);
    pave->AddText("#bf{Dose totale dans l'eau}");
    pave->AddText("");
    pave->AddText(Form("#bf{D_{total} = %.4f #muGy}", dose_totale));
    pave->AddText(Form("= %.2f nGy", dose_totale * 1000));
    pave->Draw();
    
    c10->SaveAs("dose_par_anneau.png");
    c10->SaveAs("dose_par_anneau.pdf");
    
    //==========================================================================
    // 6. FIGURE RÉCAPITULATIVE : Comparaison énergies par plan
    //==========================================================================
    std::cout << "\n=== Création de la figure récapitulative ===" << std::endl;
    
    TCanvas *c11 = new TCanvas("c11", "Récapitulatif", 1600, 1200);
    c11->Divide(2, 2);
    
    // Panneau 1: Distribution 2D (plan principal)
    c11->cd(1);
    gPad->SetLogz();
    TTree *tree0 = (TTree*)f->Get("plane_passages");
    if (tree0) {
        TH2D *h2d_recap = new TH2D("h2d_recap", 
            "Distribution XY (z=18mm);X (mm);Y (mm)", 100, -5, 5, 100, -5, 5);
        tree0->Draw("y_mm:x_mm>>h2d_recap", "", "COLZ");
    }
    
    // Panneau 2: Spectre en énergie superposé
    c11->cd(2);
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    
    TLegend *leg11 = new TLegend(0.60, 0.65, 0.88, 0.88);
    leg11->SetBorderSize(1);
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_E = new TH1D(Form("h_E_recap_%d", i),
            "Spectre en énergie;Énergie (keV);Counts", 100, 0, 50);
        h_E->SetLineColor(planeColors[i]);
        h_E->SetLineWidth(2);
        tree->Draw(Form("ekin_keV>>h_E_recap_%d", i), "", i==0 ? "" : "SAME");
        leg11->AddEntry(h_E, planeLabels[i].c_str(), "l");
    }
    leg11->Draw();
    
    // Panneau 3: Distribution radiale superposée
    c11->cd(3);
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    
    TLegend *leg11b = new TLegend(0.60, 0.65, 0.88, 0.88);
    leg11b->SetBorderSize(1);
    for (int i = 0; i < 4; i++) {
        TTree *tree = (TTree*)f->Get(planeNames[i].c_str());
        if (!tree) continue;
        
        TH1D *h_r = new TH1D(Form("h_r_recap_%d", i),
            "Distribution radiale;Rayon (mm);Counts", 100, 0, 10);
        h_r->SetLineColor(planeColors[i]);
        h_r->SetLineWidth(2);
        tree->Draw(Form("sqrt(x_mm*x_mm+y_mm*y_mm)>>h_r_recap_%d", i), "", i==0 ? "" : "SAME");
        leg11b->AddEntry(h_r, planeLabels[i].c_str(), "l");
    }
    leg11b->Draw();
    
    // Panneau 4: Dose par anneau
    c11->cd(4);
    gPad->SetGridy();
    h_dose_vs_ring->Draw("BAR");
    
    TPaveText *pave2 = new TPaveText(0.50, 0.70, 0.88, 0.88, "NDC");
    pave2->SetBorderSize(2);
    pave2->SetFillColor(kWhite);
    pave2->AddText("#bf{Dose totale}");
    pave2->AddText(Form("#bf{%.4f #muGy}", dose_totale));
    pave2->Draw();
    
    c11->SaveAs("recapitulatif_simulation.png");
    c11->SaveAs("recapitulatif_simulation.pdf");
    
    //==========================================================================
    // Fermeture et résumé
    //==========================================================================
    std::cout << "\n=== Fichiers générés ===" << std::endl;
    std::cout << "  - distribution_2D_primaires.png/.pdf" << std::endl;
    std::cout << "  - distribution_2D_secondaires.png/.pdf" << std::endl;
    std::cout << "  - distribution_radiale_primaires.png/.pdf" << std::endl;
    std::cout << "  - distribution_radiale_secondaires.png/.pdf" << std::endl;
    std::cout << "  - distribution_radiale_toutes.png/.pdf" << std::endl;
    std::cout << "  - distribution_energie_toutes.png/.pdf" << std::endl;
    std::cout << "  - distribution_energie_primaires.png/.pdf" << std::endl;
    std::cout << "  - distribution_energie_secondaires.png/.pdf" << std::endl;
    std::cout << "  - dose_par_1000evt.png/.pdf" << std::endl;
    std::cout << "  - dose_par_anneau.png/.pdf" << std::endl;
    std::cout << "  - recapitulatif_simulation.png/.pdf" << std::endl;
    
    std::cout << "\n=== Résumé des doses ===" << std::endl;
    std::cout << "  Dose totale: " << dose_totale << " µGy = " << dose_totale*1000 << " nGy" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << "  Anneau " << i << " (r=" << 2*i << "-" << 2*(i+1) << " mm): " 
                  << doses[i] << " µGy" << std::endl;
    }
    
    f->Close();
    std::cout << "\n=== Analyse terminée ===" << std::endl;
}
