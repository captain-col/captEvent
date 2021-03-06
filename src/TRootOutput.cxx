// $Id: TRootOutput.cxx,v 1.11 2011/09/07 21:55:44 mcgrew Exp $
//
// Implement the CP::TRootOutput class which is a specialization of the
// TEventOutput class that writes the events in a root format.  This is the
// native format.
//

#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>
#include <TKey.h>

#include "TRootOutput.hxx"
#include "TEvent.hxx"
#include "TManager.hxx"
#include "TCaptLog.hxx"

ClassImp(CP::TRootOutput);

CP::TRootOutput::TRootOutput(const char *fileName,
                               Option_t* opt, 
                               int compress) 
    : TFile(fileName, opt, "ROOT Output File", compress),
      fEventTree(NULL), fEventPointer(NULL), fAttached(false), 
      fEventsWritten(0), fGeometry(NULL) {
    CaptVerbose("Open output file " << fileName);
    IsAttached();
}

CP::TRootOutput::~TRootOutput(void) {
    if (IsOpen()) Close();
}

bool CP::TRootOutput::IsAttached(void) {
    if (!IsOpen()) return false;
    if (gFile != this) {
        if (gFile) {
            CaptDebug("Changing current file from " << gFile->GetName()
                       << " to " << this->GetName() << " to write.");
        }
        cd();
    }
    if (fAttached) return true;
    CaptInfo("Attaching to " << this->GetName());
    
    //////////////
    // Hmm... Not attached, so make sure it is attached.
    // Make sure the object is attached
    fEventPointer = NULL;
    if (!fEventTree) {
        CaptTrace("Create a new tree");
        fEventTree = new TTree("captainEventTree", "Tree of CAPTAIN Events");
    }
    CaptTrace("Add the branch pointer");
    fEventTree->Branch("Event","CP::TEvent",&fEventPointer,128000,0);
    fEventPointer = NULL;       // Make sure it's empty.
    fAttached = true;

    CaptTrace("Attached");
    return fAttached;
}

int CP::TRootOutput::GetEventsWritten(void) {return fEventsWritten;}

void CP::TRootOutput::WriteEvent(CP::TEvent& event) {
    if (!IsAttached()) return;
    // Copy the pointer into the location attached to the file.
    fEventPointer = &event;
    // Put the event into the tree;
    if (fEventTree->Fill()<0) {
        CaptError("Error while writing an event");
        throw CP::ERootOutputWriteFailed();
    }
    // Empty out the fEventPointer so that it can't be written twice.
    fEventPointer = NULL;
}

// Save a geometry to the output file.
void CP::TRootOutput::WriteGeometry(TGeoManager* geom) {
    if (!IsAttached()) return;
    if (!geom) return;
    TKey *key = FindKey(geom->GetName());
    if (key) return;
    fGeometry = geom;
    if (geom->Write()<1) {
        CaptError("Error while writing geometry");
        throw CP::ERootOutputWriteFailed();
    }
    Flush();
    CaptLog("** Geometry " << geom->GetName() << " written to output file ");
}

bool CP::TRootOutput::GeometryWritten(void) {
    return fGeometry;
}
    
void CP::TRootOutput::Commit(void) {
    if (!IsAttached()) return;
    fEventTree->AutoSave();
    Flush();
}

void CP::TRootOutput::Close(Option_t* opt) {
    Write();
    if (CP::TCaptLog::LogLevel <= CP::TCaptLog::GetLogLevel()) {
        TFile::ls();
    }
    TFile::Close(opt);
}

