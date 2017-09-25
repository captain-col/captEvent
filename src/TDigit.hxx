#ifndef TDigit_hxx_seen
#define TDigit_hxx_seen

#include <vector>
#include <TROOT.h>

#include "TDatum.hxx"
#include "TChannelId.hxx"
#include "TDigitContainer.hxx"

namespace CP {
    /// Base class for all exceptions associated with the TDigit classes.
    EXCEPTION(EDigit,ECore);

    /// An exception that should be thrown when the user requests information
    /// from the digit that is not available.
    EXCEPTION(EInvalidDigitRequest,EDigit);

    // The abstract base class for electronics data.
    class TDigit;

    // A handle to reference a TDigit object that may or may not be available.
    class TDigitProxy;
}

/// An abstract base class for raw digits.  This base class for detector
/// specific digit classes.  There are derived classes which define the
/// methods, and which actually contain the data associated with the object.
class CP::TDigit {
public:
    TDigit ();
    /// Construct a digit for a particular channel and cycle number.
    explicit TDigit(const CP::TChannelId& channel);

    virtual ~TDigit();

    /// Return the channel identifier for this digit.
    CP::TChannelId GetChannelId() const;

    /// Print the digit information.
    virtual void ls(Option_t* opt = "") const;
    
protected: 
    /// The electronics id number.
    CP::TChannelId fChannelId;

    ClassDef(TDigit,1);
};
#endif
