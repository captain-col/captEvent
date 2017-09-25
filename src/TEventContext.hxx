#ifndef TEventContext_hxx_seen
#define TEventContext_hxx_seen

#include <TROOT.h>
#include <TObject.h>

#include <ctime> 

namespace CP {
    class TEventContext;
    std::ostream& operator<<(std::ostream& s, const CP::TEventContext& c);
}

/// Define the context of an event.  The event context uniquely identifies an
/// event.  Due to the way events are formed, not all event (or slow control
/// data) will have all fields of the context information defined.  Fields
/// that do not contain valid data should be marked using the value
/// CP::TEventContext::Invalid.
class CP::TEventContext {
public:
    typedef ULong_t Time;
    
    /// A special field value that specifies that the field is invalid.
    static const UInt_t Invalid;
    
    /// Construct an empty context.
    TEventContext();

    /// Construct a context.
    TEventContext(int partition, 
                  int run, int subRun, int event, 
                  int spill, int stamp);

    virtual ~TEventContext();

    /// Define the partition bits that specify what type of data this is for.
    /// The values of these bit definitions need to be fixed since they are
    /// part of the event format.
    enum {
        /// This context is associated with the CAPTAIN TPC
        kCAPTAIN          = 1<<0,
        /// This context is associated with the miniCAPTAIN TPC
        kmCAPTAIN          = 1<<1,
        /// If this is detector data, then this bit will be 0.  If this is MC
        /// data, then this bit will be 1.
        kMCData           = 1<<16,
    };

    /// Return the partition for this context.  The partition code provides a
    /// quick way to identify the general contents of the event.  For
    /// instance, the partitions could be defined as the TPC, the light
    /// detection system, and the cosmic ray telescope.  The partition is set
    /// so that all components can be combined into a single event, or some
    /// can be removed without changing the overall event format.  This
    /// provides a simple way to tell which part of the hardware were used in
    /// a particular event run.
    ///
    /// \note The partition value must first be checked to make sure it is
    /// valid before the individual bits are checked.
    ///
    /// \code
    /// if (context.GetPartition() != CP::TEventContext::Invalid
    ///     && (context.GetPartition & CP::TEventContext::kGlobalPartition)) {
    ///     // The data is for the global partition.
    /// }
    /// \endcode
    UInt_t GetPartition() const;

    /// Add a convenient check to see if this is MC data.  Note that if the
    /// context partition is invalid, this will return false.
    bool IsMC() const;
    
    /// Add a convenient check to see if this is detector data.  Note that if
    /// the context partition is invalid this will return false.
    bool IsDetector() const;

    /// True if the event is associated with the CAPTAIN tpc.  This is usually
    /// not set for MC events simulated in CAPTAIN.
    bool IsCAPTAIN() const;

    /// True if the event is associated with the miniCAPTAIN tpc This is
    /// usually not set for MC events simulated for miniCAPTAIN.
    bool IsMiniCAPTAIN() const;
    
    /// Set the partition for this context.
    void SetPartition(UInt_t p);

    /// Return the run number for this context.
    UInt_t GetRun() const;

    /// Set the run number for this context.
    void SetRun(int r);

    /// Return the sub-run number for this context.
    UInt_t GetSubRun() const;

    /// Set the sub-run number for this context.
    void SetSubRun(int s);

    /// Return the event number for this context.
    UInt_t GetEvent() const;

    /// Set the event number for this context.
    void SetEvent(int e);

    /// Return the spill number for this context.  This is a synonym for
    /// GetNanoseconds() that should be used when events are correlated
    /// between systems using the accelerator spill number.
    UInt_t GetSpill() const;

    /// Set the spill number for this context.  This is a synonym for
    /// SetNanoseconds() that should be used when events are correlated
    /// between systems using the accelerator spill number.
    void SetSpill(int s);
    
    /// Return the offset in nanoseconds from the last one second tick.  This
    /// is a synonym for GetSpill() that should be used when events are
    /// correlated by GPS time instead of spill number.
    UInt_t GetNanoseconds() const {return GetSpill();}

    /// Set the offset in nanoseconds from the last one second tick.  This is
    /// a synonym for SetSpill() that should be used when events are
    /// correlated by the GPS time instead of spill number.
    void SetNanoseconds(int s) {SetSpill(s);}
    
    /// Set the time stamp for this event.  The time stamp should be the
    /// number of seconds since 00:00:00 Jan 1, 1970 UTC so that it can be
    /// converted into a time_t.  This is mostly intended to find the
    /// appropriate calibration data, and events requiring precise timing
    /// should provide the actual GPS clock data.
    void SetTimeStamp(int t);

    /// Return the time stamp for this event.
    Time GetTimeStamp() const;

    /// Return the time stamp for this event as an SQL compatible time string.
    /// Since the time stamps is mostly intended to look up calibration data,
    /// this provides a simple way to convert the time stamp in to a SQL time
    /// for a database lookup.
    std::string GetTimeStampString() const;

    /// Return true if the context is valid.  A valid context is defined as a
    /// context where one or more fields is valid.
    bool IsValid() const;

    /// Check of contexts are equal
    virtual bool operator== (const TEventContext& rhs) const;
    virtual bool operator!= (const TEventContext& rhs) const;

private:
    /// The partition associated with this context
    UInt_t fPartition;

    /// The run number associated with this context
    UInt_t fRun;

    /// The sub-run number associated with this context
    UInt_t fSubRun;

    /// The event number associated with this context.
    UInt_t fEvent;

    /// The time or "spill" information associated with this context.  This is
    /// used to correlate the information in this event with events taken in
    /// other sub-systems.  Events are generally correlated using the event
    /// time, but the context time has a 1 second precision, so it can't
    /// correlate events at a high trigger rate.  This field contains the
    /// extra information needed to correlate the events.  In T2K, it
    /// syncronizes the beam, mumon, ND280 and SK data using the spill number
    /// (with a 2^16 wrap around.  In CAPTAIN, this contains the offset in ns
    /// from the last second tick (e.g. this is how the CAPTAIN mutel and tpc
    /// are syncronized).  This can be accessed by <Set/Get>Spill() or
    /// <Set/Get>Nanoseconds() which are synonyms.  Both names are provided to
    /// let user code reference this according to its actual function.  See
    /// the comments on fTimeStamp if this is representing a GPS time.
    UInt_t fSpill;

    /// The time stamp (1 sec tick) associated with this context.  While the
    /// time stamp is not intended for precision timing of the event, but will
    /// ideally come from a GPS clock.  For applications that require
    /// precision timing, the raw GPS Clock information should be placed into
    /// the event so that the lower level information can be accessed.  This
    /// field is mostly intended to locate the correct set of calibration data
    /// (e.g. accuracy of "minutes or hours") and correlate events between
    /// sub-systems.
    Time fTimeStamp;

    ClassDef(TEventContext,2);
};
#endif
