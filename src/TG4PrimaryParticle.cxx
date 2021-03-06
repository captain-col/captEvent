////////////////////////////////////////////////////////////
// $Id: TG4PrimaryParticle.cxx,v 1.7 2008/09/04 22:34:25 mcgrew Exp $
//
#include <TClass.h>
#include <TStreamerInfo.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include "TG4PrimaryParticle.hxx"
#include "TUnitsTable.hxx"
#include "TCaptLog.hxx"

ClassImp(CP::TG4PrimaryParticle);

CP::TG4PrimaryParticle::TG4PrimaryParticle(void)
    : fTrackId(-1), fPDGCode(0), fMomentum(0,0,0,0) {}

CP::TG4PrimaryParticle::~TG4PrimaryParticle() {}

const TParticlePDG* CP::TG4PrimaryParticle::GetParticle(void) const {
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPDGCode);
    if (!particle) {
        CaptLog("TG4PrimaryParticle:: Warning -- Undefined PDG code: "
                 << fPDGCode << " [RETURNING NULL]");
    }
    return particle;
}

void CP::TG4PrimaryParticle::ls(Option_t *opt) const {
    CP::ls_header(this,opt);
    std::cout << " Id: " << fTrackId
              << " Particle: " << fPDGCode << " (" << GetTitle() << ")"
              << std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << " E: " << unit::AsString(fMomentum, "momentum")
              << std::endl;
    TROOT::DecreaseDirLevel();
}

