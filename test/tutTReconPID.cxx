#include <iostream>
#include <tut.h>

#include "TReconBase.hxx"
#include "TReconNode.hxx"
#include "TReconState.hxx"
#include "TReconCluster.hxx"
#include "TReconShower.hxx"
#include "TReconTrack.hxx"
#include "TReconVertex.hxx"
#include "TReconPID.hxx"

namespace tut {
    struct baseTReconPID {
        baseTReconPID() {
            // Run before each test.
        }
        ~baseTReconPID() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseTReconPID>::object testTReconPID;
    test_group<baseTReconPID> groupTReconPID("TReconPID");

    // Test the TReconPID constructor and destructor.
    template<> template<>
    void testTReconPID::test<1> () {
        CP::TReconPID recObj;

        ensure_distance("Default quality is zero", 
                        recObj.GetQuality(), 0.0, 
                        std::numeric_limits<double>::epsilon());

        ensure_equals("Default degrees of freedom",
                      recObj.GetNDOF(), 0);

        ensure("Default state",recObj.GetState());

        CP::THandle<CP::TPIDState> recState = recObj.GetState();
        ensure("Default state is TPIDState",recState);

        CP::THandle<CP::TClusterState> clusterState = recObj.GetState();
        ensure("Default state is not TClusterState",!clusterState);

        ensure_equals("Default TReconNodesContainer has size zero",
                      recObj.GetNodes().size(), (unsigned) 0);

        ensure("Default has no hits",!recObj.GetHits());
    }

    // Test the state definition.
    template<> template<>
    void testTReconPID::test<2> () {
        CP::TReconPID recObj;
        CP::THandle<CP::TPIDState> recState = recObj.GetState();

        ensure("The state is valid", recState);

        const int stateDim = recState->GetDimensions();
        ensure_equals("PID state dimensions", stateDim, 10);
    }

    // Test nodes which are saved have the correct type of state and all have
    // an associated object.
    template<> template<>
    void testTReconPID::test<3> () {
        CP::TReconPID recObj;
        CP::TReconNodeContainer& recNodes = recObj.GetNodes();
        CP::THandle<CP::TReconNode> recNode;
        CP::THandle<CP::TReconBase> recCluster(new CP::TReconCluster);
        CP::THandle<CP::TReconState> recState(new CP::TPIDState);

        ensure_equals("Node container starts empty.",recNodes.size(),
                      (unsigned) 0);

#ifdef CHECK_MISSING_STATE
        try {
            recNode = CP::THandle<CP::TReconNode>(new CP::TReconNode);
            recNode->SetObject(recCluster);
            recNodes.push_back(recNode);
            fail("TReconNodeContainer node state is empty");
        } 
        catch (CP::EWrongStateType& ex) {
            // OK!
            std::cout << std::endl
                      << "************************" << std::endl
                      << "Success with caught exception:" << std::endl 
                      << ex.what() << std::endl
                      << "************************" << std::endl;
        }
#endif

        try {
            recNode = CP::THandle<CP::TReconNode>(new CP::TReconNode);
            recNode->SetState(recState);
            recNodes.push_back(recNode);
            recNode->ls();
            fail("TReconNodeContainer object is empty");
        } 
        catch (CP::EObjectEmpty& ex) {
            // OK!
            std::cout << std::endl
                      << "************************" << std::endl
                      << "Success with caught exception:" << std::endl 
                      << ex.what() << std::endl
                      << "************************" << std::endl;
        }

        try {
            recNode = CP::THandle<CP::TReconNode>(new CP::TReconNode);
            recNode->SetObject(recCluster);
            recNode->SetState(recState);
            recNodes.push_back(recNode);
        }
        catch (...) {
            fail("TReconNodeContainer failed with correct object and state");
        }
    }

    // Test the getters and setters.
    template<> template<>
    void testTReconPID::test<4> () {
        CP::TReconPID recObj;
        CP::THandle<CP::TPIDState> recState = recObj.GetState();

        ensure("The state is valid", recState);

        const double momentum = 1.414;
        recState->SetMomentum(momentum);

        const TLorentzVector pos(1.1,1.2,1.3,1.4);
        recState->SetPosition(pos);

        ensure_distance("PID momentum saved",
                        recObj.GetMomentum(), momentum, 0.00001);

        ensure_lessthan("PID position saved",
                        (recObj.GetPosition()-pos).Mag(), 0.0001);
    }
};
