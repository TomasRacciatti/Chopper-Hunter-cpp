#include "GameplayUI.h"
#include "GameProgress.h"
#include "Entity.h"
#include <algorithm>
#include <cstdio>

GameplayUI::GameplayUI(ResourceManager& resources, const sf::Vector2u& winSize)
    : _resourceManager(resources)
    , _win(winSize)
    , _font(_resourceManager.GetFont("../fonts/MilitaryPoster.ttf"))
    , text (_font, "", 28)
{
    sf::Color textColor = sf::Color(sf::Color::White);
    sf::Color outlineColor = sf::Color(sf::Color::Black);
    float outlineThickness = 1.f;

    _timeText = new sf::Text(_font, "00:00", _characterSize);
    _timeText->setFillColor(textColor);
    _timeText->setOutlineColor(outlineColor);
    _timeText->setOutlineThickness(outlineThickness);

    _scoreText = new sf::Text(_font, "0", _characterSize);
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

    Layout();
}

GameplayUI::~GameplayUI()
{
    delete _timeText;
    delete _scoreText;
    delete _hpEmpty;
    delete _hpFull;
}

void GameplayUI::Update(float dt)
{
    // Time
    const float time = _progress ? _progress->GetTimeElapsed() : 0.f;
    _timeText->setString(FormatTime(time));

    // Score
    const int score = _progress ? _progress->GetScore() : 0;
    _scoreText->setString("SCORE: " + std::to_string(score));

    const sf::FloatRect bounds = _scoreText->getGlobalBounds();
    _scoreText->setPosition({ _win.x * 0.5f - bounds.size.x * 0.5f,_bottomY });

    UpdateHpBar();
}

void GameplayUI::Draw(sf::RenderTarget& rt) const
{
    // Texts
    rt.draw(*_timeText);
    rt.draw(*_scoreText);

    // hp
    rt.draw(*_hpEmpty);
    rt.draw(*_hpFull);
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

std::string GameplayUI::FormatTime(float seconds)
{
    if (seconds < 0.f) seconds = 0.f;
    int s = static_cast<int>(seconds + 0.5f);
    int m = s / 60;
    s = s % 60;

    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return std::string(buf);
}
