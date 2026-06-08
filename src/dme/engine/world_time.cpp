#include <dme/engine/world_time.h>
namespace dme::eng {
	WorldTime::WorldTime() noexcept : data(0) {}
	WorldTime::WorldTime(ulong data) noexcept : data(data) {}
	WorldTime::WorldTime(uint year, uint month, uint day, uint hour, uint second, uint frame) noexcept
		: data(
			((year& year_mask) << year_shift)
			| ((month & month_mask) << month_shift)
			| ((day & day_mask) << day_shift)
			| ((hour & hour_mask) << hour_shift)
			| ((second & second_mask) << second_shift)
			| (frame & frame_mask)
		) {
	}

	uint WorldTime::getYear() const noexcept {
		return static_cast<uint>(data >> year_shift);
	}
	uint WorldTime::getMonth() const noexcept {
		return (data & month_shift_mask) >> month_shift;
	}
	uint WorldTime::getDay() const noexcept {
		return (data & day_shift_mask) >> day_shift;
	}
	uint WorldTime::getHour() const noexcept {
		return (data & hour_shift_mask) >> hour_shift;
	}
	uint WorldTime::getSecond() const noexcept {
		return (data & second_shift_mask) >> second_shift;
	}
	uint WorldTime::getFrame() const noexcept {
		return data & frame_mask;
	}
	ulong WorldTime::getAllFrame() const noexcept {
		return data;
	}
	float WorldTime::getDayRate() const noexcept {
		return static_cast<float>(data & day_rate_mask) / static_cast<float>(one_day);
	}
	void WorldTime::update() noexcept {
		data++;
	}

	std::ostream& eng::operator<<(std::ostream& os, const WorldTime& a) {
		return os << "year: " << a.getYear()
			<< ", month: " << a.getMonth()
			<< ", day: " << a.getDay()
			<< ", hour: " << a.getHour()
			<< ", second: " << a.getSecond()
			<< ", frame: " << a.getFrame();
	}

}