#ifndef TClusterState_hxx_seen
#define TClusterState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace CP {
    class TClusterState;
}

/// A state holding the parameters associated with a TReconCluster.  
class CP::TClusterState:
    public TReconState
{
public:
    TClusterState();
    virtual ~TClusterState();
    TClusterState(const TClusterState& init);
    virtual TClusterState& operator=(const TClusterState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION;

    ENERGY_DEPOSIT_STATE_PRIVATE;
    POSITION_STATE_PRIVATE;
    
    ClassDef(TClusterState,3);
};
#endif
