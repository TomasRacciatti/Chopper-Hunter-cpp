#include "MainMenu.h"

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
	// Back button
	backButton = new sf::Sprite(btnTex);
	CenterSprite(backButton);
	backButton->setScale(sf::Vector2f(buttonScale * 0.9f, buttonScale * 0.9f));
	backButton->setPosition({ center.x, center.y + spacingY * 2.5f });
	
	backText = new sf::Text(font, "BACK", 48);
	backText->setFillColor(textColor);
	CenterText(backText);
	backText->setPosition(backButton->getPosition());

	// Volume label y value
	volumeLabel = new sf::Text(font, "VOLUME", 48);
	volumeLabel->setFillColor(textColor);
	CenterText(volumeLabel);
	volumeLabel->setPosition({ center.x - 25.f, center.y - spacingY * 0.8f });
	
	volumeValue = new sf::Text(font, "100", 42);
	volumeValue->setFillColor(textColor);
	CenterText(volumeValue);
	volumeValue->setPosition({ center.x + 75.f, center.y - spacingY * 0.8f });

	// ---- Slider ----
	sliderTrack = new sf::RectangleShape();
	sliderFill = new sf::RectangleShape();
	sliderKnob = new sf::CircleShape();

	// Track
	const float trackW = 420.f;
	const float trackH = 16.f;
	const sf::Vector2f trackPos(center.x - trackW * 0.5f, center.y - spacingY * 0.1f);
	sliderTrack->setSize(sf::Vector2f(trackW, trackH));
	sliderTrack->setPosition(trackPos);
	sliderTrack->setFillColor(sf::Color(60, 60, 60));

	// Fill
	sliderFill->setSize(sf::Vector2f(trackW * (_volume / 100.f), trackH));
	sliderFill->setPosition(trackPos);
	sliderFill->setFillColor(textColor);

	// Knob
	sliderKnob->setRadius(14.f);
	sliderKnob->setOrigin(sf::Vector2f(14.f, 14.f));
	const float knobX = trackPos.x + trackW * (_volume / 100.f);
	const float knobY = trackPos.y + trackH * 0.5f;
	sliderKnob->setPosition(sf::Vector2f(knobX, knobY));
	sliderKnob->setFillColor(sf::Color(235, 235, 90));

	_sliderRect = sf::FloatRect(sliderTrack->getPosition(),sliderTrack->getSize());

	// ============== Music ===================
	std::string musicPath = "../audio/music/MainMenuTheme.mp3";

	music.openFromFile(musicPath);

	music.setLooping(true);
	music.setVolume(50);
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
	delete backButton;
	delete backText;
	delete volumeLabel;
	delete volumeValue;
	delete sliderTrack;
	delete sliderFill;
	delete sliderKnob;
}

void MainMenu::Input()
{
}

void MainMenu::Update(float dt)
{
	if (wantsChange) return;

	if (music.getStatus() == sf::SoundSource::Status::Stopped)
		music.play();

	music.setVolume(_volume);
}

void MainMenu::Draw()
{
	_window.draw(*background);

	_window.draw(*titleText);

	if (_optionsOpen)
	{
		_window.draw(*volumeLabel);
		_window.draw(*sliderTrack);
		_window.draw(*sliderFill);
		_window.draw(*sliderKnob);
		_window.draw(*backButton);
		_window.draw(*backText);
		if (volumeValue)
			_window.draw(*volumeValue);
	}
	else if (_creditsOpen)
	{
		// Placeholder
	}
	else
	{
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
	if (const auto* mouse = ev.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouse->button == sf::Mouse::Button::Left)
		{
			sf::Vector2i pixelPos = sf::Mouse::getPosition(_window);
			sf::Vector2f worldPos = _window.mapPixelToCoords(pixelPos);

			if (_optionsOpen)
			{
				const bool onTrack =
					worldPos.x >= _sliderRect.position.x &&
					worldPos.x <= _sliderRect.position.x + _sliderRect.size.x &&
					worldPos.y >= _sliderRect.position.y - 10.f &&
					worldPos.y <= _sliderRect.position.y + _sliderRect.size.y + 10.f;

				const bool onKnob = sliderKnob->getGlobalBounds().contains(worldPos);

				// Back
				if (backButton->getGlobalBounds().contains(worldPos))
				{
					_optionsOpen = false;
					_draggingKnob = false;
					return;
				}

				if (onKnob || onTrack)
				{
					_draggingKnob = true;

					const float x0 = _sliderRect.position.x;
					const float x1 = _sliderRect.position.x + _sliderRect.size.x;
					float clampedX = std::max(x0, std::min(worldPos.x, x1));
					float t = (clampedX - x0) / (x1 - x0); 
					_volume = t * 100.f;
					sliderFill->setSize(sf::Vector2f(_sliderRect.size.x * t, _sliderRect.size.y));
					sliderKnob->setPosition(sf::Vector2f(clampedX, sliderKnob->getPosition().y));
					if (volumeValue)
					{
						int val = static_cast<int>(_volume + 0.5f);
						volumeValue->setString(std::to_string(val));
						CenterText(volumeValue);
						volumeValue->setPosition({
									_window.getSize().x * 0.5f + 75.f,
									_window.getSize().y * 0.5f - spacingY * 0.8f});
					}
				}
				return;
			}

			if (startButton->getGlobalBounds().contains(worldPos))
			{
				nextSceneID = SceneID::Gameplay;
				wantsChange = true;

				music.stop();
			}

			if (optionsButton->getGlobalBounds().contains(worldPos))
			{
				_optionsOpen = true;
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
	else if (const auto* mouseMove = ev.getIf<sf::Event::MouseMoved>())
	{
		if (_optionsOpen && _draggingKnob)
		{
			// Slider se cambia con drag
			sf::Vector2f world = _window.mapPixelToCoords(sf::Mouse::getPosition(_window));
			const float x0 = _sliderRect.position.x;
			const float x1 = _sliderRect.position.x + _sliderRect.size.x;
			float clampedX = std::max(x0, std::min(world.x, x1));
			float t = (clampedX - x0) / (x1 - x0); 
			_volume = t * 100.f;

			// Modificamos los visuales
			sliderFill->setSize(sf::Vector2f(_sliderRect.size.x * t, _sliderRect.size.y));
			sliderKnob->setPosition(sf::Vector2f(clampedX, sliderKnob->getPosition().y));
			if (volumeValue) 
			{
				int val = static_cast<int>(_volume + 0.5f);
				volumeValue->setString(std::to_string(val));
				CenterText(volumeValue);
				volumeValue->setPosition({ 
										_window.getSize().x * 0.5f + 75.f,
										_window.getSize().y * 0.5f - spacingY * 0.8f });
			}
		}
	}
	else if (const auto* mouseUp = ev.getIf<sf::Event::MouseButtonReleased>()) // [ADDED]
	{
		if (mouseUp->button == sf::Mouse::Button::Left)
		{
			_draggingKnob = false;
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
