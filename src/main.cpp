#include <iostream>
#include <vector>
#include <cstddef>

struct Position  //位置
{
    double x;
    double y;
};

enum class Team  //阵营
{
    Red,
    Blue
};

struct Robot  //机器人各项数据
{
    int id;
    Team team;
    Position position;
    int hp;
    int ammo;
    int actionPoints;
};

struct Base //基地各项数据
{
    Team team;
    Position position;
    int hp;
};

struct Outpost  //前哨站相关数据
{
    Team team;
    Position position;
    int hp;
};

std::vector<Robot> CreateRobots();

void PrintRobots(const std::vector<Robot>& robots);  //输出机器人数据

std::vector<Base> CreateBases();

void PrintBases(const std::vector<Base>& bases);  //输出基地数据

std::vector<Outpost> CreateOutposts();

void PrintOutposts(const std::vector<Outpost>& outposts);  //输出前哨站数据

bool IsInsideMap(const Position& position);

bool CheckInitialPositions(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);

bool CheckUniqueRobotIds(const std::vector<Robot>& robots);

bool CheckInitialResources(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);

bool CheckInitialOverlap(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);

int main()
{
    std::cout << "Welcome RoboMaster Tactical Simulator!" << std::endl;

    std::vector<Base> bases = CreateBases();
    std::vector<Outpost> outposts = CreateOutposts();
    std::vector<Robot> robots = CreateRobots();

    if (CheckInitialPositions(robots, bases, outposts) == false)
    {
    return 1;
    }

    if (CheckUniqueRobotIds(robots) == false)
    {
        return 1;
    }

    if (CheckInitialResources(robots, bases, outposts) == false)
    {
        return 1;
    }

    if (CheckInitialOverlap(robots, bases, outposts) == false)
    {
        return 1;
    }

    std::cout << "Initial positions valid." << std::endl;
    PrintBases(bases);
    PrintOutposts(outposts);

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
    Base redbase{ Team::Red , {2.00,7.50}, 10000 };
    Base bluebase{ Team::Blue , {26.00,7.50}, 10000 };

    std::vector<Base> bases{redbase, bluebase};
    return bases;
}


void PrintBases(const std::vector<Base>& bases)
{
    for (const Base& basecopy : bases)
    {
        if(basecopy.team == Team::Red)
        {
            std::cout << "Red";
        }
        else
        {
            std::cout << "Blue";
        }
        std::cout << " base" << std::endl;
        std::cout << "position: (" << basecopy.position.x << ", " << basecopy.position.y << ")" << std::endl;
        std::cout << "base HP: " << basecopy.hp << std::endl;
    }
}

std::vector<Outpost> CreateOutposts()
{
    Outpost redOutpost{ Team::Red, {11, 11.5}, 1000 };
    Outpost blueOutpost{ Team::Blue, {17, 3.5}, 1000 };

    std::vector<Outpost> outposts{redOutpost, blueOutpost};
    return outposts;
}

void PrintOutposts(const std::vector<Outpost>& outposts)
{
    for (const Outpost& outpost : outposts)
    {
        std::cout << (outpost.team == Team::Red ? "Red" : "Blue") << " outpost" << std::endl;
        std::cout << "position: (" << outpost.position.x << ", " << outpost.position.y << ")" << std::endl;
        std::cout << "outpost HP: " << outpost.hp << std::endl;
    }
}

bool IsInsideMap(const Position& position)
{
    return position.x >= 0 && position.x <= 28 && position.y >= 0 && position.y <= 15;
}

