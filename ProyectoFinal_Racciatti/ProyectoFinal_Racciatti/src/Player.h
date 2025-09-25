#pragma once
#include "Entity.h"
#include <memory>
#include "Weapon.h"
#include "ResourceManager.h"

class Player: public Entity
{
public:
	Player(sf::Vector2f startPos, std::string imageFilePath, ResourceManager& resourceManager);

	struct Input
	{
		bool left = false;
		bool right = false;
		bool crouch = false;
		bool fireHeld = false; // Manteniendo el disparo
		sf::Vector2f mouseWorld{ 0.f, 0.f };
	};

	void EquipWeapon(std::unique_ptr<Weapon> weapon);

	void SetInput(const Input& input) { _input = input; }

	void Update(float dt, const Level& lvl) override;
	void Draw(sf::RenderTarget& rt) const override;

private:
	// Movement
	float _baseSpeed = 320.f;
	float _crouchMul = 0.5f;
	bool  _crouched = false;

	// Stand and crouch -- Por ahora debug con rectangulos
	sf::Vector2f _standSize{ 34.f, 84.f };
	sf::Vector2f _crouchSize{ 34.f, 58.f }; // Esto probablemente vuele cuando usemos las anims

	// Weapon
	std::unique_ptr<Weapon> _weapon;

	// input
	Input _input;

	// Helpers
	void HandleCrouch(const Level& lvl);
	void MoveAndCollideX(float dx, const Level& lvl);
	void SnapToFloor(const Level& lvl);

	// Anims
	enum class Anim { Idle, Run, Crouch };

	sf::Texture* _tex = nullptr;
	sf::Sprite   _sprite;

	static constexpr int frameW = 23;
	static constexpr int frameH = 56;

	static constexpr int crouchH = 38;
	static constexpr int deathW = 29;

	static constexpr int _rowIdle = 0;
	static constexpr int _rowRun = 1;
	static constexpr int _rowCrouch = 2;
	static constexpr int _rowDeath = 3;

	// Run anim
	int   _runFrameCount = 8;
	int   _frame = 0;
	float _frameTime = 0.09f;
	float _animTimer = 0.f;

	Anim  _anim = Anim::Idle;

	void SelectAnim();
	void ApplyFrame(int col, int row);
	void UpdateRunAnim(float dt);
	void SyncSpriteToBody();
};

