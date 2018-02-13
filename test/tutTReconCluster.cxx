#include <iostream>
#include <cmath>
#include <tut.h>

#include <TDecompChol.h>

#include "TReconCluster.hxx"
#include "TReconTrack.hxx"
#include "TCaptLog.hxx"
#include "TMCHit.hxx"
#include "CaptGeomId.hxx"
#include "HEPUnits.hxx"

namespace tut {
    struct baseTReconCluster {
        baseTReconCluster() {
            // Run before each test.
        }
        ~baseTReconCluster() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseTReconCluster>::object testTReconCluster;
    test_group<baseTReconCluster> groupTReconCluster("TReconCluster");

    // Test the TReconCluster constructor and destructor.
    template<> template<>
    void testTReconCluster::test<1> () {
        CP::TReconCluster cluster;

        ensure_distance("Default quality is zero", 
                        cluster.GetQuality(), 0.0, 
                        std::numeric_limits<double>::epsilon());

        ensure_equals("Default degrees of freedom",
                      cluster.GetNDOF(), 0);

        ensure("Default state",cluster.GetState());

        CP::THandle<CP::TClusterState> clusterState = cluster.GetState();
        ensure("Default state is TClusterState",clusterState);

        CP::THandle<CP::TTrackState> trackState = cluster.GetState();
        ensure("Default state is not TTrackState",!trackState);

        ensure_equals("Default TReconNodesContainer has size zero",
                      cluster.GetNodes().size(), (unsigned) 0);

        ensure("Default has no hits",!cluster.GetHits());
    }

    // Test the state definition.
    template<> template<>
    void testTReconCluster::test<2> () {
        CP::TReconCluster recObj;
        CP::THandle<CP::TClusterState> recState = recObj.GetState();

        ensure("The state is valid", recState);

        const int stateDim = recState->GetDimensions();
        ensure_equals("Cluster state dimensions", stateDim, 5);
    }

    // Test nodes which are saved have the correct type of state and all have
    // an associated object.
    template<> template<>
    void testTReconCluster::test<3> () {
        CP::TReconCluster recObj;
        CP::TReconNodeContainer& recNodes = recObj.GetNodes();
        CP::THandle<CP::TReconNode> recNode;
        CP::THandle<CP::TReconBase> recTrack(new CP::TReconTrack);
        CP::THandle<CP::TReconState> recState(new CP::TClusterState);

        ensure_equals("Node container starts empty.",recNodes.size(),
                      (unsigned)0);

#ifdef MISSING_STATE_CHECK
        try {
            recNode = CP::THandle<CP::TReconNode>(new CP::TReconNode);
            recNode->SetObject(recTrack);
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
            CP::TCaptLog::ErrorPriority lvl = CP::TCaptLog::GetDebugLevel();
            CP::TCaptLog::SetDebugLevel(CP::TCaptLog::SilentLevel);
            recNodes.push_back(recNode);
            CP::TCaptLog::SetDebugLevel(lvl);
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
            recNode->SetObject(recTrack);
            recNode->SetState(recState);
            recNodes.push_back(recNode);
        }
        catch (...) {
            fail("TReconNodeContainer failed with correct object and state");
        }
    }

