#include <gtest/gtest.h>
#include "domain/problem.h"
#include "domain/solution.h"
#include "solvers/greedy_solver.h"

// ── Helper: construieste distante ──────────────────────────────
static std::vector<std::vector<double>> buildDistances(
    const std::vector<Zone>& zones,
    const std::vector<Candidate>& candidates)
{
    std::vector<std::vector<double>> d(
        zones.size(),
        std::vector<double>(candidates.size(), 0.0));
    return d;
}

// ─────────────────────────────────────────────────────────────
// TEST 1: trivial — o zona, un candidat in raza, buget suficient
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, TrivialOneZoneOneCandidate) {
    // zona la (46.770, 23.590), 10 conexiuni
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 10)
    };
    // candidat la ~50m distanta (acelasi loc practic)
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    // candidatul 0 trebuie deschis
    EXPECT_TRUE(sol.getLocations()[0]);

    // fractia zonei 0 catre candidatul 0 trebuie sa fie 1.0
    EXPECT_NEAR(sol.getLocationsFractions()[0][0], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 2: buget 0 → exceptie
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, BudgetZeroThrows) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 10)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 0);

    GreedySolver solver(p);
    EXPECT_THROW(solver.solve(), NoFeasibleSolutionException);
}

// ─────────────────────────────────────────────────────────────
// TEST 3: candidat in afara razei de 100m → exceptie
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, CandidateOutOfRadiusThrows) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 10)
    };
    // candidat la ~500m distanta
    std::vector<Candidate> candidates = {
        Candidate(0, 46.775, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    EXPECT_THROW(solver.solve(), NoFeasibleSolutionException);
}

// ─────────────────────────────────────────────────────────────
// TEST 4: split intre doi candidati
// zona cu 200 conexiuni, doi candidati cu capacity=150 fiecare
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, SplitBetweenTwoCandidates) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 200)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000),
        Candidate(1, 46.770, 23.591, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    // ambii candidati trebuie deschisi
    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_TRUE(sol.getLocations()[1]);

    // fractiile trebuie sa sumeze la 1.0
    double total = sol.getLocationsFractions()[0][0]
                 + sol.getLocationsFractions()[0][1];
    EXPECT_NEAR(total, 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 5: prioritate dupa marime
// zona mare servita primul, zona mica ramane fara candidat
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, LargeZoneServedFirst) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 10),   // zona mica
        Zone(1, 46.770, 23.590, 100)   // zona mare
    };
    // un singur candidat cu capacity=100 — ajunge doar pentru zona mare
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 100, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    // zona mica nu poate fi servita dupa ce zona mare epuizeaza candidatul
    EXPECT_THROW(solver.solve(), NoFeasibleSolutionException);
}

// ─────────────────────────────────────────────────────────────
// TEST 6: mai multe zone, mai multi candidati
// verifica ca fractiile fiecarei zone sumeaza la 1.0
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, MultiplZonesFractionsSum) {
    std::vector<Zone> zones = {
        Zone(0, 46.7700, 23.5900, 50),
        Zone(1, 46.7700, 23.5901, 50),
        Zone(2, 46.7701, 23.5900, 50)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.7700, 23.5900, 150, 2000),
        Candidate(1, 46.7701, 23.5900, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    int nCandidates = candidates.size();
    for(int i = 0; i < (int)zones.size(); ++i){
        double total = 0.0;
        for(int j = 0; j < nCandidates; ++j){
            total += sol.getLocationsFractions()[i][j];
        }
        EXPECT_NEAR(total, 1.0, 1e-6)
            << "Fractiile zonei " << i << " nu sumeaza la 1.0";
    }
}

TEST(GreedySolverTest, DebugThreeZones) {
    // toate cele 3 zone LA EXACT aceeasi coordonata ca candidatii
    std::vector<Zone> zones = {
        Zone(0, 46.7700, 23.5900, 50),
        Zone(1, 46.7700, 23.5900, 50),  // exact ca candidatul 0
        Zone(2, 46.7701, 23.5900, 50)   // exact ca candidatul 1
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.7700, 23.5900, 150, 2000),
        Candidate(1, 46.7701, 23.5900, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    EXPECT_NO_THROW(solver.solve());
}

// ─────────────────────────────────────────────────────────────
// TEST 8: zona cu connections=1 — cel mai mic caz posibil
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, SingleConnectionZone) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 1)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_NEAR(sol.getLocationsFractions()[0][0], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 9: zona cu connections exact egal cu capacity candidat
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, ZoneConnectionsExactlyFillsCandidate) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 150)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_NEAR(sol.getLocationsFractions()[0][0], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 10: doua zone, un singur candidat cu capacitate pentru ambele
// candidatul trebuie deschis o singura data
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, TwoZonesOneCandidate) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 50),
        Zone(1, 46.770, 23.590, 50)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    // candidatul deschis o singura data
    EXPECT_TRUE(sol.getLocations()[0]);

    // fractiile ambelor zone sumeaza la 1.0
    EXPECT_NEAR(sol.getLocationsFractions()[0][0], 1.0, 1e-6);
    EXPECT_NEAR(sol.getLocationsFractions()[1][0], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 11: buget exact suficient pentru un singur candidat
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, BudgetExactlyCoversOneCandidate) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 50)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 2000); // exact cat costa un candidat

    GreedySolver solver(p);
    Solution sol = solver.solve();

    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_NEAR(sol.getLocationsFractions()[0][0], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 12: buget suficient pentru un candidat dar nu pentru doi
// zona necesita doi candidati → exceptie
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, BudgetCoversOneCandidateButNeedTwo) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 200) // mai mult decat capacity unui singur candidat
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000),
        Candidate(1, 46.770, 23.591, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 2000); // buget doar pentru unul

    GreedySolver solver(p);
    EXPECT_THROW(solver.solve(), NoFeasibleSolutionException);
}

