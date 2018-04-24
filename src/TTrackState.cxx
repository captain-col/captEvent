#include "TTrackState.hxx"

///////////////////////////////////////////////////////
ClassImp(CP::TTrackState);

CP::TTrackState::TTrackState() {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MASS_STATE_DEFINITION;
    WIDTH_STATE_DEFINITION;

    Init();
}

CP::TTrackState::~TTrackState() {}

CP::TTrackState::TTrackState(const CP::TTrackState& init) {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MASS_STATE_DEFINITION;
    WIDTH_STATE_DEFINITION;

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

CP::TTrackState& CP::TTrackState::operator=(const CP::TTrackState& rhs) {
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