    // Test the getters and setters.
    template<> template<>
    void testTReconCluster::test<4> () {
        CP::TReconCluster recObj;
        CP::THandle<CP::TClusterState> recState = recObj.GetState();

        ensure("The state is valid", recState);

        const double energy = 1.414;
        const double energyVar = 3.14159;
        recState->SetEDeposit(energy);
        recState->SetEDepositVariance(energyVar);

        const TLorentzVector pos(1.1,1.2,1.3,1.4);
        recState->SetPosition(pos);

        int posIndex = recState->GetPositionIndex();
        double posVar = 0.0;
        for (int i=0; i<4; ++i) {
            for (int j=i; j<4; ++j) {
                posVar += 1.0;
                recState->SetCovarianceValue(i+posIndex,j+posIndex,posVar);
                double val 
                    = recState->GetCovarianceValue(i+posIndex,j+posIndex);
                ensure_distance("Cluster position variance saved correctly",
                                val, posVar, 0.001);
            }
        }

        ensure_distance("Cluster energy deposit saved",
                        recObj.GetEDeposit(), energy, 0.00001);

        ensure_lessthan("Cluster position saved",
                        (recObj.GetPosition()-pos).Mag(), 0.0001);

        CP::THandle<CP::TClusterState> s1 = recObj.GetState();
        ensure_equals("Cluster state always the same",
                      s1, recState);
        
        ensure_distance("Cluster energy variance saved",
                        s1->GetEDepositVariance(), energyVar, 0.0001);

        posVar = 0.0;
        for (int i=0; i<4; ++i) {
            for (int j=i; j<4; ++j) {
                posVar += 1.0;
                double val 
                    = recState->GetCovarianceValue(i+posIndex,j+posIndex);
                ensure_distance("Cluster position variance stored correctly",
                                val, posVar, 0.001);
            }
        }
    }

#ifdef TEST_FILLFROMHITS
    // Check TReconCluster::FillFromHits for one hit in a layer.
    template<> template<>
    void testTReconCluster::test<5> () {
        // Make a writable hit by first setting the "standard" values.  These
        // are constant for all hits to checking the output values easier.
        CP::TWritableMCHit wHit;
        wHit.SetCharge(40);
        wHit.SetChargeValidity(true);
        wHit.SetTime(1000);
        wHit.SetTimeValidity(true);

        // Build the hit selection
        CP::THitSelection inputHits;
        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster1;
        cluster1.FillFromHits("inputs1", inputHits);

        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster2;
        cluster2.FillFromHits("inputs2", inputHits);

        ensure_distance("One Layer Cluster (1 hit/layer) matches Hit X",
                        cluster1.GetPosition().X(), 
                        inputHits[0]->GetPosition().X(),
                        0.001);

        ensure_distance("One Layer Cluster (1 hit/layer) matches Hit Z",
                        cluster1.GetPosition().Z(), 
                        inputHits[0]->GetPosition().Z(),
                        0.001);

        ensure_distance("One Hit Cluster Sigma Matches Hit X",
                        std::sqrt(cluster1.GetPositionVariance().X()), 
                        inputHits[0]->GetUncertainty().X(),
                        0.001);
        ensure_greaterthan("One Hit Cluster Sigma Matches Hit Y",
                           std::sqrt(cluster1.GetPositionVariance().Y()), 
                           1E+8);
        ensure_distance("One Hit Cluster Sigma Matches Hit Z",
                        std::sqrt(cluster1.GetPositionVariance().Z()), 
                        inputHits[0]->GetUncertainty().Z(),
                        0.001);

        ensure_distance("Two Hit Cluster Sigma Matches Hit X",
                        std::sqrt(cluster2.GetPositionVariance().X()), 
                        std::sqrt(cluster1.GetPositionVariance().X()/2.0), 
                        0.001);
        ensure_greaterthan("Two Hit Cluster Sigma Matches Hit Y",
                           std::sqrt(cluster2.GetPositionVariance().Y()), 
                           1E+8);
        ensure_distance("Two Hit Cluster Sigma Matches Hit Z",
                        std::sqrt(cluster2.GetPositionVariance().Z()), 
                        std::sqrt(cluster1.GetPositionVariance().Z()/2.0), 
                        0.001);

    }

    // Check TReconCluster::FillFromHits for two hits in one layer, and
    // same projections (X & X) check the covariance.
    template<> template<>
    void testTReconCluster::test<6> () {
        // Make a writable hit by first setting the "standard" values.  These
        // are constant for all hits to checking the output values easier.
        CP::TWritableMCHit wHit;
        wHit.SetCharge(40);
        wHit.SetChargeValidity(true);
        wHit.SetTime(1000);
        wHit.SetTimeValidity(true);

        // Build the hit selection
        CP::THitSelection inputHits;
        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,1));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster;
        cluster.FillFromHits("inputs3", inputHits);

