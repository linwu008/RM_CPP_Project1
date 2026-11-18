#include <iostream>
#include <vector>
#include <cstddef>
#include <memory>
#include <cmath>

struct Position  //位置
{
    double x;
    double y;
};

struct Obstacle  //障碍
{
    Position topLeft;
    Position bottomRight;
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

class Robot  //父类：机器人各项数据
{
    public:
    Robot(int robotId, Team robotTeam, RobotType robotType, Position robotPosition,
          int initialHp, int initialAmmo, double initialActionPoints)
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
    double GetActionPoints() const //行动点读取接口，保留小数移动额度
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

    bool TryConsumeActionPoints(double cost)  //按实际移动距离消耗行动点
    {
        constexpr double epsilon = 1e-9;  //容纳小数计算产生的微小误差
        if (!std::isfinite(cost) || !std::isfinite(actionPoints) ||
            cost < 0 || actionPoints < 0 || cost > actionPoints + epsilon)
        {
            return false;
        }
        else
        {
            actionPoints = actionPoints - cost;
            if (actionPoints < 0)
            {
                actionPoints = 0;  //修正误差范围内的微小负值
            }
            return true;
        }
    }

    virtual int GetShotDamage() const  //单发基础伤害（17mm）
    {
        return 20;
    }

    virtual ~Robot() = default;  //虚析构：销毁具体机器人对象

    int id;
    Team team;
    Position position;

    private:
    int hp;
    int MaxHp;
    int ammo;
    double actionPoints;
    double MaxActionPoints;
    RobotType type;
};

using RobotList = std::vector<std::unique_ptr<Robot>>;

class HeavyRobot : public Robot  //子类：重装机器人
{

public:
    HeavyRobot(Team robotTeam, Position robotPosition)
    : Robot(1, robotTeam, RobotType::Heavy, robotPosition, 400, 0, 3.0)
    {
    }


    int GetShotDamage() const override  //重装42mm弹丸的单发基础伤害
    {
        return 200;
    }

};

class InfantryRobot : public Robot  //子类：步兵机器人
{
public:
    InfantryRobot(Team robotTeam, Position robotPosition)
    : Robot(3, robotTeam, RobotType::Infantry, robotPosition, 200, 0, 3.0)
    {
    }

    int GetShotDamage() const override  //步兵17mm弹丸的单发基础伤害
    {
        return 20;
    }
};

class SentryRobot : public Robot  //子类：哨兵机器人
{
public:
    SentryRobot(Team robotTeam, Position robotPosition)
    : Robot(7, robotTeam, RobotType::Sentry, robotPosition, 400, 300, 3.0)
    {
    }

    int GetShotDamage() const override  //哨兵17mm弹丸的单发基础伤害
    {
        return 20;
    }
};

Robot* FindRobot(RobotList& robots, Team team, int id);  //查询机器人对象



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

void PrintRobots(const RobotList& robots);  //输出机器人数据

std::vector<Base> CreateBases();  //基地vector

void PrintBases(const std::vector<Base>& bases);  //输出基地数据

std::vector<Outpost> CreateOutposts();  //前哨站vector

class Clock  //类：对局时钟：1 tick = 0.1s ，对局时长420s , 以10tick为一个回合
{
    public:
    int GetTick() const  //读取tick数
    {
        return tick;
    }

    double GetGoingTime() const  //读取已进行对局时间
    {
        return tick * 0.1;
    }

    double GetRemainingTime() const  //读取剩余对局时间
    {
        return (4200 - tick) * 0.1;
    }

    bool IsGameOver() const  //判断对局结束
    {
        return tick >= 4200;
    }

    int GetTurn() const  //输出回合数
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

