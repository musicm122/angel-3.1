#include "StdAfx.h"
#include "Player.h"
#include "RORGameManager.h"


Player::Player(Vector2 startingPosition)
{
	SetName("Player");
	SetPosition(startingPosition);
	_jumpTimeout = -1.0f;
	_onGround = false;
	_facingRight = true;
	_poweringUp = false;
	int zoom = ((RORGameManager*)theWorld.GetGameManager())->WorldZoom;
	SetSize((float)(zoom) * 0.5f, (float)(zoom));
	
	LoadSpriteFrames("Resources/Images/dude_01.png", GL_CLAMP, GL_NEAREST);
	SetSpriteFrame(0);
	
	SetDensity(2.0f);
	SetFriction(0.0f);
	SetRestitution(0.0f);
	SetShapeType(PhysicsActor::SHAPETYPE_BOX);
	SetFixedRotation(true);
	InitPhysics();
	
	b2PolygonShape sensorShape;
	b2FixtureDef sensorFixtureDef;
	sensorFixtureDef.isSensor = true;
	sensorFixtureDef.shape = &sensorShape;

	sensorShape.SetAsBox(0.5f, 0.5f, b2Vec2(0.0f, -(GetSize().Y * 0.5f)), 0.0f);
	_footSensor = GetBody()->CreateFixture(&sensorFixtureDef);
	_footSensor->SetUserData(this);

	sensorShape.SetAsBox(0.5f, 0.5f, b2Vec2(0.0f, (GetSize().Y * 0.5f)), 0.0f); 
	_headSensor = GetBody()->CreateFixture(&sensorFixtureDef);
	_headSensor->SetUserData(this);

	sensorShape.SetAsBox(0.5f, 0.5f, b2Vec2((GetSize().X * 0.5f), 0.0f), 0.0f); 
	_rightSensor = GetBody()->CreateFixture(&sensorFixtureDef);
	_rightSensor->SetUserData(this);

	sensorShape.SetAsBox(0.5f, 0.5f, b2Vec2(-(GetSize().X * 0.5f), 0.0f), 0.0f); 
	_leftSensor = GetBody()->CreateFixture(&sensorFixtureDef);
	_leftSensor->SetUserData(this);
	
	SetDrawSize((float)zoom, float(zoom));
	
	_jumpSound =    theSound.LoadSample("Resources/Sounds/Jump.wav", false);
	_powerUpSound = theSound.LoadSample("Resources/Sounds/PowerUp.wav", false);
}

void Player::SetUp()
{
	theSwitchboard.SubscribeTo(this, "CollisionStartWith" + GetName());
	theSwitchboard.SubscribeTo(this, "CollisionEndWith" + GetName());
	theSwitchboard.SubscribeTo(this, "Jump");
	theSwitchboard.SubscribeTo(this, "PowerUpDone");
}

void Player::ReceiveMessage(Message* message)
{
	String messageName = message->GetMessageName();
	if ((messageName == "CollisionStartWith" + GetName()) || (messageName == "CollisionEndWith" + GetName()))
	{
		// figure out what we're hitting and what part of us is hitting it
		TypedMessage<b2Contact*>* contactMessage = (TypedMessage<b2Contact*>*)message;
		b2Contact* contact = contactMessage->GetValue();
		PhysicsActor* other = NULL;
		b2Fixture* fixture = NULL;
		if (contact->GetFixtureA()->GetUserData() == this)
		{
			other = (PhysicsActor*)contact->GetFixtureB()->GetBody()->GetUserData();
			fixture = contact->GetFixtureA();
		}
		else
		{
			other = (PhysicsActor*)contact->GetFixtureA()->GetBody()->GetUserData();
			fixture = contact->GetFixtureB();
		}
		
		if (other == NULL)
		{
			// ain't no good can come of this
			return;
		}

		// process the collision
		if ( fixture == _footSensor )
		{
			if (messageName == "CollisionStartWith" + GetName())
			{
				ProcessStompOn(other);
			}
			else
			{
				ProcessUnStompFrom(other);
			}
		}
		
		/*if (fixture == _headSensor)
		{
			if (messageName == "CollisionStartWith" + GetName())
			{
				if (other->IsTagged("block"))
				{
					if (theSound.IsPlaying(_jumpPlaying))
					{
						theSound.StopSound(_jumpPlaying);
					}
					((SuperDudeBlock*)other)->Bonk();
				}
			}
		}*/
		
		if ( (messageName == "CollisionStartWith" + GetName()) && (other->IsTagged("powerup")) )
		{
			PowerMeUp();
			other->Destroy();
		}
	}
	else if (messageName == "Jump")
	{
		if (CanJump())
		{
			Jump();
		}
	}
	else if (messageName == "PowerUpDone")
	{
		LoadSpriteFrames("Resources/Images/dudette_01.png", GL_CLAMP, GL_NEAREST);
		theWorld.SetBackgroundColor(Color::FromInts(0, 191, 255));
		theWorld.ResumePhysics();
		_poweringUp = false;
	}
}

