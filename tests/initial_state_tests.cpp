#define main rm_project_entry
#include "../src/main.cpp"
#undef main

#include <sstream>
#include <string>
#include <limits>

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
    auto changedRobots = robots;
    auto changedBases = bases;
    auto changedOutposts = outposts;

    expectCheck("valid initial positions", [&] { return CheckInitialPositions(robots, bases, outposts); }, true);
    expectCheck("valid initial resources", [&] { return CheckInitialResources(robots, bases, outposts); }, true);
    expectCheck("valid initial occupancy", [&] { return CheckInitialOverlap(robots, bases, outposts); }, true);

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
    expectCheck("outpost positive HP", [&] { return CheckInitialResources(robots, bases, changedOutposts); }, true);

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
    changedRobots.back() = Robot{3, Team::Blue, {27, 4}, 0, 50, 3};
    expectCheck("robot zero initial HP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedBases = bases;
    changedBases.front().hp = 0;
    expectCheck("base zero initial HP", [&] { return CheckInitialResources(robots, changedBases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back() = Robot{3, Team::Blue, {27, 4}, 100, 0, 3};
    changedRobots.back().actionPoints = 0;
    expectCheck("zero robot ammo and AP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, true);
    changedRobots.back() = Robot{3, Team::Blue, {27, 4}, 100, -1, 0};
    expectCheck("negative robot ammo", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back().actionPoints = -1;
    expectCheck("negative robot AP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    expectCheck("cross-team equal IDs", [&] { return CheckUniqueRobotIds(robots); }, true);
    changedRobots = robots;
    changedRobots.back().id = 2;
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
        Robot robot{1, Team::Red, {1, 7}, test.initialHp, 50, 3};
        robot.TakeDamage(test.damage);
        expect(test.label, robot.GetHp() == test.expectedHp);
    }

    Robot repeatedlyHit{1, Team::Red, {1, 7}, 100, 50, 3};
    repeatedlyHit.TakeDamage(30);
    repeatedlyHit.TakeDamage(30);
    expect("separate calls accumulate damage", repeatedlyHit.GetHp() == 40);

    Robot firstRobot{1, Team::Red, {1, 7}, 100, 50, 3};
    Robot secondRobot{1, Team::Blue, {27, 8}, 100, 50, 3};
    firstRobot.TakeDamage(35);
    expect("damage changes only the selected robot",
           firstRobot.GetHp() == 65 && secondRobot.GetHp() == 100);

    Robot loadedRobot{1, Team::Red, {1, 7}, 100, 50, 3};
    const bool shotSucceeded = loadedRobot.TryConsumeAmmo();
    expect("single shot consumes one round", shotSucceeded && loadedRobot.GetAmmo() == 49);

    Robot lastRoundRobot{1, Team::Red, {1, 7}, 100, 1, 3};
    const bool lastShotSucceeded = lastRoundRobot.TryConsumeAmmo();
    expect("last round can be consumed", lastShotSucceeded && lastRoundRobot.GetAmmo() == 0);
    const bool extraShotSucceeded = lastRoundRobot.TryConsumeAmmo();
    expect("shot after last round fails", !extraShotSucceeded && lastRoundRobot.GetAmmo() == 0);

    Robot emptyRobot{1, Team::Red, {1, 7}, 100, 0, 3};
    const bool emptyShotSucceeded = emptyRobot.TryConsumeAmmo();
    expect("empty magazine preserves zero", !emptyShotSucceeded && emptyRobot.GetAmmo() == 0);

    Robot threeRoundRobot{1, Team::Red, {1, 7}, 100, 3, 3};
    const bool firstShot = threeRoundRobot.TryConsumeAmmo();
    const bool secondShot = threeRoundRobot.TryConsumeAmmo();
    const bool thirdShot = threeRoundRobot.TryConsumeAmmo();
    expect("three calls consume three rounds",
           firstShot && secondShot && thirdShot && threeRoundRobot.GetAmmo() == 0);
    const bool fourthShot = threeRoundRobot.TryConsumeAmmo();
    expect("fourth shot with three rounds fails", !fourthShot && threeRoundRobot.GetAmmo() == 0);

    Robot selectedRobot{1, Team::Red, {1, 7}, 100, 50, 3};
    Robot otherRobot{1, Team::Blue, {27, 8}, 100, 50, 3};
    const bool selectedShot = selectedRobot.TryConsumeAmmo();
    expect("only selected robot consumes ammo",
           selectedShot && selectedRobot.GetAmmo() == 49 && otherRobot.GetAmmo() == 50);

    std::cout << total - failed << '/' << total << " checks passed\n";
    return failed == 0 ? 0 : 1;
}
