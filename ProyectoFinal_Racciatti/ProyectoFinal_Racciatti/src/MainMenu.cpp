#include "MainMenu.h"
#include "OptionsPanel.h"

MainMenu::MainMenu(ResourceManager& resourceManager, sf::RenderWindow& window)
	: Scene(window)
	, text(resourceManager.GetFont("../fonts/MilitaryPoster.ttf"), "PLAY", 48)
{
	// BG
	std::string backgroundPath = "../sprites/backgrounds/MainMenu_bg.png";
	sf::IntRect backgroundArea({ 0, 0 }, { 1280, 720 });
	sf::Texture& backgroundTexture = resourceManager.GetTexture(backgroundPath, false, backgroundArea);
	background = new sf::Sprite(backgroundTexture);


	// Shared button data
	std::string buttonPath = "../sprites/UI/button.png";
	sf::IntRect buttonArea({ 0, 0 }, { 366, 159 });
	sf::Texture& btnTex = resourceManager.GetTexture(buttonPath, false, buttonArea);

	// Font data
	const std::string fontPath = "../fonts/MilitaryPoster.ttf";
	sf::Font& font = resourceManager.GetFont(fontPath);
	sf::Color textColor = sf::Color(230, 230, 200);

	// Button sprites
	startButton = new sf::Sprite(btnTex);
	optionsButton = new sf::Sprite(btnTex);
	creditsButton = new sf::Sprite(btnTex);
	exitButton = new sf::Sprite(btnTex);

	CenterSprite(startButton);
	CenterSprite(optionsButton);
	CenterSprite(creditsButton);
	CenterSprite(exitButton);

	startButton->setScale(sf::Vector2f(buttonScale, buttonScale));
	optionsButton->setScale(sf::Vector2f(buttonScale, buttonScale));
	creditsButton->setScale(sf::Vector2f(buttonScale, buttonScale));
	exitButton->setScale(sf::Vector2f(buttonScale, buttonScale));

	const auto winSize = _window.getSize();
	const sf::Vector2f center(winSize.x * 0.5f, winSize.y * 0.5f);

	startButton->setPosition({ center.x, center.y + spacingY * 0.f });
	optionsButton->setPosition({ center.x, center.y + spacingY * 1.f });
	creditsButton->setPosition({ center.x, center.y + spacingY * 2.f });
	exitButton->setPosition({ center.x, center.y + spacingY * 3.f });

	// Text
	playText = new sf::Text(font, "PLAY", 48);
	optionsText = new sf::Text(font, "OPTIONS", 48);
	creditsText = new sf::Text(font, "CREDITS", 48);
	exitText = new sf::Text(font, "EXIT", 48);


	playText->setFillColor(textColor);
	optionsText->setFillColor(textColor);
	creditsText->setFillColor(textColor);
	exitText->setFillColor(textColor);

	CenterText(playText);
	CenterText(optionsText);
	CenterText(creditsText);
	CenterText(exitText);

	playText->setPosition(startButton->getPosition());
	optionsText->setPosition(optionsButton->getPosition());
	creditsText->setPosition(creditsButton->getPosition());
	exitText->setPosition(exitButton->getPosition());

	// Title
	titleText = new sf::Text(font, "CHOPPER HUNTER", 100);
	titleText->setFillColor(textColor);
	CenterText(titleText);
	titleText->setPosition({ winSize.x * 0.5f, 180.f });


	// =============== Opciones =========================
	
	_options = new OptionsPanel(resourceManager, _window);
	_options->SetVolume(100.f);

	// ============== Music ===================
	std::string musicPath = "../audio/music/MainMenuTheme.mp3";

	music.openFromFile(musicPath);

	music.setLooping(true);
	music.setVolume(100);
	music.play();
}

MainMenu::~MainMenu()
{
	delete titleText;
	delete playText;
	delete optionsText;
	delete creditsText;
	delete exitText;
	delete background;
	delete startButton;
	delete optionsButton;
	delete creditsButton;
	delete exitButton;

	// Options
	delete _options;
}

void MainMenu::Input()
{
}

void MainMenu::Update(float dt)
{
	if (wantsChange) return;

	if (music.getStatus() == sf::SoundSource::Status::Stopped)
		music.play();

	if (_options) 
	{
		_options->Update(dt);
		music.setVolume(_options->GetVolume());
	}
}

void MainMenu::Draw()
{
	_window.draw(*background);

	

	if (_options->IsOpen())
	{
		_options->Draw(_window);
	}
	else if (_creditsOpen)
	{
		// Placeholder
	}
	else
	{
		_window.draw(*titleText);

		_window.draw(*startButton);
		_window.draw(*optionsButton);
		_window.draw(*creditsButton);
		_window.draw(*exitButton);

		_window.draw(*playText);
		_window.draw(*optionsText);
		_window.draw(*creditsText);
		_window.draw(*exitText);
	}

}

void MainMenu::HandleEvents(const sf::Event& ev)
{
	if (_options->IsOpen())
	{
		_options->HandleEvent(ev);
		if (_options->BackRequested())
		{
			_options->ClearBackRequest();
			_options->Close();
		}
		return;
	}
	
	if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouse->button == sf::Mouse::Button::Left)
		{
			sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
			sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

			if (startButton->getGlobalBounds().contains(worldPos))
			{
				nextSceneID = SceneID::Gameplay;
				wantsChange = true;

				music.stop();
			}

			if (optionsButton->getGlobalBounds().contains(worldPos))
			{
				_options->Open();
				return;
			}

			if (creditsButton->getGlobalBounds().contains(worldPos))
			{
				_creditsOpen = true;
				return;
			}

			if (exitButton->getGlobalBounds().contains(worldPos))
			{
				_window.close();
				return;
			}
		}
	}
}

// Helpers

void MainMenu::CenterSprite(sf::Sprite* sprite)
{
	if (!sprite) return;

	sprite->setOrigin({ sprite->getTexture().getSize().x * 0.5f,
						sprite->getTexture().getSize().y * 0.5f });

}

void MainMenu::CenterText(sf::Text* text)
{
	if (!text) return;

	sf::FloatRect btn = text->getLocalBounds();
	sf::Vector2f origin(btn.position.x + btn.size.x * 0.5f,
						btn.position.y + btn.size.y * 0.5f);

	text->setOrigin(origin);
}
