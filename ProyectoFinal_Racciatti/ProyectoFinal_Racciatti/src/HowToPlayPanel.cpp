#include "HowToPlayPanel.h"

static constexpr const char* buttonPng = "../sprites/UI/button.png";
static constexpr const char* bgPng = "../sprites/backgrounds/MainMenu_bg.png";
static constexpr const char* titleFont = "../fonts/MilitaryPoster.ttf";
static constexpr const char* bodyFont = "../fonts/soldier.ttf";
static const sf::Color textColor(230, 230, 200);

HowToPlayPanel::HowToPlayPanel(ResourceManager& rm, sf::RenderWindow& window)
    : _window(window)
{
    // Bg
    {
        sf::IntRect area({ 0,0 }, { 1280,720 });
        sf::Texture& bgTex = rm.GetTexture(bgPng, false, area);
        _background = new sf::Sprite(bgTex);
    }

    // Font
    _titleFont = &rm.GetFont(titleFont);
    _bodyFont = &rm.GetFont(bodyFont);

    BuildCommonUI(rm);
    BuildControlsPage(rm);
    BuildEnemiesPage(rm);
    BuildWeaponsPage(rm);
}

HowToPlayPanel::~HowToPlayPanel()
{
    auto destroy = [&](std::vector<IconLabel>& v) {
        for (auto& e : v) { delete e.icon; delete e.text; }
        v.clear();
        };
    destroy(_controls);
    destroy(_enemies);
    destroy(_weapons);

    delete _title;
    delete _backBtn;   delete _backTxt;
    delete _prevBtn;   delete _prevTxt;
    delete _nextBtn;   delete _nextTxt;
    delete _background;
}

void HowToPlayPanel::BuildCommonUI(ResourceManager& rm)
{
    const auto win = _window.getSize();
    const sf::Vector2f center(win.x * 0.5f, win.y * 0.5f);

    _title = new sf::Text(*_titleFont, "HOW TO PLAY", 100);
    _title->setFillColor(textColor);
    CenterText(_title);
    _title->setPosition({ center.x, center.y - _spacingY * 3.5f });

    _backBtn = MakeButton(rm, _buttonScale * 0.9f);
    CenterSprite(_backBtn);
    _backBtn->setPosition({ center.x, center.y + _spacingY * 3.5f });

    _backTxt = new sf::Text(*_titleFont, "BACK", 48);
    _backTxt->setFillColor(textColor);
    CenterText(_backTxt);
    _backTxt->setPosition(_backBtn->getPosition());

    _prevBtn = MakeButton(rm, _buttonScale * 0.8f);
    _nextBtn = MakeButton(rm, _buttonScale * 0.8f);
    CenterSprite(_prevBtn); CenterSprite(_nextBtn);

    _prevBtn->setPosition({ center.x - 280.f, _backBtn->getPosition().y - 30.f });
    _nextBtn->setPosition({ center.x + 280.f, _backBtn->getPosition().y - 30.f });

    _prevTxt = new sf::Text(*_titleFont, "PREV", 48);
    _nextTxt = new sf::Text(*_titleFont, "NEXT", 48);
    _prevTxt->setFillColor(textColor);
    _nextTxt->setFillColor(textColor);
    CenterText(_prevTxt); CenterText(_nextTxt);
    _prevTxt->setPosition(_prevBtn->getPosition());
    _nextTxt->setPosition(_nextBtn->getPosition());
}

void HowToPlayPanel::BuildControlsPage(ResourceManager& rm)
{
    const auto win = _window.getSize();
    const sf::Vector2f center(win.x * 0.5f, win.y * 0.5f);

    const float topY = 200.f;
    const float rowH = 56.f;
    const float leftX = center.x - 260.f;
    const float rightX = center.x - 60.f;

    auto AddPair = [&](const std::string& key, const std::string& desc, float y, unsigned ks = 42, unsigned ds = 42)
        {
            // Izquierda
            IconLabel keys;
            keys.icon = nullptr;
            keys.text = new sf::Text(*_bodyFont, key, ks);
            keys.text->setFillColor(textColor);
            sf::FloatRect bounds1 = keys.text->getLocalBounds();
            keys.text->setOrigin({ bounds1.position.x, bounds1.position.y });
            keys.pos = { leftX, y };
            _controls.push_back(keys);

            // Derecha
            IconLabel description;
            description.icon = nullptr;
            description.text = new sf::Text(*_bodyFont, desc, ds);
            description.text->setFillColor(textColor);
            sf::FloatRect bounds2 = description.text->getLocalBounds();
            description.text->setOrigin({ bounds2.position.x, bounds2.position.y });
            description.pos = { rightX, y };
            _controls.push_back(description);
        };

    float y = topY;
    AddPair("CONTROLS:", "", y, 42, 1); y += rowH; 
    AddPair("A / D:", "Move", y); y += rowH;
    AddPair("S:", "Crouch", y); y += rowH;
    AddPair("1 / 2 / 3:", "Change Weapon", y); y += rowH;
    AddPair("Mouse:", "Aim", y); y += rowH;
    AddPair("LMB:", "Shoot -can be held-", y, 42, 38); y += rowH;
}

