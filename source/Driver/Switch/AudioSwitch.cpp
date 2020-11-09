#include "../../../include/Driver/Switch/AudioSwitch.hpp"

#include "../../../include/Driver/Platform.hpp"
#include "../../../include/Driver/Switch/PlayerMusSwitch.hpp"
#include "../../../include/Driver/Switch/PlayerSfxSwitch.hpp"

namespace SuperHaxagon {

	AudioSwitch::AudioSwitch(Platform& platform, const std::string& path, const Stream stream) {
		if (stream == Stream::DIRECT) _sfx = Mix_LoadWAV((path + ".wav").c_str());
		if (stream == Stream::INDIRECT) _music = Mix_LoadMUS((path + ".ogg").c_str());

		if (!_music && stream == Stream::INDIRECT) {
			platform.message(Dbg::WARN, "music", Mix_GetError());
		}
	}

	AudioSwitch::~AudioSwitch() {
		if (_sfx) Mix_FreeChunk(_sfx);
		if (_music) Mix_FreeMusic(_music);
	}

	std::unique_ptr<Player> AudioSwitch::instantiate() {
		if (_sfx) return std::make_unique<PlayerSfxSwitch>(_sfx);
		if (_music) return std::make_unique<PlayerMusSwitch>(_music);
		return nullptr;
	}
}
