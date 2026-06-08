#pragma once
#include <dme/core/all.h>
namespace dme::eng {
	enum class OperatingSystemName {
		WINDOWS = 0,
	};
	enum class OperatingSystemVersion {
		v10 = 10,
	};

	enum class GraphicsAPIName {
		Directx = 0,
		OpenGL = 1,
	};
	enum class GraphicsAPIVersion {
		v11 = 11,
		v330 = 330,
	};

	struct OperatingSystem {
		OperatingSystemName operatingSystemName;
		OperatingSystemVersion operatingSystemVersion;
	};
	struct GraphicsAPI {
		GraphicsAPIName graphicsAPIName;
		GraphicsAPIVersion graphicsAPIVersion;
	};
	struct GamePlatform {
		OperatingSystem operatingSystem;
		GraphicsAPI graphicsAPI;
	};

	inline GraphicsAPI OpenGL330 = {
		.graphicsAPIName = GraphicsAPIName::OpenGL,
		.graphicsAPIVersion = GraphicsAPIVersion::v330,
	};
	inline OperatingSystem WIN10 = {
		.operatingSystemName = OperatingSystemName::WINDOWS,
		.operatingSystemVersion = OperatingSystemVersion::v10,
	};
	inline GamePlatform _gamePlatform = {
		.operatingSystem = WIN10,
		.graphicsAPI = OpenGL330
	};
	inline GamePlatform* const Platform = &_gamePlatform;
}