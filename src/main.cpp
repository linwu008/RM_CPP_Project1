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

enum class RobotType  //机器人类型
{
    Heavy,
    Infantry,
    Sentry
};

class Robot  //类：机器人各项数据
{
    public:
    Robot(int robotId, Team robotTeam, RobotType robotType, Position robotPosition,
          int initialHp, int initialAmmo, int initialActionPoints)
    /*定义构造机器人函数*/
        : id(robotId),
          team(robotTeam),
          position(robotPosition),
          hp(initialHp),
          MaxHp(initialHp),
          ammo(initialAmmo),
          actionPoints(initialActionPoints),
          MaxActionPoints(initialActionPoints),
          type(robotType)
    {
    }
    int GetHp() const //hp调用接口
    {
        return hp;
    }
    int GetMaxHp() const //血量上限读取接口
    {
        return MaxHp;
    }
    int GetAmmo() const //弹药读取接口
    {
        return ammo;
    }
    int GetActionPoints() const //行动点读取接口
    {
        return actionPoints;
    }

    void ResetActionPoints()  //重置行动点
    {
        actionPoints = MaxActionPoints;
    }

    RobotType GetType() const //类型读取接口
    {
        return type;
    }

    void TakeDamage(int damage)  //掉血规则
    {
        if( damage >= hp )
        {
            hp = 0;
            return ;
        }
        else if( damage <= 0 )
        {
            return;
        }
        else
        {
            hp = hp - damage;
        }
    }

    bool TryConsumeAmmo()  //弹药消耗规则
    {
        if( ammo <= 0 )
        {
            return false;
        }
        else
        {
            ammo--;
            return true;
        }
    }

    bool TryConsumeActionPoints(int cost)  //行动点规则
    {
        if( cost < 0 || cost > actionPoints )
        {
            return false;
        }
        else
        {
            actionPoints = actionPoints - cost;
            return true;
        }
    }

    int id;
    Team team;
    Position position;

    private:
    int hp;
    int MaxHp;
    int ammo;
    int actionPoints;
    int MaxActionPoints;
    RobotType type;
};

struct Base //基地数据
{
    Team team;
    Position position;
    int hp;
};

struct Outpost  //前哨站数据
{
    Team team;
    Position position;
    int hp;
};

std::vector<Robot> CreateRobots();  //机器人vector

void PrintRobots(const std::vector<Robot>& robots);  //输出机器人数据

std::vector<Base> CreateBases();  //基地vector

void PrintBases(const std::vector<Base>& bases);  //输出基地数据

std::vector<Outpost> CreateOutposts();  //前哨站vector

class Clock  //类：对局时钟：1 tick = 0.1s ，对局时长420s , 以10tick为一个回合
{
    public:
    int GetTick() const
    {
        return tick;
    }

    double GetGoingTime() const
    {
        return tick * 0.1;
    }

    double GetRemainingTime() const
    {
        return (4200 - tick) * 0.1;
    }

    bool IsGameOver() const
    {
        return tick >= 4200;
    }

    int GetTurn() const
    {
        if( tick >= 4200 )
        {
            return 420;
        }
        else
        {
            return tick / 10 + 1;  //开局从第一回合开始
        }
    }

    bool AdvanceTick()
    {
        if (tick < 4200 )  //仍在对局中
        {
            tick++;
            return true;
        }
        else
        {
            return false;
        }
    }

    private:
    int tick = 0;
    int turn = GetTurn();
};

void PrintOutposts(const std::vector<Outpost>& outposts);  //输出前哨站数据

bool IsInsideMap(const Position& position);  //判断是否在地图里面

