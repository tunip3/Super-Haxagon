#include "../../../include/Driver/Win/PlatformWin.hpp"

#include "../../../include/Core/Twist.hpp"

#include <direct.h>
#include <iostream>
#include <windows.h>
#include <filesystem>

namespace SuperHaxagon {
	PlatformWin::PlatformWin(const Dbg dbg) : PlatformSFML(dbg, sf::VideoMode(
		static_cast<int>(sf::VideoMode::getDesktopMode().width * 0.75),
		static_cast<int>(sf::VideoMode::getDesktopMode().height * 0.75)
	)) {
		//SetForegroundWindow(getWindow().getSystemHandle());
		//_mkdir(".\\sdmc");
	}

	std::string PlatformWin::getPath(const std::string& partial) {
		auto path = partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		auto test = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
		return std::string(test->Begin(), test->End()) + path;
	}

	std::string PlatformWin::getPathRom(const std::string& partial) {
		auto path = partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return std::string(std::filesystem::current_path().string()) + path;
	}

	void PlatformWin::message(const Dbg dbg, const std::string& where, const std::string& message) {
		if (dbg == Dbg::INFO) {
			std::cout << "[win:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[win:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[win:fatal] " + where + ": " + message << std::endl;
			//MessageBox(nullptr, (where + ": " + message).c_str(), "A fatal error occurred!", MB_OK);
		}
	}

	std::unique_ptr<Twist> PlatformWin::getTwister() {
		try {
			std::random_device source;
			if (source.entropy() == 0) {
				message(Dbg::INFO, "rng", "entropy too low, using time");
				throw std::runtime_error("lacking entropy");
			}

			std::mt19937::result_type data[std::mt19937::state_size];
			generate(std::begin(data), std::end(data), ref(source));
			return std::make_unique<Twist>(
					std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
			);
		} catch (std::runtime_error&) {
			auto* a = new std::seed_seq{time(nullptr)};
			return std::make_unique<Twist>(
					std::unique_ptr<std::seed_seq>(a)
			);
		}
	}
}