#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

sf::Texture& ResourceManager::GetTexture(const std::string& path, bool useMipmap, sf::IntRect area /*opcional pero sirve para anims*/)
{
	auto iterator = texturesMap.find(path);
	if (iterator == texturesMap.end())
	{
		sf::Texture* texture = new sf::Texture();

		if (!texture->loadFromFile(path, useMipmap, area))
		{
			delete texture;
			throw std::runtime_error("No se pudo cargar la textura: " + path);
		}
		iterator = texturesMap.emplace(path, texture).first;
	}
	return *iterator->second;  // Ponemos el puntero en iterator porque definimos el map para tener una key con puntero
}

sf::Font& ResourceManager::GetFont(const std::string& path)
{
	auto iterator = fontsMap.find(path);
	if (iterator == fontsMap.end())
	{
		sf::Font* font = new sf::Font();

		if (!font->openFromFile(path))
		{
			delete font;
			throw std::runtime_error("No se pudo cargar la fuente: " + path);
		}
		iterator = fontsMap.emplace(path, font).first;
	}
	return *iterator->second;
}

sf::SoundBuffer& ResourceManager::GetSound(const std::string& path)
{
	auto iterator = soundsMap.find(path);
	if (iterator == soundsMap.end())
	{
		sf::SoundBuffer* buffer = new sf::SoundBuffer();

		if (!buffer->loadFromFile(path))
		{
			delete buffer;
			throw std::runtime_error("No se pudo cargar la fuente: " + path);
		}
		iterator = soundsMap.emplace(path, buffer).first;
	}
	return *iterator->second;
}
