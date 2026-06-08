#pragma once
#include <dme/core/type_traits.h>
namespace dme {
	struct Config : Unconstructible {
		//默认内存对齐，必须是size_t的倍数
		static constexpr uint memory_alignment = size_t_size * 2;
		//默认容器初始化容量
		static constexpr uint default_init_size = 2;
		//默认世界为0
		static constexpr sint default_world = 0;
		//区块边长，不允许超过32（因为32占5bit，xyz就占15bit，而Cbpos是ushort类型）；由于shader中也需要知道chunk边长，所以如果需要修改不光要改这里也要改shader
		static constexpr uint chunk_length = 16;
		static_assert(chunk_length <= 32 && chunk_length >= 4, "Config::chunk_length is not allowed to exceed 32 or be less than 4");
		//逻辑帧每秒帧数
		static constexpr uint logic_fps = 32;
		//默认窗口宽度
		static constexpr sint default_win_width = 800;
		//默认窗口高度
		static constexpr sint default_win_height = 800;


		//资源文件夹路径
		static constexpr const char* path_assets = DME_PATH_ASSETS;
		//block纹理路径
		static constexpr const char* path_tex_block = DME_PATH_TEX_BLOCK;
		//item纹理路径
		static constexpr const char* path_tex_item = DME_PATH_TEX_ITEM;
		//skybox路径
		static constexpr const char* path_tex_skybox = DME_PATH_TEX_SKYBOX;
		//ui纹理路径
		static constexpr const char* path_tex_ui = DME_PATH_TEX_UI;

		//block shader路径
		static constexpr const char* path_shader_block = DME_PATH_SHADER_BLOCK;
		//ui shader路径
		static constexpr const char* path_shader_ui = DME_PATH_SHADER_UI;
	};
}