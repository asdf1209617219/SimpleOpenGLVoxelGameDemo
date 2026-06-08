#include <dme/util/log.h>
#include <Windows.h>
namespace dme::util {
	static HANDLE _handle = GetStdHandle(STD_OUTPUT_HANDLE);

	void Log::SetConsoleTextColorDefault() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
	void Log::SetConsoleTextColorGrey() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY);
	}
	void Log::SetConsoleTextColorBlue() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	}
	void Log::SetConsoleTextColorGreen() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	}
	void Log::SetConsoleTextColorRed() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_RED);
	}
	void Log::SetConsoleTextColorCyan() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}
	void Log::SetConsoleTextColorPink() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED);
	}
	void Log::SetConsoleTextColorYellow() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
	}
	void Log::SetConsoleTextColorWhite() noexcept {
		SetConsoleTextAttribute(_handle, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

}