void HowToPlayPanel::BuildEnemiesPage(ResourceManager& rm)
{
    const auto win = _window.getSize();
    const sf::Vector2f center(win.x * 0.5f, win.y * 0.5f);

    {
        IconLabel t; t.icon = nullptr;
        t.text = new sf::Text(*_bodyFont, "Enemies:", 42);
        t.text->setFillColor(textColor);
        CenterText(t.text);
        t.pos = { center.x, center.y - 150.f };
        _enemies.push_back(t);
    }

    const float yRow = center.y - 60.f;
    const float gapX = 260.f;

    // Enemies
    auto AddEnemy = [&](const std::string& iconPath, const std::string& desc, float x)
        {
            IconLabel enemy;
            enemy.icon = MakeIcon(rm, iconPath, 96.f);
            CenterSprite(enemy.icon);
            enemy.icon->setPosition({ x, yRow });

            enemy.text = new sf::Text(*_bodyFont, desc, 36);
            enemy.text->setFillColor(textColor);
            CenterText(enemy.text);
            enemy.pos = { x, yRow + 90.f };
            _enemies.push_back(enemy);
        };

    AddEnemy(_heliPath,
        "Helicopters will attempt to shoot you.\nCan be shot down. May drop ammo/health crates.",
        center.x - gapX);

    AddEnemy(_dronePath,
        "Drones fly toward you and explode.\nCan be shot down.",
        center.x);

    AddEnemy(_artilleryPath,
        "Artillery drops from the sky and explodes.\nCannot be destroyed.",
        center.x + gapX);

    // Crates
    {
        IconLabel t; t.icon = nullptr;
        t.text = new sf::Text(*_bodyFont, "Crate Drops:", 42);
        t.text->setFillColor(textColor);
        CenterText(t.text);
        t.pos = { center.x, center.y + 80.f };
        _enemies.push_back(t);
    }

    auto AddCrate = [&](const std::string& iconPath, const std::string& label, float x)
        {
            IconLabel crate;
            crate.icon = MakeIcon(rm, iconPath, 64.f);
            CenterSprite(crate.icon);
            crate.icon->setPosition({ x, center.y + 150.f });

            crate.text = new sf::Text(*_bodyFont, label, 32);
            crate.text->setFillColor(textColor);
            CenterText(crate.text);
            crate.pos = { x, center.y + 210.f };
            _enemies.push_back(crate);
        };

    AddCrate(_crateHealthPath, "Health", center.x - gapX);
    AddCrate(_crateSgPath, "Shotgun Ammo", center.x);
    AddCrate(_crateRpgPath, "Missiles", center.x + gapX);
}

void HowToPlayPanel::BuildWeaponsPage(ResourceManager& rm)
{
    const auto win = _window.getSize();
    const sf::Vector2f center(win.x * 0.5f, win.y * 0.5f);

    IconLabel t; t.icon = nullptr;
    t.text = new sf::Text(*_bodyFont, "Weapons:", 42);
    t.text->setFillColor(textColor);
    CenterText(t.text);
    t.pos = { center.x, center.y - 150.f };
    _weapons.push_back(t);

    const float yIcon = center.y - 40.f;
    const float yText1 = center.y + 40.f;
    const float yText2 = center.y + 88.f;
    const float gapX = 260.f;

    auto AddWeapon = [&](const std::string& wPath, const std::string& line1, const std::string& line2,
        const std::string& cratePath, const std::string& crateLabel, float x)
        {
            IconLabel w;
            w.icon = MakeIcon(rm, wPath, 90.f);
            CenterSprite(w.icon);
            w.icon->setPosition({ x, yIcon });

            w.text = new sf::Text(*_bodyFont, line1, 34);
            w.text->setFillColor(textColor);
            CenterText(w.text);
            w.pos = { x, yText1 };
            _weapons.push_back(w);

            IconLabel w2;
            w2.icon = nullptr;
            w2.text = new sf::Text(*_bodyFont, line2, 28);
            w2.text->setFillColor(textColor);
            CenterText(w2.text);
            w2.pos = { x, yText2 };
            _weapons.push_back(w2);

            if (!cratePath.empty())
            {
                IconLabel c;
                c.icon = MakeIcon(rm, cratePath, 56.f);
                CenterSprite(c.icon);
                c.icon->setPosition({ x, yText2 + 60.f });
                c.text = new sf::Text(*_bodyFont, crateLabel, 26);
                c.text->setFillColor(textColor);
                CenterText(c.text);
                c.pos = { x, yText2 + 110.f };
                _weapons.push_back(c);
            }
        };

    AddWeapon(_pistolPath, "Pistol", "Single rapid fire (hold LMB).", "", "", center.x - gapX);
    AddWeapon(_shotgunPath, "Shotgun", "Fires 5 pellets.", _crateSgPath, "Shotgun Ammo", center.x);
    AddWeapon(_missilePath, "Missile", "Single heavy missile.\nHold LMB to guide.", _crateRpgPath, "Missile Crate", center.x + gapX);
}

