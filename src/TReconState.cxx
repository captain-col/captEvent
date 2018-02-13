#include <iostream>
#include <iomanip>
#include <cmath>

#include <TROOT.h>

#include "TCaptLog.hxx"
#include "TReconState.hxx"

ClassImp(CP::TReconState);

CP::TReconState::TReconState() { }

CP::TReconState::TReconState(const TReconState& state) 
    : TObject(state), fValues(state.fValues),
      fFieldNames(state.fFieldNames) { }

CP::TReconState::~TReconState() { }

std::string CP::TReconState::GetStateFields(void) const {
    // Construct a type name out of the field names.  This in turn is used by
    // the TCorrValues class to construct a type hash which is used to make
    // sure that operations are done on compatible TCorrValues objects.
    std::string typeName;
    for (std::vector<std::string>::const_iterator n = fFieldNames.begin();
         n != fFieldNames.end();
         ++n) {
        typeName += *n;
        typeName += " ";
    };
    return typeName;
}

// Build the internal state vector.
void CP::TReconState::Init() {
    fValues.ResizeTo(fFieldNames.size());
    fValues.SetType(GetStateFields().c_str());
}

int CP::TReconState::GetDimensions() const {
    return fValues.GetDimensions();
}

double CP::TReconState::GetValue(int i) const {
    return fValues.GetValue(i);
}

void CP::TReconState::SetValue(int i, double val) {
    return fValues.SetValue(i, val);
}

double CP::TReconState::GetCovarianceValue(int i, int j) const {
    return fValues.GetCovarianceValue(i,j);
}

void CP::TReconState::SetCovarianceValue(int i, int j, double val) {
    fValues.SetCovarianceValue(i,j,val);
}

void CP::TReconState::SetFree(int i) {
    fValues.SetFree(i);
}

bool CP::TReconState::IsFree(int i) const {
    return fValues.IsFree(i);
}

bool CP::TReconState::IsFree(double v) const {
    return fValues.IsFree(v);
}

void CP::TReconState::SetFixed(int i) {
    fValues.SetFixed(i);
}

bool CP::TReconState::IsFixed(int i) const {
    return fValues.IsFixed(i);
}

bool CP::TReconState::IsFixed(double v) const {
    return fValues.IsFixed(v);
}

void CP::TReconState::Validate() {
    fValues.Validate(true);
}

CP::TCorrValues CP::TReconState::ProjectState(
    const CP::THandle<CP::TReconState>& state) {
    return state->fValues;
}

/// Print the object information.
void CP::TReconState::ls(Option_t*) const {
    TROOT::IndentLevel();
    std::cout << ClassName() << "(" << this << ")::" << std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetStateFields() << std::endl;
    TROOT::IncreaseDirLevel();
    std::ios::fmtflags save = std::cout.flags();
    for (int i = 0; i<GetDimensions(); ++i) {
        if (IsFree(i)) continue;
        TROOT::IndentLevel();
        std::cout << "  " << std::setw(6) << fFieldNames[i];
        std::cout << ":: " 
                  << std::setw(9) << std::setprecision(3) 
                  << GetValue(i);
        if (IsFixed(i)) {
            std::cout << "    fixed";
        }
        else {
            std::cout << " +- " 
                      << std::setw(6) << std::setprecision(2) 
                      << std::sqrt(GetCovarianceValue(i,i))
                      << " :";
            for (int j=0 ; j<i; ++j) {
                if (IsFree(j)) continue;
                if (IsFixed(j)) {
                    std::cout << "  fixed";
                    continue;
                }
                double c = GetCovarianceValue(i,j);
                c /= std::sqrt(GetCovarianceValue(i,i));
                c /= std::sqrt(GetCovarianceValue(j,j));
                if (std::abs(c) < 0.01) {
                    std::cout << "   negl";
                    continue;
                }
                std::cout << " "
                          << std::setw(6) << std::setprecision(2) 
                          << c;
            }
        }
        std::cout << std::endl;
    }
    std::cout.flags(save);
    TROOT::DecreaseDirLevel();
    TROOT::DecreaseDirLevel();
}
