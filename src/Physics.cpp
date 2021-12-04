                         
#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto& aTransform = a->getComponent<CTransform>();
    auto& aBoundingBox = a->getComponent<CBoundingBox>();

    auto& bTransform = b->getComponent<CTransform>();
    auto& bBoundingBox = b->getComponent<CBoundingBox>();

    float x, y;

    if (aTransform.pos.x > bTransform.pos.x)
    {
        x = bTransform.pos.x - aTransform.pos.x + aBoundingBox.halfSize.x + bBoundingBox.halfSize.x;
    }
    else
    {
        x = aTransform.pos.x - bTransform.pos.x + aBoundingBox.halfSize.x + bBoundingBox.halfSize.x;
    }
    if (aTransform.pos.y > bTransform.pos.y)
    {
        y = bTransform.pos.y - aTransform.pos.y + aBoundingBox.halfSize.y + bBoundingBox.halfSize.y;
    }
    else
    {
        y = aTransform.pos.y - bTransform.pos.y + aBoundingBox.halfSize.y + bBoundingBox.halfSize.y;
    }

    return Vec2(x, y);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto& aTransform = a->getComponent<CTransform>();
    auto& aBoundingBox = a->getComponent<CBoundingBox>();

    auto& bTransform = b->getComponent<CTransform>();
    auto& bBoundingBox = b->getComponent<CBoundingBox>();

    float x, y;

    if (aTransform.prevPos.x > bTransform.prevPos.x)
    {
        x = aTransform.prevPos.x - bTransform.prevPos.x + aBoundingBox.halfSize.x + bBoundingBox.halfSize.x;
    }
    else
    {
        x = bTransform.prevPos.x - aTransform.prevPos.x + aBoundingBox.halfSize.x + bBoundingBox.halfSize.x;
    }
    if (aTransform.prevPos.y > bTransform.prevPos.y)
    {
        y = aTransform.prevPos.y - bTransform.prevPos.y + aBoundingBox.halfSize.y + bBoundingBox.halfSize.y;
    }
    else
    {
        y = bTransform.prevPos.y - aTransform.prevPos.y + aBoundingBox.halfSize.y + bBoundingBox.halfSize.y;
    }

    return Vec2(x, y);
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
    if (!e->hasComponent<CAnimation>()) { return false; }

    auto& halfsize = e->getComponent<CAnimation>().animation.getSize() / 2;

    Vec2 delta = (e->getComponent<CTransform>().pos - pos).abs();

    return (delta.x <= halfsize.x) && (delta.y <= halfsize.y);
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
    Vec2 r = (b - a);
    Vec2 s = (d - c);
    float rxs = r.crossProduct(s);
    Vec2 cma = c - a;
    float t = (cma.crossProduct(s)) / (r.crossProduct(s));
    float u = (cma.crossProduct(r)) / (r.crossProduct(s));
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        return { true,Vec2(a.x + t * r.x,a.y + t * r.y) };
    else
        return { false, Vec2(0,0) };
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
    Vec2 A(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y);
    Vec2 B(e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y);
    Vec2 C(e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y);
    Vec2 D(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y);
    auto m = Physics::LineIntersect(a, b, A, B);
    auto n = Physics::LineIntersect(a, b, A, D);
    auto o = Physics::LineIntersect(a, b, B, C);
    auto p = Physics::LineIntersect(a, b, C, D);
    if (m.result || n.result || o.result || p.result)
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

