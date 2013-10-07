#pragma once

class Player : public PhysicsActor
{
public:
	Player(Vector2 startingPoint);
	void SetUp();
	
	virtual void Update(float dt);
	virtual void Render();
	virtual void ReceiveMessage(Message* message);
private: 
	bool CanJump();
	void Jump();
	void PowerMeUp();
	void ProcessStompOn(PhysicsActor* other);
	void ProcessUnStompFrom(PhysicsActor* other);

	void FlipLeft();
	void FlipRight();

	float _jumpTimeout;
	bool _onGround;
	bool _facingRight;
	b2Fixture* _footSensor;
	b2Fixture* _rightSensor;
	b2Fixture* _leftSensor;
	b2Fixture* _headSensor;
	
	bool _poweringUp;
	
	AngelSampleHandle _jumpSound;
	AngelSoundHandle _jumpPlaying;
	AngelSampleHandle _powerUpSound;
};