void HowToPlayPanel::HandleEvent(const sf::Event& ev)
{
    if (!_open) return;

    if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouse->button == sf::Mouse::Button::Left)
        {
            sf::Vector2i pixel = sf::Mouse::getPosition(_window);
            sf::Vector2f world = _window.mapPixelToCoords(pixel);

            if (_backBtn && _backBtn->getGlobalBounds().contains(world)) { _backRequested = true; return; }
            if (_prevBtn && _prevBtn->getGlobalBounds().contains(world)) { _page = (_page + _pageCount - 1) % _pageCount; return; }
            if (_nextBtn && _nextBtn->getGlobalBounds().contains(world)) { _page = (_page + 1) % _pageCount; return; }
        }
    }
}

void HowToPlayPanel::Draw(sf::RenderTarget& rt) const
{
    if (!_open) return;

    rt.draw(*_background);
    rt.draw(*_title);

    if (_page == 0) 
        DrawPage(rt, _controls);
    else if (_page == 1) 
        DrawPage(rt, _enemies);
    else
        DrawPage(rt, _weapons);

    rt.draw(*_prevBtn);
    rt.draw(*_nextBtn);
    rt.draw(*_prevTxt);
    rt.draw(*_nextTxt);

    rt.draw(*_backBtn);
    rt.draw(*_backTxt);
}

// --------- Helpers ------------
sf::Sprite* HowToPlayPanel::MakeButton(ResourceManager& rm, float scale)
{
    sf::IntRect area({ 0,0 }, { 366,159 });
    sf::Texture& tex = rm.GetTexture(buttonPng, false, area);
    auto* sprite = new sf::Sprite(tex);
    sprite->setScale({ scale, scale });
    return sprite;
}

sf::Text* HowToPlayPanel::MakeText(ResourceManager& rm, const std::string& string, unsigned size)
{
    auto* text = new sf::Text(*_bodyFont, string, size);
    text->setFillColor(textColor);
    return text;
}

sf::Sprite* HowToPlayPanel::MakeIcon(ResourceManager& rm, const std::string& path, float targetHeight)
{
    sf::Texture& tex = rm.GetTexture(path, false, sf::IntRect());
    auto* sprite = new sf::Sprite(tex);
    const auto size = tex.getSize();
    if (size.y > 0)
    {
        const float scale = targetHeight / static_cast<float>(size.y);
        sprite->setScale({ scale, scale });
    }
    return sprite;
}

void HowToPlayPanel::DrawPage(sf::RenderTarget& rt, const std::vector<IconLabel>& elems) const
{
    for (const auto& elements : elems)
    {
        if (elements.icon) rt.draw(*elements.icon);
        if (elements.text)
        {
            sf::Text tmp = *elements.text;
            tmp.setPosition(elements.pos);
            rt.draw(tmp);
        }
    }
}

void HowToPlayPanel::CenterSprite(sf::Sprite* sprite)
{
    if (!sprite) return;

    sprite->setOrigin({ sprite->getTexture().getSize().x * 0.5f,
                        sprite->getTexture().getSize().y * 0.5f });

}

void HowToPlayPanel::CenterText(sf::Text* text)
{
    if (!text) return;

    sf::FloatRect btn = text->getLocalBounds();
    sf::Vector2f origin(btn.position.x + btn.size.x * 0.5f,
        btn.position.y + btn.size.y * 0.5f);

    text->setOrigin(origin);
}