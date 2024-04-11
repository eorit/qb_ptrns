
#include <conio.h>
#include <windows.h>
#include <random>

#include "MyTools.h"
#include "SBomber.h"
#include "Bomb.h"
#include "Ground.h"
#include "Tank.h"
#include "House.h"
#include "FileLogger.h"

using namespace std;
using namespace MyTools;

extern FileLogger logger;

// Returns random integer in closed range [low, high]
class UniformRandomInt 
{
    std::random_device _rd{};
    std::mt19937 _gen{ _rd() };
    std::uniform_int_distribution<int> _dist;

public:

    UniformRandomInt() {
        set(1, 10);
    }
    UniformRandomInt(int low, int high) {
        set(low, high);
    }

    // Set the distribution parameters low and high.
    void set(int low, int high) {
        std::uniform_int_distribution<int>::param_type param(low, high);
        _dist.param(param);
    }

    // Get random integer.
    int get() {
        return _dist(_gen);
    }
};

class SBomberImpl
{
public:

    SBomberImpl();
    ~SBomberImpl();

    bool GetExitFlag() const { return exitFlag; }

    void ProcessKBHit();
    void TimeStart();
    void TimeFinish();

    void DrawFrame();
    void MoveObjects();
    void CheckObjects();

private:

    void CheckPlaneAndLevelGUI();
    void CheckBombsAndGround();
    void __fastcall CheckDestoyableObjects(Bomb* pBomb);

    void __fastcall DeleteDynamicObj(DynamicObject* pBomb);
    void __fastcall DeleteStaticObj(GameObject* pObj);

    Ground* FindGround() const;
    Plane* FindPlane() const;
    LevelGUI* FindLevelGUI() const;
    std::vector<DestroyableGroundObject*> FindDestoyableGroundObjects() const;
    std::vector<Bomb*> FindAllBombs() const;

    void DropBomb();

    std::vector<DynamicObject*> vecDynamicObj;
    std::vector<GameObject*> vecStaticObj;

    bool exitFlag;

    uint64_t startTime, finishTime, passedTime;
    uint16_t bombsNumber, deltaTime, fps;
    int16_t score;
    UniformRandomInt* _pURInt;
};

SBomberImpl::SBomberImpl()
    : exitFlag(false),
    startTime(0),
    finishTime(0),
    deltaTime(0),
    passedTime(0),
    fps(0),
    bombsNumber(10),
    score(0)
{
    logger.WriteToLog(string(__FUNCTION__) + " was invoked");

    _pURInt = new UniformRandomInt;

    Plane* p = new Plane;
    p->SetDirection(1, 0.1);
    p->SetSpeed(4);
    p->SetPos(5, 10);
    vecDynamicObj.push_back(p);

    LevelGUI* pGUI = new LevelGUI;
    pGUI->SetParam(passedTime, fps, bombsNumber, score);
    const uint16_t maxX = GetMaxX();
    const uint16_t maxY = GetMaxY(); 
    const uint16_t offset = 3;
    const uint16_t width = maxX - 7;
    pGUI->SetPos(offset, offset);
    pGUI->SetWidth(width);
    pGUI->SetHeight(maxY - 4);
    pGUI->SetFinishX(offset + width - 4);
    vecStaticObj.push_back(pGUI);

    Ground* pGr = new Ground;
    const uint16_t groundY = maxY - 5;
    pGr->SetPos(offset + 1, groundY);
    pGr->SetWidth(width - 2);
    vecStaticObj.push_back(pGr);
    logger.WriteToLog("Ground width: " + std::to_string(pGr->GetWidth()));

    Tank* pTank = new Tank;
    pTank->SetWidth(13);
    pTank->SetPos(10, groundY - 1);
    vecStaticObj.push_back(pTank);

    //pTank = new Tank;
    //pTank->SetWidth(13);
    //pTank->SetPos(25, groundY - 1);
    //vecStaticObj.push_back(pTank);

    House * pHouse = new House;
    pHouse->SetWidth(13);
    pHouse->SetPos(95, groundY - 1);
    vecStaticObj.push_back(pHouse);

    /*
    Bomb* pBomb = new Bomb;
    pBomb->SetDirection(0.3, 1);
    pBomb->SetSpeed(2);
    pBomb->SetPos(51, 5);
    pBomb->SetSize(SMALL_CRATER_SIZE);
    vecDynamicObj.push_back(pBomb);
    */
}

SBomberImpl::~SBomberImpl()
{
    for (size_t i = 0; i < vecDynamicObj.size(); i++)
    {
        if (vecDynamicObj[i] != nullptr)
        {
            delete vecDynamicObj[i];
        }
    }

    for (size_t i = 0; i < vecStaticObj.size(); i++)
    {
        if (vecStaticObj[i] != nullptr)
        {
            delete vecStaticObj[i];
        }
    }

    delete _pURInt;
}

