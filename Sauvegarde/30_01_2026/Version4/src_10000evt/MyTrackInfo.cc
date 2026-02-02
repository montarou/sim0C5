#include "MyTrackInfo.hh"

MyTrackInfo::MyTrackInfo()
: G4VUserTrackInformation(), enteredCube(false), enteredSphere(false), creatorProcess("unknown")
{}

MyTrackInfo::~MyTrackInfo() {}

