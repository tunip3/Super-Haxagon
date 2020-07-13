#ifndef SUPER_HAXAGON_FONT_SWITCH_HPP
#define SUPER_HAXAGON_FONT_SWITCH_HPP

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class PlatformSFML;

	class FontSwitch : public Font {
	public:
		FontSwitch(PlatformSFML& platform, const std::string& path, double size);
		~FontSwitch() override;

		void setScale(double scale) override;
		double getHeight() const override;
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) const override;

	private:
		PlatformSFML& _platform;
		sf::Font _font;
		bool _loaded = false;
		double _scale;
		double _size;
	};
}

#endif //SUPER_HAXAGON_FONT_SWITCH_HPP
