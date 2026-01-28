#include "MyTrackInfo.hh"
#include "G4EventManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Track.hh"
#include "G4TrajectoryContainer.hh"


#include "EventAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"

#include "SphereHit.hh"
#include "RunAction.hh"


//******************************************************************************************
//  Ce code correspond à la classe EventAction, qui permet de gérer les actions spécifiques
//        à chaque événement dans Geant4, notamment :
//  Initialiser des variables au début de chaque événement
//  Collecter les résultats (hits, compteurs) à la fin de chaque événement
//  Enregistrer des données dans des ntuples ou les transmettre au RunAction
//******************************************************************************************

//  Constructeur et destructeur par défaut.
//
//  Pas de traitement spécifique ici,
//  mais l'objet EventAction est prêt à être utilisé dans ActionInitialization.
//

EventAction::EventAction(){}

EventAction::~EventAction(){}

//  Réinitialise les variables locales à chaque début d’événement.
//  Cela prépare la collecte d’informations pendant le reste de l’événement, via SteppingAction.
void EventAction::BeginOfEventAction(const G4Event*  event)
{
    // Réinitialisation pour chaque événement
    enteredCube = false;
    enteredSphere = false;
    creatorProcess = "unknown";

    fNbEntrantInBe = 0;
    fNbInteractedInBe = 0;
    fNbEntrantInWaterSphere = 0;
    fNbInteractedInWaterSphere = 0;

    // 🔍 Récupérer la particule primaire
    G4PrimaryVertex* primaryVertex = event->GetPrimaryVertex();
    if (primaryVertex) {
        G4PrimaryParticle* primary = primaryVertex->GetPrimary();
        if (primary) {
            G4ParticleDefinition* particleDef = primary->GetG4code();
            G4String name = (particleDef ? particleDef->GetParticleName() : "unknown");

            G4ThreeVector mom = primary->GetMomentumDirection();
            G4double energy = primary->GetTotalEnergy();

            if (fEventVerboseLevel == 1) {
            G4cout << "[DEBUG BeginOfEventAction] Particule primaire = " << name << G4endl;
            G4cout << "[DEBUG BeginOfEventAction] Direction         = " << mom << G4endl;
            G4cout << "[DEBUG BeginOfEventAction] Énergie totale    = " << energy / keV << " keV" << G4endl;
            }
        } else {
            if (fEventVerboseLevel == 1) {
            G4cout << "[DEBUG BeginOfEventAction] Pas de particule primaire." << G4endl;
            }
        }
    } else {
        if (fEventVerboseLevel == 1) {
        G4cout << "[DEBUG BeginOfEventAction] Pas de vertex primaire." << G4endl;
        }
    }
}
//  Appelée à la fin de l’événement, pour :
//        - Enregistrer les résultats (ntuples, hits)
//        - Transmettre les données à RunAction

