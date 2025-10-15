#include "OptionsPanel.h"


static constexpr const char* btnPath = "../sprites/UI/button.png";
static constexpr const char* fontPath = "../fonts/MilitaryPoster.ttf";

OptionsPanel::OptionsPanel(ResourceManager& resourceManager, sf::RenderWindow& window, AudioSettings& audio)
    : _window(window)
    , _audio (audio)
    , text(resourceManager.GetFont("../fonts/MilitaryPoster.ttf"), "OPTIONS", 48)
{
    // BG
    std::string backgroundPath = "../sprites/backgrounds/MainMenu_bg.png";
    sf::IntRect backgroundArea({ 0, 0 }, { 1280, 720 });
    sf::Texture& backgroundTexture = resourceManager.GetTexture(backgroundPath, false, backgroundArea);
    background = new sf::Sprite(backgroundTexture); 

    const auto winSize = _window.getSize();
    const sf::Vector2f center(winSize.x * 0.5f, winSize.y * 0.5f);

    // Title
    
    sf::Font& font = resourceManager.GetFont(fontPath);
    sf::Color textColor = sf::Color(230, 230, 200);
    _title = new sf::Text(font, "OPTIONS", 100); // Analizar si dejar options o el titulo del juego
    _title->setFillColor(textColor);
    CenterText(_title);
    _title->setPosition({ center.x, center.y - _spacingY * 2.f });

    // Back btn
    sf::IntRect buttonArea({ 0, 0 }, { 366, 159 });
    sf::Texture& btnTex = resourceManager.GetTexture(btnPath, false, buttonArea);
    _backBtn = new sf::Sprite(btnTex);
    CenterSprite(_backBtn);
    _backBtn->setScale(sf::Vector2f(_buttonScale * 0.9f, _buttonScale * 0.9f));
    _backBtn->setPosition({ center.x, center.y + _spacingY * 2.5f });

    _backTxt = new sf::Text(font, "BACK", 48);
    _backTxt->setFillColor(textColor);
    CenterText(_backTxt);
    _backTxt->setPosition(_backBtn->getPosition());

    // Slider
    CreateSliders(resourceManager);
    LayoutSliders();
}

OptionsPanel::~OptionsPanel()
{
    for (auto& s : _sliders)
    {
        delete s._label;
        delete s._value;
        delete s._track;
        delete s._fill;
        delete s._knob;
    }
    _sliders.clear();

    delete _title;
    delete background;
    delete _backTxt;
    delete _backBtn;
}

void OptionsPanel::HandleEvent(const sf::Event& ev)
{
    if (!_open) return;

    if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>()) 
    {
        if (mouse->button == sf::Mouse::Button::Left) 
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

            // Back
            if (_backBtn->getGlobalBounds().contains(worldPos)) 
            {
                _backRequested = true;
                return;
            }

            // Track & knob
            for (Slider& slider : _sliders)
            {
                const bool onTrack =
                    worldPos.x >= slider._trackRect.position.x &&
                    worldPos.x <= slider._trackRect.position.x + slider._trackRect.size.x &&
                    worldPos.y >= slider._trackRect.position.y - 10.f &&
                    worldPos.y <= slider._trackRect.position.y + slider._trackRect.size.y + 10.f;

                const bool onKnob = slider._knob->getGlobalBounds().contains(worldPos);

                if (onTrack || onKnob) {
                    slider._dragging = true;
                    SetSliderFromX(slider, worldPos.x);
                }
            }
        }
    }
    else if (const auto* mouseMove = ev.getIf<sf::Event::MouseMoved>()) 
    {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
        sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);
        for (Slider& slider : _sliders)
            if(slider._dragging)
                SetSliderFromX(slider, worldPos.x);
    }
    else if (const auto* mouseRelease = ev.getIf<sf::Event::MouseButtonReleased>()) 
    {
        if (mouseRelease->button == sf::Mouse::Button::Left) 
        {
            for (Slider& slider : _sliders)
                slider._dragging = false;
        }
    }
    else if (ev.is<sf::Event::FocusLost>()) 
    {
        for (Slider& slider : _sliders)
            slider._dragging = false;
    }
}

void OptionsPanel::Update(float dt)
{

}

