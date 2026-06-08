#pragma once
#include <dme/core/hash.h>
namespace dme {
	namespace util {
#define _DME_LOG_PARAM_INFO_ARGS_(param) #param ": ", (param), "; "
#define DME_LOG_PARAM_INFO(...) ::dme::util::Log::Info(DME_EVAL_FOREACH(_DME_LOG_PARAM_INFO_ARGS_, __VA_ARGS__));

		class Log : Unconstructible {
			template<typename... Args, size_t... Indices>
			static inline void splitImpl(const char* split, std::index_sequence<Indices...>, const Args&... args) noexcept {
				if constexpr (sizeof...(Args) == 0) {
					std::cout << std::endl;
				}
				else {
					((std::cout << args << (Indices + 1 == sizeof...(Args) ? "" : split)), ...) << std::endl;
				}
			}
		public:
			static void SetConsoleTextColorDefault() noexcept;
			static void SetConsoleTextColorGrey() noexcept;
			static void SetConsoleTextColorBlue() noexcept;
			static void SetConsoleTextColorGreen() noexcept;
			static void SetConsoleTextColorRed() noexcept;
			static void SetConsoleTextColorCyan() noexcept;
			static void SetConsoleTextColorPink() noexcept;
			static void SetConsoleTextColorYellow() noexcept;
			static void SetConsoleTextColorWhite() noexcept;

			template<typename... Args>
			static inline void Info(const Args&... args) noexcept {
				std::cout << "[Info] ";
				(std::cout << ... << args) << std::endl;
			}
			template<typename... Args>
			static inline void InfoSplit(const char* split, const Args&... args) noexcept {
				std::cout << "[Info] ";
				splitImpl(split, std::index_sequence_for<Args...>(), args...);
			}
			template<typename... Args>
			static inline void Warning(const Args&... args) noexcept {
				SetConsoleTextColorYellow();
				std::cout << "[Warning] ";
				SetConsoleTextColorDefault();
				(std::cout << ... << args) << std::endl;
			}
			template<typename... Args>
			static inline void WarningSplit(const char* split, const Args&... args) noexcept {
				SetConsoleTextColorYellow();
				std::cout << "[Warning] ";
				SetConsoleTextColorDefault();
				splitImpl(split, std::index_sequence_for<Args...>(), args...);
			}
			template<typename... Args>
			static inline void Error(const Args&... args) noexcept {
				SetConsoleTextColorRed();
				std::cout << "[Error] ";
				SetConsoleTextColorDefault();
				(std::cout << ... << args) << std::endl;
			}
			template<typename... Args>
			static inline void ErrorSplit(const char* split, const Args&... args) noexcept {
				SetConsoleTextColorRed();
				std::cout << "[Error] ";
				SetConsoleTextColorDefault();
				splitImpl(split, std::index_sequence_for<Args...>(), args...);
			}
		};

	}
	using util::Log;

}