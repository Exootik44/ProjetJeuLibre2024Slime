#pragma once
//#include <SFML/Audio.hpp>
//#include <SFML/Graphics/Texture.hpp>
//#include <unordered_map>
//#include <string>

class AssetManager
{

protected:

	// __ Textures __
	inline static std::unordered_map<std::string, sf::Texture*> m_textureList;

	// __ Sound effects __ 
	inline static std::unordered_map<std::string, sf::SoundBuffer*> m_soundBufferList;
	inline static std::unordered_map<std::string, sf::Sound*> m_soundList;
	inline static sf::Sound* m_errorSound;
	inline static std::list<sf::Sound*> m_activeSounds;  // Garde les sons en cours

	// __ Musics __ 
	inline static std::unordered_map<std::string, sf::Music*> m_musicList;


public:
	AssetManager();
	~AssetManager();

	// __ Loading error __ 
	static void LoadErrorResponses();

	// __ Textures __ 
	static bool AddTexture(const std::string& textureName, const std::string& texturePath, const sf::IntRect& area = sf::IntRect());
	static const sf::Texture& GetTexture(const std::string& textureName, const sf::IntRect& area = sf::IntRect());

	// __ Sound effects __ 
	static bool AddSound(const std::string& soundName, const std::string& soundPath);
	static const sf::Sound& GetSound(const std::string& soundName);
	static void PlayLocalSound(const std::string& soundName);

	// __ Musics __ 
	static bool AddMusic(const std::string& musicName, const std::string& musicPath);
	static sf::Music& GetMusic(const std::string& musicName);
	
	// __ Release memory __ 
	static void ReleaseAll();

	static std::string GetExecutablePath();
};