void SBomberImpl::MoveObjects()
{
    logger.WriteToLog(string(__FUNCTION__) + " was invoked");

    for (size_t i = 0; i < vecDynamicObj.size(); i++)
    {
        if (vecDynamicObj[i] != nullptr)
        {
            vecDynamicObj[i]->Move(deltaTime);
        }
    }
};

void SBomberImpl::CheckObjects()
{
    logger.WriteToLog(string(__FUNCTION__) + " was invoked");

    CheckPlaneAndLevelGUI();
    CheckBombsAndGround();
};

void SBomberImpl::CheckPlaneAndLevelGUI()
{
    if (FindPlane()->GetX() > FindLevelGUI()->GetFinishX())
    {
        exitFlag = true;
    }
}

void SBomberImpl::CheckBombsAndGround()
{
    vector<Bomb*> vecBombs = FindAllBombs();
    Ground* pGround = FindGround();
    const double y = pGround->GetY();
    for (size_t i = 0; i < vecBombs.size(); i++)
    {
        if (vecBombs[i]->GetY() >= y) // Пересечение бомбы с землей
        {
            pGround->AddCrater(vecBombs[i]->GetX());
            CheckDestoyableObjects(vecBombs[i]);
            DeleteDynamicObj(vecBombs[i]);
        }
    }

}

void SBomberImpl::CheckDestoyableObjects(Bomb * pBomb)
{
    vector<DestroyableGroundObject*> vecDestoyableObjects = FindDestoyableGroundObjects();
    const double size = pBomb->GetWidth();
    const double size_2 = size / 2;
    for (size_t i = 0; i < vecDestoyableObjects.size(); i++)
    {
        const double x1 = pBomb->GetX() - size_2;
        const double x2 = x1 + size;
        if (vecDestoyableObjects[i]->isInside(x1, x2))
        {
            score += vecDestoyableObjects[i]->GetScore();
            DeleteStaticObj(vecDestoyableObjects[i]);
        }
    }
}

void SBomberImpl::DeleteDynamicObj(DynamicObject* pObj)
{
    auto it = vecDynamicObj.begin();
    for (; it != vecDynamicObj.end(); it++)
    {
        if (*it == pObj)
        {
            vecDynamicObj.erase(it);
            break;
        }
    }
}

void SBomberImpl::DeleteStaticObj(GameObject* pObj)
{
    auto it = vecStaticObj.begin();
    for (; it != vecStaticObj.end(); it++)
    {
        if (*it == pObj)
        {
            vecStaticObj.erase(it);
            break;
        }
    }
}

vector<DestroyableGroundObject*> SBomberImpl::FindDestoyableGroundObjects() const
{
    vector<DestroyableGroundObject*> vec;
    Tank* pTank;
    House* pHouse;
    for (size_t i = 0; i < vecStaticObj.size(); i++)
    {
        pTank = dynamic_cast<Tank*>(vecStaticObj[i]);
        if (pTank != nullptr)
        {
            vec.push_back(pTank);
            continue;
        }

        pHouse = dynamic_cast<House*>(vecStaticObj[i]);
        if (pHouse != nullptr)
        {
            vec.push_back(pHouse);
            continue;
        }
    }

    return vec;
}

Ground* SBomberImpl::FindGround() const
{
    Ground* pGround;

    for (size_t i = 0; i < vecStaticObj.size(); i++)
    {
        pGround = dynamic_cast<Ground *>(vecStaticObj[i]);
        if (pGround != nullptr)
        {
            return pGround;
        }
    }

    return nullptr;
}

vector<Bomb*> SBomberImpl::FindAllBombs() const
{
    vector<Bomb*> vecBombs;

    for (size_t i = 0; i < vecDynamicObj.size(); i++)
    {
        Bomb* pBomb = dynamic_cast<Bomb*>(vecDynamicObj[i]);
        if (pBomb != nullptr)
        {
            vecBombs.push_back(pBomb);
        }
    }

    return vecBombs;
}

Plane* SBomberImpl::FindPlane() const
{
    for (size_t i = 0; i < vecDynamicObj.size(); i++)
    {
        Plane* p = dynamic_cast<Plane*>(vecDynamicObj[i]);
        if (p != nullptr)
        {
            return p;
        }
    }

    return nullptr;
}

LevelGUI* SBomberImpl::FindLevelGUI() const
{
    for (size_t i = 0; i < vecStaticObj.size(); i++)
    {
        LevelGUI* p = dynamic_cast<LevelGUI*>(vecStaticObj[i]);
        if (p != nullptr)
        {
            return p;
        }
    }

    return nullptr;
}

