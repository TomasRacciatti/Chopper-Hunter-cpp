#include "PauseMenu.h"

#include "PauseMenu.h"

PauseMenu::PauseMenu(ResourceManager& resourceManager, sf::RenderWindow& window, AudioSettings& audio)
    : _window(window)
    , _audio(audio)
    , text(resourceManager.GetFont("../fonts/MilitaryPoster.ttf"), "PAUSED", 48)
{
    // Font data
    const std::string fontPath = "../fonts/MilitaryPoster.ttf";
    sf::Font& font = resourceManager.GetFont(fontPath);
    sf::Color textColor = sf::Color(230, 230, 200);

    const auto winSize = _window.getSize();
    const sf::Vector2f center(winSize.x * 0.5f, winSize.y * 0.5f);

    // Overlay
    _panel = new sf::RectangleShape();
    const sf::Vector2f panelSize(winSize.x * 0.85f, winSize.y * 0.6f);
    _panel->setSize(panelSize);
    _panel->setFillColor(sf::Color(0, 0, 0, 180));
    _panel->setOutlineThickness(2.f);
    _panel->setOutlineColor(sf::Color(255, 255, 255, 40));
    _panel->setOrigin({ panelSize.x * 0.5f, panelSize.y * 0.5f });
    _panel->setPosition({ center.x, center.y });

    // Title
    _titleTxt = new sf::Text(font, "PAUSED", 86);
    _titleTxt->setFillColor(textColor);
    CenterText(_titleTxt);
    _titleTxt->setPosition({ center.x, center.y - _spacingY * 1.8f });

    // Buttons
    _resumeTxt = new sf::Text(font, "RESUME", 48);
    _resumeTxt->setFillColor(textColor);
    CenterText(_resumeTxt);
    _resumeTxt->setPosition({ center.x, center.y + _spacingY * 0.0f });

    _optionsTxt = new sf::Text(font, "OPTIONS", 48);
    _optionsTxt->setFillColor(textColor);
    CenterText(_optionsTxt);
    _optionsTxt->setPosition({ center.x, center.y + _spacingY * 1.0f });

    _toMenuTxt = new sf::Text(font, "MAIN MENU", 48);
    _toMenuTxt->setFillColor(textColor);
    CenterText(_toMenuTxt);
    _toMenuTxt->setPosition({ center.x, center.y + _spacingY * 2.0f });

    // Reuso de OptionsPanel
    _optionsPanel = new OptionsPanel(resourceManager, _window, _audio);
    _optionsPanel->SetVolume(_optionsPanel->GetVolume());
}

PauseMenu::~PauseMenu()
{
    delete _panel;
    delete _titleTxt;
    delete _resumeTxt;
    delete _optionsTxt;
    delete _toMenuTxt;
    delete _optionsPanel;
}

void PauseMenu::HandleEvent(const sf::Event& ev)
{
    if (!_open) return;

    if (_optionsPanel && _optionsPanel->IsOpen())
    {
        _optionsPanel->HandleEvent(ev);
        if (_optionsPanel->BackRequested()) {
            _optionsPanel->ClearBackRequest();
            _optionsPanel->Close();
        }
        return;
    }

    if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouse->button == sf::Mouse::Button::Left)
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

            if (_resumeTxt && _resumeTxt->getGlobalBounds().contains(worldPos))
            {
                _resumeRequested = true;
                return;
            }
            if (_optionsTxt && _optionsTxt->getGlobalBounds().contains(worldPos))
            {
                if (_optionsPanel) _optionsPanel->Open(); 
                return;
            }
            if (_toMenuTxt && _toMenuTxt->getGlobalBounds().contains(worldPos))
            {
                _menuRequested = true;        
                return;
            }
        }
    }
}

void PauseMenu::Update(float dt)
{
    if (!_open) return;
    if (_optionsPanel) 
        _optionsPanel->Update(dt);
}

void PauseMenu::Draw(sf::RenderTarget& rt) const
{
    if (!_open) return;

    if (_panel)    
        rt.draw(*_panel);

    if (!_optionsPanel || !_optionsPanel->IsOpen())
    {
        if (_titleTxt) rt.draw(*_titleTxt);
        if (_resumeTxt) rt.draw(*_resumeTxt);
        if (_optionsTxt) rt.draw(*_optionsTxt);
        if (_toMenuTxt) rt.draw(*_toMenuTxt);
    }

    if (_optionsPanel) 
        _optionsPanel->Draw(rt);
}

// Helper
void PauseMenu::CenterText(sf::Text* text) const
{
    if (!text) return;

    sf::FloatRect rect = text->getLocalBounds();
    text->setOrigin({
        rect.position.x + rect.size.x * 0.5f,
        rect.position.y + rect.size.y * 0.5f});
}