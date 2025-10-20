#include "DeathPanel.h"
#include "HighScores.h"
#include "ScoreEntry.h"
#include "Utils.h"
#include <algorithm>
#include <cstdio>

static constexpr const char* btnPath = "../sprites/UI/button.png";
static constexpr const char* fontTitlePath = "../fonts/MilitaryPoster.ttf";
static constexpr const char* fontBodyPath = "../fonts/soldier.ttf";

namespace
{
	inline void CenterSprite(sf::Sprite* sprite)
	{
		if (!sprite) return;

		sprite->setOrigin({ sprite->getTexture().getSize().x * 0.5f,
							sprite->getTexture().getSize().y * 0.5f });

	}

	inline void CenterText(sf::Text* text)
	{
		if (!text) return;

		sf::FloatRect btn = text->getLocalBounds();
		sf::Vector2f origin(btn.position.x + btn.size.x * 0.5f,
			btn.position.y + btn.size.y * 0.5f);

		text->setOrigin(origin);
	}

	inline std::string ToUpperAscii(const std::string& string)
	{
		std::string out = string;
		for (char& character : out) 
			character = (character >= 'a' && character <= 'z') ? character - 32 : character;

		return out;
	}
}

DeathPanel::DeathPanel(ResourceManager& resources, sf::RenderWindow& window)
	: _rm(resources)
	, _window(window)
	, text(_rm.GetFont(fontTitlePath), "SEMPER FIDELIS", 48)
{
	// Overlay
	const sf::Vector2f panelSize(_window.getSize().x * 0.6f, _window.getSize().y * 0.85f);
	_overlay.setSize(panelSize);
	_overlay.setFillColor(sf::Color(0, 0, 0, 180));
	_overlay.setOutlineThickness(2.f);
	_overlay.setOutlineColor(sf::Color(255, 255, 255, 40));
	_overlay.setOrigin({ panelSize.x * 0.5f, panelSize.y * 0.5f });
	_overlay.setPosition({ _window.getSize().x * 0.5f, _window.getSize().y * 0.5f });

	// Text
	_titleTxt = new sf::Text(_rm.GetFont(fontTitlePath), "SEMPER FIDELIS", _titleSize);
	_scoreLabelTxt = new sf::Text(_rm.GetFont(fontBodyPath), "SCORE", _labelSize);
	_scoreValTxt = new sf::Text(_rm.GetFont(fontBodyPath), "0", _valueSize);
	_timeLabelTxt = new sf::Text(_rm.GetFont(fontBodyPath), "TIME", _labelSize);
	_timeValTxt = new sf::Text(_rm.GetFont(fontBodyPath), "00:00", _valueSize);

	_nameLabelTxt = new sf::Text(_rm.GetFont(fontBodyPath), "NAME", _labelSize);
	_nameValTxt = new sf::Text(_rm.GetFont(fontBodyPath), "___", _inputSize);

	// Text data
	const sf::Color textColor(230, 230, 200);
	for (sf::Text* text : { _titleTxt, _scoreLabelTxt, _scoreValTxt, _timeLabelTxt, _timeValTxt,
						 _nameLabelTxt, _nameValTxt, _saveTxt, _menuTxt })
	{
		if (text) text->setFillColor(textColor);
	}

	// Buttons
	sf::IntRect buttonArea({ 0, 0 }, { 366, 159 });
	sf::Texture& btnTex = _rm.GetTexture(btnPath, false, buttonArea);

	_saveBtn = new sf::Sprite(btnTex);
	_menuBtn = new sf::Sprite(btnTex);

	_saveTxt = new sf::Text(_rm.GetFont(fontTitlePath), "SAVE", 32);
	_menuTxt = new sf::Text(_rm.GetFont(fontTitlePath), "MAIN MENU", 32);

}

DeathPanel::~DeathPanel()
{
	delete _titleTxt;
	delete _scoreLabelTxt;
	delete _scoreValTxt;
	delete _timeLabelTxt;
	delete _timeValTxt;

	delete _nameLabelTxt;
	delete _nameValTxt;

	delete _saveBtn;  delete _saveTxt;
	delete _menuBtn;  delete _menuTxt;
}

void DeathPanel::Open(int finalScore, float finalTimeSeconds, HighScores* board)
{
	_open = true;
	_state = State::Input;
	_returnToMenu = false;

	_finalScore = finalScore;
	_finalTime = finalTimeSeconds;
	_highscores = board;

	_name.clear();
	_cursorTimer = 0.f;
	_cursorOn = true;

	if (_scoreValTxt) _scoreValTxt->setString(std::to_string(_finalScore));
	if (_timeValTxt)  _timeValTxt->setString(Utils::FormatTime(_finalTime));

	LayoutInput();
	UpdateNameVisual();
}