    bool AdvanceTick()  //使tick数随时间增加
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

bool CheckInitialPositions(const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //判断初始位置

bool CheckUniqueRobotIds(const RobotList& robots);  //检查ID重复

bool CheckInitialResources(const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //检查初始数据合法性

bool CheckInitialOverlap(const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //检查是否重叠

bool AdvanceSimulation(Clock& clock, RobotList& robots);  //推进对局

RobotList CreateRobotObjects();  //创建机器人智能指针vector

double GetDistance(const Position& from, const Position& to);  //获取距离

bool TryMoveRobot(Robot& robot, const Position& target, const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts, const std::vector<Obstacle>& obstacles);  //尝试移动

bool IsRobotPositionOccupied(const Position& target, const Robot& movingRobot,const RobotList& robots);

bool IsFacilityPositionOccupied(const Position& target, const std::vector<Base>& bases, const std::vector<Outpost>& outposts);  //检查固定设施占位

bool IsInsideObstacle(const Position& point, const Obstacle& obstacle);  //检查是否在障碍物内

bool CrossesVerticalEdge(const Position& from, const Position& to, double edgeX, double topY, double bottomY);  //与一条垂直边相交

bool CrossesHorizontalEdge(const Position& from, const Position& to, double edgeY, double leftX, double rightX);  //与一条水平边相交

bool DoesPathIntersectObstacle(const Position& from, const Position& to, const Obstacle& obstacle);  //检查路径是否与障碍物相交

                                /*主函数*/

int main()
{
    std::cout << "Welcome RoboMaster Tactical Simulator!" << std::endl;

    std::vector<Base> bases = CreateBases();
    std::vector<Outpost> outposts = CreateOutposts();
    RobotList robots = CreateRobotObjects();

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

void PrintRobots(const RobotList& robots)  //输出机器人数据
{
    for (const auto& robotPointer : robots)
    {
        const Robot& robotcopy = *robotPointer;
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
        std::cout << "robot shot damage: " << robotcopy.GetShotDamage() << std::endl;
    }
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

bool CheckInitialPositions(const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (const auto& robotPointer : robots)
    {
        const Robot& robotcopy = *robotPointer;
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

bool CheckUniqueRobotIds(const RobotList& robots)
{
    for (std::size_t i = 0; i < robots.size(); ++i)
    {
        for (std::size_t j = i + 1; j < robots.size(); ++j)
        {
            if (robots[i]->team == robots[j]->team && robots[i]->id == robots[j]->id)
            {
                std::cerr << "repetition in" << robots[i]->id << "and" << robots[j]->id << std::endl;
                return false;
            }
        }

    }

    return true;
}

bool CheckInitialResources(const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (const auto& robotPointer : robots)
    {
        const Robot& robotcopy = *robotPointer;
        if(!(robotcopy.GetHp() > 0 && robotcopy.GetAmmo() >= 0 &&
             std::isfinite(robotcopy.GetActionPoints()) && robotcopy.GetActionPoints() >= 0 ))
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
            if(!std::isfinite(robotcopy.GetActionPoints()) || !(robotcopy.GetActionPoints() >= 0 ))
            {
                std::cerr << "The actionPoints should be finite and non-negative. " << std::endl;
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

bool CheckInitialOverlap(const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (std::size_t i = 0; i < robots.size(); ++i)
    {
        for (std::size_t j = i + 1; j < robots.size(); ++j)
        {
            if (robots[i]->position.x == robots[j]->position.x && robots[i]->position.y == robots[j]->position.y)
            {
                std::cerr << "Overlapping in" << robots[i]->id << "and" << robots[j]->id << std::endl;
                return false;
            }
        }

    }

    for (const auto& robotPointer : robots)
    {
        const Robot& robotcopy = *robotPointer;
        for ( const Base& basecopy : bases )
        {
            if(robotcopy.position.x == basecopy.position.x && robotcopy.position.y == basecopy.position.y )
            {
                std::cerr << "Overlapping in " << robotcopy.id << "and " << (basecopy.team == Team::Red ? "Red" : "Blue") << "base " << std::endl;
                return false;
            }
        }
    }

    for (const auto& robotPointer : robots)
    {
        const Robot& robotcopy = *robotPointer;
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

bool AdvanceSimulation(Clock& clock, RobotList& robots)
{
    int oldTurn = clock.GetTurn();
    if (clock.AdvanceTick() == false)
    {
        return false;
    }
    if (clock.IsGameOver() == false && clock.GetTurn() != oldTurn)
    {
        for (auto& robotPointer : robots)
        {
            Robot& robotcopy = *robotPointer;
            if (robotcopy.GetHp() > 0)
            {
                robotcopy.ResetActionPoints();
            }
        }
    }
    return true;
}

RobotList CreateRobotObjects()
{
    RobotList robots;

    robots.push_back(std::make_unique<HeavyRobot>(Team::Red, Position{1, 7}));
    robots.push_back(std::make_unique<HeavyRobot>(Team::Blue, Position{27, 8}));
    robots.push_back(std::make_unique<InfantryRobot>(Team::Red, Position{1, 9}));
    robots.push_back(std::make_unique<InfantryRobot>(Team::Blue, Position{27, 6}));
    robots.push_back(std::make_unique<SentryRobot>(Team::Red, Position{1, 11}));
    robots.push_back(std::make_unique<SentryRobot>(Team::Blue, Position{27, 4}));

    return robots;
}

Robot* FindRobot(RobotList& robots, Team team, int id)
{

    for (const auto& robotPointer : robots)
    {
        if (robotPointer->team == team && robotPointer->id == id)
        {
            return robotPointer.get();
        }
    }

    return nullptr;
}

double GetDistance(const Position& from, const Position& to)
{
    double dx = to.x - from.x;
    double dy = to.y - from.y;
    return std::sqrt(dx * dx + dy * dy);

}

bool TryMoveRobot(Robot& robot, const Position& destination, const RobotList& robots, const std::vector<Base>& bases, const std::vector<Outpost>& outposts, const std::vector<Obstacle>& obstacles)
{
    if (robot.GetHp() <= 0)
    {
        std::cerr << "Robot " << robot.id << " has been destroyed " << std::endl;
        return false;
    }
    if( !IsInsideMap(destination))
    {
        std::cerr << "(" << destination.x << ", " << destination.y << ") is outside the map." << std::endl;
        return false;
    }

    if (IsRobotPositionOccupied(destination, robot, robots))
    {
        std::cerr << "Destination (" << destination.x << ", " << destination.y
                  << ") is occupied by another robot." << std::endl;
        return false;
    }
    if (IsFacilityPositionOccupied(destination, bases, outposts))
    {
        std::cerr << "Destination (" << destination.x << ", " << destination.y
                  << ") is occupied by a base or outpost." << std::endl;
        return false;
    }
    for (const Obstacle& obstacle : obstacles)
    {
        if (DoesPathIntersectObstacle(robot.position, destination, obstacle))
        {
            std::cerr << "Destination (" << destination.x << ", " << destination.y << ") intersects with an obstacle." << std::endl;
            return false;
        }
    }

    double distance = GetDistance(robot.position, destination);

    if (robot.TryConsumeActionPoints(distance) == false)
    {
        std::cerr << "Robot " << robot.id << " does not have enough action points to move to (" << destination.x << ", " << destination.y << ")." << std::endl;
        return false;
    }

    robot.position = destination;

    return true;
}

bool IsRobotPositionOccupied(const Position& target, const Robot& movingRobot, const RobotList& robots)
{
    for (const auto& robotPointer : robots)
    {
        if (robotPointer.get() == &movingRobot)
        {
            continue;
        }
        if (robotPointer->position.x == target.x && robotPointer->position.y == target.y)
        {
            return true;
        }
    }

    return false;
}

bool IsFacilityPositionOccupied(const Position& target, const std::vector<Base>& bases, const std::vector<Outpost>& outposts)
{
    for (const Base& base : bases)
    {
        if (base.position.x == target.x && base.position.y == target.y)
        {
            return true;
        }
    }
    for (const Outpost& outpost : outposts)
    {
        if (outpost.position.x == target.x && outpost.position.y == target.y)
        {
            return true;
        }
    }
    return false;
}


bool IsInsideObstacle(const Position& point, const Obstacle& obstacle)
{
    if (point.x >= obstacle.topLeft.x && point.x <= obstacle.bottomRight.x && point.y >= obstacle.topLeft.y && point.y <= obstacle.bottomRight.y)
    {
        return true;
    }

    return false;
}

bool CrossesVerticalEdge(const Position& fromX, const Position& toX, double edgeX, double topY, double bottomY)
{
    double dx = toX.x - fromX.x;
    if (dx == 0)
    {
        return false;
    }
    double ratio = (edgeX - fromX.x) / dx;

    if (ratio < 0 || ratio > 1)
    {
        return false;
    }

    double intersectionY = fromX.y + ratio * (toX.y - fromX.y);

    return intersectionY >= topY && intersectionY <= bottomY;
}

bool CrossesHorizontalEdge(const Position& from, const Position& to, double edgeY,
                           double leftX, double rightX)
{
    double dy = to.y - from.y;
    if (dy == 0)
    {
        return false;
    }
    double ratio = (edgeY - from.y) / dy;
    if (ratio < 0 || ratio > 1)
    {
        return false;
    }
    double intersectionX = from.x + ratio * (to.x - from.x);
    return intersectionX >= leftX && intersectionX <= rightX;
}

bool DoesPathIntersectObstacle(const Position& from, const Position& to, const Obstacle& obstacle)
{
    if (IsInsideObstacle(from, obstacle) || IsInsideObstacle(to, obstacle))
    {
        return true;
    }

    if (CrossesVerticalEdge(from, to, obstacle.topLeft.x, obstacle.topLeft.y, obstacle.bottomRight.y) ||
        CrossesVerticalEdge(from, to, obstacle.bottomRight.x, obstacle.topLeft.y, obstacle.bottomRight.y) ||
        CrossesHorizontalEdge(from, to, obstacle.topLeft.y, obstacle.topLeft.x, obstacle.bottomRight.x) ||
        CrossesHorizontalEdge(from, to, obstacle.bottomRight.y, obstacle.topLeft.x, obstacle.bottomRight.x))
    {
        return true;
    }

    return false;
}
