#ifndef THitSelection_hxx_seen
#define THitSelection_hxx_seen

#include <iostream>
#include <vector>

#include <TObject.h>
#include <TVector3.h>

#include "ECore.hxx"
#include "TDatum.hxx"
#include "THit.hxx"

namespace CP {
    class THitSelection;
    EXCEPTION(EHitSelection,ECore);

    /// An invalid hit was added to the hit selection.  This exception can be
    /// caught and continue, with the THitSelection object intact.
    EXCEPTION(EInvalidHit, EHitSelection);
}

/// A container of THitHandle objects for the hit detector information.  This
/// is an enhanced vector that works well with ROOT.
class CP::THitSelection : public TDatum, public std::vector< THandle<THit> > {
public:
    THitSelection(const char* name="hits", 
                  const char* title="Hit Handles");
    virtual ~THitSelection();

    /// This is the usual std::vector::push_back, but enhanced to make
    /// sure that only valid hits are inserted into the THitSelection.
    virtual void push_back(const CP::THandle<CP::THit>& hit);

    /// A convenience method to make sure that a hit is only added to the
    /// THitSelection once.  The AddHit method is much slower than a
    /// push_back(), so it should only be used when the hit might already be
    /// in the selection.
    virtual void AddHit(const CP::THandle<CP::THit>&);

    /// A convenience method to make sure that a hit is not in a
    /// THitSelection.
    virtual void RemoveHit(const CP::THandle<CP::THit>&);

    /// Print the data vector information.
    virtual void ls(Option_t* opt = "") const;

private:
    ClassDef(THitSelection,2);
};
#endif
