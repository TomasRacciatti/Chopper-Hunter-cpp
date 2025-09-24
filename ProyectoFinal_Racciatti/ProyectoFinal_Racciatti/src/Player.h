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

	void EquipWeapon(std::unique_ptr<Weapon> w);

	void SetInput(const Input& input) { _input = input; }

	void Update(float dt, const Level& lvl) override;
	void Draw(sf::RenderTarget& rt) const override;

private:
	// Movement
	float _baseSpeed = 320.f;
	float _crouchMul = 0.5f;
	bool  _crouched = false;

	// Stand and crouch -- Por ahora debug con rectangulos
	sf::Vector2f _standSize{ 32.f, 48.f };
	sf::Vector2f _crouchSize{ 32.f, 32.f };

	// Weapon
	std::unique_ptr<Weapon> _weapon;

	// input
	Input _input;

	// Helpers
	void HandleCrouch(const Level& lvl);
	void MoveAndCollideX(float dx, const Level& lvl);
	void SnapToFloor(const Level& lvl);

	// Anim
	static constexpr int kFrameW = 25;
	static constexpr int kFrameH = 50;
	static constexpr int kCrouchH = 39;

	static constexpr int kIdleRow = 0;
	static constexpr int kIdleCol = 0;
	static constexpr int kCrouchRow = 2;
	static constexpr int kCrouchCol = 0;

};

