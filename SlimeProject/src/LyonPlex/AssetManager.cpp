#include "pch.h"

#include "AssetManager.h"

void AssetManager::LoadErrorResponses()
{
	AddTexture("error", "../../../res/error.jpg");
	AddSound("error", "../../../res/error.wav");
}

bool AssetManager::AddTexture(const std::string& textureName, const std::string& texturePath, const sf::IntRect& area)
{
	if (m_textureList.find(textureName) != m_textureList.end())
	{
		std::cout << "Texture " << textureName << " already exists" << std::endl;
		return false;
	}

	sf::Texture* texture = new sf::Texture();

	if (area != sf::IntRect())
	{
		if (!texture->loadFromFile(texturePath, area))
		{
			std::cout << "Texture " << textureName << " (Rect) failed to load" << std::endl;
			delete texture;
			return false;
		}
		else
			std::cout << "Texture "<< textureName <<" (Rect) loaded succesfully" << std::endl;
	}
	else
	{
		if (!texture->loadFromFile(texturePath))
		{
			std::cout << "Texture " << textureName << " (All) failed to load" << std::endl;
			delete texture;
			return false;
		}
		else
			std::cout << "Texture " << textureName << " (All) loaded succesfully" << std::endl;
	}

	m_textureList.emplace(textureName, texture);
	return true;
}

const sf::Texture& AssetManager::GetTexture(const std::string& textureName, const sf::IntRect& area)
{
	auto textureItr = m_textureList.find(textureName);
	if (textureItr == m_textureList.end())
	{
		std::cout << "Texture '" << textureName << "' cannot be found" << std::endl;
		return *m_textureList["error"];
	}

	return *textureItr->second;
}

bool AssetManager::AddSound(const std::string& soundName, const std::string& soundPath)
{
	if (m_soundBufferList.find(soundName) != m_soundBufferList.end())
	{
		std::cout << "Sound " << soundName << " already exists" << std::endl;
		return false;
	}

	sf::SoundBuffer* soundBuffer = new sf::SoundBuffer();
	if (!soundBuffer->loadFromFile(soundPath))
	{
		wchar_t debugBuffer[512];
		swprintf_s(debugBuffer, 512, L"LoadFromFile failed for path: %hs\n", soundPath.c_str());
		OutputDebugString(debugBuffer);

		std::cout << "Sound " << soundName << " failed to load from " << soundPath << std::endl;
		delete soundBuffer;
		return false;
	}
	else
	{
		wchar_t debugBuffer[512];
		swprintf_s(debugBuffer, 512, L"LoadFromFile succeeded for path: %hs\n", soundPath.c_str());
		OutputDebugString(debugBuffer);

		std::cout << "Sound " << soundName << " loaded successfully from " << soundPath << std::endl;
	}

	m_soundBufferList.emplace(soundName, std::move(soundBuffer));

	sf::Sound* sound = new sf::Sound();
	if (m_soundBufferList.find(soundName) == m_soundBufferList.end())
	{
		std::cout << "SoundBuffer not found for " << soundName << std::endl;
		OutputDebugString(L"SoundBuffer not found\n");
		delete sound;
		return false;
	}

	sound->setBuffer(*m_soundBufferList[soundName]);
	m_soundList.emplace(soundName, sound);
	return true;
}

const sf::Sound& AssetManager::GetSound(const std::string& soundName)
{
	auto soundItr = m_soundList.find(soundName);
	if (soundItr == m_soundList.end())
	{
		std::cout << "Sound '" << soundName << "' cannot be found" << std::endl;
		return *m_soundList["error"];
	}
	return *soundItr->second;
}

void AssetManager::PlayLocalSound(const std::string& soundName)
{
	auto it = m_soundBufferList.find(soundName);
	if (it == m_soundBufferList.end() || it->second == nullptr)
	{
		OutputDebugString(L"PlayLocalSound: SoundBuffer not found or null.\n");
		return;
	}

	// Cree dynamiquement un nouvel objet sf::Sound
	sf::Sound* newSound = new sf::Sound();
	newSound->setBuffer(*it->second);
	newSound->play();

	// Stocke le son dans une liste pour eviter qu'il soit supprime immediatement
	m_activeSounds.push_back(newSound);

	// Nettoie les sons termines
	m_activeSounds.remove_if([](sf::Sound* sound) 
	{
		if (sound->getStatus() == sf::Sound::Stopped) 
		{
			delete sound;  // Libere la memoire
			return true;    // Supprime de la liste
		}
		return false;
	});
}



bool AssetManager::AddMusic(const std::string& musicName, const std::string& musicPath)
{
	if (m_musicList.find(musicName) != m_musicList.end())
	{
		std::cout << "Music " << musicName << " already exists" << std::endl;
		return false;
	}

	sf::Music* music = new sf::Music();
	if (!music->openFromFile(musicPath))
	{
		std::cout << "Music " << musicName << " failed to load" << std::endl;
		delete music;
		return false;
	}
	std::cout << "Music " << musicName << " loaded successfully" << std::endl;

	m_musicList.emplace(musicName, music);

	return true;
}


sf::Music& AssetManager::GetMusic(const std::string& musicName)
{
	return *m_musicList.at(musicName);
}

void AssetManager::ReleaseAll()
{
	for (auto& pair : m_textureList) {
		delete pair.second;
	}
	m_textureList.clear();

	for (auto& pair : m_soundBufferList) {
		delete pair.second;
	}
	m_soundBufferList.clear();

	for (auto& pair : m_soundList) {
		delete pair.second;
	}
	m_soundList.clear();

	for (auto& pair : m_musicList) {
		delete pair.second;
	}
	m_musicList.clear();

	std::cout << "All assets have been released." << std::endl;
}

std::string AssetManager::GetExecutablePath()
{//lyo-t2-atari-p7-07
	std::string currentFile = __FILE__;           // Recupere le chemin complet du fichier actuel
	size_t pos = currentFile.find("\\src\\LyonPlex");       // Trouve "/src/" dans le chemin
	if (pos != std::string::npos) {
		return currentFile.substr(0, pos) + "\\";  // Garde la partie avant "/src/"
	}
	return "";
}
