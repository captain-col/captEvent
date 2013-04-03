// $Id: TRootOutput.hxx,v 1.9 2011/05/06 16:05:36 mcgrew Exp $
#ifndef TRootOutput_hxx_seen
#define TRootOutput_hxx_seen

#include <TROOT.h>
#include <TFile.h>

#include <ECore.hxx>

class TTree;
class TGeoManager;

namespace CP {
    class TEvent;
    class TTPCPadManager;
    class TRootOutput;

    /// Base class for output errors.
    EXCEPTION(END280Output, ECore);

    /// An error occurred during WriteEvent.
    EXCEPTION(END280OutputWriteFailed, END280Output);
}

/// The preferred file name extension for ND280 event files.
#define ROOT_OUTPUT_PREFERRED_EXTENSION ".event"

/// Attach to a file so that the events can be written.  This can also write
/// the geometry to the output file.  This will work with any file name, but
/// the preferred file extension is [name].root (using this extension will
/// help root identify this file as a ND280 event file).
class CP::TRootOutput : public TFile {
public:
    /// Open a new output file.
    TRootOutput(const char* name,
                Option_t* opt="CREATE",
                Int_t compress = 1);

    virtual ~TRootOutput(void);

    /// Return the name of the output file.
    virtual const char* GetOutputName(void) {return GetName();};

    /// Return true if the file is attached and ready for writting.
    virtual bool IsAttached(void);

    /// Return the number of events written to the output file.
    virtual int GetEventsWritten(void);
    
    /// Write an event to the current output file. 
    virtual void WriteEvent(TEvent& event);
    
    /// Write the geometry data base to the output file.
    virtual void WriteGeometry(TGeoManager* geom);

    /// Return a flag indicating whether or not a geometry record has already
    /// been saved to the output file.
    virtual bool GeometryWritten(void);

    /// Make sure that all of the events currently in memory are flushed to
    /// the output file.
    virtual void Commit(void);
    
    /// Close the ROOT output file.  No default parameter since there
    /// must be a Close(void) function to satisfy the TRootOutput
    /// abstract class which defines a pure virtual Close(void).
    virtual void Close(Option_t* opt = "");
    
private:
    TRootOutput(const TRootOutput& aFile);
    
    TTree *fEventTree;          // The tree with events. 
    TEvent *fEventPointer; // A memory location for the event pointer.
    
    bool fAttached;             // True if the file is ready for writing.
    int fEventsWritten;         // Number of events written to file.
    TGeoManager* fGeometry;     // The geometry saved in the output file.

    ClassDef(TRootOutput,0);
};
#endif
