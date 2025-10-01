#include "OptionsPanel.h"

OptionsPanel::OptionsPanel(ResourceManager& resourceManager, sf::RenderWindow& window, AudioSettings& audio)
    : _window(window)
    , _audio (audio)
    , text(resourceManager.GetFont("../fonts/MilitaryPoster.ttf"), "VOLUME", 48)
{
    // BG
    std::string backgroundPath = "../sprites/backgrounds/MainMenu_bg.png";
    sf::IntRect backgroundArea({ 0, 0 }, { 1280, 720 });
    sf::Texture& backgroundTexture = resourceManager.GetTexture(backgroundPath, false, backgroundArea);
    background = new sf::Sprite(backgroundTexture);


    // Shared button data
    std::string buttonPath = "../sprites/UI/button.png";
    sf::IntRect buttonArea({ 0, 0 }, { 366, 159 });
    sf::Texture& btnTex = resourceManager.GetTexture(buttonPath, false, buttonArea);

    // Font data
    const std::string fontPath = "../fonts/MilitaryPoster.ttf";
    sf::Font& font = resourceManager.GetFont(fontPath);
    sf::Color textColor = sf::Color(230, 230, 200);

    const auto winSize = _window.getSize();
    const sf::Vector2f center(winSize.x * 0.5f, winSize.y * 0.5f);

    // Title
    _title = new sf::Text(font, "OPTIONS", 100); // Analizar si dejar options o el titulo del juego
    _title->setFillColor(textColor);
    CenterText(_title);
    _title->setPosition({ center.x, center.y - _spacingY * 2.f });

    // Volume label + value
    _label = new sf::Text(font, "VOLUME", 48);
    _label->setFillColor(textColor);
    CenterText(_label);
    _label->setPosition({ center.x - 25.f, center.y - _spacingY * 0.8f });

    _value = new sf::Text(font, "100", 42);
    _value->setFillColor(textColor);
    CenterText(_value);
    _value->setPosition({ center.x + 75.f, center.y - _spacingY * 0.8f });

    // Back btn
    _backBtn = new sf::Sprite(btnTex);
    CenterSprite(_backBtn);
    _backBtn->setScale(sf::Vector2f(_buttonScale * 0.9f, _buttonScale * 0.9f));
    _backBtn->setPosition({ center.x, center.y + _spacingY * 2.5f });

    _backTxt = new sf::Text(font, "BACK", 48);
    _backTxt->setFillColor(textColor);
    CenterText(_backTxt);
    _backTxt->setPosition(_backBtn->getPosition());

    // Slider
    _track = new sf::RectangleShape();
    _fill = new sf::RectangleShape();
    _knob = new sf::CircleShape();

    // Track
    const float trackW = 420.f;
    const float trackH = 16.f;
    const sf::Vector2f trackPos(center.x - trackW * 0.5f, center.y - _spacingY * 0.1f);
    _track->setSize(sf::Vector2f(trackW, trackH));
    _track->setPosition(trackPos);
    _track->setFillColor(sf::Color(60, 60, 60));

    // Fill
    _fill->setSize(sf::Vector2f(trackW * (_audio.GetMasterVolume() / 100.f), trackH));
    _fill->setPosition(trackPos);
    _fill->setFillColor(textColor);

    // Knob
    _knob->setRadius(14.f);
    _knob->setOrigin(sf::Vector2f(14.f, 14.f));
    const float knobX = trackPos.x + trackW * (_audio.GetMasterVolume() / 100.f);
    const float knobY = trackPos.y + trackH * 0.5f;
    _knob->setPosition(sf::Vector2f(knobX, knobY));
    _knob->setFillColor(sf::Color(235, 235, 90));

    _trackRect = sf::FloatRect(_track->getPosition(), _track->getSize());
}

OptionsPanel::~OptionsPanel()
{
    delete _title;
    delete background;
    delete _label;
    delete _value;
    delete _backTxt;
    delete _backBtn;
    delete _track;
    delete _fill;
    delete _knob;
}

void OptionsPanel::SetVolume(float value)
{
    if (value < 0.f)   value = 0.f;
    if (value > 100.f) value = 100.f;

    _audio.SetMasterVolume(value);
    
    UpdateVisualSlider();
}

void OptionsPanel::HandleEvent(const sf::Event& ev)
{
    if (!_open) return;

    if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) 
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

            // Back
            if (_backBtn->getGlobalBounds().contains(worldPos)) 
            {
                _backRequested = true;
                _dragging = false;
                return;
            }

            // Track & knob
            const bool onTrack =
                worldPos.x >= _trackRect.position.x &&
                worldPos.x <= _trackRect.position.x + _trackRect.size.x &&
                worldPos.y >= _trackRect.position.y - 10.f &&
                worldPos.y <= _trackRect.position.y + _trackRect.size.y + 10.f;

            const bool onKnob = _knob->getGlobalBounds().contains(worldPos);

            if (onTrack || onKnob) {
                _dragging = true;
                SetVolumeFromX(worldPos.x);
            }
        }
    }
    else if (const auto* mouseMove = ev.getIf<sf::Event::MouseMoved>()) {
        if (_dragging) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);
            SetVolumeFromX(worldPos.x);
        }
    }
    else if (const auto* mouseRelease = ev.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseRelease->button == sf::Mouse::Button::Left) {
            _dragging = false;
        }
    }
    else if (ev.is<sf::Event::FocusLost>()) {
        _dragging = false;
    }
}

void OptionsPanel::Update(float dt)
{

}

void OptionsPanel::Draw(sf::RenderTarget& renderTarget) const
{
    if (!_open) return;

    renderTarget.draw(*_title);
    renderTarget.draw(*_label);
    renderTarget.draw(*_track);
    renderTarget.draw(*_fill);
    renderTarget.draw(*_knob);
    renderTarget.draw(*_backBtn);
    renderTarget.draw(*_backTxt);
    renderTarget.draw(*_value);
}

// Helpers
void OptionsPanel::CenterSprite(sf::Sprite* sprite) const
{
    if (!sprite) return;

    sprite->setOrigin({ sprite->getTexture().getSize().x * 0.5f,
                        sprite->getTexture().getSize().y * 0.5f });

}

void OptionsPanel::CenterText(sf::Text* text) const
{
    if (!text) return;

    sf::FloatRect btn = text->getLocalBounds();
    sf::Vector2f origin(btn.position.x + btn.size.x * 0.5f,
        btn.position.y + btn.size.y * 0.5f);

    text->setOrigin(origin);
}

void OptionsPanel::SetVolumeFromX(float worldX)
{
    float x0 = _trackRect.position.x;
    float x1 = _trackRect.position.x + _trackRect.size.x;
    if (worldX < x0) worldX = x0;
    if (worldX > x1) worldX = x1;

    const float t = (worldX - x0) / (x1 - x0);
    OptionsPanel::SetVolume(t * 100.f);
}

void OptionsPanel::UpdateVisualSlider()
{
    const float fillValue = _audio.GetMasterVolume() / 100.f;
    
    _fill->setSize({ _trackRect.size.x * fillValue, _trackRect.size.y });
    const float knobPos = _trackRect.position.x + _trackRect.size.x * fillValue;
    _knob->setPosition({ knobPos, _knob->getPosition().y });

    if (_value)
    {
        _value->setString(std::to_string(static_cast<int>(_audio.GetMasterVolume() + 0.5f)));
        CenterText(_value);
        _value->setPosition({
            _window.getSize().x * 0.5f + 75.f,
            _window.getSize().y * 0.5f - _spacingY * 0.8f });
    }
}
