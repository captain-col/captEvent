#ifndef TShowerState_hxx_seen
#define TShowerState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace CP {
    class TShowerState;
}

/// A state holding the parameters associated with a TReconShower. 
class CP::TShowerState:
    public TReconState
{
public:
    TShowerState();
    TShowerState(const TShowerState& init);
    virtual ~TShowerState();
    virtual TShowerState& operator=(const TShowerState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION;
    DIRECTION_STATE_DECLARATION;
    CONE_STATE_DECLARATION;
    
    ENERGY_DEPOSIT_STATE_PRIVATE;
    POSITION_STATE_PRIVATE;
    DIRECTION_STATE_PRIVATE;
    CONE_STATE_PRIVATE;
    
    ClassDef(TShowerState,3);
};
#endif
