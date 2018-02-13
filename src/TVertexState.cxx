#include "TVertexState.hxx"

///////////////////////////////////////////////////////
ClassImp(CP::TVertexState);

CP::TVertexState::TVertexState() {

    POSITION_STATE_DEFINITION;

    Init();
}

CP::TVertexState::~TVertexState() {}

CP::TVertexState::TVertexState(const CP::TVertexState& init) {

    POSITION_STATE_DEFINITION;

    Init();

    for (int i=0; i<GetDimensions(); ++i) {
        SetValue(i,init.GetValue(i));
    }

    for (int i=0; i<GetDimensions(); ++i) {
        for (int j=0; j<GetDimensions(); ++j) {
            SetCovarianceValue(i,j,init.GetCovarianceValue(i,j));
        }
    }
    
}

CP::TVertexState& CP::TVertexState::operator=(const CP::TVertexState& rhs) {
    if (this == &rhs) return *this;

    for (int i=0; i<GetDimensions(); ++i) {
        SetValue(i,rhs.GetValue(i));
    }

    for (int i=0; i<GetDimensions(); ++i) {
        for (int j=0; j<GetDimensions(); ++j) {
            SetCovarianceValue(i,j,rhs.GetCovarianceValue(i,j));
        }
    }
    
    return *this;
}

