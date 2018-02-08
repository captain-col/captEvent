#ifndef TPIDState_hxx_seen
#define TPIDState_hxx_seen

#include "TReconState.hxx"
#include "TTrackState.hxx"
#include "TShowerState.hxx"
#include "TReconNode.hxx"

namespace CP {
    class TPIDState;
}
    
/// A state holding parameters associated with a TReconPID.  
class CP::TPIDState:
    public TReconState
{
public:
    TPIDState();
    TPIDState(const CP::TPIDState& init);
    TPIDState(const CP::TTrackState& tstate);
    TPIDState(const CP::TShowerState& tstate);

    virtual ~TPIDState();
    virtual TPIDState& operator=(const CP::TPIDState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION;
    DIRECTION_STATE_DECLARATION;
    MOMENTUM_STATE_DECLARATION;
    CHARGE_STATE_DECLARATION;

    ENERGY_DEPOSIT_STATE_PRIVATE;
    POSITION_STATE_PRIVATE;
    DIRECTION_STATE_PRIVATE;
    MOMENTUM_STATE_PRIVATE;
    CHARGE_STATE_PRIVATE;

    ClassDef(TPIDState,2);
};
#endif