void EventAction::EndOfEventAction(const G4Event* event)
{
    if (fEventVerboseLevel == 1) {
        G4cout << "[DEBUG EndOfEventAction] EndOfEventAction appelé pour EventID = "<<event->GetEventID()<<G4endl;
        G4cout << "[DEBUG EndOfEventAction] NbEntrantInBe = "<<fNbEntrantInBe<<G4endl;
        G4cout << "[DEBUG EndOfEventAction] NbInteractedInBe = "<<fNbInteractedInBe<<G4endl;}

    // Récupération des données générales :
    auto analysisManager = G4AnalysisManager::Instance();
    G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

    // Remplissage du ntuple "trackInfo" (ID = 1)
    //  Pour chaque SphereHit :
    //  On enregistre sa position et son énergie déposée dans l’ntuple[0]
    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, enteredCube ? 1 : 0);
    analysisManager->FillNtupleIColumn(1, 2, enteredSphere ? 1 : 0);
    analysisManager->FillNtupleSColumn(1, 3, creatorProcess);
    analysisManager->FillNtupleIColumn(1, 4, fNbInteractedInBe);
    analysisManager->AddNtupleRow(1);

    if (fRunAction) {
    fRunAction->UpdateFromEvent(this);
    }

    auto runAction = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
    if (runAction) {
        if (fEventVerboseLevel == 1) {
            G4cout << "\n[DEBUG EndOfEventAction] [EndOfEventAction DEBUG] Compteurs globaux (fin event #" << G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() << ") :" << G4endl;
            G4cout<<" [DEBUG EndOfEventAction ↪ fTotalEntrantInBe          = "<<runAction->GetTotalEntrantInBe()<<G4endl;
            G4cout<<" [DEBUG EndOfEventAction ↪ fTotalInteractedInBe       = "<<runAction->GetTotalInteractedInBe()<<G4endl;
            G4cout<<" [DEBUG EndOfEventAction ↪ fTotalEntrantInWaterSphere = "<<runAction->GetTotalEntrantInWaterSphere()<<G4endl;
            G4cout<<" [DEBUG EndOfEventAction ↪ fTotalInteractedInWaterSphere = "<<runAction->GetTotalInteractedInWaterSphere()<< G4endl;}
    }

    // Accès à la collection de hits de la sphère
    //
    //  =>  Récupère le conteneur de toutes les HitsCollections enregistrées pendant cet événement.
    //  =>  G4HCofThisEvent = Hit Collection of This Event
    G4HCofThisEvent* hce = event->GetHCofThisEvent();

    //  Si ce conteneur est vide ou nul → il n’y a aucune collection attachée à cet événement.
    //  on quitte immédiatement la fonction si aucune collection de hits n’est disponible.
    if (!hce)
    {

        analysisManager->FillNtupleIColumn(2, 0, eventID);
        analysisManager->FillNtupleIColumn(2, 1, nH);
        analysisManager->FillNtupleDColumn(2, 2, to);
        analysisManager->FillNtupleDColumn(2, 3, me);
        analysisManager->FillNtupleDColumn(2, 4, ma);
        analysisManager->FillNtupleSColumn(2, 5, mo);
        analysisManager->AddNtupleRow(2);

        analysisManager->FillH1(6, nH);

        return;
    }

    //  Variable locale statique pour mémoriser l’ID unique de la collection de hits.
    //  Cela évite de redemander à Geant4 à chaque événement.
    static G4int hcID = -1;

    //Récupère une seule fois (au premier événement) l’ID numérique de la collection "SphereSD/SphereHitsCollection"
    //    - "SphereSD" est le nom du détecteur sensible,
    //    - "SphereHitsCollection" est le nom de la collection déclarée dans ton SphereSurfaceSD
    //    - Cette opération n’est faite qu’une seule fois grâce à static.
    if (hcID < 0) {
        hcID = G4SDManager::GetSDMpointer()->GetCollectionID("SphereSD/SphereHitsCollection");
    }

    //  Récupère la collection de hits propre à la sphère d’eau pour cet événement
    //  =>  fait un cast explicite vers le bon type : G4THitsCollection<SphereHit>
    //  =>  C’est maintenant un conteneur d’objets SphereHit* pour cet événement.
    auto hitsCollection = static_cast<G4THitsCollection<SphereHit>*>(hce->GetHC(hcID));

    //  Vérifie si la collection est bien présente.
    //  Si elle ne l’est pas (pas de hits, SD non activé, erreur de nom…),
    //      => on quitte.
    if (!hitsCollection)
    {

        analysisManager->FillNtupleIColumn(2, 0, eventID);
        analysisManager->FillNtupleIColumn(2, 1, nH);
        analysisManager->FillNtupleDColumn(2, 2, to);
        analysisManager->FillNtupleDColumn(2, 3, me);
        analysisManager->FillNtupleDColumn(2, 4, ma);
        analysisManager->FillNtupleSColumn(2, 5, mo);
        analysisManager->AddNtupleRow(2);

        analysisManager->FillH1(6, nH);

        return;
    }


    //  Récupère le nombre de hits enregistrés dans la collection pour cet événement.
    G4int nHits = hitsCollection->entries();
    analysisManager->FillH1(6, nHits);
    if (fEventVerboseLevel == 1) {
        G4cout<<"[DEBUG EndOfEventAction] nHits ="<<nHits<<G4endl;}

    //  Si aucun hit n’a été enregistré dans la sphère
    //  => inutile de continuer à analyser → on quitte.
    if (nHits == 0)
    {
        analysisManager->FillNtupleIColumn(2, 0, eventID);
        analysisManager->FillNtupleIColumn(2, 1, nHits);
        analysisManager->FillNtupleDColumn(2, 2, to);
        analysisManager->FillNtupleDColumn(2, 3, me);
        analysisManager->FillNtupleDColumn(2, 4, ma);
        analysisManager->FillNtupleSColumn(2, 5, mo);
        analysisManager->AddNtupleRow(2);

        analysisManager->FillH1(6, nHits);

        return;
    }
    // Copie locale des hits pour stockage global
    //
    //  Ce bloc de code est situé dans EventAction::EndOfEventAction(...). Il sert à :
    //   -  Copier localement tous les hits détectés dans la sphère d’eau (SphereHit)
    //   -  Les enregistrer dans un ntuple pour analyse individuelle
    //   -  Transmettre la collection au RunAction pour un traitement ou un résumé global en fin de run
    //
    //  std::vector<SphereHit> eventHits; déclare un vecteur local pour stocker tous les hits de cet événement.
    //
    //  Le type SphereHit est la classe personnalisée qui contient les données d’un hit (position, énergie, particule...).
    std::vector<SphereHit> eventHits;

    //  Boucle sur les hits de la collection
    for (G4int i = 0; i < nHits; ++i) {

        //  Pour chaque hit dans la collection hitsCollection (récupérée via GetHC(...))
        //  On récupère le pointeur vers le hit (auto hit = (*hitsCollection)[i];)
        auto hit = (*hitsCollection)[i];
        //   Puis on copie son contenu dans eventHits avec :
        eventHits.push_back(*hit);
        //  *hit fait une copie complète de l'objet, et non pas un simple pointeur.
        //  On construist donc une liste de hits propre à l’événement, que tu pourras archiver ou analyser globalement.

        // Traitement ntuple, etc.
        G4ThreeVector pos = hit->GetPosition();
        G4double eDep = hit->GetEnergy();
        analysisManager->FillNtupleIColumn(0,0, eventID);
        analysisManager->FillNtupleDColumn(0,1, pos.x() / mm);
        analysisManager->FillNtupleDColumn(0,2, pos.y() / mm);
        analysisManager->FillNtupleDColumn(0,3, pos.z() / mm);
        analysisManager->FillNtupleDColumn(0,4, eDep / keV);
        analysisManager->FillNtupleIColumn(0,5, hit->GetPDG());
        analysisManager->FillNtupleSColumn(0,6, hit->GetParticleName());
        analysisManager->FillNtupleSColumn(0,7, hit->GetProcessName());
        analysisManager->FillNtupleIColumn(0,8, hit->GetProcessType());
        analysisManager->FillNtupleIColumn(0,9, hit->GetProcessSubType());
        analysisManager->AddNtupleRow(0);
    }

    // Remplissage du ntuple 2 (SphereStats)
    G4double totalEdep = 0;
    G4double maxEdep = 0;
    std::map<G4String, int> particleCount;

    for (const auto& hit : eventHits) {
        G4double edep = hit.GetEdep();
        totalEdep += edep;
        if (edep > maxEdep) maxEdep = edep;
        particleCount[hit.GetParticleName()]++;
    }

    G4double meanEdep = totalEdep / nHits;
    G4String mostCommonParticle = "unknown";
    int maxCount = 0;
    for (const auto& kv : particleCount) {
        if (kv.second > maxCount) {
            maxCount = kv.second;
            mostCommonParticle = kv.first;
        }
    }

    analysisManager->FillNtupleIColumn(2, 0, eventID);
    analysisManager->FillNtupleIColumn(2, 1, nHits);
    analysisManager->FillNtupleDColumn(2, 2, totalEdep / keV);
    analysisManager->FillNtupleDColumn(2, 3, meanEdep / keV);
    analysisManager->FillNtupleDColumn(2, 4, maxEdep / keV);
    analysisManager->FillNtupleSColumn(2, 5, mostCommonParticle);
    analysisManager->AddNtupleRow(2);

    // Transfert vers RunAction pour un bilan final
    //Si fRunAction est défini, on lui transmet tous les hits de l’événement courant.
    //Cela permet à RunAction :
    //    - De faire un bilan global en fin de run
    //    - D’archiver les hits par événement (std::map<G4int, std::vector<SphereHit>>)
    //  transmet tous les SphereHit à RunAction, où ils seront utilisés en fin de simulation (ex : résumé par événement).
    if (fRunAction) {
        fRunAction->AddHitsForEvent(event->GetEventID(), eventHits);
    }
}

