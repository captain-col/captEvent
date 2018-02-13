#ifndef TTrackState_hxx_seen
#define TTrackState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace CP {
    class TTrackState;
}

/// A state holding parameters associated with a TReconTrack, and the
/// intermediate states.
class CP::TTrackState:
    public TReconState
{
public:
    TTrackState();
    TTrackState(const TTrackState& init);
    virtual ~TTrackState();
    virtual TTrackState& operator=(const TTrackState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION;
    DIRECTION_STATE_DECLARATION;
    MASS_STATE_DECLARATION;
    WIDTH_STATE_DECLARATION;

    ENERGY_DEPOSIT_STATE_PRIVATE;
    POSITION_STATE_PRIVATE;
    DIRECTION_STATE_PRIVATE;
    MASS_STATE_PRIVATE;
    WIDTH_STATE_PRIVATE;

    ClassDef(TTrackState,3);
};

#endif