// ─────────────────────────────────────────────────────────────
// TEST 13: niciun candidat in lista → exceptie
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, NoCandidatesThrows) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 50)
    };
    std::vector<Candidate> candidates = {};
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    EXPECT_THROW(solver.solve(), NoFeasibleSolutionException);
}

// ─────────────────────────────────────────────────────────────
// TEST 14: nicio zona → solutie valida cu toti y=false
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, NoZonesReturnsEmptySolution) {
    std::vector<Zone> zones = {};
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    // niciun candidat deschis
    EXPECT_FALSE(sol.getLocations()[0]);
}

// ─────────────────────────────────────────────────────────────
// TEST 15: trei candidati in raza, zona servita de cel mai apropiat
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, ClosestCandidateSelectedFirst) {
    std::vector<Zone> zones = {
        Zone(0, 46.7700, 23.5900, 50)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.7700, 23.5900, 150, 2000), // distanta 0 — cel mai apropiat
        Candidate(1, 46.7701, 23.5900, 150, 2000), // ~11m
        Candidate(2, 46.7702, 23.5900, 150, 2000)  // ~22m
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    // candidatul 0 trebuie ales, nu 1 sau 2
    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_FALSE(sol.getLocations()[1]);
    EXPECT_FALSE(sol.getLocations()[2]);
    EXPECT_NEAR(sol.getLocationsFractions()[0][0], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 16: zona servita de trei candidati (connections > 2*capacity)
// fractiile trebuie sa sumeze la 1.0
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, ZoneSplitAcrossThreeCandidates) {
    std::vector<Zone> zones = {
        Zone(0, 46.7700, 23.5900, 350) // mai mult decat 2*150
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.7700, 23.5900, 150, 2000),
        Candidate(1, 46.7700, 23.5901, 150, 2000),
        Candidate(2, 46.7700, 23.5902, 150, 2000)
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    double total = 0.0;
    for(int j = 0; j < 3; ++j)
        total += sol.getLocationsFractions()[0][j];

    EXPECT_NEAR(total, 1.0, 1e-6);

    // toti trei candidati trebuie deschisi
    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_TRUE(sol.getLocations()[1]);
    EXPECT_TRUE(sol.getLocations()[2]);
}

// ─────────────────────────────────────────────────────────────
// TEST 17: candidat cu capacity=0 initial → ignorat
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, CandidateWithZeroCapacityIgnored) {
    std::vector<Zone> zones = {
        Zone(0, 46.770, 23.590, 50)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.770, 23.590, 0,   2000), // capacity 0
        Candidate(1, 46.770, 23.590, 150, 2000)  // capacity normala
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    EXPECT_FALSE(sol.getLocations()[0]); // candidatul cu capacity 0 ignorat
    EXPECT_TRUE(sol.getLocations()[1]);
    EXPECT_NEAR(sol.getLocationsFractions()[0][1], 1.0, 1e-6);
}

// ─────────────────────────────────────────────────────────────
// TEST 18: zone multiple, verifica ca y[j]=true doar pentru
// candidatii care sunt efectiv folositi
// ─────────────────────────────────────────────────────────────
TEST(GreedySolverTest, OnlyUsedCandidatesAreOpen) {
    std::vector<Zone> zones = {
        Zone(0, 46.7700, 23.5900, 50)
    };
    std::vector<Candidate> candidates = {
        Candidate(0, 46.7700, 23.5900, 150, 2000), // in raza
        Candidate(1, 46.7800, 23.5900, 150, 2000)  // departe, nu in raza
    };
    auto distances = buildDistances(zones, candidates);
    Problem p(zones, candidates, distances, 10000);

    GreedySolver solver(p);
    Solution sol = solver.solve();

    EXPECT_TRUE(sol.getLocations()[0]);
    EXPECT_FALSE(sol.getLocations()[1]); // nu trebuie deschis
}