void SBomberImpl::ProcessKBHit()
{
    int c = _getch();

    if (c == 224)
    {
        c = _getch();
    }

    logger.WriteToLog(string(__FUNCTION__) + " was invoked. key = ", c);

    switch (c) {

    case 27: // esc
        exitFlag = true;
        break;

    case 72: // up
        FindPlane()->ChangePlaneY(-0.25);
        break;

    case 80: // down
        FindPlane()->ChangePlaneY(0.25);
        break;

    case 'b':
        DropBomb();
        break;

    case 'B':
        DropBomb();
        break;

    case 'D':   // clone Tank or House
    case 'd':
    {
        vector<DestroyableGroundObject*> vecDestoyableObjects = FindDestoyableGroundObjects();

        auto limitHi = vecDestoyableObjects.size();
        if (limitHi)
            limitHi -= 1;

        _pURInt->set(0, limitHi);
        auto indexDGO = _pURInt->get();
        logger.WriteToLog("Random index: " + std::to_string(indexDGO));

        bool genSuccess = false;
        for (int i = 0; i < 5; i++)
        {
            _pURInt->set(10, 100);
            auto position = _pURInt->get();
            logger.WriteToLog("Random position: " + std::to_string(position));

            bool goodPosition = true;

            for (size_t j = 0; j < vecDestoyableObjects.size(); j++)
            {
                if (vecDestoyableObjects[j]->isInside(position - 2, 
                    position + vecDestoyableObjects[indexDGO]->GetWidth() + 4))
                {
                    goodPosition = false;
                    break;
                }
            }
     
            if (goodPosition)
            {
                auto newObj = vecDestoyableObjects[indexDGO]->clone();
                newObj->SetPos(position, newObj->GetY());
                vecStaticObj.push_back(newObj);
                genSuccess = true;
                break;
            }
        }

        if (!genSuccess)
            logger.WriteToLog("Position generation 5 attenpts fault!");

        break;
    }

    default:
        break;
    }
}

void SBomberImpl::DrawFrame()
{
    logger.WriteToLog(string(__FUNCTION__) + " was invoked");

    for (size_t i = 0; i < vecDynamicObj.size(); i++)
    {
        if (vecDynamicObj[i] != nullptr)
        {
            vecDynamicObj[i]->Draw();
        }
    }

    for (size_t i = 0; i < vecStaticObj.size(); i++)
    {
        if (vecStaticObj[i] != nullptr)
        {
            vecStaticObj[i]->Draw();
        }
    }

    GotoXY(0, 0);
    fps++;

    FindLevelGUI()->SetParam(passedTime, fps, bombsNumber, score);
}

void SBomberImpl::TimeStart()
{
    logger.WriteToLog(string(__FUNCTION__) + " was invoked");
    startTime = GetTickCount64();
}

void SBomberImpl::TimeFinish()
{
    finishTime = GetTickCount64();
    deltaTime = uint16_t(finishTime - startTime);
    passedTime += deltaTime;

    logger.WriteToLog(string(__FUNCTION__) + " deltaTime = ", (int)deltaTime);
}

void SBomberImpl::DropBomb()
{
    if (bombsNumber > 0)
    {
        logger.WriteToLog(string(__FUNCTION__) + " was invoked");

        Plane* pPlane = FindPlane();
        double x = pPlane->GetX() + 4;
        double y = pPlane->GetY() + 2;

        Bomb* pBomb = new Bomb;
        pBomb->SetDirection(0.3, 1);
        pBomb->SetSpeed(2);
        pBomb->SetPos(x, y);
        pBomb->SetWidth(SMALL_CRATER_SIZE);

        vecDynamicObj.push_back(pBomb);
        bombsNumber--;
        score -= Bomb::BombCost;
    }
}

SBomber::SBomber() : _sBomber(new SBomberImpl())
{
}

SBomber::~SBomber()
{
    delete _sBomber;
}

bool SBomber::GetExitFlag() const
{
    return _sBomber->GetExitFlag();
}

void SBomber::ProcessKBHit()
{
    _sBomber->ProcessKBHit();
}

void SBomber::TimeStart()
{
    _sBomber->TimeStart();
}

void SBomber::TimeFinish()
{
    _sBomber->TimeFinish();
}

void SBomber::DrawFrame()
{
    _sBomber->DrawFrame();
}

void SBomber::MoveObjects()
{
    _sBomber->MoveObjects();
}

void SBomber::CheckObjects()
{
    _sBomber->CheckObjects();
}
