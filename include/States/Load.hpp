#ifndef SUPER_HAXAGON_LOAD_HPP
#define SUPER_HAXAGON_LOAD_HPP

#include "State.hpp"

#include "../Core/Structs.hpp"

namespace SuperHaxagon {
	class Game;
	class Platform;

	class Load : public State {
	public:
		static const char* PROJECT_HEADER;
		static const char* PROJECT_FOOTER;
		static const char* SCORE_HEADER;
		static const char* SCORE_FOOTER;

		explicit Load(Game& game);
		Load(Load&) = delete;
		~Load() override;

		bool loadFile(std::ifstream& file, LocLevel location) const;
		bool loadScores(std::ifstream& file) const;

		std::unique_ptr<State> update(double dilation) override;
		void enter() override;
		void drawTop(double) override {};
		void drawBot(double) override {};

	private:
		Game& _game;
		Platform& _platform;
		bool _loaded = false;
	};
}

#endif //SUPER_HAXAGON_LOAD_HPP
