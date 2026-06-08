#pragma once
#include <dme/core.h>
namespace dme::eng {

	//每个世界的时间，以太阳刚刚从地平线升起时为0点，太阳落下月亮升起时为8点
	// 1year = 16month, 1month = 16day, 1day = 16hour, 1hour = 64second, 1second = Config::logic_fps frame
	//TODO 目前逻辑帧与渲染帧分离没做好
	class WorldTime {
	private:
		ulong data; // ...y mmmm dddd hhhh ssssss fffff

		static constexpr uint second_per_hour = 64;
		static constexpr uint hour_per_day = 16;
		static constexpr uint day_per_month = 16;
		static constexpr uint month_per_year = 16;

		static constexpr ulong frame_mask = Config::logic_fps - 1;

		static constexpr uint second_shift = BitWidth(frame_mask);
		static constexpr ulong second_mask = second_per_hour - 1;
		static constexpr ulong second_shift_mask = second_mask << second_shift;

		static constexpr uint hour_shift = BitWidth(second_shift_mask);
		static constexpr ulong hour_mask = hour_per_day - 1;
		static constexpr ulong hour_shift_mask = hour_mask << hour_shift;

		static constexpr uint day_shift = BitWidth(hour_shift_mask);
		static constexpr ulong day_mask = day_per_month - 1;
		static constexpr ulong day_shift_mask = day_mask << day_shift;
		static constexpr ulong one_day = 1ui64 << day_shift;
		static constexpr ulong day_rate_mask = one_day - 1;

		static constexpr uint month_shift = BitWidth(day_shift_mask);
		static constexpr ulong month_mask = month_per_year - 1;
		static constexpr ulong month_shift_mask = month_mask << month_shift;

		static constexpr uint year_shift = BitWidth(month_shift_mask);
		static constexpr ulong year_shift_mask = ~BitFill(month_shift_mask);
		static constexpr ulong year_mask = year_shift_mask >> year_shift;
	public:
		WorldTime() noexcept;
		WorldTime(ulong data) noexcept;
		WorldTime(uint year, uint month, uint day, uint hour, uint second, uint frame) noexcept;

		//获取当前年
		uint getYear() const noexcept;
		//获取当前月
		uint getMonth() const noexcept;
		//获取当前日
		uint getDay() const noexcept;
		//获取当前时
		uint getHour() const noexcept;
		//获取当前秒
		uint getSecond() const noexcept;
		//获取当前帧
		uint getFrame() const noexcept;
		//获取总共过了几帧
		ulong getAllFrame() const noexcept;
		//获取当天过了一天时间的多少占比
		float getDayRate() const noexcept;
		//每帧调用该函数更新时间
		void update() noexcept;

		friend std::ostream& operator<<(std::ostream& os, const WorldTime& a);
	};

}