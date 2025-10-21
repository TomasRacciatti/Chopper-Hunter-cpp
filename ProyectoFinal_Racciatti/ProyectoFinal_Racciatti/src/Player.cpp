#include "Player.h"
#include "Level.h"
#include "Utils.h"
#include <SFML/Window/Keyboard.hpp>

Player::Player(sf::Vector2f startPos, std::string imageFilePath, AudioSettings& audio, ResourceManager& resourceManager)
    : Entity(startPos, { 34.f, 84.f }, audio, /*hp*/10)
    , _tex(&resourceManager.GetTexture(imageFilePath, /*mipmap*/false, {}))
    , _sprite(*_tex)
{

    ApplyFrame(0, _rowIdle);

    _visualScale = 1.5f;
    _sprite.setOrigin(sf::Vector2f(frameW * 0.5f, static_cast<float>(frameH)));
    _sprite.setScale({ _visualScale, _visualScale });
    SyncSpriteToBody();
}

void Player::EquipWeapon(std::unique_ptr<Weapon> weapon)
{
    if (!weapon) return;
    _weapons.emplace_back(std::move(weapon));
    if (_activeWeapon < 0) _activeWeapon = 0; // Selecciona la pistola automaticamente
}

bool Player::SelectWeapon(int slot)
{
    if (slot < 0 || slot >= static_cast<int>(_weapons.size())) return false;
    _activeWeapon = slot;
    return true;
}

Weapon* Player::CurrentWeapon()
{
    if (_activeWeapon < 0 || _activeWeapon >= static_cast<int>(_weapons.size()))
        return nullptr;
    return _weapons[_activeWeapon].get();
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

    // Anim
    SelectAnim();

    if (_anim == Anim::Run)
        UpdateRunAnim(dt);

    SyncSpriteToBody();

    // Disparo
    if (Weapon* weapon = CurrentWeapon())
    {
        const auto weaponBody = _body.getGlobalBounds();
        sf::Vector2f muzzle{
            weaponBody.position.x + weaponBody.size.x * 0.5f,
            weaponBody.position.y + weaponBody.size.y * 0.40f
        };

        weapon->Update(dt, _input.fireHeld, muzzle, _input.mouseWorld, lvl);
    }

    for (int i = 0; i < static_cast<int>(_weapons.size()); i++)
    {
        if (i == _activeWeapon) continue;
        if (_weapons[i]) _weapons[i]->TickPassive(dt, lvl);
    }
}

void Player::Draw(sf::RenderTarget& rt) const
{
    // Dibujamos al jugador
    rt.draw(_sprite);

    // Dibujamos el arma
    if (_activeWeapon >= 0 && _activeWeapon < static_cast<int>(_weapons.size()))
    {
        if (auto* weapon = _weapons[_activeWeapon].get())
            weapon->Draw(rt);
    }
}

void Player::Heal(int amount)
{
    const int hp = GetLife();
    const int max = GetMaxLife();
    const int newHp = std::min(max, hp + amount);

    if (hp <= 0) return; // Por si se muere masomenos al mismo tiempo que no reviva

    SetLife(newHp);
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

// ===== Anim =====
void Player::SelectAnim()
{
    if (_input.crouch) 
    {
        if (_anim != Anim::Crouch) 
        {
            _anim = Anim::Crouch;
            ApplyFrame(0, _rowCrouch);
        }
        return;
    }

    const bool moving = _input.left || _input.right;
    if (moving) 
    {
        if (_anim != Anim::Run) 
        {
            _anim = Anim::Run;
            _frame = 0;
            _animTimer = 0.f;
            ApplyFrame(_frame, _rowRun);
        }
    }
    else 
    {
        if (_anim != Anim::Idle) 
        {
            _anim = Anim::Idle;
            ApplyFrame(0, _rowIdle);
        }
    }
}

void Player::ApplyFrame(int col, int row)
{
    const int left = col * frameW;
    const int top = row * frameH;

    _sprite.setTextureRect(sf::IntRect({ left, top }, { frameW, frameH }));

    _sprite.setOrigin({ frameW * 0.5f, static_cast<float>(frameH) });
}

void Player::UpdateRunAnim(float dt)
{
    _animTimer += dt;
    if (_animTimer >= _frameTime) 
    {
        _animTimer -= _frameTime;
        _frame = (_frame + 1) % _runFrameCount;
        ApplyFrame(_frame, _rowRun);
    }
}

void Player::SyncSpriteToBody() 
{
    const auto rect = _body.getGlobalBounds();
    _sprite.setPosition({ rect.position.x + rect.size.x * 0.5f, rect.position.y + rect.size.y });
}