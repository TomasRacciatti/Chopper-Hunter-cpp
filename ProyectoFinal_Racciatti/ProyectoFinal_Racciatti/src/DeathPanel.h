#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "ResourceManager.h"

class HighScores;
struct ScoreEntry;

class DeathPanel
{
public:
	DeathPanel(ResourceManager& resources, const sf::Vector2u& winSize);
	~DeathPanel();

	void Open(int finalScore, float finalTimeSeconds, HighScores* board);
	void Close();

	bool IsOpen() const { return _open; }
	bool OnBoardScreen()const { return _state == State::Board; }

	void HandleEvent(const sf::Event& ev);
	void Update(float dt);
	void Draw(sf::RenderTarget& rt) const;

	bool WantsReturnToMenu() const { return _returnToMenu; }

private:
	enum class State { Input, Board };

	ResourceManager& _rm;
	sf::Vector2u _win{ 0,0 };
	HighScores* _highscores = nullptr;

	// --- State ---
	bool  _open = false;
	State _state = State::Input;

	int   _finalScore = 0;
	float _finalTime = 0.f;

	// Input
	std::string _name;
	std::size_t _nameMax = 3;

	// Board
	std::vector<ScoreEntry> _top10;
	int _playerRank = -1;

	bool _returnToMenu = false;

	// Visual
	sf::Text text;

	sf::Text* _titleTxt = nullptr; 
	sf::Text* _scoreLabelTxt = nullptr; 
	sf::Text* _scoreValTxt = nullptr; 
	sf::Text* _timeLabelTxt = nullptr;
	sf::Text* _timeValTxt = nullptr; 

	// Name input
	sf::Text* _nameLabelTxt = nullptr; 
	sf::Text* _nameValTxt = nullptr; 

	// Buttons 
	sf::Sprite* _saveBtn = nullptr; 
	sf::Text* _saveTxt = nullptr;

	sf::Sprite* _skipBtn = nullptr; 
	sf::Text* _skipTxt = nullptr;

	sf::Sprite* _menuBtn = nullptr; 
	sf::Text* _menuTxt = nullptr;

	sf::RectangleShape _overlay;

	// Layout
	float _margin = 16.f;
	float _rowGap = 8.f;
	float _btnGap = 20.f;
	unsigned _titleSize = 48;
	unsigned _labelSize = 28;
	unsigned _valueSize = 28;
	unsigned _boardRowSize = 24;

	unsigned _inputSize = 32;

	// Cursor blink
	float _cursorTimer = 0.f;
	float _cursorPeriod = 0.6f;
	bool  _cursorOn = true;

	// Helpers
	void LayoutInput();        
	void LayoutBoard();        
	void UpdateNameVisual();   
	void TrySaveAndShowBoard();
	void OnSkip();             
	void OnMainMenu();         

	bool IsOver(const sf::Sprite* s, sf::Vector2f pt) const;

	void HandleTextEntered(char32_t codepoint);
	void HandleKeyPressed(const sf::Event::KeyPressed& key);
};

