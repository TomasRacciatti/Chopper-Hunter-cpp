#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "ResourceManager.h"

class MainMenu : public Scene
{
private:
	sf::Music music;

	sf::Text* buttonText;
	sf::Text text;

	sf::Sprite* background;

	sf::Sprite* startButton;

public:
	MainMenu(ResourceManager& resourceManager, sf::RenderWindow& window);
	~MainMenu() override;

	void Input() override;
	void Update(float dt) override;
	void Draw() override;

	void HandleEvents(const sf::Event& ev) override;

};