void OptionsPanel::Draw(sf::RenderTarget& renderTarget) const
{
    if (!_open) return;

    renderTarget.draw(*_title);

    for (const Slider& slider : _sliders)
    {
        if (slider._label) renderTarget.draw(*slider._label);
        if (slider._track) renderTarget.draw(*slider._track);
        if (slider._fill)  renderTarget.draw(*slider._fill);
        if (slider._knob)  renderTarget.draw(*slider._knob);
        if (slider._value) renderTarget.draw(*slider._value);
    }

    renderTarget.draw(*_backBtn);
    renderTarget.draw(*_backTxt);
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

void OptionsPanel::CreateSliders(ResourceManager& rm)
{
    const auto& font = rm.GetFont(fontPath);
    const sf::Color textColor(230, 230, 200);

    auto makeText = [&](const std::string& s, unsigned size)->sf::Text*
        {
            auto* t = new sf::Text(font, s, size);
            t->setFillColor(textColor);
            return t;
        };
    auto makeTrack = [&]()->sf::RectangleShape*
        {
            auto* r = new sf::RectangleShape();
            r->setFillColor(sf::Color(60, 60, 60));
            r->setSize({ 460.f, 16.f });
            return r;
        };
    auto makeFill = [&]()->sf::RectangleShape*
        {
            auto* r = new sf::RectangleShape();
            r->setFillColor(textColor);
            return r;
        };
    auto makeKnob = [&]()->sf::CircleShape*
        {
            auto* c = new sf::CircleShape();
            c->setRadius(14.f);
            c->setOrigin({ 14.f, 14.f });
            c->setFillColor(sf::Color(235, 235, 90));
            return c;
        };

    _sliders.clear();
    _sliders.reserve(3);

    // Master
    {
        Slider s;
        s._label = makeText("MASTER", 48);
        s._value = makeText("100", 42);
        s._track = makeTrack();
        s._fill = makeFill();
        s._knob = makeKnob();
        s.getter = [&]() { return _audio.GetMasterVolume(); };
        s.setter = [&](float v) { _audio.SetMasterVolume(v); };
        _sliders.push_back(s);
    }

    // Music
    {
        Slider s;
        s._label = makeText("MUSIC", 48);
        s._value = makeText("100", 42);
        s._track = makeTrack();
        s._fill = makeFill();
        s._knob = makeKnob();
        s.getter = [&]() { return _audio.GetMusicSlider(); };
        s.setter = [&](float v) { _audio.SetMusicVolume(v); };
        _sliders.push_back(s);
    }

    // Sfx
    {
        Slider s;
        s._label = makeText("SFX", 48);
        s._value = makeText("100", 42);
        s._track = makeTrack();
        s._fill = makeFill();
        s._knob = makeKnob();
        s.getter = [&]() { return _audio.GetSfxSlider(); };
        s.setter = [&](float v) { _audio.SetSfxVolume(v); };
        _sliders.push_back(s);
    }
}

void OptionsPanel::SetSliderFromX(Slider& slider, float worldX)
{
    float x0 = slider._trackRect.position.x;
    float x1 = slider._trackRect.position.x + slider._trackRect.size.x;
    if (worldX < x0) worldX = x0;
    if (worldX > x1) worldX = x1;

    const float t = (x1 > x0) ? ((worldX - x0) / (x1 - x0)) : 0.f;
    const float value = t * 100.f;

    if (slider.setter) slider.setter(value);
    UpdateSliderVisual(slider);
}

void OptionsPanel::LayoutSliders()
{
    const auto win = _window.getSize();
    const sf::Vector2f center(win.x * 0.5f, win.y * 0.5f);

    const float firstRowY = center.y - _spacingY * 0.6f;
    const float rowGap = _spacingY * 0.95f;

    const float trackW = 460.f;
    const float trackH = 16.f;

    for (int i = 0; i < static_cast<int>(_sliders.size()); ++i)
    {
        auto& s = _sliders[i];
        const float rowY = firstRowY + i * rowGap;

        // Label y valor
        if (s._label) { CenterText(s._label); s._label->setPosition({ center.x - 90.f, rowY }); }
        if (s._value) { CenterText(s._value); s._value->setPosition({ center.x + 110.f, rowY }); }

        // Track y knobb
        const sf::Vector2f trackPos(center.x - trackW * 0.5f, rowY + _spacingY * 0.55f);
        if (s._track) 
        {
            s._track->setSize({ trackW, trackH });
            s._track->setPosition(trackPos);
        }
        if (s._fill) s._fill->setPosition(trackPos);

        s._trackRect = sf::FloatRect(trackPos, { trackW, trackH });

        UpdateSliderVisual(s);
    }
}

void OptionsPanel::UpdateSliderVisual(Slider& slider)
{
    const float raw = slider.getter ? slider.getter() : 0.f;
    const float t = std::clamp(raw / 100.f, 0.f, 1.f);

    const float x0 = slider._trackRect.position.x;
    const float w = slider._trackRect.size.x;

    if (slider._fill) slider._fill->setSize({ w * t, slider._trackRect.size.y });
    if (slider._knob) slider._knob->setPosition({ x0 + w * t, slider._trackRect.position.y + slider._trackRect.size.y * 0.5f });

    if (slider._value) {
        slider._value->setString(std::to_string(static_cast<int>(raw + (raw >= 0.f ? 0.5f : -0.5f))));
        CenterText(slider._value);
    }
}

void OptionsPanel::SetVolume(float value)
{
    _audio.SetMasterVolume(value);
    _audio.SetMusicVolume(value);
    _audio.SetSfxVolume(value);

    if (_open)
        for (Slider& s : _sliders) UpdateSliderVisual(s);
}
