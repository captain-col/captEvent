#ifndef TReconState_hxx_seen
#define TReconState_hxx_seen
////////////////////////////////////////////////////////////////
// 
// A class to hold the state information for the reconstruction objects.  This
// file defines several classes and needs to be split into multiple files
// after the design is complete.  
#include <vector>
#include <string>

#include <TObject.h>
#include <TLorentzVector.h>
#include <TVector3.h>

#include "TCorrValues.hxx"
#include "THandle.hxx"
#include "TCaptLog.hxx"

namespace CP {
    class TReconState;
    class TMReconState;
    class TMEDepositState;
    class TMPositionState;
    class TMDirectionState;
    class TMPositionDirectionState;
    class TMConeState;
    class TMMassState;
    class TMPosDirMassState;
    class TMWidthState;
    class TMMomentumState;
    class TMChargeState;

    class TClusterState;
    class TShowerState;
    class TTrackState;
    class TPIDState;
    class TVertexState;
}

/// The TReconState class represents the value of parameters and covariances
/// associated with reconstruction objects.  It does not contain general
/// status information such as the goodness, degree's of freedom, or objects
/// which contributed to the reconstruction.  The values saved in the particle
/// states are described in \ref recObjGeometric and \ref recObjPhysics. 
///
/// This is the polymorphic base class that all other state classes are
/// derived from, and it provides minimal operations.  The main purpose of
/// this class is to allow polymorphic vectors of states.  However, it
/// provides some minimal access to the contained data.
class CP::TReconState: public TObject {
public:
    TReconState();
    TReconState(const CP::TReconState& state);
    virtual ~TReconState();

    /// Return a string with all of the state field names.  This name is used
    /// to build a type has for the state.
    std::string GetStateFields() const;

    /// Return the number of dimensions in the state (the size of the state).
    int GetDimensions() const;

    /// Not the preferred interface, but get the value by index.  The best way
    /// to get a value is through the Get method provided by the mix-in
    /// classes (e.g. CP::TMEDepositState::GetEDeposit()).
    double GetValue(int i) const;
    
    /// Not the preferred interface, but set the value by index.    The best way
    /// to get a value is through the Set method provided by the mix-in
    /// classes (e.g. CP::TMEDepositState::SetEDeposit()).
    void SetValue(int i, double value);

    /// Not the preferred interface: get the covariance by index.
    double GetCovarianceValue(int i, int j) const;

    /// Not the preferred interface: set the covariance by index.
    void SetCovarianceValue(int i, int j, double value);

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

    /// Set a parameter to be free (unconstrained).
    void SetFree(int i);

    /// Check if a parameter is free.
    bool IsFree(int i) const;

    /// Check if a variance corresponds to a free parameter.
    bool IsFree(double v) const;

    /// Set a parameter to be fixed.
    void SetFixed(int i);

    /// Check if a parameter is fixed.
    bool IsFixed(int i) const;

    /// Check if a variance corresponds to a fixed parameter.
    bool IsFixed(double v) const;

    /// Validate the covariance.
    void Validate();

    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;
    virtual void Print(Option_t *opt = "") const {ls(opt);}

protected:
    friend class TMReconState;
    friend class TMEDepositState;
    friend class TMPositionState;
    friend class TMDirectionState;
    friend class TMPositionDirectionState;
    friend class TMConeState;
    friend class TMMassState;
    friend class TMPosDirMassState;
    friend class TMWidthState;
    friend class TMMomentumState;
    friend class TMChargeState;

    friend class TClusterState;
    friend class TShowerState;
    friend class TTrackState;
    friend class TPIDState;
    friend class TVertexState;

    /// A final initialization routine that is called in the constructor of
    /// the instantiated class.  This builds the actual state vector.
    void Init();

    /// The vector of correlated values (a vector and a covariance) that holds
    /// the state information.
    TCorrValues fValues;

    /// A vector of parameter names.  This identifies the fields in the state.
    std::vector<std::string> fFieldNames;

    ClassDef(TReconState,1);
};

/// A mixable base class
class CP::TMReconState {
public:
    TMReconState();
    TMReconState(TReconState* state);

    virtual ~TMReconState();

    TReconState& GetThis() const;

private:
    TReconState* fThis; //! do not save

protected:
    std::vector<std::string> fLocalNames; //! do not save

    ClassDef(TMReconState,0);
};

/// A mixable base class that adds the energy deposit property to a state.
/// The position state is defined as a scaler and the
/// associated uncertainty.  
///
/// \bug The units of the energy deposit depend on the context.  After
/// particle ID, the deposit can be in uncalibrated or calibrated energy.  In
/// the TPC the deposit might be in terms of fempto-coulombs, but for a PMT
/// the deposit might be based on photo-electrons.
class CP::TMEDepositState: virtual public TMReconState {
public:
    TMEDepositState();

    virtual ~TMEDepositState();

    /// Get the value of the EDeposit.
    double GetEDeposit() const;

