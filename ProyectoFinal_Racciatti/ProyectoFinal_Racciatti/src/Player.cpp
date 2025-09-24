#include "Player.h"
#include "Level.h"
#include "Utils.h"
#include <SFML/Window/Keyboard.hpp>

Player::Player(sf::Vector2f startPos, std::string imageFilePath, ResourceManager& resourceManager)
    : Entity(startPos, { 32.f, 48.f }, 10)
{
    _body.setFillColor(sf::Color(255, 255, 255));

    sf::IntRect area({ 0,0 }, { 50,25 });
    sf::Texture& texture = resourceManager.GetTexture(imageFilePath, false, area);

}

void Player::EquipWeapon(std::unique_ptr<Weapon> w)
{
    _weapon = std::move(w);
}

void Player::Update(float dt, const Level& lvl)
{
    // Movimiento
    HandleCrouch(lvl);

    float speed = _baseSpeed * (_crouched ? _crouchMul : 1.0f);
    float horizontalV = 0.f;
    if (_input.left) horizontalV -= speed;
    if (_input.right) horizontalV += speed;

    MoveAndCollideX(horizontalV * dt, lvl);
    SnapToFloor(lvl);

    // Disparo
    if (_weapon)
    {
        const auto weaponBody = _body.getGlobalBounds();
        sf::Vector2f muzzle{
            weaponBody.position.x + weaponBody.size.x * 0.5f,
            weaponBody.position.y + weaponBody.size.y * 0.40f
        };

        _weapon->Update(dt, _input.fireHeld, muzzle, _input.mouseWorld, lvl);
    }
}

void Player::Draw(sf::RenderTarget& rt) const
{
    // Dibujamos al jugador
    Entity::Draw(rt);

    // Dibujamos el arma
    if (_weapon)
        _weapon->Draw(rt);
}

void Player::HandleCrouch(const Level& lvl)
{
    if (_input.crouch == _crouched) return;

    // Mantenemos los pies en el lugar cuando se agacha
    auto pos = _body.getPosition();
    auto size = _body.getSize();
    const sf::Vector2f newSize = _input.crouch ? _crouchSize : _standSize;

    float deltaH = newSize.y - size.y;
    pos.y -= deltaH; // Esto es lo que hace que los pies sigan en el piso

    _body.setSize(newSize);
    _body.setPosition(pos);

    _crouched = _input.crouch;
    SnapToFloor(lvl);
}

void Player::MoveAndCollideX(float dx, const Level& lvl)
{
    if (dx == 0.f) return;

    auto pos = _body.getPosition();
    pos.x += dx;

    sf::FloatRect playerColl = _body.getGlobalBounds();
    playerColl.position.x = pos.x;

    const auto& solids = lvl.Colliders();
    for (const auto& coll : solids)
    {
        if (!Utils::RectIntersects(playerColl, coll)) continue;

        if (dx > 0.f) {
            pos.x = coll.position.x - playerColl.size.x;
        }
        else if (dx < 0.f) {
            pos.x = coll.position.x + coll.size.x;
        }

        playerColl.position.x = pos.x;
    }

    _body.setPosition(pos);
}

void Player::SnapToFloor(const Level& lvl) // Como no tenemos salto:
{
    const auto& floor = lvl.FloorRect();
    auto pos = _body.getPosition();
    auto size = _body.getSize();

    pos.y = floor.position.y - size.y;
    _body.setPosition(pos);
}