bool CheckInitialPositions(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //判断初始位置

bool CheckUniqueRobotIds(const std::vector<Robot>& robots);  //检查ID重复

bool CheckInitialResources(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //检查初始数据合法性

bool CheckInitialOverlap(const std::vector<Robot>& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //检查是否重叠

bool AdvanceSimulation(Clock& clock, std::vector<Robot>& robots);

                                /*主函数*/

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

                                /*调用函数*/

void PrintRobots(const std::vector<Robot>& robots)  //输出机器人数据
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
        std::cout << "robot type: ";
        if (robotcopy.GetType() == RobotType::Heavy)
        {
            std::cout << "Heavy";
        }
        else if (robotcopy.GetType() == RobotType::Infantry)
        {
            std::cout << "Infantry";
        }
        else
        {
            std::cout << "Sentry";
        }
        std::cout << std::endl;
        std::cout << "position: (" << robotcopy.position.x << ", " << robotcopy.position.y << ")" << std::endl;
        std::cout << "robot HP: " << robotcopy.GetHp() << "/" << robotcopy.GetMaxHp() << std::endl;
        std::cout << "robot Ammo: " << robotcopy.GetAmmo() << std::endl;
        std::cout << "robot Action Points: " << robotcopy.GetActionPoints() << std::endl;
    }
}

std::vector<Robot> CreateRobots()
{
    /*初始重装、步兵、哨兵*/
    Robot redHeavy{1, Team::Red, RobotType::Heavy, {1, 7}, 400, 0, 3};
    Robot blueHeavy{1, Team::Blue, RobotType::Heavy, {27, 8}, 400, 0, 3};
    Robot redInfantry{3, Team::Red, RobotType::Infantry, {1, 9}, 200, 0, 3};
    Robot blueInfantry{3, Team::Blue, RobotType::Infantry, {27, 6}, 200, 0, 3};
    Robot redSentry{7, Team::Red, RobotType::Sentry, {1, 11}, 400, 300, 3};
    Robot blueSentry{7, Team::Blue, RobotType::Sentry, {27, 4}, 400, 300, 3};

    std::vector<Robot> robots{redHeavy, blueHeavy, redInfantry, blueInfantry, redSentry, blueSentry};
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
        if(!(robotcopy.GetHp() > 0 && robotcopy.GetAmmo() >= 0 && robotcopy.GetActionPoints() >= 0 ))
        {
            if(!(robotcopy.GetHp() > 0 ))
            {
                std::cerr << "The robot's hp should be larger than 0. " << std::endl;
                return false;
            }
            if(!(robotcopy.GetAmmo() >= 0 ))
            {
                std::cerr << "The ammo should be a natural number. " << std::endl;
                return false;
            }
            if(!(robotcopy.GetActionPoints() >= 0 ))
            {
                std::cerr << "The actionPoints should be a natural number. " << std::endl;
                return false;
            }
        }
        if (robotcopy.GetHp() != robotcopy.GetMaxHp())
        {
            std::cerr << (robotcopy.team == Team::Red ? "Red" : "Blue")
                      << " robot " << robotcopy.id << " must start at full HP: "
                      << robotcopy.GetHp() << "/" << robotcopy.GetMaxHp() << std::endl;
            return false;
        }
    }
    for (const Base& basecopy : bases)
    {
        if (basecopy.hp != 10000)
        {
            std::cerr << (basecopy.team == Team::Red ? "Red" : "Blue")
                      << " base must start with 10000 HP: " << basecopy.hp << std::endl;
            return false;
        }
    }

    for (const Outpost& outpost : outposts)
    {
        if (outpost.hp != 1000)
        {
            std::cerr << (outpost.team == Team::Red ? "Red" : "Blue")
                      << " outpost must start with 1000 HP: " << outpost.hp << std::endl;
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

bool AdvanceSimulation(Clock& clock, std::vector<Robot>& robots)
{
    int oldTurn = clock.GetTurn();
    if (clock.AdvanceTick() == false)
    {
        return false;
    }
    if (clock.IsGameOver() == false && clock.GetTurn() != oldTurn)
    {
        for (Robot& robotcopy : robots)
        {
            if (robotcopy.GetHp() > 0)
            {
                robotcopy.ResetActionPoints();
            }
        }


    }
    return true;
}
