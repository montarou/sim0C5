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
#include "ScorePlane6SD.hh"
#include "G4Run.hh"

// Variables globales pour stocker les IDs des ntuples
static int g_planePassageNtupleId = -1;
static int g_scorePlane2NtupleId = -1;
static int g_scorePlane3NtupleId = -1;
static int g_scorePlane4NtupleId = -1;
static int g_scorePlane5NtupleId = -1;
static int g_scorePlane6NtupleId = -1;

void SetupAnalysis()
{

    auto man = G4AnalysisManager::Instance();
    auto analysisManager = man;
    man->SetVerboseLevel(1);
    man->SetFirstNtupleId(0);
    man->SetFirstHistoId(0);

    // Déclaration des histogrammes
    analysisManager->CreateH1("E_in",  "Énergie des particules entrantes dans la sphere", 75, 0., 75*keV); //  0
    analysisManager->CreateH1("E_out", "Énergie des particules sortantes de la sphere", 75, 0., 75*keV);   //  1
    analysisManager->CreateH1("E_in_Be",  "Énergie des particules entrant dans Beryllium", 75, 0., 75*keV);//  2
    analysisManager->CreateH1("E_out_Be", "Énergie des particules sortant du Beryllium", 75, 0., 75*keV);  //  3
    analysisManager->CreateH1("E_dep_Be", "Énergie deposee dans le Beryllium", 100, 0., 50*keV);           //  4
    analysisManager->CreateH1("E_dep_WaterSphere", "Énergie deposee dans la WaterSphere", 100, 0., 50*keV);//  5
    analysisManager->CreateH1("NbHitsPerEvent", "Nombre de hits par événement", 10, 0., 10.);              //  6
    analysisManager->CreateH1("theta primaire", "Distribution theta primaire", 90, 0., 180.);              //  7
    analysisManager->CreateH1("phi primaire", "Distribution phi primaire", 90, -180., 180.);               //  8
    analysisManager->CreateH1("theta secondaire", "Distribution theta secondaire", 90, 0., 180.);          //  9
    analysisManager->CreateH1("phi secondaire", "Distribution phi secondaire", 90, -180., 180.);           // 10
    analysisManager->CreateH1("theta emission", "Distribution theta primaire initial", 90, 0., 180.);      // 11
    analysisManager->CreateH1("energie a l'emission", "Distribution energie initiale", 100, 0., 50*keV);   // 12

    // ==================== Ntuples existants ====================
    // Ntuple pour les hits dans la sphère
    analysisManager->CreateNtuple("SphereHits", "Dépôts d'énergie dans la sphère");
    analysisManager->CreateNtupleIColumn("Event"); // Event Number
    analysisManager->CreateNtupleDColumn("x");     // position X
    analysisManager->CreateNtupleDColumn("y");     // position Y
    analysisManager->CreateNtupleDColumn("z");     // position Z
    analysisManager->CreateNtupleDColumn("Edep");  // énergie déposée
    analysisManager->CreateNtupleIColumn("particle_pdg");
    analysisManager->CreateNtupleSColumn("particle_name");
    analysisManager->CreateNtupleSColumn("process_name");
    analysisManager->CreateNtupleIColumn("process_type");
    analysisManager->CreateNtupleIColumn("process_subtype");
    analysisManager->FinishNtuple();

    // Ntuple pour les infos de trace
    analysisManager->CreateNtuple("trackInfo", "Track user info");
    analysisManager->CreateNtupleIColumn("eventID");
    analysisManager->CreateNtupleIColumn("enteredCube");
    analysisManager->CreateNtupleIColumn("enteredSphere");
    analysisManager->CreateNtupleSColumn("creatorProcess");
    analysisManager->CreateNtupleIColumn("nbInteractedInBe");
    analysisManager->FinishNtuple();

    // Statistiques par événement
    analysisManager->CreateNtuple("SphereStats", "Statistiques par événement");
    analysisManager->CreateNtupleIColumn("Event");
    analysisManager->CreateNtupleIColumn("NbHits");
    analysisManager->CreateNtupleDColumn("TotalEdep");
    analysisManager->CreateNtupleDColumn("MeanEdep");            // énergie moyenne
    analysisManager->CreateNtupleDColumn("MaxEdep");             // max edep
    analysisManager->CreateNtupleSColumn("MostCommonParticle");  // nom le plus fréquent
    analysisManager->FinishNtuple();

    // Ntuple des passages plan +Z (ScorePlane à z = 6 mm)
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
    // Ntuple pour le plan de comptage ScorePlane2 (z = 8 mm)
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
    // Ntuple pour le plan de comptage ScorePlane5 (z = 118 mm)
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

    // ==================== Ntuple ScorePlane6 ====================
    // Ntuple pour le plan de comptage ScorePlane6 (z = 168 mm)
    g_scorePlane6NtupleId = analysisManager->CreateNtuple("ScorePlane6_passages", 
        "Traversées +Z du plan ScorePlane6");
    analysisManager->CreateNtupleIColumn(g_scorePlane6NtupleId, "pdg");             // 0: Code PDG
    analysisManager->CreateNtupleSColumn(g_scorePlane6NtupleId, "name");            // 1: Nom particule
    analysisManager->CreateNtupleIColumn(g_scorePlane6NtupleId, "is_secondary");    // 2: 0=primaire, 1=secondaire
    analysisManager->CreateNtupleDColumn(g_scorePlane6NtupleId, "x_mm");            // 3: Position X (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane6NtupleId, "y_mm");            // 4: Position Y (mm)
    analysisManager->CreateNtupleDColumn(g_scorePlane6NtupleId, "ekin_keV");        // 5: Énergie cinétique (keV)
    analysisManager->CreateNtupleIColumn(g_scorePlane6NtupleId, "trackID");         // 6: TrackID
    analysisManager->CreateNtupleIColumn(g_scorePlane6NtupleId, "parentID");        // 7: ParentID
    analysisManager->CreateNtupleSColumn(g_scorePlane6NtupleId, "creator_process"); // 8: Processus créateur
    analysisManager->FinishNtuple(g_scorePlane6NtupleId);

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

    //  Raccorder l'ID au SD ScorePlane6 (si défini)
    if (auto* sd6 = dynamic_cast<ScorePlane6SD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("ScorePlane6SD", /*warning=*/false))) {
        sd6->SetNtupleId(g_scorePlane6NtupleId);
        G4cout << "[SetupAnalysis] ScorePlane6SD connecté au ntuple id=" << g_scorePlane6NtupleId << G4endl;
    }
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

int GetScorePlane6NtupleId()
{
    return g_scorePlane6NtupleId;
}