void Player::PowerMeUp()
{
	sysLog.Log("Power UP!");
	
	theWorld.PausePhysics();
	theWorld.SetBackgroundColor(Color::FromInts(239, 255, 57));
	theSwitchboard.DeferredBroadcast(new Message("PowerUpDone"), 2.0f);
	_poweringUp = true;
	
	theSound.PlaySound(_powerUpSound);
}

void Player::ProcessStompOn(PhysicsActor* other)
{
	if (other->IsTagged("ground"))
	{
		_onGround = true;
	}
}

void Player::ProcessUnStompFrom(PhysicsActor* other)
{
	if (other->IsTagged("ground"))
	{
		_onGround = false;
	}
}

void Player::Update(float dt)
{
	if (_jumpTimeout > 0.0f)
	{
		_jumpTimeout -= dt;
	}

	b2Vec2 currentVelocity = GetBody()->GetLinearVelocity();

	if (!_onGround)
	{
		SetSpriteFrame(2);
		_spriteFrameDelay = 0.0f;
	}
	else if (MathUtil::FuzzyEquals(currentVelocity.x, 0.0f))
	{
		SetSpriteFrame(0);
		_spriteFrameDelay = 0.0f;
	}
	else
	{
		if (!IsSpriteAnimPlaying())
		{
			PlaySpriteAnimation(0.1f, SAT_Loop, 0, 1);
		}
	}
	
	if ((currentVelocity.x < 0.0f) && _facingRight)
	{
		FlipLeft();
	}
	else if ((currentVelocity.x > 0.0f) && !_facingRight)
	{
		FlipRight();
	}

	if (_poweringUp)
	{
		// physics is off right now anyway
		return;
	}

	float maxVel = theTuning.GetFloat("DudeMaxSpeed");

	float xVector = 0.0f; 
	if (theController.IsConnected())
	{
		xVector = theController.GetLeftThumbVec2().X;
	}
	if (theInput.IsKeyDown(ANGEL_KEY_RIGHTARROW))
	{
		xVector = 1.0f;
	}
	if (theInput.IsKeyDown(ANGEL_KEY_LEFTARROW))
	{
		xVector = -1.0f;
	}
	if (theInput.IsKeyDown(ANGEL_KEY_LEFTARROW) && theInput.IsKeyDown(ANGEL_KEY_RIGHTARROW))
	{
		xVector = 0.0f;
	}

	float desiredVelocity = xVector * maxVel;	
	float velocityChange = desiredVelocity - currentVelocity.x;
	float impulse = GetBody()->GetMass() * velocityChange;
	ApplyLinearImpulse(Vector2(impulse, 0), Vector2());

	PhysicsActor::Update(dt);
}

void Player::FlipLeft()
{
	SetUVs(Vector2(1, 0), Vector2(0, 1));
	_facingRight = false;
}

void Player::FlipRight()
{
	SetUVs(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	_facingRight = true;
}

void Player::Render()
{
	PhysicsActor::Render();
}

bool Player::CanJump()
{
	if (_jumpTimeout > 0.0f)
	{
		return false;
	}
	if (!_onGround)
	{
		return false;
	}

	return true;
}

void Player::Jump()
{
	_jumpTimeout = theTuning.GetFloat("DudeJumpTimeout");
	_onGround = false;
	GetBody()->SetLinearVelocity(b2Vec2(0.0f, theTuning.GetFloat("DudeJumpHeight")));
	_jumpPlaying = theSound.PlaySound(_jumpSound, 0.25f);
}


