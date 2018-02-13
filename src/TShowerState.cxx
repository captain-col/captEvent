#include "TShowerState.hxx"

///////////////////////////////////////////////////////
ClassImp(CP::TShowerState);

CP::TShowerState::TShowerState() {
    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    CONE_STATE_DEFINITION;
    Init();
}

CP::TShowerState::~TShowerState() {}

CP::TShowerState::TShowerState(const CP::TShowerState& init) {
    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    CONE_STATE_DEFINITION;
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

CP::TShowerState& CP::TShowerState::operator=(const CP::TShowerState& rhs) {
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
