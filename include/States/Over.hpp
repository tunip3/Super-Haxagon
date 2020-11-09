#ifndef SUPER_HAXAGON_OVER_HPP
#define SUPER_HAXAGON_OVER_HPP

#include "State.hpp"

#include "../Core/Structs.hpp"

namespace SuperHaxagon {
	class Level;
	class LevelFactory;
	class Game;
	class Platform;

	class Over : public State {
	public:
		static constexpr double GAME_OVER_ROT_SPEED = TAU/400.0;
		static constexpr double GAME_OVER_ACCELERATION_RATE = 0.1;
		static constexpr int FRAMES_PER_GAME_OVER = 60;
		static constexpr int PULSE_TIME = 75;

		Over(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, double score, std::string text);
		Over(Over&) = delete;
		~Over() override;

		std::unique_ptr<State> update(double dilation) override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;
		void enter() override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _selected;
		std::unique_ptr<Level> _level;
		std::string _text = "GAME OVER";

		bool _high = false;
		double _score = 0;
		double _frames = 0;
		double _offset = 1.0;
	};
}

#endif //SUPER_HAXAGON_OVER_HPP
