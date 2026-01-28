#ifndef MYTRACKINFO_HH
#define MYTRACKINFO_HH

#include "G4VUserTrackInformation.hh"
#include "globals.hh"

class MyTrackInfo : public G4VUserTrackInformation
{
public:
    MyTrackInfo();
    virtual ~MyTrackInfo();

    inline void SetEnteredCube(G4bool val) { enteredCube = val; }
    inline G4bool HasEnteredCube() const { return enteredCube; }

    void SetEnteredSphere(G4bool val) { enteredSphere = val; }
    G4bool HasEnteredSphere() const { return enteredSphere; }

    // Processus créateur
    void SetCreatorProcess(const G4String& name) { creatorProcess = name; }
    G4String GetCreatorProcess() const {
        return creatorProcess.empty() ? "unknown" : creatorProcess;

    }

private:
    G4bool enteredCube;
    G4bool enteredSphere;
    G4String creatorProcess;

};

#endif // MYTRACKINFO_HH
