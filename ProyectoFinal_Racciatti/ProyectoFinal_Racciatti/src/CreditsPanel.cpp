#include "CreditsPanel.h"

CreditsPanel::CreditsPanel(ResourceManager& resourceManager, sf::RenderWindow& window)
	:_window(window)
	, text(resourceManager.GetFont("../fonts/MilitaryPoster.ttf"), "CREDITS", 48)
{
    // BG
    std::string backgroundPath = "../sprites/backgrounds/MainMenu_bg.png";
    sf::IntRect backgroundArea({ 0, 0 }, { 1280, 720 });
    sf::Texture& backgroundTexture = resourceManager.GetTexture(backgroundPath, false, backgroundArea);
    _background = new sf::Sprite(backgroundTexture);

    // Shared button data
    std::string buttonPath = "../sprites/UI/button.png";
    sf::IntRect buttonArea({ 0, 0 }, { 366, 159 });
    sf::Texture& btnTex = resourceManager.GetTexture(buttonPath, false, buttonArea);

    // Font data
    const std::string titleFontPath = "../fonts/MilitaryPoster.ttf";
    sf::Font& titleFont = resourceManager.GetFont(titleFontPath);

    const std::string bodyFontPath = "../fonts/soldier.ttf";
    sf::Font& bodyFont = resourceManager.GetFont(bodyFontPath);

    sf::Color textColor = sf::Color(230, 230, 200);

    const auto winSize = _window.getSize();
    const sf::Vector2f center(winSize.x * 0.5f, winSize.y * 0.5f);

    // Title
    _titleTxt = new sf::Text(titleFont, "CREDITS", 100);
    _titleTxt->setFillColor(textColor);
    CenterText(_titleTxt);
    _titleTxt->setPosition({ center.x, center.y - _spacingY * 2.f });

    // Back btn
    _backBtn = new sf::Sprite(btnTex);
    CenterSprite(_backBtn);
    _backBtn->setScale(sf::Vector2f(_buttonScale * 0.9f, _buttonScale * 0.9f));
    _backBtn->setPosition({ center.x, center.y + _spacingY * 2.5f });

    _backTxt = new sf::Text(titleFont, "BACK", 48);
    _backTxt->setFillColor(textColor);
    CenterText(_backTxt);
    _backTxt->setPosition(_backBtn->getPosition());

    // Body
    const char* body =
        "Developer\n"
        "Tomas Racciatti\n"
        "\n"
        "Assets\n"
        "Soldier, weapons & crates by SquareCircleCo.\n"
        "Attack Helicopter by Paul Chin\n"
        "Drone by CraftPix\n"
        "Explosions by Ansimuz\n"
        "Misiles by Anim86\n"
        "\n"
        "Music\n"
        "Goliath by Scott Buckley\n"
        "Lock and Load by Hayden Folker";

    _bodyTxt = new sf::Text(bodyFont, body, 36);
    _bodyTxt->setFillColor(textColor);

    sf::FloatRect bodyRect = _bodyTxt->getLocalBounds();
    _bodyTxt->setOrigin({ bodyRect.position.x, bodyRect.position.y });

    const float padX = winSize.x * 0.1f;
    const float topY = center.y - winSize.y * 0.35f;
    _bodyTxt->setPosition({ center.x - winSize.x * 0.5f + padX, topY });
}

CreditsPanel::~CreditsPanel()
{
    delete _background;
    delete _titleTxt;
    delete _backBtn;
    delete _backTxt;
    delete _bodyTxt;
}

void CreditsPanel::HandleEvent(const sf::Event& ev)
{
    if (!_open) return;

    if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouse->button == sf::Mouse::Button::Left)
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
            sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

            if (_backBtn && _backBtn->getGlobalBounds().contains(worldPos)) 
            {
                _backRequested = true;
                return;
            }
        }
    }
}

void CreditsPanel::Draw(sf::RenderTarget& rt) const
{
    if (!_open) return;

    if (_background) rt.draw(*_background);
    if (_titleTxt) rt.draw(*_titleTxt);
    if (_bodyTxt) rt.draw(*_bodyTxt);
    if (_backBtn) rt.draw(*_backBtn);
    if (_backTxt) rt.draw(*_backTxt);
}


// Helpers
void CreditsPanel::CenterSprite(sf::Sprite* sprite) const
{
    if (!sprite) return;

    sprite->setOrigin({ sprite->getTexture().getSize().x * 0.5f,
                        sprite->getTexture().getSize().y * 0.5f });

}

void CreditsPanel::CenterText(sf::Text* text) const
{
    if (!text) return;

    sf::FloatRect btn = text->getLocalBounds();
    sf::Vector2f origin(btn.position.x + btn.size.x * 0.5f,
                        btn.position.y + btn.size.y * 0.5f);

    text->setOrigin(origin);
}