        ensure_distance("One Layer Cluster (2 hit/layer) Matches Hit X",
                        std::sqrt(cluster.GetPositionVariance().X()), 
                        3.007*unit::mm,
                        0.001);
        ensure_distance("One Layer Cluster (2 hit/layer) Matches Hit Z",
                        std::sqrt(cluster.GetPositionVariance().Z()), 
                        inputHits[0]->GetUncertainty().Z()/std::sqrt(2.0),
                        0.001);
    }

    // Check TReconCluster::FillFromHits for two hits in one layer, and
    // same projections (X & X) check the moments.
    template<> template<>
    void testTReconCluster::test<7> () {
        // Make a writable hit by first setting the "standard" values.  These
        // are constant for all hits to checking the output values easier.
        CP::TWritableMCHit wHit;
        wHit.SetCharge(40);
        wHit.SetChargeValidity(true);
        wHit.SetTime(1000);
        wHit.SetTimeValidity(true);

        // Build the hit selection
        CP::THitSelection inputHits;
        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,1));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster;
        cluster.FillFromHits("inputs4", inputHits);

        ensure_distance("Two Layer Moments (XX)",
                        (double) cluster.GetMoments()(0,0), 
                        161.5, 0.1);
        ensure_distance("Two Layer Moments (XY)  ",
                        (double) cluster.GetMoments()(0,1), 
                        0.0, 0.01);
        ensure_distance("Two Layer Moments (XZ)",
                        (double) cluster.GetMoments()(0,2), 
                        0.19, 0.01);

        // The hits are both in the X layer, so the Y moment is huge and
        // without any correlations.
        ensure_distance("Two Layer Moments (YX)",
                        (double) cluster.GetMoments()(1,0), 
                        0.0, 0.01);
        ensure_greaterthan("Two Layer Moments (YY)",
                           (double) cluster.GetMoments()(1,1), 
                           1E+8);
        ensure_distance("Two Layer Moments (YZ)",
                        (double) cluster.GetMoments()(1,2), 
                        0.0, 0.01);

        ensure_distance("Two Layer Moments (ZX)",
                        (double) cluster.GetMoments()(2,0), 
                        0.19, 0.01);
        ensure_distance("Two Layer Moments (ZY)",
                        (double) std::sqrt(cluster.GetMoments()(2,1)), 
                        0.0, 0.01);
        ensure_distance("Two Layer Moments (ZZ)",
                        (double) cluster.GetMoments()(2,2), 
                        25.06, 0.01);

        ///// Check the identity (moments*inverse)
        TMatrixD inverse(cluster.GetMoments()); inverse.InvertFast();
        TMatrixD ident(cluster.GetMoments(),TMatrixD::kMult,inverse);

        ensure_distance("Two Layer Moments Identity (XX)",
                        (double) ident(0,0), 1.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (XY)  ",
                        (double) ident(0,1), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (XZ)",
                        (double) ident(0,2), 0.0, 1E-6);

        // The hits are both in the X layer, so the Y moment is huge and
        // without any correlations.
        ensure_distance("Two Layer Moments Identity (YX)",
                        (double) ident(1,0), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (YY)",
                        (double) ident(1,1), 1.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (YZ)",
                        (double) ident(1,2), 0.0, 1E-6);

        ensure_distance("Two Layer Moments Identity (ZX)",
                        (double) ident(2,0), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (ZY)",
                        (double) ident(2,1), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (ZZ)",
                        (double) ident(2,2), 1.0, 1E-6);

    }

    // Check TReconCluster::FillFromHits for two hits in different layers, and
    // different projections (X & Y) check the covariance.
    template<> template<>
    void testTReconCluster::test<8> () {
        // Make a writable hit by first setting the "standard" values.  These
        // are constant for all hits to checking the output values easier.
        CP::TWritableMCHit wHit;
        wHit.SetCharge(40);
        wHit.SetChargeValidity(true);
        wHit.SetTime(1000);
        wHit.SetTimeValidity(true);

        // Build the hit selection
        CP::THitSelection inputHits;
        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,1,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster;
        cluster.FillFromHits("inputs4", inputHits);

        ensure_distance("Two Layer Cluster (1 hit/layer) matches Hit X",
                        cluster.GetPosition().X(), 
                        inputHits[0]->GetPosition().X(),
                        0.001);

        ensure_distance("Two Layer Cluster (1 hit/layer) matches Hit Y",
                        cluster.GetPosition().Y(), 
                        inputHits[1]->GetPosition().Y(),
                        0.001);

        ensure_distance("Two Layer Cluster (1 hit/layer) sigma matches Hit X",
                        std::sqrt(cluster.GetPositionVariance().X()), 
                        inputHits[0]->GetUncertainty().X(),
                        0.001);

        ensure_distance("Two Layer Cluster (1 hit/layer) sigma matches Hit Y",
                        std::sqrt(cluster.GetPositionVariance().Y()), 
                        inputHits[1]->GetUncertainty().Y(),
                        0.001);
        ensure_distance("Two Layer Cluster (1 hit/layer) sigma matches Hit Z",
                        std::sqrt(cluster.GetPositionVariance().Z()), 
                        3.5*unit::mm,
                        0.1);
    }

    // Check TReconCluster::FillFromHits for two hits in different layers, and
    // same projections (X & X) have a correct covariance matrix.
    template<> template<>
    void testTReconCluster::test<9> () {
        // Make a writable hit by first setting the "standard" values.  These
        // are constant for all hits to checking the output values easier.
        CP::TWritableMCHit wHit;
        wHit.SetCharge(40);
        wHit.SetChargeValidity(true);
        wHit.SetTime(1000);
        wHit.SetTimeValidity(true);

        // Build the hit selection
        CP::THitSelection inputHits;
        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        wHit.SetGeomId(CP::GeomId::P0D::Bar(6,0,1));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster;
        cluster.FillFromHits("inputs5", inputHits);
        CP::THandle<CP::TClusterState> state = cluster.GetState();

        ensure_distance("Two Layer Cluster (1 hit/layer) matches Hit X",
                        cluster.GetPosition().X(), 
                        0.5*(inputHits[0]->GetPosition().X()
                             +inputHits[1]->GetPosition().X()),
                        0.001);

        ensure_distance("Two Layer Cluster (1 hit/layer) matches Hit Z",
                        cluster.GetPosition().Z(), 
                        0.5*(inputHits[0]->GetPosition().Z()
                             +inputHits[1]->GetPosition().Z()),
                        0.001);

        ensure_distance("Two Layer Cluster (1 hit/layer) sigma matches Hit X",
                        std::sqrt(cluster.GetPositionVariance().X()), 
                        2.99*unit::mm,
                        0.01);

        ensure_greaterthan("Two Layer Cluster (1 hit/layer) sigma matches Hit Y",
                           std::sqrt(cluster.GetPositionVariance().Y()), 
                           1E+8);

        ensure_distance("Two Layer Cluster (1 hit/layer) sigma matches Hit Z",
                        std::sqrt(cluster.GetPositionVariance().Z()), 
                        7.2*unit::mm,
                        0.1);

        ensure_distance("Two Layer Covariance (XZ)",
                        state->GetCovarianceValue(
                            state->GetXIndex(), state->GetZIndex()),
                        20.15,
                        0.1);
        ensure_distance("Two Layer Covariance (ZX)",
                        state->GetCovarianceValue(
                            state->GetZIndex(), state->GetXIndex()),
                        state->GetCovarianceValue(
                            state->GetXIndex(), state->GetZIndex()),
                        0.1);

        ensure_distance("Two Layer Covariance (XY)",
                        state->GetCovarianceValue(
                            state->GetXIndex(), state->GetYIndex()),
                        0.0, 0.001);
        ensure_distance("Two Layer Covariance (YX)",
                        state->GetCovarianceValue(
                            state->GetYIndex(), state->GetXIndex()),
                        0.0, 0.001);

        ensure_distance("Two Layer Covariance (ZY)",
                        state->GetCovarianceValue(
                            state->GetZIndex(), state->GetYIndex()),
                        0.0, 0.001);
        ensure_distance("Two Layer Covariance (YZ)",
                        state->GetCovarianceValue(
                            state->GetYIndex(), state->GetZIndex()),
                        0.0, 0.001);
    }

    // Check TReconCluster::FillFromHits for two hits in different layers, and
    // same projections (X & X) have a good set of moments.
    template<> template<>
    void testTReconCluster::test<10> () {
        // Make a writable hit by first setting the "standard" values.  These
        // are constant for all hits to checking the output values easier.
        CP::TWritableMCHit wHit;
        wHit.SetCharge(40);
        wHit.SetChargeValidity(true);
        wHit.SetTime(1000);
        wHit.SetTimeValidity(true);

        // Build the hit selection
        CP::THitSelection inputHits;
        wHit.SetGeomId(CP::GeomId::P0D::Bar(5,0,0));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        wHit.SetGeomId(CP::GeomId::P0D::Bar(6,0,1));
        inputHits.push_back(CP::THandle<CP::THit>(new CP::TMCHit(wHit)));

        CP::TReconCluster cluster;
        cluster.FillFromHits("inputs5", inputHits);

        cluster.GetMoments().Print();

        ensure_distance("Two Layer Moments (XX)",
                        (double) cluster.GetMoments()(0,0), 
                        160.5, 0.1);
        ensure_distance("Two Layer Moments (XY)  ",
                        (double) cluster.GetMoments()(0,1), 
                        0.0, 0.01);
        ensure_distance("Two Layer Moments (XZ)",
                        (double) cluster.GetMoments()(0,2), 
                        180.2, 0.1);

        // The hits are both in the X layer, so the Y moment is huge and
        // without any correlations.
        ensure_distance("Two Layer Moments (YX)",
                        (double) cluster.GetMoments()(1,0), 
                        0.0, 0.01);
        ensure_greaterthan("Two Layer Moments (YY)",
                           (double) cluster.GetMoments()(1,1), 
                           1E+8);
        ensure_distance("Two Layer Moments (YZ)",
                        (double) cluster.GetMoments()(1,2), 
                        0.0, 0.01);

        ensure_distance("Two Layer Moments (ZX)",
                        (double) cluster.GetMoments()(2,0), 
                        180.2, 0.1);
        ensure_distance("Two Layer Moments (ZY)",
                        (double) cluster.GetMoments()(2,1), 
                        0.0, 0.01);
        ensure_distance("Two Layer Moments (ZZ)",
                        (double) cluster.GetMoments()(2,2), 
                        490.9, 0.1);

        ///// Check the identity (moments*inverse)
        TMatrixD inverse(cluster.GetMoments()); inverse.InvertFast();
        TMatrixD ident(cluster.GetMoments(),TMatrixD::kMult,inverse);

        ensure_distance("Two Layer Moments Identity (XX)",
                        (double) ident(0,0), 1.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (XY)  ",
                        (double) ident(0,1), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (XZ)",
                        (double) ident(0,2), 0.0, 1E-6);

        // The hits are both in the X layer, so the Y moment is huge and
        // without any correlations.
        ensure_distance("Two Layer Moments Identity (YX)",
                        (double) ident(1,0), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (YY)",
                        (double) ident(1,1), 1.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (YZ)",
                        (double) ident(1,2), 0.0, 1E-6);

        ensure_distance("Two Layer Moments Identity (ZX)",
                        (double) ident(2,0), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (ZY)",
                        (double) ident(2,1), 0.0, 1E-6);
        ensure_distance("Two Layer Moments Identity (ZZ)",
                        (double) ident(2,2), 1.0, 1E-6);

    }
#endif /* TEST_FILLFROMHITS */

};
