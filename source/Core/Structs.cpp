#include "../../include/Core/Structs.hpp"

#include "../../include/Driver/Platform.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace SuperHaxagon {
	uint8_t clamp(const double v) {
		if (v < 0) return 0;
		if (v > 255) return 255;
		return static_cast<uint8_t>(v);
	}
	
	Color interpolateColor(const Color& one, const Color& two, const double percent) {
		Color result{};
		result.r = static_cast<uint8_t>(linear(one.r, two.r, percent));
		result.g = static_cast<uint8_t>(linear(one.g, two.g, percent));
		result.b = static_cast<uint8_t>(linear(one.b, two.b, percent));
		result.a = static_cast<uint8_t>(linear(one.a, two.a, percent));
		return result;
	}

	// Source: https://stackoverflow.com/a/30488508
	Color rotateColor(const Color& in, const double degrees) {
		Color out{};
		const auto cosA = cos(degrees * PI / 180.0);
		const auto sinA = sin(degrees * PI / 180.0);

		// Calculate the rotation matrix, only depends on Hue
		// This means I COULD cache it, but I won't.
		double matrix[3][3] = {
			{cosA + (1.0 - cosA) / 3.0, 1.0 / 3.0 * (1.0 - cosA) - sqrt(1.0 / 3.0) * sinA, 1.0 / 3.0 * (1.0 - cosA) + sqrt(1.0 / 3.0) * sinA},
			{1.0 / 3.0 * (1.0 - cosA) + sqrt(1.0 / 3.0) * sinA, cosA + 1.0 / 3.0 * (1.0 - cosA), 1.0 / 3.0 * (1.0 - cosA) - sqrt(1.0 / 3.0) * sinA},
			{1.0 / 3.0 * (1.0 - cosA) - sqrt(1.0 / 3.0) * sinA, 1.0 / 3.0 * (1.0 - cosA) + sqrt(1.0 / 3.0) * sinA, cosA + 1.0 / 3.0 * (1.0 - cosA)}
		};

		// Use the rotation matrix to convert the RGB directly
		out.r = clamp(in.r * matrix[0][0] + in.g * matrix[0][1] + in.b * matrix[0][2]);
		out.g = clamp(in.r * matrix[1][0] + in.g * matrix[1][1] + in.b * matrix[1][2]);
		out.b = clamp(in.r * matrix[2][0] + in.g * matrix[2][1] + in.b * matrix[2][2]);
		out.a = in.a;
		return out;
	}

	double linear(const double start, const double end, const double percent) {
		return (end - start) * percent + start;
	}

	Point rotateAroundOrigin(const Point& point, const double rotation) {
		const auto c = cos(rotation);
		const auto s = sin(rotation + PI);
		return { point.x * c - point.y * s,  point.x * s + point.y * c };
	}

	std::string getTime(const double score) {
		std::stringstream buffer;
		const auto scoreInt = static_cast<int>(score / 60.0);
		const auto decimalPart = static_cast<int>((score / 60.0 - scoreInt) * 100.0);
		buffer << std::fixed << std::setfill('0')
			<< std::setprecision(3) << std::setw(3)
			<< scoreInt
			<< "."
			<< std::setprecision(2) << std::setw(2)
			<< decimalPart;
		return buffer.str();
	}

	double getPulse(double frame, const double range, const double start) {
		frame -= start;
		// Alternate algorithm:
		//double percent = sin((double)frame * <speed>) / 2.0 + 0.5;
		return fabs(((frame - floor(frame / (range * 2)) * range * 2) - range) / range);
	}

	const char* getScoreText(const int score, const bool reduced) {
	
		if (score < 15 * 60) return "POINT";
		if (score < 30 * 60) return "LINE";
		if (score < 40 * 60) return reduced ? "TRI" : "TRIANGLE";
		if (score < 50 * 60) return "SQUARE";
		if (score < 60 * 60) return reduced ? "PENTA" : "PENTAGON";
		if (score < 70 * 60) return reduced ? "HEXA"  : "HEXAGON";
		if (score < 80 * 60) return reduced ? "HEPTA" : "HEPTAGON";
		if (score < 90 * 60 + 10) return reduced ? "OCTA" : "OCTAGON";
		return "WONDERFUL";
	}

	bool readCompare(std::ifstream& file, const std::string& str) {
		const auto read = std::make_unique<char[]>(str.length() + 1);
		file.read(read.get(), str.length());
		read[str.length()] = '\0';
		return read.get() == str;
	}

	int32_t read32(std::ifstream& file, const int32_t min, const int32_t max, Platform& platform, const std::string& noun) {
		int32_t num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));

		if (num < min) {
			num = min;
			platform.message(Dbg::WARN, "int", noun + " is too small, but continuing anyway.");
		}

		if (num > max) {
			num = max;
			platform.message(Dbg::WARN, "int", noun + " is too large, but continuing anyway.");
		}

		return num;
	}

	int16_t read16(std::ifstream& file) {
		int16_t num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));
		return num;
	}

	float readFloat(std::ifstream& file) {
		float num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));
		return num;
	}

	Color readColor(std::ifstream& file) {
		Color color{};
		file.read(reinterpret_cast<char*>(&color.r), sizeof(color.r));
		file.read(reinterpret_cast<char*>(&color.g), sizeof(color.g));
		file.read(reinterpret_cast<char*>(&color.b), sizeof(color.b));
		color.a = 0xFF;
		return color;
	}

	std::string readString(std::ifstream& file, Platform& platform, const std::string& noun) {
		const size_t length = read32(file, 1, 300, platform, noun + " string");
		const auto read = std::make_unique<char[]>(length + 1);
		file.read(read.get(), length);
		read[length] = '\0';
		return std::string(read.get());
	}

	void writeString(std::ofstream& file, const std::string& str) {
		auto len = static_cast<uint32_t>(str.length());
		file.write(reinterpret_cast<char*>(&len), sizeof(len));
		file.write(str.c_str(), str.length());
	}
}