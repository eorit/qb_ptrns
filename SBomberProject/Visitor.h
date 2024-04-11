#pragma once
#pragma once

#include "Bomb.h"
#include "Plane.h"

//class Bomb;
//class Plane;

class Visitor
{
public:
    virtual void visit(Bomb* e) = 0;
    virtual void visit(Plane* e) = 0;
};

class LogVisitor : public Visitor
{
public:
    void visit(Bomb* e) override;
    void visit(Plane* e) override;
};

