#include "TClusterState.hxx"
#include "TCaptLog.hxx"
///////////////////////////////////////////////////////
ClassImp(CP::TClusterState);

CP::TClusterState::TClusterState() {

    POSITION_STATE_DEFINITION;
    ENERGY_DEPOSIT_STATE_DEFINITION;

    Init();
}

CP::TClusterState::~TClusterState() {}

CP::TClusterState::TClusterState(const CP::TClusterState& init) {

    POSITION_STATE_DEFINITION;
    ENERGY_DEPOSIT_STATE_DEFINITION;

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

CP::TClusterState& CP::TClusterState::operator=(const CP::TClusterState& rhs) {
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
