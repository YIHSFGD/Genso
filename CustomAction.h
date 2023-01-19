#pragma once
#include"cocos2d.h"
#include "SimpleAudioEngine.h"
#define ROTATE 24
constexpr float PI = 3.14159265358979323846264338327950;
constexpr float RAD = 6.28319;
using namespace cocos2d;
using namespace CocosDenshion;
static float GetRadFromVec2(Vec2 a) {
	/*float size = sqrtf((a.x + a.y) * (a.x + a.y));*/
	a.normalize();
	float r = asinf(a.y);
	r -= RAD;
	if (r < 0) {
		r += RAD;
	}
	return r;
}

static float RadToDeg(float rad) {
	return rad * (360 / PI);
}

enum Rotation {
	up,
	right,
	left,
	down
};

class MoveCircleBy : public ActionInterval {
public:
	static MoveCircleBy* create(float time, Vec2 center, float startRadian, float endRadian, bool reverse);
	static MoveCircleBy* create(float time, Vec2 center, float endRadian, bool reverse);
	static MoveCircleBy* create(float time, float startRadian, float radian, float radius, bool reverse) {
		MoveCircleBy* moveCircle = new MoveCircleBy();
		moveCircle->_startInit = true;
		moveCircle->radius = radius;
		moveCircle->initWithDuration(time, Vec2(0, 0), startRadian, radian, reverse);
		return moveCircle;
	}
	bool initWithDuration(float time, Vec2 center, float startRadian, float endRadian, bool reverse);

	virtual void update(float time) override;

private:
	Vec3 _previousPosition;
	Vec2 _newPosition;
	Vec2 _nowPosition;
	Vec2 _center;
	float radius = 0;
	float radian;
	float t;
	float _sr;
	bool firstFlag = true;
	bool _reverse = false;
	bool _startInit = false;
};

class SetUnvisible : public ActionInterval {
public:
	static SetUnvisible* create(float time);

	bool initWithDuration(float time) {
		return ActionInterval::initWithDuration(time);
	}

	virtual void update(float time) override {
		if (_target) {
			if (!(_target->isVisible())) {
				CCLOG("already unvisible");
			}
			if (time == 1) {
				_target->setVisible(0);
			}
		}
	}
};

class RunAway : public ActionInterval {
public:
	static RunAway* create(float time, Vec2 from, float length, float weight, bool reverse = 0) {
		RunAway* ra = new RunAway;
		ra->initWithDuration(time, from, length, weight, reverse);
		return ra;
	}

	bool initWithDuration(float time, Vec2 from, float length, float weight, bool reverse) {
		_from = from;
		_weight = weight;
		_length = length;
		_reverse = reverse;
		return ActionInterval::initWithDuration(0);
	}

	RunAway* create(RunAway* copy) {
		return RunAway::create(0, _from, _length, _weight);
	}

	virtual void update(float time) override {
		if (_target) {
			Vec2 nowPos = _target->getPosition();
			Vec2 len = nowPos - _from;
			Vec2 pos = (len).getNormalized();
			len.x = sqrtf(len.x * len.x);
			len.y = sqrtf(len.y * len.y);
			pos.x *= _length + _weight*len.getLengthSq();
			pos.y *= _length + _weight*len.getLengthSq();
			if (_reverse) {
				_target->setPosition(nowPos - pos);
			}
			else {
				_target->setPosition(nowPos + pos);
			}
		}
	}

	virtual ActionInterval* clone() const override{
		return RunAway::create(0, _from, _length, _weight, _reverse);
	}
private:
	Vec2 _from;
	float _length;
	float _weight;
	bool _reverse;
	
};

class SetVisible : public ActionInterval
{
public:
	static SetVisible* create(float time);
	bool initWithDuration(float time) {
		return ActionInterval::initWithDuration(time);
	}
	virtual void update(float time) override {
		if (_target) {
			if (time == 1) {
				_target->setVisible(1);
			}
		}
	}

};

// ���� : ������ �׼� �߰�
class ActionEnd : public ActionInterval
{
public:
	static ActionEnd* create(float time, bool *actionDone)
	{
		
		ActionEnd* AE = new ActionEnd;
		AE->initWithDuration(time, actionDone);
		return AE;
	}
	

	bool initWithDuration(float time, bool *actionDone) {
		_actionDone = actionDone;

		return ActionInterval::initWithDuration(time);
	}
	virtual void update(float time) override {
		*_actionDone = true;
	}

private:
	bool *_actionDone;
};


class Sound : public ActionInterval
{
public:
	static Sound* create(float time, char _filename[])
	{

		Sound* _sound = new Sound;
		_sound->initWithDuration(time, _filename);
		return _sound;
	}


	bool initWithDuration(float time, char _filename[]) {
		
		strcpy(filename, _filename);
		return ActionInterval::initWithDuration(time);
	}
	virtual void update(float time) override {
		SimpleAudioEngine::getInstance()->playEffect(filename,false);
	}
private:
	char filename[100];
};

class SetString : public ActionInterval
{
public:
	static SetString* create(float time, Label *ptr, std::string text, Color4B color = Color4B::RED)
	{

		SetString* _setstring = new SetString;
		_setstring->initWithDuration(time, ptr, text, color);
		return _setstring;
	}


	bool initWithDuration(float time, Label* ptr, std::string text, Color4B color) {
		_text = text;
		t = ptr;
		_color = color;
		return ActionInterval::initWithDuration(time);
	}
	virtual void update(float time) override {
		t->setString(_text);
		t->setTextColor(_color);
	}
private:
	Label* t;
	std::string _text;
	Color4B _color;
};

class ChangeTexture : public ActionInterval {
public:
	static ChangeTexture* create(float time, Texture2D* texture, Sprite* sp) {
		ChangeTexture* ct = new ChangeTexture;
		ct->initWithDuration(time, texture, sp);
		return ct;
	}
	bool initWithDuration(float time, Texture2D* texture, Sprite* sp) {
		tx = texture;
		sprite = sp;
		return ActionInterval::initWithDuration(time);
	}
	virtual void update(float time) override {
		if (_target) {
			sprite->setTexture(tx);
		}
	}
private:
	Texture2D* tx;
	Sprite* sprite;
};

class SetBlendFunc : public ActionInterval {
public:
	static SetBlendFunc* create(float time, BlendFunc blendFunc, Sprite* sp) {
		SetBlendFunc* ct = new SetBlendFunc;
		ct->initWithDuration(time, blendFunc, sp);
		return ct;
	}
	bool initWithDuration(float time, BlendFunc blendFunc, Sprite* sp) {
		bf = blendFunc;
		sprite = sp;
		return ActionInterval::initWithDuration(time);
	}
	virtual void update(float time) override {
		if (_target) {
			sprite->setBlendFunc(bf);
		}
	}
private:
	BlendFunc bf;
	Sprite* sprite;
};