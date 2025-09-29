#include "MainMenu.h"

MainMenu::MainMenu(ResourceManager& resourceManager, sf::RenderWindow& window) 
	: Scene(window)
	, text(resourceManager.GetFont("../fonts/MilitaryPoster.ttf"), "PLAY", 48)
{
	std::string backgroundPath = "../sprites/bacgrounds/MainMenu_bg.png";
	sf::IntRect backgroundArea({ 0, 0 }, { 1280, 720 });
	sf::Texture& backgroundTexture = resourceManager.GetTexture(backgroundPath, false, backgroundArea);
	background = new sf::Sprite(backgroundTexture);

	std::string buttonPath = "../sprites/UI/button.png";
	sf::IntRect buttonArea({ 0, 0 }, { 361, 88 });
	sf::Texture& startButtonTexture = resourceManager.GetTexture(buttonPath, false, buttonArea);
	startButton = new sf::Sprite(startButtonTexture);

	startButton->setOrigin({ startButton->getTexture().getSize().x * 0.5f,
							 startButton->getTexture().getSize().y * 0.5f });

	startButton->setPosition({ 1280 * 0.5f, 720 * 0.5f });

	std::string fontPath = "../fonts/MilitaryPoster.ttf";
	sf::Font& font = resourceManager.GetFont(fontPath);
	buttonText = new sf::Text(font, "PLAY", 48);

	/*buttonText->setFont(font);
	buttonText->setString("PLAY");
	buttonText->setCharacterSize(48);*/
	buttonText->setFillColor(sf::Color(230, 230, 200));
	buttonText->setPosition({ 1280 * 0.5f, 720 * 0.5f });
	buttonText->setOrigin(buttonText->getLocalBounds().getCenter());

	std::string musicPath = "../audio/music/MainMenuTheme.mp3";

	music.openFromFile(musicPath);

	music.setLooping(true);
	music.setVolume(50);
	music.play();
}

MainMenu::~MainMenu()
{
	delete buttonText;
	delete background;
	delete startButton;
}

void MainMenu::Input()
{
}

void MainMenu::Update(float dt)
{
	if (wantsChange) return;

	if (music.getStatus() == sf::SoundSource::Status::Stopped)
		music.play();
}

void MainMenu::Draw()
{
	window.draw(*background);
	window.draw(*startButton);
	window.draw(*buttonText);
	//window->draw(text);
}

void MainMenu::HandleEvents(const sf::Event& ev)
{
	if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouse->button == sf::Mouse::Button::Left)
		{
			sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

			if (startButton->getGlobalBounds().contains(worldPos))
			{
				nextSceneID = SceneID::Gameplay;
				wantsChange = true;

				music.stop();
			}
		}
	}
}
