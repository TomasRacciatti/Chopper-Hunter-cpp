#include "GameplayUI.h"
#include "GameProgress.h"
#include "Player.h"
#include "Weapon.h"
#include "Pistol.h"
#include "Shotgun.h"
#include "Rpg.h"
#include "Utils.h"
#include <algorithm>
#include <cstdio>

GameplayUI::GameplayUI(ResourceManager& resources, const sf::Vector2u& winSize)
    : _resourceManager(resources)
    , _win(winSize)
    , text (_resourceManager.GetFont("../fonts/soldier.ttf"), "", 28)
{
    const std::string fontPath = "../fonts/soldier.ttf";
    sf::Font& font = _resourceManager.GetFont(fontPath);

    sf::Color textColor = sf::Color(sf::Color::White);
    sf::Color outlineColor = sf::Color(sf::Color::Black);
    float outlineThickness = 1.f;

    _timeText = new sf::Text(font, "00:00", _characterSize);
    _timeText->setFillColor(textColor);
    _timeText->setOutlineColor(outlineColor);
    _timeText->setOutlineThickness(outlineThickness);

    _scoreText = new sf::Text(font, "0", _characterSize);
    _scoreText->setFillColor(textColor);
    _scoreText->setOutlineColor(outlineColor);
    _scoreText->setOutlineThickness(outlineThickness);

    // hp
    sf::Texture& emptyTex = _resourceManager.GetTexture("../sprites/UI/EmptyHP.png", false, {});
    sf::Texture& fullTex = _resourceManager.GetTexture("../sprites/UI/FullHP.png", false, {});
    _hpEmpty = new sf::Sprite(emptyTex);
    _hpFull = new sf::Sprite(fullTex);
    _hpEmpty->setScale({ _hpScale, _hpScale });
    _hpFull->setScale({ _hpScale, _hpScale });

    // Ammo
    sf::Texture& pistolAmmo = _resourceManager.GetTexture("../sprites/player/PistolAmmo.png", false, {});
    sf::Texture& pistolEmpty = _resourceManager.GetTexture("../sprites/UI/PistolAmmoEmpty.png", false, {});
    
    _ammoIcon = new sf::Sprite(pistolAmmo);
    _ammoIconEmpty = new sf::Sprite(pistolEmpty);

    _ammoIcon->setScale({ _ammoScale, _ammoScale });
    _ammoIconEmpty->setScale({ _ammoScale, _ammoScale });

    _ammoIconKey = "Pistol";
    _ammoIconNativeSize = pistolAmmo.getSize();

    _ammoText = new sf::Text(font, "99", _characterSize);
    _ammoText->setFillColor(textColor);
    _ammoText->setOutlineColor(outlineColor);
    _ammoText->setOutlineThickness(outlineThickness);

    Layout();
}

GameplayUI::~GameplayUI()
{
    delete _timeText;
    delete _scoreText;
    delete _hpEmpty;
    delete _hpFull;
    delete _ammoIcon;
    delete _ammoIconEmpty;
    delete _ammoText;
}

void GameplayUI::Update(float dt)
{
    // Time
    const float time = _progress ? _progress->GetTimeElapsed() : 0.f;
    _timeText->setString(Utils::FormatTime(time));

    // Score
    const int score = _progress ? _progress->GetScore() : 0;
    _scoreText->setString("SCORE: " + std::to_string(score));

    const sf::FloatRect bounds = _scoreText->getGlobalBounds();
    _scoreText->setPosition({ _win.x * 0.5f - bounds.size.x * 0.5f,_bottomY });

    UpdateHpBar();
    UpdateAmmo();
}

void GameplayUI::Draw(sf::RenderTarget& rt) const
{
    // Texts
    rt.draw(*_timeText);
    rt.draw(*_scoreText);

    // hp
    rt.draw(*_hpEmpty);
    rt.draw(*_hpFull);

    // Ammo
    rt.draw(*_ammoIconEmpty);
    rt.draw(*_ammoIcon);
    rt.draw(*_ammoText);
}