    /// Set the value of the EDeposit.
    void SetEDeposit(double enr);

    /// Get the variance of the EDeposit.
    double GetEDepositVariance(void) const;

    /// Set the variance of the EDeposit.
    void SetEDepositVariance(double var);

    /// Get the index of the EDeposit field in the TCorrValues vector.
    int GetEDepositIndex() const {return fEDepositIndex;}

    /// Return the number of entries for the EDeposit in the TCorrValues
    /// vector.
    static int GetSize() {return 1;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the EDeposit field in the TCorrValues object.
    int fEDepositIndex; //! Do not save.

    ClassDef(TMEDepositState,0);
};

/// A mixable base class that adds the position property to a state.  The
/// position state is defined as a four vector (X,Y,Z,T) and the associated
/// uncertainties. 
class CP::TMPositionState: virtual public TMReconState {
public:
    TMPositionState();

    virtual ~TMPositionState();

    /// Get the value of the Position.
    TLorentzVector GetPosition() const;

    /// Set the value of the Position.
    void SetPosition(double x, double y, double z, double t);

    /// Set the value of the Position.
    void SetPosition(const TLorentzVector& pos);

    /// Get the variance of the Position.
    TLorentzVector GetPositionVariance(void) const;

    /// Set the variance of the Position.
    void SetPositionVariance(double x, double y, double z, double t);

    /// Set the covariance of the position variables.  The indices run from 0
    /// to 3 (0: X, 1: Y, 2: Z, 3: T).
    void SetPositionCovariance(int i, int j, double v);

    /// Get the covariance of the position variables.  The indices run from 0
    /// to 3 (0: X, 1: Y, 2: Z, 3: T).
    double GetPositionCovariance(int i, int j);

    /// Get the index of the Position field in the TCorrValues vector.
    int GetPositionIndex() const {return fPositionIndex;}

    /// Get the index of the X,Y,Z,T fields in the TCorrValues vector.
    int GetXIndex() const {return fPositionIndex;}
    int GetYIndex() const {return fPositionIndex+1;}
    int GetZIndex() const {return fPositionIndex+2;}
    int GetTIndex() const {return fPositionIndex+3;}

    /// Return the number of entries for the Position in the TCorrValues
    /// vector.
    static int GetSize() {return 4;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:

    /// The index of the Position field in the TCorrValues object.
    int fPositionIndex; //! Do not save.

    ClassDef(TMPositionState,0);
};

/// A mixable base class that adds the direction property to a state.  The
/// direction state is defined as a three vector (dX,dY,dZ) and the associated
/// uncertainties.  The corelations between the components are enforced in the
/// covariance matrix.
class CP::TMDirectionState: virtual public TMReconState {
public:
    TMDirectionState();
    virtual ~TMDirectionState();

    /// Get the value of the Direction.
    TVector3 GetDirection() const;

    /// Set the value of the Direction.
    void SetDirection(double x, double y, double z);

    /// Set the value of the Direction.
    void SetDirection(const TVector3&);

    /// Get the variance of the Direction.
    TVector3 GetDirectionVariance(void) const;

    /// Set the variance of the Direction.
    void SetDirectionVariance(double x, double y, double z);

    /// Set the covariance of the direction variables.  The indices run from 0
    /// to 2 (0: X, 1: Y, 2: Z).
    void SetDirectionCovariance(int i, int j, double v);

    /// Get the covariance of the direction variables.  The indices run from 0
    /// to 2 (0: X, 1: Y, 2: Z).
    double GetDirectionCovariance(int i, int j);

    /// Get the index of the Direction field in the TCorrValues vector.
    int GetDirectionIndex() const {return fDirectionIndex;}

    /// Return the number of entries for the Direction in the TCorrValues
    /// vector.
    static int GetSize() {return 3;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the Direction field in the TCorrValues object.
    int fDirectionIndex; //! Do not save.

    ClassDef(TMDirectionState,0);
};

/// A mixable base class that adds the position and direction properties to a
/// state using the TPositionState and TDirectionState classes.
class CP::TMPositionDirectionState: virtual public TMPositionState, 
                                    virtual public TMDirectionState {
public:
    TMPositionDirectionState() {};
    virtual ~TMPositionDirectionState();

    /// Get the index of the Direction field in the TCorrValues vector.
    int GetPositionDirectionIndex() const {return GetPositionIndex();}

    /// Return the number of entries for the Direction in the TCorrValues
    /// vector.
    static int GetSize() {
        return TMPositionState::GetSize() + TMDirectionState::GetSize();
    }

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

    ClassDef(TMPositionDirectionState,0);
};

/// A mixable base class that adds the width of a TReconShower object.  The
/// cone value depends on the type of the shower fit.  For a EM fit, the cone
/// value will represent the local width of the shower.
class CP::TMConeState: virtual public TMReconState {
public:
    TMConeState();
    virtual ~TMConeState();

    /// Get the value of the Cone.
    double GetCone() const;

