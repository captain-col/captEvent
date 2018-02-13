#ifndef TVertexState_hxx_seen
#define TVertexState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace CP {
    class TVertexState;
}

/// A state holding parameters associated with a TReconVertex.
class CP::TVertexState: public TReconState
{
public:
    TVertexState();
    virtual ~TVertexState();
    TVertexState(const TVertexState& init);
    virtual TVertexState& operator=(const TVertexState& rhs);

    POSITION_STATE_DECLARATION;
    
    POSITION_STATE_PRIVATE;
    
    ClassDef(TVertexState,3);
};
#endif
