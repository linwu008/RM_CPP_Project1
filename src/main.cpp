#include <iostream>
#include <vector>

struct Position
{
    double x;
    double y;
};

enum class Team
{
    Red,
    Blue
};

struct Robot
{
    int id;
    Team team;
    Position position;
    int hp;
    int ammo;
    int actionPoints;
};

struct Base
{
    Team team;
    Position position;
    int hp;
};

std::vector<Base> CreateBases();

void PrintRobots(const std::vector<Robot>& robots);

void PrintBases(const std::vector<Base>& bases);

std::vector<Robot> CreateRobots();

int main() {
    std::cout << "Welcome RoboMaster Tactical Simulator" << std::endl;

    std::vector<Base> bases = CreateBases();
    PrintBases(bases);

    std::vector<Robot> robots = CreateRobots();
    std::cout << "Robots count: " << robots.size() << std::endl;

    PrintRobots(robots);

    return 0;
}

void PrintRobots(const std::vector<Robot>& robots)
{
    for (const Robot& robotcopy : robots)
    {
        if(robotcopy.team == Team::Red)
        {
            std::cout<< "Red";
        }
        else
        {
        std::cout<< "Blue";
        }
        std::cout << "robot " << robotcopy.id << std::endl;
        std::cout << "position: (" << robotcopy.position.x << ", " << robotcopy.position.y << ")" << std::endl;
        std::cout << "robot HP: " << robotcopy.hp << std::endl;
        std::cout << "robot Ammo: " << robotcopy.ammo << std::endl;
        std::cout << "robot Action Points: " << robotcopy.actionPoints << std::endl;
    }
}

std::vector<Robot> CreateRobots()
{
    Robot redRobot1{ 1, Team::Red, {1,7}, 100, 50, 3};
    Robot redRobot2{ 2, Team::Red, {1,9}, 100, 50, 3};
    Robot redRobot3{ 3, Team::Red, {1,11}, 100, 50, 3};
    Robot blueRobot1{ 1, Team::Blue, {27,8},100,50,3};
    Robot blueRobot2{ 2, Team::Blue, {27,6},100,50,3};
    Robot blueRobot3{ 3, Team::Blue, {27,4},100,50,3};

    std::vector<Robot> robots{redRobot1, blueRobot1, redRobot2, blueRobot2, redRobot3, blueRobot3};
    return robots;
}


std::vector<Base> CreateBases()
{
    Base redbase{ Team::Red , {2.00,7.50}, 1000 };
    Base bluebase{ Team::Blue , {26.00,7.50}, 1000 };

    std::vector<Base> bases{redbase, bluebase};
    return bases;
}

void PrintBases(const std::vector<Base>& bases)
{
    for (const Base& base : bases)
    {
        if(base.team == Team::Red)
        {
            std::cout << "Red";
        }
        else
        {
            std::cout << "Blue";
        }
        std::cout << " base" << std::endl;
        std::cout << "position: (" << base.position.x << ", " << base.position.y << ")" << std::endl;
        std::cout << "base HP: " << base.hp << std::endl;
    }
}