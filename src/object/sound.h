
/* $Id: sound.h,v 1.25 2003/05/05 08:51:44 pstrand Exp $ */

#ifndef OBJECT_SOUND_H
#define OBJECT_SOUND_H


namespace reaper
{
namespace object
{
namespace sound
{



namespace snd = reaper::sound;

class Base
{

public:
	virtual ~Base();
};

class Ping
{
public:
	virtual void ping() = 0;
};

class Engine
{
public:
	virtual void engine_pitch(float p) = 0;
};

class Moving
{
public:
	virtual void set_location(const Matrix& m) = 0;
	virtual void set_velocity(const Vector& vel) = 0;
};

class Projectile : public Base, public Moving { };

class Ship : public Base, public Engine, public Moving
{
public:
	virtual void kill() = 0;
};

class Static : public Base, public Ping { };

Static* create_static(const std::string& name, const Point& pos);

Ship* create_ship(const std::string& name, const Matrix& mat, const Vector& vel);

Projectile* create_proj(const std::string& name, const Matrix& mat, const Vector& vel);


}
}
}

#endif
	

