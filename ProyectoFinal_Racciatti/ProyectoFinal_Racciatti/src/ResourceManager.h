#pragma once

#include <map>
#include <string>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	sf::Texture& GetTexture(const std::string& path, bool useMipmap, sf::IntRect area);
	sf::Font& GetFont(const std::string& path);
	sf::SoundBuffer& GetSound(const std::string& path);

private:
	std::map<std::string, sf::Texture*> texturesMap;
	std::map<std::string, sf::Font*> fontsMap;
	std::map<std::string, sf::SoundBuffer*> soundsMap;

};
