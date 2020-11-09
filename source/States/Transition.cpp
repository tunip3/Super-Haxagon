#include "../../include/States/Transition.hpp"

#include "../../include/Core/Game.hpp"
#include "../../include/Driver/Platform.hpp"
#include "../../include/Driver/Font.hpp"
#include "../../include/Factories/Level.hpp"
#include "../../include/States/Play.hpp"
#include "../../include/States/Quit.hpp"

namespace SuperHaxagon {

	Transition::Transition(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, const double score) :
		_game(game),
		_platform(game.getPlatform()),
		_selected(selected),
		_level(std::move(level)),
		_score(score)
	{}

	Transition::~Transition() = default;

	void Transition::enter() {
		_platform.playSFX(_game.getSFXWonderful());
	}

	std::unique_ptr<State> Transition::update(const double dilation) {
		_frames += dilation;
		_level->rotate(_level->getLevelFactory().getSpeedRotation(), dilation);
		_level->clamp();

		const auto press = _platform.getPressed();
		if (press.quit) return std::make_unique<Quit>(_game);

		if (_frames <= TRANSITION_FRAMES) {
			_offset *= TRANSITION_ACCELERATION_RATE * dilation + 1.0;
		}

		if (_frames >= TRANSITION_FRAMES) {
			// In order to get here _game.getLevels()[next] must be valid,
			// as the previous state verified this. If it does not exist, the
			// game just keeps going
			const auto next = _level->getLevelFactory().getNextIndex();
			auto& factory = *_game.getLevels()[next];
			_game.loadBGMAudio(factory);
			return std::make_unique<Play>(_game, factory, _selected, _score);
		}

		return nullptr;
	}

	void Transition::drawTop(const double scale) {
		_level->draw(_game, scale, _offset);
	}

	void Transition::drawBot(const double scale) {
		auto& large = _game.getFontLarge();
		large.setScale(scale);

		const auto* const text = "WONDERFUL";
		const auto pad = 6 * scale;
		const auto width = large.getWidth(text);
		const auto center = _platform.getScreenDim().x / 2;

		const Point posText = {center, pad};
		const Point bkgSize = {width + pad * 2, large.getHeight() + pad * 2};
		const std::vector<Point> trap = {
			{center - bkgSize.x/2 - bkgSize.y/2, 0},
			{center + bkgSize.x/2 + bkgSize.y/2, 0},
			{center + bkgSize.x/2, bkgSize.y},
			{center - bkgSize.x/2, bkgSize.y},
		};

		const auto percent = getPulse(_frames, Play::PULSE_TIME, 0);
		const auto pulse = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
		_platform.drawPoly(COLOR_TRANSPARENT, trap);
		large.draw(pulse, posText, Alignment::CENTER, text);
	}
}
