#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "ResourceManager.h"
#include <vector>

class OptionsPanel;

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

	sf::Text* titleText;
	sf::Text* playText;
	sf::Text* optionsText;
	sf::Text* creditsText;
	sf::Text* exitText;

	sf::Sprite* background;
	sf::Sprite* startButton;
	sf::Sprite* optionsButton;
	sf::Sprite* creditsButton;
	sf::Sprite* exitButton;

	// Options
	OptionsPanel* _options = nullptr;

	// Credits
	bool _creditsOpen = false;

	// Helpers
	static void CenterSprite(sf::Sprite* sprite);
	static void CenterText(sf::Text* text);

	float buttonScale = 0.5f;
	float spacingY = 100.f;
};

