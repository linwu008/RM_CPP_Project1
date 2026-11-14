#include <iostream>

struct Position
{
    int x;
    int y;
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


int main() {
    std::cout << "RoboMaster Tactical Simulator" << std::endl;

    Robot redRobot1{1, Team::Red, {1,7}, 100, 50, 3};

    std::cout << "Red start position: (" << redRobot1.position.x << ", " << redRobot1.position.y << ")" << std::endl;


    if(redRobot1.team == Team::Red)
    {
        std::cout<< "Team: Red" << std::endl;
    }
    else
    {
        std::cout<< "Team: Blue" << std::endl;
    }

    std::cout << "Red robot ID: " << redRobot1.id << std::endl;
    std::cout << "Red robot HP: " << redRobot1.hp << std::endl;
    std::cout << "Red robot Ammo: " << redRobot1.ammo << std::endl;
    std::cout << "Red robot Action Points: " << redRobot1.actionPoints << std::endl;


    return 0;
}