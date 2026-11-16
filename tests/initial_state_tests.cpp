#define main rm_project_entry
#include "../src/main.cpp"
#undef main

#include <sstream>
#include <string>

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
    changedRobots.back().hp = 0;
    expectCheck("robot zero initial HP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedBases = bases;
    changedBases.front().hp = 0;
    expectCheck("base zero initial HP", [&] { return CheckInitialResources(robots, changedBases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back().ammo = 0;
    changedRobots.back().actionPoints = 0;
    expectCheck("zero robot ammo and AP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, true);
    changedRobots.back().ammo = -1;
    expectCheck("negative robot ammo", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    changedRobots = robots;
    changedRobots.back().actionPoints = -1;
    expectCheck("negative robot AP", [&] { return CheckInitialResources(changedRobots, bases, outposts); }, false);
    expectCheck("cross-team equal IDs", [&] { return CheckUniqueRobotIds(robots); }, true);
    changedRobots = robots;
    changedRobots.back().id = 2;
    expectCheck("later same-team duplicate ID", [&] { return CheckUniqueRobotIds(changedRobots); }, false);

    std::cout << total - failed << '/' << total << " checks passed\n";
    return failed == 0 ? 0 : 1;
}
