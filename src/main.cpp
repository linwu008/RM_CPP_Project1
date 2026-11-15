#include <iostream>
#include <vector>

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
    std::cout << "Welcome RoboMaster Tactical Simulator" << std::endl;

    Robot redRobot1{1, Team::Red, {1,7}, 100, 50, 3};
    Robot redRobot2{2, Team::Red, {1,9}, 100, 50, 3};
    Robot redRobot3{3, Team::Red, {1,11}, 100, 50, 3};
    Robot blueRobot1{1, Team::Blue, {27,8},100,50,3};
    Robot blueRobot2{2, Team::Blue, {27,6},100,50,3};
    Robot blueRobot3{3, Team::Blue, {27,4},100,50,3};

    std::vector<Robot> robots{redRobot1, blueRobot1, redRobot2, blueRobot2, redRobot3, blueRobot3};

    std::cout << "Robots count: " << robots.size() << std::endl;
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
    
    return 0;
}