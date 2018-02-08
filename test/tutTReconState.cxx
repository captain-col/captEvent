#include <iostream>
#include <tut.h>

#include "THandle.hxx"
#include "TReconState.hxx"
#include "TCorrValues.hxx"
#include "TClusterState.hxx"
#include "TShowerState.hxx"
#include "TTrackState.hxx"
#include "TPIDState.hxx"
#include "TVertexState.hxx"

namespace tut {
    struct baseTReconState {
        baseTReconState() {
            // Run before each test.
        }
        ~baseTReconState() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseTReconState>::object testTReconState;
    test_group<baseTReconState> groupTReconState("TReconState");

    // Test the default constructor and destructor.
    template<> template<>
    void testTReconState::test<1> () {
        CP::TClusterState v;
    }

    // Test the TClusterState projection operator.
    template<> template<>
    void testTReconState::test<2> () {
        CP::THandle<CP::TClusterState> s(new CP::TClusterState);

        ensure_greaterthan("TClusterState dimensionality",s->GetDimensions(),4);
        for (int i = 0; i<s->GetDimensions(); ++i) {
            s->SetCovarianceValue(i,i,double(i+1));
        }
        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Expected cluster state covariance", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        CP::TCorrValues v = CP::TClusterState::ProjectState(s);

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Cluster state covariance after projection", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Projected cluster state covariance", 
                            v.GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }
    }

    // Test the TShowerState projection operator.
    template<> template<>
    void testTReconState::test<3> () {
        CP::THandle<CP::TShowerState> s(new CP::TShowerState);

        ensure_greaterthan("TShowerState dimensionality",s->GetDimensions(),4);
        for (int i = 0; i<s->GetDimensions(); ++i) {
            s->SetCovarianceValue(i,i,double(i+1));
        }
        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Expected shower state covariance", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        CP::TCorrValues v = CP::TShowerState::ProjectState(s);

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Shower state covariance after projection", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Projected shower state covariance", 
                            v.GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }
    }

    // Test the TTrackState projection operator.
    template<> template<>
    void testTReconState::test<4> () {
        CP::THandle<CP::TTrackState> s(new CP::TTrackState);

        ensure_greaterthan("TTrackState dimensionality",s->GetDimensions(),4);
        for (int i = 0; i<s->GetDimensions(); ++i) {
            s->SetCovarianceValue(i,i,double(i+1));
        }
        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Expected track state covariance", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        CP::TCorrValues v = CP::TTrackState::ProjectState(s);

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Track state covariance after projection", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Projected track state covariance", 
                            v.GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }
    }

    // Test the TVertexState projection operator.
    template<> template<>
    void testTReconState::test<5> () {
        CP::THandle<CP::TVertexState> s(new CP::TVertexState);

        ensure_greaterthan("TVertexState dimensionality",s->GetDimensions(),3);
        for (int i = 0; i<s->GetDimensions(); ++i) {
            s->SetCovarianceValue(i,i,double(i+1));
        }
        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Expected vertex state covariance", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        CP::TCorrValues v = CP::TVertexState::ProjectState(s);

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Vertex state covariance after projection", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Projected vertex state covariance", 
                            v.GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }
    }

    // Test the TPIDState projection operator.
    template<> template<>
    void testTReconState::test<6> () {
        CP::THandle<CP::TPIDState> s(new CP::TPIDState);

        ensure_greaterthan("TPIDState dimensionality",s->GetDimensions(),4);
        for (int i = 0; i<s->GetDimensions(); ++i) {
            s->SetCovarianceValue(i,i,double(i+1));
        }
        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Expected PID state covariance", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        CP::TCorrValues v = CP::TPIDState::ProjectState(s);

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("PID state covariance after projection", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Projected PID state covariance", 
                            v.GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }
    }

    // Test the TPIDState projection operator.
    template<> template<>
    void testTReconState::test<7> () {
        CP::THandle<CP::TVertexState> s(new CP::TVertexState);

        ensure_greaterthan("TVertexState dimensionality",s->GetDimensions(),3);
        for (int i = 0; i<s->GetDimensions(); ++i) {
            s->SetCovarianceValue(i,i,double(i+1));
        }
        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Expected PID state covariance", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        CP::TCorrValues v = CP::TVertexState::ProjectState(s);

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Vertex state covariance after projection", 
                            s->GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }

        for (int i = 0; i<s->GetDimensions(); ++i) {
            ensure_distance("Projected vertex state covariance", 
                            v.GetCovarianceValue(i,i),
                            double(i+1), 0.0001);
        }
    }
    
};