void DeathPanel::Close()
{
	_open = false;
	_returnToMenu = false;
}

bool DeathPanel::IsOver(const sf::Sprite* s, sf::Vector2f pt) const
{
	return s && s->getGlobalBounds().contains(pt);
}

void DeathPanel::HandleEvent(const sf::Event& ev)
{
	if (!_open) return;

	// Texto
	if (_state == State::Input)
	{
		if (ev.is<sf::Event::TextEntered>())
		{
			HandleTextEntered(ev.getIf<sf::Event::TextEntered>()->unicode);
			return;
		}
		if (ev.is<sf::Event::KeyPressed>())
		{
			HandleKeyPressed(*ev.getIf<sf::Event::KeyPressed>());
			return;
		}
	}

	// Mouse
	if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouse->button == sf::Mouse::Button::Left)
		{
			sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
			sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

			if (_state == State::Input)
			{
				if (IsOver(_saveBtn, worldPos))
				{
					TrySaveAndShowBoard();
					return;
				}
			}
			else // Board
			{
				if (IsOver(_menuBtn, worldPos))
				{
					_returnToMenu = true;
					return;
				}
			}
		}
	}
}

void DeathPanel::HandleTextEntered(char32_t codepoint)
{
	if (_name.size() >= _nameMax) return;

	if (codepoint >= U'a' && codepoint <= U'z')
		_name.push_back(static_cast<char>('A' + (int(codepoint) - int('a'))));
	else if (codepoint >= U'A' && codepoint <= U'Z')
		_name.push_back(static_cast<char>(codepoint));
	else if (codepoint >= U'0' && codepoint <= U'9') 
		_name.push_back(static_cast<char>(codepoint));
	else 
		return;

	if (_name.size() > _nameMax) 
		_name.resize(_nameMax);
	_name = ToUpperAscii(_name);

	UpdateNameVisual();
}

void DeathPanel::HandleKeyPressed(const sf::Event::KeyPressed& key)
{
	using K = sf::Keyboard::Scancode;
	if (key.scancode == K::Backspace) 
	{
		if (!_name.empty()) 
		{
			_name.pop_back();
			UpdateNameVisual();
		}
		return;
	}
	if (key.scancode == K::Enter)
	{
		TrySaveAndShowBoard();
		return;
	}
}

void DeathPanel::Update(float dt)
{
	if (!_open) return;

	if (_state == State::Input)
	{
		_cursorTimer += dt;
		if (_cursorTimer >= _cursorPeriod)
		{
			_cursorTimer = 0.f;
			_cursorOn = !_cursorOn;
			UpdateNameVisual();
		}
	}
}

void DeathPanel::Draw(sf::RenderTarget& rt) const
{
	if (!_open) return;

	rt.draw(_overlay);

	const sf::Vector2f center(_window.getSize().x * 0.5f, _window.getSize().y * 0.5f);

	if (_state == State::Input)
	{
		if (_titleTxt)      rt.draw(*_titleTxt);
		if (_scoreLabelTxt) rt.draw(*_scoreLabelTxt);
		if (_scoreValTxt)   rt.draw(*_scoreValTxt);
		if (_timeLabelTxt)  rt.draw(*_timeLabelTxt);
		if (_timeValTxt)    rt.draw(*_timeValTxt);
		if (_nameLabelTxt)  rt.draw(*_nameLabelTxt);
		if (_nameValTxt)    rt.draw(*_nameValTxt);
		if (_saveBtn)       rt.draw(*_saveBtn);
		if (_saveTxt)       rt.draw(*_saveTxt);
	}
	else
	{
		if (_titleTxt) rt.draw(*_titleTxt);

		const sf::Font& bodyFont = _rm.GetFont(fontBodyPath);
		const float listLeft = center.x - _overlay.getSize().x * 0.45f;
		float y = center.y - _overlay.getSize().y * 0.25f;

		const sf::Color normal(230, 230, 200);
		const sf::Color highlight(255, 240, 120);

		for (int i = 0; i < static_cast<int>(_top10.size()); ++i)
		{
			const auto& row = _top10[i];
			char line[128];
			std::snprintf(line, sizeof(line), "%2d. %-3s   %6d   %s",
				i + 1,
				row.name.c_str(),
				row.score,
				Utils::FormatTime(row.time).c_str());

			sf::Text rowText(bodyFont, line, _boardRowSize);
			rowText.setFillColor((i == _playerRank) ? highlight : normal);
			rowText.setPosition({ listLeft, y });
			rt.draw(rowText);

			y += _boardRowSize + _rowGap;
		}

		if (_menuBtn) rt.draw(*_menuBtn);
		if (_menuTxt) rt.draw(*_menuTxt);
	}
}

