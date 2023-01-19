#include"CustomAction.h"

MoveCircleBy* MoveCircleBy::create(float time, Vec2 center, float startRadian, float radian, bool reverse)
{
    MoveCircleBy* moveCircle = new MoveCircleBy();
    moveCircle->initWithDuration(time, center, startRadian, radian, reverse);
    return moveCircle;
}

MoveCircleBy* MoveCircleBy::create(float time, Vec2 center, float endRadian, bool reverse)
{
    MoveCircleBy* moveCircle = new MoveCircleBy();
    moveCircle->initWithDuration(time, center, INFINITE, endRadian, reverse);
    return moveCircle;
}

bool MoveCircleBy::initWithDuration(float time, Vec2 center, float startRadian, float rad, bool reverse)
{
    _center = Vec2(center.x, center.y);
    _sr = startRadian;
    radian = rad;
    t = time;
    _reverse = reverse;

    return ActionInterval::initWithDuration(time);
}

void MoveCircleBy::update(float time)
{
    if (_target) {
        _nowPosition = _target->getPosition();
        if (_startInit) {
            _center = Vec2(_nowPosition.x + sinf(_sr) * radius, _nowPosition.y + cosf(_sr) * radius);
            _sr -= CC_DEGREES_TO_RADIANS(180);
            if (_sr < 0) {
                _sr += CC_DEGREES_TO_RADIANS(360);
            }
            firstFlag = false;
            _startInit = false;
        }
        if (firstFlag) {
            radius = (_nowPosition - _center).getLength();
            firstFlag = false;
        }
        if (radian == INFINITE) {
            radian = acosf((_nowPosition.getNormalized()).dot(Vec2(0, 1)));
        }

        float rad;
        if (_reverse) {
            rad = _sr - (radian * time);
        }
        else {
            rad = _sr + (radian * time);
        }
        _newPosition = Vec2(
            sin(rad) * radius,
            cos(rad) * radius
        ) + _center;

        Vec2 speed = _nowPosition - _newPosition;
        _target->runAction(RotateTo::create(0, RadToDeg(GetRadFromVec2(speed))));

        _target->setPosition(_newPosition);
    }
}

SetUnvisible* SetUnvisible::create(float time) {
    SetUnvisible* setUnvisible = new SetUnvisible();
    setUnvisible->initWithDuration(time);
    return setUnvisible;
}

SetVisible* SetVisible::create(float time) {
    SetVisible* setvisible = new SetVisible();
    setvisible->initWithDuration(time);
    return setvisible;
}