//  Méthode est appelée par SteppingAction pour transmettre à EventAction
//  les informations spécifiques au track primaire, stockées dans l’objet MyTrackInfo.
//
//  Ces infos serviront ensuite à :
//    - Remplir un ntuple (dans EndOfEventAction)
//    - Compter ou filtrer les événements par type de trajectoire ou origine
//
//  Méthode appelée par SteppingAction, utilisée pour :
//    - Récupérer les infos du track primaire
//    - Les copier localement dans EventAction pour les stocker dans un ntuple
//
//  Ce qu’elle enregistre :
//    - enteredCube = info->HasEnteredCube();
//    - enteredSphere = info->HasEnteredSphere();
//    - creatorProcess = (p.empty() ? "unknown" : p);

void EventAction::SetTrackInfo(MyTrackInfo* info)
{
    //  Vérifie que le pointeur passé est valide.
    if (!info) return; // on quitte la fonction pour éviter un crash.

    //Copie le booléen HasEnteredCube() depuis MyTrackInfo.
    //Il indique si le track primaire est entré dans le volume logicWaterCube pendant cet événement.
    enteredCube = info->HasEnteredCube();

    // copie si le track primaire est entré dans la sphère d’eau (logicsphereWater).
    enteredSphere = info->HasEnteredSphere();

    // appelée dans SteppingAction au tout premier step du track primaire :
    G4String p = info->GetCreatorProcess();
    creatorProcess = (p.empty() ? "unknown" : p);

    if (fEventVerboseLevel == 1) {
        G4cout<<"[DEBUG SetTrackInfo] ✅ Infos copiées : process="<<creatorProcess<<", cube="<<enteredCube<<", sphère="<<enteredSphere<<G4endl;}
}

