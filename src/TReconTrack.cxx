#include "TReconTrack.hxx"
#include "TCorrValues.hxx"

ClassImp(CP::TReconTrack);

CP::TReconTrack::TReconTrack() {
    fState = new TTrackState;
    fBackState = new TTrackState;
    fNodes = new TReconNodeContainerImpl<CP::TTrackState>;
}

CP::TReconTrack::TReconTrack(const CP::TReconTrack& track)
    : CP::TReconBase(track) {
    fNodes = new TReconNodeContainerImpl<CP::TTrackState>;
    
    // Copy the nodes 
    // Create new nodes with TTrackState's 
    CP::TReconNodeContainer::const_iterator in;
    for (in=track.GetNodes().begin(); in!=track.GetNodes().end(); ++in){
        CP::THandle<CP::TReconNode> node(new CP::TReconNode);
        CP::THandle<CP::TReconBase> object = (*in)->GetObject();
        node->SetObject(object);
        CP::THandle<CP::TTrackState> tstate = (*in)->GetState();
        if (tstate) {
            CP::THandle<CP::TReconState> pstate(new CP::TTrackState(*tstate));
            node->SetState(pstate);
        }
        node->SetQuality((*in)->GetQuality());
        
        fNodes->push_back(node);
    }

    if (track.GetState()) {
        CP::THandle<CP::TTrackState> state = track.GetState();  
        fState = new TTrackState(*state);
    }
    else {
        fState = new TTrackState;
    }

    if (track.GetBack()) {
        CP::THandle<CP::TTrackState> state = track.GetBack();  
        fBackState = new TTrackState(*state);
    }
    else {
        fBackState = new TTrackState;
    }

}

CP::TReconTrack::~TReconTrack() {}

double CP::TReconTrack::GetEDeposit() const {
    THandle<CP::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetEDeposit();
}

TLorentzVector CP::TReconTrack::GetPosition() const {
    // This is the preferred way to access a state field.  
    THandle<CP::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPosition();
}

TLorentzVector CP::TReconTrack::GetPositionVariance() const {
    // This is the preferred way to access a state field.  
    THandle<CP::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPositionVariance();
}

bool CP::TReconTrack::IsXTrack() const {
    TLorentzVector var = GetPositionVariance();
    if (CP::TCorrValues::IsFree(var.X())) return false;
    return true;
}

bool CP::TReconTrack::IsYTrack() const {
    TLorentzVector var = GetPositionVariance();
    if (CP::TCorrValues::IsFree(var.Y())) return false;
    return true;
}

bool CP::TReconTrack::IsZTrack() const {
    TLorentzVector var = GetPositionVariance();
    if (CP::TCorrValues::IsFree(var.Z())) return false;
    return true;
}

int CP::TReconTrack::GetDimensions() const{
    TLorentzVector var = GetPositionVariance();
    int dim = 0;
    if (IsXTrack()) ++dim;
    if (IsYTrack()) ++dim;
    if (IsZTrack()) ++dim;
    return dim;
}

TVector3 CP::TReconTrack::GetDirection() const {
    // This is the preferred way to access a state field.  
    THandle<CP::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetDirection();
}

double CP::TReconTrack::GetMass() const {
    // This is the preferred way to access a state field.  
    THandle<CP::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetMass();
}

double CP::TReconTrack::GetWidth() const {
    THandle<CP::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetWidth();
}

void CP::TReconTrack::ReverseTrack() {
    // Reverse the order of the nodes.
    std::reverse(GetNodes().begin(), GetNodes().end());

    // Reverse the state directions.
    for (CP::TReconNodeContainer::iterator n = GetNodes().begin();
         n != GetNodes().end(); ++n) {
        CP::THandle<CP::TTrackState> state = (*n)->GetState();
        state->SetDirection(-state->GetDirection());
    }
    
    // Swap the front and back states.
    TTrackState tempState(*(GetFront()));
    *GetFront() = *GetBack();
    *GetBack() = tempState;

    // Reverse the direction at the front and back.
    GetFront()->SetDirection(-GetFront()->GetDirection());
    GetBack()->SetDirection(-GetBack()->GetDirection());

    // Swap the energy deposit at the front and back.
    double tempDeposit = GetFront()->GetEDeposit();
    GetFront()->SetEDeposit(GetBack()->GetEDeposit());
    GetBack()->SetEDeposit(tempDeposit);

}

void CP::TReconTrack::ls(Option_t *opt) const {
    ls_base(opt);

    TROOT::IncreaseDirLevel();
    std::string option(opt);
    if (fState) {
        TROOT::IncreaseDirLevel();
        fState->ls(opt);
        TROOT::DecreaseDirLevel();
    }
    if (fBackState) {
        TROOT::IncreaseDirLevel();
        fBackState->ls(opt);
        TROOT::DecreaseDirLevel();
    }
    if (fNodes && (option.find("dump") != std::string::npos
                   || option.find("recon") != std::string::npos)) {
        TROOT::IncreaseDirLevel();
        fNodes->ls(opt);
        TROOT::DecreaseDirLevel();
    }

    TROOT::IncreaseDirLevel();
    for (const_iterator v = begin(); v != end(); ++v) {
        (*v)->ls(opt);
    };
    TROOT::DecreaseDirLevel();

    TROOT::DecreaseDirLevel();
}
