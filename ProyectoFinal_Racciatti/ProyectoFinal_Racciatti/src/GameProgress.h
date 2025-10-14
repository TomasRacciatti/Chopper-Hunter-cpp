#pragma once

class GameProgress
{
public:
	GameProgress() = default;

	void Reset() { _score = 0; _timeElapsed = 0.0f; }
	void Update(float dt) { _timeElapsed += dt; }
	void AddScore(int value) { _score += value; }

	int   GetScore() const { return _score; }
	float GetTimeElapsed() const { return _timeElapsed; }

private:
	int   _score = 0;
	float _timeElapsed = 0.0f;
};
