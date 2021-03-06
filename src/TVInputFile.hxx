#ifndef TVInputFile_hxx_seen
#define TVInputFile_hxx_seen

#include "ECore.hxx"

namespace CP {
    class TVInputFile;
    class TEvent;

    EXCEPTION(EInputFile,ECore);
};

/// An abstract base class for input files that produce TEvent objects.
/// There are currently two derived class TRootInput which is the normal way
/// to read an offline event file, and TMidasFile which is the normal way to
/// read a MIDAS .mid file and convert the raw data into TEvent objects.  
class CP::TVInputFile {
public:
    TVInputFile() {};
    virtual ~TVInputFile() {};

    /// Return the first event in the input file.  If the file does not
    /// support rewind, then this should throw an exception if it is called a
    /// second time.
    virtual CP::TEvent* FirstEvent() = 0;

    /// Get the next event from the input file.  If skip is greater than zero,
    /// then skip this many events before returning.
    virtual CP::TEvent* NextEvent(int skip=0) = 0;
    
    /// Read the previous event in the file.  If the skip is greater than
    /// zero, then skip backward this many events before returning.  If the
    /// event cannot be returned, this will return NULL.  In a ROOT file, this
    /// means that the read would be before the beginning of the file (and the
    /// new position is the beginning).  In other types of files, it may not
    /// be possible to read backwards and this will always return NULL.
    virtual TEvent* PreviousEvent(int skip = 0);

    /// Return the position of the event just read in the file.  A position of
    /// zero means that the first event was read.  A position of -1 means that
    /// no events have been read and we are not at the end of file.  The
    /// maximum position is the total number of events in the file.
    virtual int GetPosition(void) const = 0;

    /// Flag that the file is open.
    virtual bool IsOpen() = 0;

    /// Flag that the end of the file has been reached.
    virtual bool EndOfFile() = 0;

    /// Close the input file.
    virtual void CloseFile() = 0;

    /// Return the filename if it is available.  The name of the file is
    /// returned.  If the name is not available, this returns an empty string.
    virtual const char* GetFilename() const;

};

#endif
