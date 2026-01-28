#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Types.hh"
#include "g4root_defs.hh"
#include "G4AnalysisManager.hh"
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"

#include <vector>
#include <map>
#include <fstream>

class G4Run;
class G4Event;
class RunMessenger;
class SphereHit;
class EventAction;

class RunAction : public G4UserRunAction
{
    public:
        RunAction();                          // pour BuildForMaster
        RunAction(EventAction* eventAction);  // pour Build
        virtual ~RunAction();

        virtual void BeginOfRunAction(const G4Run*) override;
        virtual void EndOfRunAction(const G4Run*) override;

        void AddToEntrantInBe(G4int n) { fTotalEntrantInBe += n; }
        void AddToInteractedInBe(G4int n) { fTotalInteractedInBe += n; }

        void AddToEntrantInWaterSphere(G4int n) { fTotalEntrantInWaterSphere += n; }
        void AddToInteractedInWaterSphere(G4int n) { fTotalInteractedInWaterSphere += n;}

        void UpdateFromEvent(const EventAction* event);

        // Ajoute les hits d’un événement
        void AddHitsForEvent(G4int eventID, const std::vector<SphereHit>& hits);

        G4int GetTotalEntrantInBe() const;
        G4int GetTotalInteractedInBe() const;
        G4int GetTotalEntrantInWaterSphere() const;
        G4int GetTotalInteractedInWaterSphere() const;

        void SetVerbose(G4int val) { fRunVerbose = val; }

        void IncrementValid1Particles() const;
        G4int GetNValid1Particles() const { return fNValidParticles_lt_35.GetValue(); }

        void IncrementValid2Particles() const;
        G4int GetNValid2Particles() const { return fNValidParticles_gt_35.GetValue(); }

        void CountPrimary() const;

    private:

        mutable G4Accumulable<G4int> fNValidParticles_lt_35;
        mutable G4Accumulable<G4int> fNValidParticles_gt_35;

        G4int fRunVerbose = 0;
        RunMessenger* fRunMessenger;

        EventAction* fEventAction = nullptr;

        G4Accumulable<G4int> fTotalEntrantInBe;
        G4Accumulable<G4int> fTotalInteractedInBe;

        G4Accumulable<G4int> fTotalEntrantInWaterSphere;
        G4Accumulable<G4int> fTotalInteractedInWaterSphere;

        //Ajoute : stockage des hits par événement
        std::map<G4int, std::vector<SphereHit>> fHitsByEvent;

        // ← mutable pour autoriser l’incrément depuis une méthode const
        mutable G4Accumulable<G4long> fPrimariesGenerated;

};
#endif
