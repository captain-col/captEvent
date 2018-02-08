#include <cmath>

#include "TPIDState.hxx"
#include "HEPUnits.hxx"

///////////////////////////////////////////////////////
ClassImp(CP::TPIDState);

CP::TPIDState::TPIDState() {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

    Init();
}

CP::TPIDState::TPIDState(const CP::TTrackState& tstate) {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

    Init();

    // retrieve the position and it's covariance. 
    for(int i = 0;i < 4; ++i) {
        SetValue(i+GetPositionIndex(),
                 tstate.GetValue(i+tstate.GetPositionIndex()));
        for(int j = 0;j < 4; ++j) {
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(), 
                                   j+tstate.GetPositionIndex()));
        }
    }  
    
    // retrieve the direction and it's covariance.
    for(int i = 0;i < 3;++i){
        SetValue(i+GetDirectionIndex(),
                 tstate.GetValue(i+tstate.GetDirectionIndex()));
        for(int j = 0;j < 3;++j){
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetPositionIndex()));
        }
    }  
    
#ifdef USE_MAGNETIC_FIELD
    // If curvature is available, then convert to momentum.  This is bogus
    // since it depends on a magnetic field, so the momentum is initialized
    // assuming a constant 0.2T field, but is set as a free parameter.  be
    // free (by default).
    double B=0.2;
    double factor = -0.3*B;
    double p = 0;
    if (!tstate.IsFree(tstate.GetCurvatureIndex())) {
        p=std::fabs(factor*1/tstate.GetValue(tstate.GetCurvatureIndex()));
    }
    
    SetValue(GetMomentumIndex(),p);
    SetFree(GetMomentumIndex());
#endif

    // The track charge can't be determined, so set it to free with a charge
    // of zero.
    SetValue(GetChargeIndex(),0.0);
    SetFree(GetChargeIndex());
}


CP::TPIDState::TPIDState(const CP::TShowerState& tstate) {

    Init();

    // retrieve the position and it's covariance. 
    for(int i = 0;i < 4; ++i) {
        SetValue(i+GetPositionIndex(),
                 tstate.GetValue(i+tstate.GetPositionIndex()));
        for(int j = 0;j < 4; ++j) {
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(), 
                                   j+tstate.GetPositionIndex()));
        }
    }  
    
    // retrieve the direction and it's covariance.
    for(int i = 0;i < 3;++i){
        SetValue(i+GetDirectionIndex(),
                 tstate.GetValue(i+tstate.GetDirectionIndex()));
        for(int j = 0;j < 3;++j){
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetPositionIndex()));
        }
    }  
    
    // Set momentum and charge
    double p = tstate.GetEDeposit(); // Use the deposited energy (bogus)
    double q = 0;                    // Don't have any curvature.
    
    SetValue(GetMomentumIndex(),p);
    SetFree(GetMomentumIndex());

    SetValue(GetChargeIndex(),q);
    SetFree(GetChargeIndex());
}

CP::TPIDState::TPIDState(const CP::TPIDState& init) {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

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

CP::TPIDState& CP::TPIDState::operator=(const CP::TPIDState& rhs) {
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

CP::TPIDState::~TPIDState() {}


