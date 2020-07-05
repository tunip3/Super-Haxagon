#ifndef SUPER_HAXAGON_AUDIO_WIN_HPP
#define SUPER_HAXAGON_AUDIO_WIN_HPP

#include "Audio.hpp"

namespace SuperHaxagon {
	class AudioWin : public Audio {
	public:
		explicit AudioWin(const std::string& path);
		~AudioWin();

		std::unique_ptr<Player> instantiate() override;
	};
}

#endif //SUPER_HAXAGON_AUDIO_WIN_HPP