bool CheckInitialPositions(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (const Robot& robotcopy : robots)
    {
        if(IsInsideMap(robotcopy.position) == 0)
        {
            if (robotcopy.team == Team::Red)
            {
                std::cerr << "Red";
            }
            else
            {
                std::cerr << "Blue";
            }
            std::cerr << "Robot " << robotcopy.id << "outside map: ("<< robotcopy.position.x << ", "<< robotcopy.position.y << ")" << std::endl;
            return false;
        }
    }

    for (const Base& basecopy : bases)
    {
        if(IsInsideMap(basecopy.position) == 0)
        {
            if (basecopy.team == Team::Red)
            {
                std::cerr << "Red";
            }
            else
            {
                std::cerr << "Blue";
            }
            std::cerr << "Base outside map: ("<< basecopy.position.x << ", "<< basecopy.position.y << ")" << std::endl;
            return false;
        }
    }

    for (const Outpost& outpost : outposts)
    {
        if (IsInsideMap(outpost.position) == false)
        {
            std::cerr << (outpost.team == Team::Red ? "Red" : "Blue")
                      << " outpost outside map: (" << outpost.position.x
                      << ", " << outpost.position.y << ")" << std::endl;
            return false;
        }
    }

    return true;
}

bool CheckUniqueRobotIds(const std::vector<Robot>& robots)
{
    for (std::size_t i = 0; i < robots.size(); ++i)
    {
        for (std::size_t j = i + 1; j < robots.size(); ++j)
        {
            if (robots[i].team == robots[j].team && robots[i].id == robots[j].id)
            {
                std::cerr << "repetition in" << robots[i].id << "and" << robots[j].id << std::endl;
                return false;
            }
        }

    }

    return true;
}

bool CheckInitialResources(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (const Robot& robotcopy : robots)
    {
        if(!(robotcopy.hp > 0 && robotcopy.ammo >= 0 && robotcopy.actionPoints >= 0 ))
        {
            if(!(robotcopy.hp > 0 ))
            {
                std::cerr << "The robot's hp should be larger than 0. " << std::endl;
                return false;
            }
            if(!(robotcopy.ammo >= 0 ))
            {
                std::cerr << "The ammo should be a natural number. " << std::endl;
                return false;
            }
            if(!(robotcopy.actionPoints >= 0 ))
            {
                std::cerr << "The actionPoints should be a natural number. " << std::endl;
                return false;
            }
        }
    }
    for (const Base& basecopy : bases)
    {
        if(!(basecopy.hp > 0))
        {
            std::cerr << "The base's hp should be larger than 0. " << std::endl;
            return false;
        }
    }

    for (const Outpost& outpost : outposts)
    {
        if (outpost.hp <= 0)
        {
            std::cerr << (outpost.team == Team::Red ? "Red" : "Blue")
                      << " outpost HP must be larger than 0: " << outpost.hp << std::endl;
            return false;
        }
    }

    return true;
}

bool CheckInitialOverlap(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (std::size_t i = 0; i < robots.size(); ++i)
    {
        for (std::size_t j = i + 1; j < robots.size(); ++j)
        {
            if (robots[i].position.x == robots[j].position.x && robots[i].position.y == robots[j].position.y)
            {
                std::cerr << "Overlapping in" << robots[i].id << "and" << robots[j].id << std::endl;
                return false;
            }
        }

    }

    for (const Robot& robotcopy : robots )
    {
        for ( const Base& basecopy : bases )
        {
            if(robotcopy.position.x == basecopy.position.x && robotcopy.position.y == basecopy.position.y )
            {
                std::cerr << "Overlapping in " << robotcopy.id << "and " << (basecopy.team == Team::Red ? "Red" : "Blue") << "base " << std::endl;
                return false;
            }
        }
    }

    for (const Robot& robotcopy : robots)
    {
        for (const Outpost& outpost : outposts)
        {
            if (robotcopy.position.x == outpost.position.x && robotcopy.position.y == outpost.position.y)
            {
                std::cerr << (robotcopy.team == Team::Red ? "Red" : "Blue")
                          << " robot " << robotcopy.id << " overlaps "
                          << (outpost.team == Team::Red ? "Red" : "Blue")
                          << " outpost at (" << outpost.position.x
                          << ", " << outpost.position.y << ")" << std::endl;
                return false;
            }
        }
    }

    return true;
}
