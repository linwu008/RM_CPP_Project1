#define main rm_project_entry
#include "../src/main.cpp"
#undef main

#include <sstream>
#include <string>
#include <limits>
#include <cmath>

int main()
{
    int total = 0;
    int failed = 0;
    auto expect = [&](const char* label, bool passed)
    {
        ++total;
        if (!passed) ++failed;
        std::cout << (passed ? "PASS " : "FAIL ") << label << '\n';
    };

    auto expectTime = [&](const char* label, const Clock& current, double elapsed, double remaining)
    {
        expect(label, std::abs(current.GetGoingTime() - elapsed) < 1e-9 &&
                      std::abs(current.GetRemainingTime() - remaining) < 1e-9);
    };
    Clock clock;
    expect("clock starts at tick zero", clock.GetTick() == 0);
    expect("match starts in turn one and is active", clock.GetTurn() == 1 && !clock.IsGameOver());
    expectTime("initial time is zero elapsed and 420 seconds remaining", clock, 0.0, 420.0);
    expect("first tick advances successfully", clock.AdvanceTick() && clock.GetTick() == 1);
    expectTime("first tick retains fractional seconds", clock, 0.1, 419.9);
    bool allClockStepsSucceeded = true;
    for (int i = 1; i < 9; ++i)
        allClockStepsSucceeded = clock.AdvanceTick() && allClockStepsSucceeded;
    expect("tick nine remains in turn one", clock.GetTick() == 9 && clock.GetTurn() == 1 && !clock.IsGameOver());
    allClockStepsSucceeded = clock.AdvanceTick() && allClockStepsSucceeded;
    expect("ten ticks represent one simulated second",
           allClockStepsSucceeded && clock.GetTick() == 10);
    expectTime("one second elapsed leaves 419 seconds", clock, 1.0, 419.0);
    expect("tick ten starts turn two", clock.GetTurn() == 2 && !clock.IsGameOver());
    for (int i = 10; i < 4199; ++i)
        allClockStepsSucceeded = clock.AdvanceTick() && allClockStepsSucceeded;
    expect("clock reaches the step before the time limit",
           allClockStepsSucceeded && clock.GetTick() == 4199);
    expectTime("one tick remains before the time limit", clock, 419.9, 0.1);
    expect("last turn is active before the final tick", clock.GetTurn() == 420 && !clock.IsGameOver());
    expect("last permitted tick reaches the time limit",
           clock.AdvanceTick() && clock.GetTick() == 4200);
    expect("final tick ends the match in turn 420", clock.IsGameOver() && clock.GetTurn() == 420);
    bool stoppedAtLimit = true;
    for (int i = 0; i < 10; ++i)
        stoppedAtLimit = !clock.AdvanceTick() && clock.GetTick() == 4200 && stoppedAtLimit;
    expect("repeated advances after the time limit preserve the clock", stoppedAtLimit);
    expectTime("finished time stays at 420 seconds with zero remaining", clock, 420.0, 0.0);
    expect("extra advances preserve the finished turn", clock.IsGameOver() && clock.GetTurn() == 420);
    Clock anotherClock;
    expect("new clocks have independent elapsed time", anotherClock.GetTick() == 0);

    Clock simulationClock;
    std::vector<Robot> simulationRobots{
        {1, Team::Red, RobotType::Heavy, {1, 7}, 400, 0, 3},
        {3, Team::Blue, RobotType::Infantry, {27, 6}, 200, 5, 5},
        {7, Team::Red, RobotType::Sentry, {1, 11}, 400, 300, 3}
    };
    for (Robot& robot : simulationRobots)
        robot.TryConsumeActionPoints(robot.GetActionPoints());
    simulationRobots[2].TakeDamage(400);
    bool simulationStepsSucceeded = true;
    for (int i = 0; i < 9; ++i)
        simulationStepsSucceeded = AdvanceSimulation(simulationClock, simulationRobots) && simulationStepsSucceeded;
    expect("simulation advances within a turn without restoring movement",
           simulationStepsSucceeded && simulationClock.GetTick() == 9 &&
           simulationRobots[0].GetActionPoints() == 0 && simulationRobots[1].GetActionPoints() == 0);
    const bool crossedTurn = AdvanceSimulation(simulationClock, simulationRobots);
    expect("new simulation turn restores both teams' configured movement budgets",
           crossedTurn && simulationClock.GetTick() == 10 && simulationClock.GetTurn() == 2 &&
           simulationRobots[0].GetActionPoints() == 3 && simulationRobots[1].GetActionPoints() == 5);
    expect("new simulation turn preserves defeated robot movement",
           simulationRobots[2].GetHp() == 0 && simulationRobots[2].GetActionPoints() == 0);
    simulationRobots[0].TryConsumeActionPoints(2);
    expect("following simulation tick preserves spent movement",
           AdvanceSimulation(simulationClock, simulationRobots) && simulationClock.GetTick() == 11 &&
           simulationRobots[0].GetActionPoints() == 1);
    while (simulationClock.GetTick() < 4199)
        simulationClock.AdvanceTick();
    simulationRobots[1].TryConsumeActionPoints(4);
    expect("final simulation tick ends the match without restoring movement",
           AdvanceSimulation(simulationClock, simulationRobots) && simulationClock.IsGameOver() &&
           simulationClock.GetTick() == 4200 && simulationClock.GetTurn() == 420 &&
           simulationRobots[0].GetActionPoints() == 1 && simulationRobots[1].GetActionPoints() == 1);
    expect("finished simulation rejects further ticks and preserves resources",
           !AdvanceSimulation(simulationClock, simulationRobots) && simulationClock.GetTick() == 4200 &&
           simulationRobots[0].GetActionPoints() == 1 && simulationRobots[1].GetActionPoints() == 1);
    expect("simulation advancement preserves HP and ammo",
           simulationRobots[0].GetHp() == 400 && simulationRobots[0].GetAmmo() == 0 &&
           simulationRobots[1].GetHp() == 200 && simulationRobots[1].GetAmmo() == 5 &&
           simulationRobots[2].GetHp() == 0 && simulationRobots[2].GetAmmo() == 300);
    Clock emptySimulationClock;
    std::vector<Robot> emptySimulationRobots;
    for (int i = 0; i < 9; ++i)
        emptySimulationClock.AdvanceTick();
    expect("empty simulation can cross a turn boundary",
           AdvanceSimulation(emptySimulationClock, emptySimulationRobots) && emptySimulationClock.GetTurn() == 2);

    std::ostringstream startupOutput;
    std::ostringstream startupErrors;
    auto* previousOutput = std::cout.rdbuf(startupOutput.rdbuf());
    auto* previousErrors = std::cerr.rdbuf(startupErrors.rdbuf());
    const int startupResult = rm_project_entry();
    std::cout.rdbuf(previousOutput);
    std::cerr.rdbuf(previousErrors);

    const std::string output = startupOutput.str();
    expect("startup succeeds", startupResult == 0 && startupErrors.str().empty());
    expect("red base shows official HP at chosen position",
           output.find("Red base\nposition: (2, 7.5)\nbase HP: 10000") != std::string::npos);
    expect("blue base shows official HP at chosen position",
           output.find("Blue base\nposition: (26, 7.5)\nbase HP: 10000") != std::string::npos);
    expect("red outpost is displayed with its initial state",
           output.find("Red outpost\nposition: (11, 11.5)\noutpost HP: 1000") != std::string::npos);
    expect("blue outpost is displayed with its initial state",
           output.find("Blue outpost\nposition: (17, 3.5)\noutpost HP: 1000") != std::string::npos);
    expect("400 HP configuration displays current and maximum HP",
           output.find("robot HP: 400/400") != std::string::npos);
    expect("200 HP configuration displays current and maximum HP",
           output.find("robot HP: 200/200") != std::string::npos);

    // Exercise the real validators with changed data, including the last objects.
    auto expectCheck = [&](const char* label, auto check, bool expected)
    {
        std::ostringstream errors;
        auto* previous = std::cerr.rdbuf(errors.rdbuf());
        const bool actual = check();
        std::cerr.rdbuf(previous);
        expect(label, actual == expected &&
               (expected ? errors.str().empty() : !errors.str().empty()));
    };

    const auto robots = CreateRobots();
    const auto bases = CreateBases();
    const auto outposts = CreateOutposts();

    struct StartingRobotCase
    {
        const char* label;
        Team team;
        int id;
        RobotType type;
        int hp;
        int allowedAmmo;
    };
    const StartingRobotCase startingRobotCases[] = {
        {"red heavy initial configuration", Team::Red, 1, RobotType::Heavy, 400, 0},
        {"blue heavy initial configuration", Team::Blue, 1, RobotType::Heavy, 400, 0},
        {"red infantry initial configuration", Team::Red, 3, RobotType::Infantry, 200, 0},
        {"blue infantry initial configuration", Team::Blue, 3, RobotType::Infantry, 200, 0},
        {"red automatic sentry initial configuration", Team::Red, 7, RobotType::Sentry, 400, 300},
        {"blue automatic sentry initial configuration", Team::Blue, 7, RobotType::Sentry, 400, 300}
    };
    for (const StartingRobotCase& test : startingRobotCases)
    {
        int matches = 0;
        bool correctState = false;
        for (const Robot& robot : robots)
        {
            if (robot.team == test.team && robot.id == test.id)
            {
                ++matches;
                correctState = robot.GetType() == test.type && robot.GetHp() == test.hp &&
                               robot.GetMaxHp() == test.hp &&
                               robot.GetAmmo() == test.allowedAmmo;
            }
        }
        expect(test.label, matches == 1 && correctState);
    }

    auto changedRobots = robots;
    auto changedBases = bases;
    auto changedOutposts = outposts;

    expectCheck("valid initial positions", [&] { return CheckInitialPositions(robots, bases, outposts); }, true);
    expectCheck("valid initial resources", [&] { return CheckInitialResources(robots, bases, outposts); }, true);
    expectCheck("valid initial occupancy", [&] { return CheckInitialOverlap(robots, bases, outposts); }, true);

    changedRobots.front().TakeDamage(1);
    expectCheck("wounded first robot cannot start", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back().TakeDamage(1);
    expectCheck("wounded last robot cannot start", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedBases.front().hp = 9999;
    expectCheck("base initial HP below official value", [&] { return CheckInitialResources(robots, changedBases, outposts); }, false);
    changedBases = bases;
    changedBases.back().hp = 10001;
    expectCheck("base initial HP above official value", [&] { return CheckInitialResources(robots, changedBases, outposts); }, false);
    changedOutposts.back().hp = 1001;
    expectCheck("outpost initial HP above official value", [&] { return CheckInitialResources(robots, bases, changedOutposts); }, false);
    changedRobots = robots;
    changedBases = bases;
    changedOutposts = outposts;

    changedOutposts.front().position.x = -0.01;
    expectCheck("red outpost outside map", [&] { return CheckInitialPositions(robots, bases, changedOutposts); }, false);
    changedOutposts = outposts;
    changedOutposts.back().position.y = 15.01;
    expectCheck("blue outpost outside map", [&] { return CheckInitialPositions(robots, bases, changedOutposts); }, false);
    changedOutposts = outposts;
    changedOutposts.front().position = {0, 0};
    changedOutposts.back().position = {28, 15};
    expectCheck("outposts on inclusive boundaries", [&] { return CheckInitialPositions(robots, bases, changedOutposts); }, true);

    changedOutposts = outposts;
    changedOutposts.front().hp = 0;
    expectCheck("red outpost zero HP", [&] { return CheckInitialResources(robots, bases, changedOutposts); }, false);
    changedOutposts = outposts;
    changedOutposts.back().hp = -1;
    expectCheck("blue outpost negative HP", [&] { return CheckInitialResources(robots, bases, changedOutposts); }, false);
    changedOutposts = outposts;
    changedOutposts.back().hp = 1;
    expectCheck("outpost initial HP below official value", [&] { return CheckInitialResources(robots, bases, changedOutposts); }, false);

    changedRobots.front().position = {11, 11.5};
    expectCheck("robot overlaps red outpost", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back().position = {17, 3.5};
    expectCheck("last robot overlaps blue outpost", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.front().position = {17, 3.5};
    expectCheck("robot overlaps opposing outpost", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.front().position.x = 11;
    expectCheck("only outpost x matches", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, true);
    changedRobots = robots;
    changedRobots.front().position.y = 11.5;
    expectCheck("only outpost y matches", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, true);

    // Preserve previously verified robot/base rules after adding the parameter.
    changedRobots = robots;
    changedRobots.back().position = changedRobots[4].position;
    expectCheck("last robot pair overlaps", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.front().position = {2, 7.5};
    expectCheck("robot overlaps red base", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back().position = {26, 7.5};
    expectCheck("last robot overlaps blue base", [&] { return CheckInitialOverlap(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.front().position.x = -1;
    expectCheck("robot outside map", [&] { return CheckInitialPositions(changedRobots, bases, outposts); }, false);
    changedBases.back().position.x = 28.1;
    expectCheck("base outside map", [&] { return CheckInitialPositions(robots, changedBases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back() = Robot{7, Team::Blue, RobotType::Sentry, {27, 4}, 0, 300, 3};
    expectCheck("robot zero initial HP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedBases = bases;
    changedBases.front().hp = 0;
    expectCheck("base zero initial HP", [&] { return CheckInitialResources(robots, changedBases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back() = Robot{7, Team::Blue, RobotType::Sentry, {27, 4}, 400, 0, 0};
    expectCheck("zero robot ammo and AP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, true);
    changedRobots.back() = Robot{7, Team::Blue, RobotType::Sentry, {27, 4}, 400, -1, 0};
    expectCheck("negative robot ammo", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back() = Robot{7, Team::Blue, RobotType::Sentry, {27, 4}, 400, 300, -1};
    expectCheck("negative robot AP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    expectCheck("cross-team equal IDs", [&] { return CheckUniqueRobotIds(robots); }, true);
    changedRobots = robots;
    changedRobots.back().id = 3;
    expectCheck("later same-team duplicate ID", [&] { return CheckUniqueRobotIds(changedRobots); }, false);

    struct DamageCase
    {
        const char* label;
        int initialHp;
        int damage;
        int expectedHp;
    };
    const DamageCase damageCases[] = {
        {"ordinary damage applies once", 100, 30, 70},
        {"exact lethal damage", 100, 100, 0},
        {"overkill stops at zero", 100, 150, 0},
        {"zero damage preserves HP", 100, 0, 100},
        {"negative damage preserves HP", 100, -20, 100},
        {"damage to dead robot", 0, 30, 0},
        {"negative damage to dead robot", 0, -20, 0},
        {"one HP lethal damage", 1, 1, 0},
        {"maximum integer damage", 100, std::numeric_limits<int>::max(), 0},
        {"minimum integer damage", 100, std::numeric_limits<int>::min(), 100}
    };
    for (const DamageCase& test : damageCases)
    {
        Robot robot{1, Team::Red, RobotType::Infantry, {1, 7}, test.initialHp, 50, 3};
        robot.TakeDamage(test.damage);
        expect(test.label, robot.GetHp() == test.expectedHp);
    }

    Robot repeatedlyHit{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    repeatedlyHit.TakeDamage(30);
    repeatedlyHit.TakeDamage(30);
    expect("separate calls accumulate damage", repeatedlyHit.GetHp() == 40);

    Robot firstRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    Robot secondRobot{1, Team::Blue, RobotType::Infantry, {27, 8}, 100, 50, 3};
    firstRobot.TakeDamage(35);
    expect("damage changes only the selected robot",
           firstRobot.GetHp() == 65 && secondRobot.GetHp() == 100);

    Robot loadedRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    const bool shotSucceeded = loadedRobot.TryConsumeAmmo();
    expect("single shot consumes one round", shotSucceeded && loadedRobot.GetAmmo() == 49);

    Robot lastRoundRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 1, 3};
    const bool lastShotSucceeded = lastRoundRobot.TryConsumeAmmo();
    expect("last round can be consumed", lastShotSucceeded && lastRoundRobot.GetAmmo() == 0);
    const bool extraShotSucceeded = lastRoundRobot.TryConsumeAmmo();
    expect("shot after last round fails", !extraShotSucceeded && lastRoundRobot.GetAmmo() == 0);

    Robot emptyRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 0, 3};
    const bool emptyShotSucceeded = emptyRobot.TryConsumeAmmo();
    expect("empty magazine preserves zero", !emptyShotSucceeded && emptyRobot.GetAmmo() == 0);

    Robot threeRoundRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 3, 3};
    const bool firstShot = threeRoundRobot.TryConsumeAmmo();
    const bool secondShot = threeRoundRobot.TryConsumeAmmo();
    const bool thirdShot = threeRoundRobot.TryConsumeAmmo();
    expect("three calls consume three rounds",
           firstShot && secondShot && thirdShot && threeRoundRobot.GetAmmo() == 0);
    const bool fourthShot = threeRoundRobot.TryConsumeAmmo();
    expect("fourth shot with three rounds fails", !fourthShot && threeRoundRobot.GetAmmo() == 0);

    Robot selectedRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    Robot otherRobot{1, Team::Blue, RobotType::Infantry, {27, 8}, 100, 50, 3};
    const bool selectedShot = selectedRobot.TryConsumeAmmo();
    expect("only selected robot consumes ammo",
           selectedShot && selectedRobot.GetAmmo() == 49 && otherRobot.GetAmmo() == 50);

    struct ActionPointCase
    {
        const char* label;
        int initialPoints;
        int cost;
        bool expectedSuccess;
        int expectedPoints;
    };
    const ActionPointCase actionPointCases[] = {
        {"consume part of action budget", 3, 1, true, 2},
        {"consume exact action budget", 3, 3, true, 0},
        {"insufficient action budget is preserved", 3, 4, false, 3},
        {"zero action cost succeeds without spending", 3, 0, true, 3},
        {"negative action cost is rejected", 3, -1, false, 3},
        {"empty action budget is preserved", 0, 1, false, 0},
        {"zero cost succeeds at empty budget", 0, 0, true, 0},
        {"maximum integer action cost", 3, std::numeric_limits<int>::max(), false, 3},
        {"minimum integer action cost", 3, std::numeric_limits<int>::min(), false, 3}
    };
    for (const ActionPointCase& test : actionPointCases)
    {
        Robot robot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, test.initialPoints};
        const bool success = robot.TryConsumeActionPoints(test.cost);
        expect(test.label, success == test.expectedSuccess &&
               robot.GetActionPoints() == test.expectedPoints);
    }

    Robot sequentialActions{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    const bool firstAction = sequentialActions.TryConsumeActionPoints(1);
    const bool excessiveAction = sequentialActions.TryConsumeActionPoints(3);
    expect("failed action preserves remaining budget",
           firstAction && !excessiveAction && sequentialActions.GetActionPoints() == 2);
    const bool finalAction = sequentialActions.TryConsumeActionPoints(2);
    const bool exhaustedAction = sequentialActions.TryConsumeActionPoints(1);
    expect("successive actions stop at zero",
           finalAction && !exhaustedAction && sequentialActions.GetActionPoints() == 0);

    Robot actingRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    Robot waitingRobot{1, Team::Blue, RobotType::Infantry, {27, 8}, 100, 50, 3};
    const bool actingSuccess = actingRobot.TryConsumeActionPoints(2);
    expect("only acting robot consumes action points",
           actingSuccess && actingRobot.GetActionPoints() == 1 && waitingRobot.GetActionPoints() == 3);

    Robot resetRobot{1, Team::Red, RobotType::Infantry, {1, 7}, 100, 50, 3};
    resetRobot.TryConsumeActionPoints(2);
    resetRobot.TakeDamage(20);
    resetRobot.TryConsumeAmmo();
    resetRobot.ResetActionPoints();
    expect("partial movement budget resets to configured capacity", resetRobot.GetActionPoints() == 3);
    expect("movement reset preserves HP and ammunition", resetRobot.GetHp() == 80 && resetRobot.GetAmmo() == 49);
    resetRobot.TryConsumeActionPoints(3);
    resetRobot.ResetActionPoints();
    expect("exhausted movement budget can be used after reset",
           resetRobot.TryConsumeActionPoints(3) && resetRobot.GetActionPoints() == 0);
    Robot customBudgetRobot{3, Team::Blue, RobotType::Infantry, {27, 8}, 100, 50, 5};
    customBudgetRobot.TryConsumeActionPoints(4);
    customBudgetRobot.ResetActionPoints();
    expect("reset uses each robot's own configured capacity", customBudgetRobot.GetActionPoints() == 5);
    customBudgetRobot.ResetActionPoints();
    expect("repeated resets preserve capacity and other robots",
           customBudgetRobot.GetActionPoints() == 5 && resetRobot.GetActionPoints() == 0);
    Robot zeroBudgetRobot{7, Team::Red, RobotType::Sentry, {1, 11}, 100, 50, 0};
    zeroBudgetRobot.ResetActionPoints();
    expect("zero configured movement budget remains zero after reset", zeroBudgetRobot.GetActionPoints() == 0);

    Robot woundedHeavy{1, Team::Red, RobotType::Heavy, {1, 7}, 400, 0, 3};
    woundedHeavy.TakeDamage(50);
    expect("heavy damage preserves maximum HP",
           woundedHeavy.GetHp() == 350 && woundedHeavy.GetMaxHp() == 400);

    Robot woundedInfantry{3, Team::Red, RobotType::Infantry, {1, 9}, 200, 0, 3};
    woundedInfantry.TakeDamage(30);
    expect("infantry damage preserves maximum HP",
           woundedInfantry.GetHp() == 170 && woundedInfantry.GetMaxHp() == 200);

    Robot defeatedSentry{7, Team::Blue, RobotType::Sentry, {27, 4}, 400, 300, 3};
    defeatedSentry.TakeDamage(500);
    expect("sentry death preserves maximum HP",
           defeatedSentry.GetHp() == 0 && defeatedSentry.GetMaxHp() == 400);

    std::cout << total - failed << '/' << total << " checks passed\n";
    return failed == 0 ? 0 : 1;
}
