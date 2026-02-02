#include "AnalysisManagerSetup.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4SDManager.hh"
#include "SurfaceSpectrumSD.hh"
#include "ScorePlane2SD.hh"
#include "ScorePlane3SD.hh"
#include "ScorePlane4SD.hh"
#include "ScorePlane5SD.hh"
// ScorePlane6SD supprimé
#include "G4Run.hh"

// Variables globales pour stocker les IDs des ntuples
static int g_planePassageNtupleId = -1;
static int g_scorePlane2NtupleId = -1;
static int g_scorePlane3NtupleId = -1;
static int g_scorePlane4NtupleId = -1;
static int g_scorePlane5NtupleId = -1;
// g_scorePlane6NtupleId supprimé

void SetupAnalysis()
{

    auto man = G4AnalysisManager::Instance();
    auto analysisManager = man;
    man->SetVerboseLevel(1);
    man->SetFirstNtupleId(0);
    man->SetFirstHistoId(0);

    // ==================== Histogrammes 1D ====================
    
    // H0: Énergie des gammas primaires à l'émission
    analysisManager->CreateH1("E_emission", 
        "Énergie des gammas primaires à l'émission", 150, 0., 50.*keV);  // ID 0
    
    // H1: Angle theta des gammas primaires
    analysisManager->CreateH1("theta_emission", 
        "Angle theta des gammas primaires", 180, 0., 180.);  // ID 1 (degrés)
    
    // H2: Angle phi des gammas primaires
    analysisManager->CreateH1("phi_emission", 
        "Angle phi des gammas primaires", 90, -180., 180.);  // ID 2 (degrés)
    
    // H3: Dose totale dans tout l'eau (run complet)
    analysisManager->CreateH1("Dose_total_run", 
        "Dose totale dans l'eau (run)", 200, 0., 1.);  // ID 3 (µGy)
    
    // H4: Dose totale dans tout l'eau (par 1000 événements)
    analysisManager->CreateH1("Dose_total_1000evt", 
        "Dose totale dans l'eau (1000 evt)", 200, 0., 1.);  // ID 4 (µGy)
    
    // H5-H9: Dose par anneau (run complet)
    analysisManager->CreateH1("Dose_ring0_run", 
        "Dose anneau 0 (r=0-2mm) run", 200, 0., 1.);  // ID 5 (µGy)
    analysisManager->CreateH1("Dose_ring1_run", 
        "Dose anneau 1 (r=2-4mm) run", 200, 0., 1.);  // ID 6 (µGy)
    analysisManager->CreateH1("Dose_ring2_run", 
        "Dose anneau 2 (r=4-6mm) run", 200, 0., 1.);  // ID 7 (µGy)
    analysisManager->CreateH1("Dose_ring3_run", 
        "Dose anneau 3 (r=6-8mm) run", 200, 0., 1.);  // ID 8 (µGy)
    analysisManager->CreateH1("Dose_ring4_run", 
        "Dose anneau 4 (r=8-10mm) run", 200, 0., 1.);  // ID 9 (µGy)
    
    // H10-H14: Dose par anneau (par 1000 événements)
    analysisManager->CreateH1("Dose_ring0_1000evt", 
        "Dose anneau 0 (r=0-2mm) 1000evt", 200, 0., 1.);  // ID 10 (µGy)
    analysisManager->CreateH1("Dose_ring1_1000evt", 
        "Dose anneau 1 (r=2-4mm) 1000evt", 200, 0., 1.);  // ID 11 (µGy)
    analysisManager->CreateH1("Dose_ring2_1000evt", 
        "Dose anneau 2 (r=4-6mm) 1000evt", 200, 0., 1.);  // ID 12 (µGy)
    analysisManager->CreateH1("Dose_ring3_1000evt", 
        "Dose anneau 3 (r=6-8mm) 1000evt", 200, 0., 1.);  // ID 13 (µGy)
    analysisManager->CreateH1("Dose_ring4_1000evt", 
        "Dose anneau 4 (r=8-10mm) 1000evt", 200, 0., 1.);  // ID 14 (µGy)

    // ==================== Ntuple plane_passages ====================
    // Ntuple des passages plan +Z (ScorePlane à z = 18 mm)
    // Colonnes : x_mm, y_mm, z_mm, ekin_keV, pdg, name, trackID, parentID, creator_process
    g_planePassageNtupleId = analysisManager->CreateNtuple("plane_passages", "Traversées +Z du plan mince");
    analysisManager->CreateNtupleDColumn(g_planePassageNtupleId, "x_mm");            // 0
    analysisManager->CreateNtupleDColumn(g_planePassageNtupleId, "y_mm");            // 1
    analysisManager->CreateNtupleDColumn(g_planePassageNtupleId, "z_mm");            // 2
    analysisManager->CreateNtupleDColumn(g_planePassageNtupleId, "ekin_keV");        // 3
    analysisManager->CreateNtupleIColumn(g_planePassageNtupleId, "pdg");             // 4
    analysisManager->CreateNtupleSColumn(g_planePassageNtupleId, "name");            // 5
    analysisManager->CreateNtupleIColumn(g_planePassageNtupleId, "trackID");         // 6
    analysisManager->CreateNtupleIColumn(g_planePassageNtupleId, "parentID");        // 7
    analysisManager->CreateNtupleSColumn(g_planePassageNtupleId, "creator_process"); // 8
    analysisManager->FinishNtuple(g_planePassageNtupleId);

    // ==================== Ntuple ScorePlane2 ====================
    // Ntuple pour le plan de comptage ScorePlane2 (z = 28 mm)
    // Colonnes : pdg, name, is_secondary, x_mm, y_mm, ekin_keV, trackID, parentID, creator_process
    g_scorePlane2NtupleId = analysisManager->CreateNtuple("ScorePlane2_passages", 
        "Traversées +Z du plan ScorePlane2");
    analysisManager->CreateNtupleIColumn(g_scorePlane2NtupleId, "pdg");           // 0: Code PDG
    analysisManager->CreateNtupleSColumn(g_scorePlane2NtupleId, "name");          // 1: Nom particule
    analysisManager->CreateNtupleIColumn(g_scorePlane2NtupleId, "is_secondary");  // 2: 0=primaire, 1=secondaire
    analysisManager->CreateNtupleDColumn(g_scorePlane2NtupleId, "x_mm");          // 3: Position X (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane2NtupleId, "y_mm");          // 4: Position Y (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane2NtupleId, "ekin_keV");      // 5: Énergie cinétique (keV)
    analysisManager->CreateNtupleIColumn(g_scorePlane2NtupleId, "trackID");       // 6: TrackID
    analysisManager->CreateNtupleIColumn(g_scorePlane2NtupleId, "parentID");      // 7: ParentID
    analysisManager->CreateNtupleSColumn(g_scorePlane2NtupleId, "creator_process"); // 8: Processus créateur
    analysisManager->FinishNtuple(g_scorePlane2NtupleId);

    // ==================== Ntuple ScorePlane3 ====================
    // Ntuple pour le plan de comptage ScorePlane3 (z = 38 mm)
    // Colonnes : pdg, name, is_secondary, x_mm, y_mm, ekin_keV, trackID, parentID, creator_process
    g_scorePlane3NtupleId = analysisManager->CreateNtuple("ScorePlane3_passages", 
        "Traversées +Z du plan ScorePlane3");
    analysisManager->CreateNtupleIColumn(g_scorePlane3NtupleId, "pdg");             // 0: Code PDG
    analysisManager->CreateNtupleSColumn(g_scorePlane3NtupleId, "name");            // 1: Nom particule
    analysisManager->CreateNtupleIColumn(g_scorePlane3NtupleId, "is_secondary");    // 2: 0=primaire, 1=secondaire
    analysisManager->CreateNtupleDColumn(g_scorePlane3NtupleId, "x_mm");            // 3: Position X (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane3NtupleId, "y_mm");            // 4: Position Y (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane3NtupleId, "ekin_keV");        // 5: Énergie cinétique (keV)
    analysisManager->CreateNtupleIColumn(g_scorePlane3NtupleId, "trackID");         // 6: TrackID
    analysisManager->CreateNtupleIColumn(g_scorePlane3NtupleId, "parentID");        // 7: ParentID
    analysisManager->CreateNtupleSColumn(g_scorePlane3NtupleId, "creator_process"); // 8: Processus créateur
    analysisManager->FinishNtuple(g_scorePlane3NtupleId);

    // ==================== Ntuple WaterRings ====================
    // Ntuple pour les couronnes d'eau (z = 68 mm, fond de l'eau)
    // Remplace l'ancien ScorePlane4
    g_scorePlane4NtupleId = analysisManager->CreateNtuple("WaterRings_passages", 
        "Traversées dans les couronnes d'eau");
    analysisManager->CreateNtupleIColumn(g_scorePlane4NtupleId, "pdg");             // 0: Code PDG
    analysisManager->CreateNtupleSColumn(g_scorePlane4NtupleId, "name");            // 1: Nom particule
    analysisManager->CreateNtupleIColumn(g_scorePlane4NtupleId, "is_secondary");    // 2: 0=primaire, 1=secondaire
    analysisManager->CreateNtupleDColumn(g_scorePlane4NtupleId, "x_mm");            // 3: Position X (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane4NtupleId, "y_mm");            // 4: Position Y (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane4NtupleId, "ekin_keV");        // 5: Énergie cinétique (keV)
    analysisManager->CreateNtupleIColumn(g_scorePlane4NtupleId, "trackID");         // 6: TrackID
    analysisManager->CreateNtupleIColumn(g_scorePlane4NtupleId, "parentID");        // 7: ParentID
    analysisManager->CreateNtupleSColumn(g_scorePlane4NtupleId, "creator_process"); // 8: Processus créateur
    analysisManager->FinishNtuple(g_scorePlane4NtupleId);

    // ==================== Ntuple ScorePlane5 ====================
    // Ntuple pour le plan de comptage ScorePlane5 (z = 70 mm)
    g_scorePlane5NtupleId = analysisManager->CreateNtuple("ScorePlane5_passages", 
        "Traversées +Z du plan ScorePlane5");
    analysisManager->CreateNtupleIColumn(g_scorePlane5NtupleId, "pdg");             // 0: Code PDG
    analysisManager->CreateNtupleSColumn(g_scorePlane5NtupleId, "name");            // 1: Nom particule
    analysisManager->CreateNtupleIColumn(g_scorePlane5NtupleId, "is_secondary");    // 2: 0=primaire, 1=secondaire
    analysisManager->CreateNtupleDColumn(g_scorePlane5NtupleId, "x_mm");            // 3: Position X (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane5NtupleId, "y_mm");            // 4: Position Y (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane5NtupleId, "ekin_keV");        // 5: Énergie cinétique (keV)
    analysisManager->CreateNtupleIColumn(g_scorePlane5NtupleId, "trackID");         // 6: TrackID
    analysisManager->CreateNtupleIColumn(g_scorePlane5NtupleId, "parentID");        // 7: ParentID
    analysisManager->CreateNtupleSColumn(g_scorePlane5NtupleId, "creator_process"); // 8: Processus créateur
    analysisManager->FinishNtuple(g_scorePlane5NtupleId);

    // Ntuple ScorePlane6 supprimé

    //  Raccorder l'ID au SD spectral (maintenant défini)
    if (auto* sd = dynamic_cast<SurfaceSpectrumSD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("SpecSD", /*warning=*/false))) {
        sd->Reset();
        sd->SetPassageNtupleId(g_planePassageNtupleId);
        // (L'aire [cm^2] est réglée côté DetectorConstruction via specSD->SetArea_cm2(...))
    }

    //  Raccorder l'ID au SD ScorePlane2 (si défini)
    if (auto* sd2 = dynamic_cast<ScorePlane2SD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("ScorePlane2SD", /*warning=*/false))) {
        sd2->SetNtupleId(g_scorePlane2NtupleId);
        G4cout << "[SetupAnalysis] ScorePlane2SD connecté au ntuple id=" << g_scorePlane2NtupleId << G4endl;
    }

    //  Raccorder l'ID au SD ScorePlane3 (si défini)
    if (auto* sd3 = dynamic_cast<ScorePlane3SD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("ScorePlane3SD", /*warning=*/false))) {
        sd3->SetNtupleId(g_scorePlane3NtupleId);
        G4cout << "[SetupAnalysis] ScorePlane3SD connecté au ntuple id=" << g_scorePlane3NtupleId << G4endl;
    }

    //  Raccorder l'ID au SD ScorePlane4 (WaterRings) (si défini)
    if (auto* sd4 = dynamic_cast<ScorePlane4SD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("ScorePlane4SD", /*warning=*/false))) {
        sd4->SetNtupleId(g_scorePlane4NtupleId);
        G4cout << "[SetupAnalysis] ScorePlane4SD (WaterRings) connecté au ntuple id=" << g_scorePlane4NtupleId << G4endl;
    }

    //  Raccorder l'ID au SD ScorePlane5 (si défini)
    if (auto* sd5 = dynamic_cast<ScorePlane5SD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("ScorePlane5SD", /*warning=*/false))) {
        sd5->SetNtupleId(g_scorePlane5NtupleId);
        G4cout << "[SetupAnalysis] ScorePlane5SD connecté au ntuple id=" << g_scorePlane5NtupleId << G4endl;
    }

    // ScorePlane6SD supprimé
}

void FinalizeAnalysis()
{
    auto man = G4AnalysisManager::Instance();
    #if 0
    // Déplacé dans RunAction::EndOfRunAction()
    man->Write();
    man->CloseFile(false); // false = pratique en mode interactif
    #endif
}

// Implémentation des fonctions getter pour les IDs des ntuples
int GetPlanePassageNtupleId()
{
    return g_planePassageNtupleId;
}

int GetScorePlane2NtupleId()
{
    return g_scorePlane2NtupleId;
}

int GetScorePlane3NtupleId()
{
    return g_scorePlane3NtupleId;
}

int GetScorePlane4NtupleId()
{
    return g_scorePlane4NtupleId;
}

int GetScorePlane5NtupleId()
{
    return g_scorePlane5NtupleId;
}

// GetScorePlane6NtupleId() supprimé