void GameplayUI::Layout()
{
    _bottomY = _win.y - _margin - _characterSize;

    // Time a la izquierda
    _timeText->setPosition({ _margin, _bottomY });

    // Score al centro
    _scoreText->setPosition({ _win.x * 0.5f, _bottomY });

    // Hp a la derecha arriba
    const auto textureSize = _hpEmpty->getTexture().getSize();
    const sf::Vector2f barSize{ textureSize.x * _hpScale, textureSize.y * _hpScale };

    const float hpX = _win.x - _margin - barSize.x;
    const float hpY = (_win.y - barSize.y) * 0.5f;

    _hpEmpty->setPosition({ hpX, hpY });
    _hpFull->setPosition({ hpX, hpY });

    // Ammo
    const float gap = 6.f;

    const auto size = _ammoIconEmpty->getTexture().getSize();
    const sf::Vector2f iconSize{ size.x * _ammoScale, size.y * _ammoScale };

    const float iconX = _win.x - _margin - iconSize.x;
    const float iconY = _bottomY - (iconSize.y - _characterSize) * 0.5f;

    _ammoIconTopLeft = { iconX, iconY };
    _ammoIconNativeSize = size;

    _ammoIconEmpty->setPosition({ iconX, iconY });

    const sf::FloatRect t = _ammoText->getGlobalBounds();
    const float textX = iconX - gap - t.size.x;
    const float textY = _bottomY;
    _ammoText->setPosition({ textX, textY });
}

void GameplayUI::UpdateHpBar()
{
    if (!_player) return;

    const float hp = _player->GetLife();
    const float hpMax = _player->GetMaxLife();
    const float percentage = std::clamp(hp / hpMax, 0.f, 1.f);

    const auto texSize = _hpFull->getTexture().getSize();
    const int fullHp = static_cast<int>(texSize.y);
    const int visH = static_cast<int>(percentage * fullHp + 0.5f);

    const int top = fullHp - visH;
    _hpFull->setTextureRect(sf::IntRect({ 0, top }, { static_cast<int>(texSize.x), visH }));

    const sf::Vector2f base = _hpEmpty->getPosition();
    _hpFull->setPosition({ base.x, base.y + static_cast<float>(top) * _hpScale });
}

void GameplayUI::UpdateAmmo()
{
    if (!_player || !_ammoIcon || !_ammoText) return;

    Weapon* weapon = _player->CurrentWeapon();
    if (!weapon) return;

    const char* key = "Pistol";

    if (dynamic_cast<Shotgun*>(weapon))
        key = "Shotgun";
    else if (dynamic_cast<Rpg*>(weapon))
        key = "RPG";
    else
        key = "Pistol";

    if (_ammoIconKey != key)
    {
        std::string fullPath = std::string("../sprites/player/") + key + "Ammo.png";
        std::string emptyPath = std::string("../sprites/UI/") + key + "AmmoEmpty.png";

        sf::Texture& fullTex = _resourceManager.GetTexture(fullPath, false, {});
        sf::Texture& emptyTex = _resourceManager.GetTexture(emptyPath, false, {});

        _ammoIcon->setTexture(fullTex, true);
        _ammoIconEmpty->setTexture(emptyTex, true);

        _ammoIcon->setScale({ _ammoScale, _ammoScale });
        _ammoIconEmpty->setScale({ _ammoScale, _ammoScale });

        _ammoIconNativeSize = fullTex.getSize(); 
        _ammoIconKey = key;

        const auto size = _ammoIconEmpty->getTexture().getSize();
        const sf::Vector2f iconSize{ size.x * _ammoScale, size.y * _ammoScale };
        const float iconX = _win.x - _margin - iconSize.x;
        const float iconY = _bottomY - (iconSize.y - _characterSize) * 0.5f;
        _ammoIconTopLeft = { iconX, iconY };
        _ammoIconEmpty->setPosition(_ammoIconTopLeft);
    }

    const int texW = static_cast<int>(_ammoIconNativeSize.x);
    const int texH = static_cast<int>(_ammoIconNativeSize.y);
    
    float fill = std::clamp(weapon->GetCooldownNormalized(), 0.f, 1.f);
    if (weapon->GetAmmo() <= 0) fill = 0.f;

    int filledPx = static_cast<int>(std::round(fill * texH));
    filledPx = std::clamp(filledPx, 0, texH);

    sf::IntRect rect(sf::Vector2i{ 0, texH - filledPx }, sf::Vector2i{ texW, filledPx });
    _ammoIcon->setTextureRect(rect);

    _ammoIconEmpty->setPosition(_ammoIconTopLeft);

    const float fullTopY = _ammoIconTopLeft.y + (texH - filledPx) * _ammoScale;
    _ammoIcon->setPosition({ _ammoIconTopLeft.x, fullTopY });

    _ammoText->setString(std::to_string(weapon->GetAmmo()));
    const float gap = 6.f;
    const sf::FloatRect t = _ammoText->getGlobalBounds();
    _ammoText->setPosition({ _ammoIconTopLeft.x - gap - t.size.x, _bottomY });
}