    /// Set the value of the Cone.
    void SetCone(double c1);

    /// Get the variance of the Cone.
    double GetConeVariance(void) const;

    /// Set the variance of the Cone.
    void SetConeVariance(double c1);

    /// Get the index of the Cone field in the TCorrValues vector.
    int GetConeIndex() const {return fConeIndex;}

    /// Return the number of entries for the Cone in the TCorrValues
    /// vector.
    static int GetSize() {return 1;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the Cone field in the TCorrValues object.
    int fConeIndex; //! Do not save.

    ClassDef(TMConeState,0);
};

/// A mixable base class that adds the mass to a state.  The mass state also
/// holds the associated uncertainties.
class CP::TMMassState: virtual public TMReconState {
public:
    TMMassState();

    virtual ~TMMassState();

    /// Get the value of the Mass.
    double GetMass() const;

    /// Set the value of the Mass.
    void SetMass(double enr);

    /// Get the variance of the Mass.
    double GetMassVariance(void) const;

    /// Set the variance of the Mass.
    void SetMassVariance(double var);

    /// Get the index of the Mass field in the TCorrValues vector.
    int GetMassIndex() const {return fMassIndex;}

    /// Return the number of entries for the Mass in the TCorrValues
    /// vector.
    static int GetSize() {return 1;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the Mass field in the TCorrValues object.
    int fMassIndex; //! Do not save.

    ClassDef(TMMassState,0);
};

/// A mixable base class that adds the position, direction and mass
/// properties to a state using the TMPositionDirectionState and
/// TMMassState classes.
class CP::TMPosDirMassState: virtual public TMPositionDirectionState,
                             virtual public TMMassState {
public:
    TMPosDirMassState() {}
    virtual ~TMPosDirMassState();

    int GetPosDirMassIndex() const {return GetPositionDirectionIndex();}

    /// Return the number of entries for the Direction in the TCorrValues
    /// vector.
    static int GetSize() {
        return TMPositionDirectionState::GetSize()
            + TMMassState::GetSize();
    }

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

    ClassDef(TMPosDirMassState,0);
};

/// A mixable base class that adds the width of a curvilinear energy deposit
/// property to a state.  The width is the extent of an energy deposition
/// perpendicular to local direction and the associated uncertainties.
class CP::TMWidthState: virtual public TMReconState {
public:
    TMWidthState();

    virtual ~TMWidthState();

    /// Get the value of the Width.
    double GetWidth() const;

    /// Set the value of the Width.
    void SetWidth(double w1);

    /// Get the variance of the Width.
    double GetWidthVariance(void) const;

    /// Set the variance of the Width.
    void SetWidthVariance(double w1);

    /// Get the index of the Width field in the TCorrValues vector.
    int GetWidthIndex() const {return fWidthIndex;}

    /// Return the number of entries for the Width in the TCorrValues
    /// vector.
    static int GetSize() {return 1;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the Width field in the TCorrValues object.
    int fWidthIndex; //! Do not save.

    ClassDef(TMWidthState,0);
};

/// A mixable base class that adds a property for the magnitude of the
/// momentum to a state.  
class CP::TMMomentumState: virtual public TMReconState {
public:
    TMMomentumState();

    virtual ~TMMomentumState();

    /// Get the value of the Momentum.
    double GetMomentum() const;

    /// Set the value of the Momentum.
    void SetMomentum(double enr);

    /// Get the variance of the Momentum.
    double GetMomentumVariance(void) const;

    /// Set the variance of the Momentum.
    void SetMomentumVariance(double var);

    /// Get the index of the Momentum field in the TCorrValues vector.
    int GetMomentumIndex() const {return fMomentumIndex;}

    /// Return the number of entries for the Momentum in the TCorrValues
    /// vector.
    static int GetSize() {return 1;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the Momentum field in the TCorrValues object.
    int fMomentumIndex; //! Do not save.

    ClassDef(TMMomentumState,0);
};

/// A mixable base class that adds a property for the magnitude of the
/// particle charge to a state.  
class CP::TMChargeState: virtual public TMReconState {
public:
    TMChargeState();

    virtual ~TMChargeState();

    /// Get the value of the Charge.
    double GetCharge() const;

    /// Set the value of the Charge.
    void SetCharge(double enr);

    /// Get the variance of the Charge.
    double GetChargeVariance(void) const;

    /// Set the variance of the Charge.
    void SetChargeVariance(double var);

    /// Get the index of the Charge field in the TCorrValues vector.
    int GetChargeIndex() const {return fChargeIndex;}

    /// Return the number of entries for the Charge in the TCorrValues
    /// vector.
    static int GetSize() {return 1;}

    /// The projection operator to get the full state.
    static CP::TCorrValues ProjectState(const CP::THandle<TReconState>& state);

protected:
    /// The index of the Charge field in the TCorrValues object.
    int fChargeIndex; //! Do not save.

    ClassDef(TMChargeState,0);
};
#endif
