#include "../../include/States/Load.hpp"

#include "../../include/Core/Game.hpp"
#include "../../include/Driver/Platform.hpp"
#include "../../include/Factories/Level.hpp"
#include "../../include/Factories/Pattern.hpp"
#include "../../include/States/Menu.hpp"
#include "../../include/States/Quit.hpp"

#include <memory>
#include <fstream>
#include <climits>
#include <filesystem>

namespace SuperHaxagon {
	const char* Load::PROJECT_HEADER = "HAX1.1";
	const char* Load::PROJECT_FOOTER = "ENDHAX";
	const char* Load::SCORE_HEADER = "SCDB1.0";
	const char* Load::SCORE_FOOTER = "ENDSCDB";

	Load::Load(Game& game) : _game(game), _platform(game.getPlatform()) {}
	Load::~Load() = default;

	bool Load::loadFile(std::ifstream& file, LocLevel location) const {
		std::vector<std::shared_ptr<PatternFactory>> patterns;

		// Used to make sure that external levels link correctly.
		const auto levelIndexOffset = _game.getLevels().size();

		if(!readCompare(file, PROJECT_HEADER)) {
			_platform.message(Dbg::WARN, "file", "file header invalid!");
			return false;
		}

		const auto numPatterns = read32(file, 1, 300, _platform, "number of patterns");
		patterns.reserve(numPatterns);
		for (auto i = 0; i < numPatterns; i++) {
			auto pattern = std::make_shared<PatternFactory>(file, _platform);
			if (!pattern->isLoaded()) {
				_platform.message(Dbg::WARN, "file", "a pattern failed to load");
				return false;
			}

			patterns.emplace_back(std::move(pattern));
		}

		if (patterns.empty()) {
			_platform.message(Dbg::WARN, "file", "no patterns loaded");
			return false;
		}

		const auto numLevels = read32(file, 1, 300, _platform, "number of levels");
		for (auto i = 0; i < numLevels; i++) {
			auto level = std::make_unique<LevelFactory>(file, patterns, location, _platform, levelIndexOffset);
			if (!level->isLoaded()) {
				_platform.message(Dbg::WARN, "file", "a level failed to load");
				return false;
			}

			_game.addLevel(std::move(level));
		}

		if(!readCompare(file, PROJECT_FOOTER)) {
			_platform.message(Dbg::WARN, "load", "file footer invalid");
			return false;
		}

		return true;
	}

	bool Load::loadScores(std::ifstream& file) const {
		if (!file) {
			_platform.message(Dbg::INFO, "scores", "no score database");
			return true;
		}

		if (!readCompare(file, SCORE_HEADER)) {
			_platform.message(Dbg::WARN,"scores", "score header invalid, skipping scores");
			return true; // If there is no score database silently fail.
		}

		const auto numScores = read32(file, 1, 300, _platform, "number of scores");
		for (auto i = 0; i < numScores; i++) {
			auto name = readString(file, _platform, "score level name");
			auto difficulty = readString(file, _platform, "score level difficulty");
			auto mode = readString(file, _platform, "score level mode");
			auto creator = readString(file, _platform, "score level creator");
			const auto score = read32(file, 0, INT_MAX, _platform, "score");
			for (const auto& level : _game.getLevels()) {
				if (level->getName() == name && level->getDifficulty() == difficulty && level->getMode() == mode && level->getCreator() == creator) {
					level->setHighScore(score);
				}
			}
		}

		if (!readCompare(file, SCORE_FOOTER)) {
			_platform.message(Dbg::WARN,"scores", "file footer invalid, db broken");
			return false;
		}

		return true;
	}

	void Load::enter() {
		std::vector<std::pair<LocLevel, std::string>> locations;
		locations.emplace_back(std::pair<LocLevel, std::string>(LocLevel::INTERNAL, _platform.getPathRom("/levels.haxagon")));

		std::string tempuwu = _platform.getPath("/");
		auto files = std::filesystem::directory_iterator(tempuwu);
		for (const auto& file : files) {
			if (file.path().extension() != ".haxagon") continue;
			_platform.message(Dbg::INFO, "load", "found " + file.path().string());
			locations.emplace_back(std::pair<LocLevel, std::string>(LocLevel::EXTERNAL, file.path().string()));
		}

		for (const auto& location : locations) {
			const auto& path = location.second;
			const auto loc = location.first;
			std::ifstream file(path, std::ios::in | std::ios::binary);
			if (!file) continue;
			loadFile(file, loc);
		}

		if (_game.getLevels().empty()) {
			_platform.message(Dbg::FATAL, "levels", "no levels loaded");
			return;
		}

		std::ifstream scores(_platform.getPath("/scores.db"), std::ios::in | std::ios::binary);
		if (!loadScores(scores)) return;

		_loaded = true;
	}

	std::unique_ptr<State> Load::update(double) {
		if (_loaded) return std::make_unique<Menu>(_game, *_game.getLevels()[0]);
		return std::make_unique<Quit>(_game);
	}
}
