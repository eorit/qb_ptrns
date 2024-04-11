#include "Visitor.h"

#include "FileLoggerSingletone.h"

using namespace std;

void LogVisitor::visit(Bomb* e)
{
    LoggerSingletone::getInstance().WriteToLog("\tBomb speed = ", e->GetSpeed());
    LoggerSingletone::getInstance().WriteToLog("\tBomb xDirection = ", e->GetXDirection());
    LoggerSingletone::getInstance().WriteToLog("\tBomb yDirection = ", e->GetYDirection());
}

void LogVisitor::visit(Plane* e)
{
    LoggerSingletone::getInstance().WriteToLog("\tPlane speed = ", e->GetSpeed());
    LoggerSingletone::getInstance().WriteToLog("\tPlane xDirection = ", e->GetXDirection());
    LoggerSingletone::getInstance().WriteToLog("\tPlane yDirection = ", e->GetYDirection());
}
