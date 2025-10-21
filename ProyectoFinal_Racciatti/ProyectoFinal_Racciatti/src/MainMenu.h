#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "ResourceManager.h"
#include <vector>

class OptionsPanel;
class CreditsPanel;
class HowToPlayPanel;

class MainMenu : public Scene
{
public:
	MainMenu(ResourceManager& resourceManager, sf::RenderWindow& window, AudioSettings& audio);
	~MainMenu() override;

	void Input() override;
	void Update(float dt) override;
	void Draw() override;

	void HandleEvents(const sf::Event& ev) override;

private:
	sf::Music music;
	AudioSettings& _audio;

	sf::Text text;

	sf::Text* titleText = nullptr;
	sf::Text* playText = nullptr;
	sf::Text* optionsText = nullptr;
	sf::Text* howToText = nullptr;
	sf::Text* creditsText = nullptr;
	sf::Text* exitText = nullptr;

	sf::Sprite* background = nullptr;
	sf::Sprite* startButton = nullptr;
	sf::Sprite* optionsButton = nullptr;
	sf::Sprite* howToButton = nullptr;
	sf::Sprite* creditsButton = nullptr;
	sf::Sprite* exitButton = nullptr;

	// Panels
	OptionsPanel* _options = nullptr;
	CreditsPanel* _credits = nullptr;
	HowToPlayPanel* _howTo = nullptr;

	// Credits
	bool _creditsOpen = false;


	// Helpers
	static void CenterSprite(sf::Sprite* sprite);
	static void CenterText(sf::Text* text);

	float buttonScale = 0.5f;
	float spacingY = 100.f;
};