void DeathPanel::LayoutInput()
{
	const sf::Vector2f center(_window.getSize().x * 0.5f, _window.getSize().y * 0.5f);

	// Titulo
	if (_titleTxt) {
		_titleTxt->setString("ENTER NAME");
		CenterText(_titleTxt);
		_titleTxt->setPosition({ center.x, center.y - _overlay.getSize().y * 0.35f });
	}

	// Valores
	const float leftX = center.x - _overlay.getSize().x * 0.35f;
	float y = center.y - _overlay.getSize().y * 0.20f;

	if (_scoreLabelTxt) { _scoreLabelTxt->setPosition({ leftX, y }); }
	if (_scoreValTxt) { _scoreValTxt->setPosition({ leftX + 180.f, y }); }
	y += _labelSize + _rowGap;

	if (_timeLabelTxt) { _timeLabelTxt->setPosition({ leftX, y }); }
	if (_timeValTxt) { _timeValTxt->setPosition({ leftX + 180.f, y }); }
	y += _labelSize + _rowGap * 2.f;

	if (_nameLabelTxt) { _nameLabelTxt->setPosition({ leftX, y }); }
	if (_nameValTxt)
	{
		_nameValTxt->setPosition({ leftX + 180.f, y - 4.f });
	}

	// Botones
	const float btnScale = 0.5f; 
	const float btnY = center.y + _overlay.getSize().y * 0.30f;

	if (_saveBtn && _saveTxt)
	{
		_saveBtn->setScale({ btnScale, btnScale });
		CenterSprite(_saveBtn);
		_saveBtn->setPosition({ center.x, btnY });

		CenterText(_saveTxt);
		_saveTxt->setPosition(_saveBtn->getPosition());
	}

	// Escondemos el boton del menu
	if (_menuBtn) _menuBtn->setPosition({ -10000.f, -10000.f });
	if (_menuTxt) _menuTxt->setPosition({ -10000.f, -10000.f });
}

void DeathPanel::LayoutBoard()
{
	const sf::Vector2f center(_window.getSize().x * 0.5f, _window.getSize().y * 0.5f);

	if (_titleTxt) {
		_titleTxt->setString("SEMPER FIDELIS");
		CenterText(_titleTxt);
		_titleTxt->setPosition({ center.x, center.y - _overlay.getSize().y * 0.35f });
	}

	// Boton Main Menu
	const float btnScale = 0.5f;
	const float btnY = center.y + _overlay.getSize().y * 0.35f;
	if (_menuBtn && _menuTxt)
	{
		_menuBtn->setScale({ btnScale, btnScale });
		CenterSprite(_menuBtn);
		_menuBtn->setPosition({ center.x, btnY });

		CenterText(_menuTxt);
		_menuTxt->setPosition(_menuBtn->getPosition());
	}

	// Escondemos el boton de save
	if (_saveBtn) _saveBtn->setPosition({ -10000.f, -10000.f });
	if (_saveTxt) _saveTxt->setPosition({ -10000.f, -10000.f });
}

void DeathPanel::UpdateNameVisual()
{
	if (!_nameValTxt) return;

	std::string shown = _name;
	while (shown.size() < _nameMax) shown.push_back('_');

	if (_cursorOn && _name.size() < _nameMax)
		shown[_name.size()] = '|';

	_nameValTxt->setString(shown);
}

void DeathPanel::TrySaveAndShowBoard()
{
	if (_state != State::Input) return;

	if (_name.size() < _nameMax) return;

	if (_highscores)
	{
		ScoreEntry e;
		e.name = ToUpperAscii(_name);
		e.score = _finalScore;
		e.time = _finalTime;

		_highscores->Submit(e);
		_highscores->Save();

		_top10 = _highscores->Top();

		_playerRank = -1;
		for (int i = 0; i < static_cast<int>(_top10.size()); ++i)
		{
			if (_top10[i].name == e.name && _top10[i].score == e.score && _top10[i].time == e.time)
			{
				_playerRank = i;
				break;
			}
		}
	}
	else
	{
		_top10.clear();
		_playerRank = -1;
	}

	_state = State::Board;
	LayoutBoard();
}