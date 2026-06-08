#pragma once
#include <dme/graph/vertex/vertex_type.h>
#include <dme/graph/texture/tex2d.h>
#include <dme/graph/texture/tex2d_multisample.h>

namespace dme::graph {
	//Framebuffer Object，默认是启用深度测试的
	class FBO {
		friend class Game;
	private:
		uint target; //绑定多重采样纹理的FBO
		uint result; //绑定还原后的纹理的FBO
		uint rbo; //Renderbuffer Object
		Tex2dMultisample texMultisample;
		Tex2d tex;
		sint width; //buffer的宽度
		sint height; //buffer的高度

		FBO();

		void genTexRenderbuffer(sint width, sint height);
		void resize(sint width, sint height);

		bool check() const; //检验buffer完整性（调用前先调用bind）
	public:
		FBO(FBO&& a) noexcept;
		~FBO();
		//仅申请空间，返回引用
		static FBO& Allocate();
		//创建并初始化，返回引用
		static FBO& Create();

		void init(); //创建buffer，分配id

		//绑定当前buffer，开始对帧缓冲进行离屏渲染
		void startRender() const;
		//结束离屏渲染，并将渲染结果输入到材质中，切换回默认的帧缓冲
		void endRender() const;
		//[[nodiscard]]
		//void createRbo() ; //为当前buffer创建纹理和渲染缓冲对象

		//获取纹理
		Tex2d getTex() const;
		//获取多重采样纹理
		Tex2dMultisample getTexMultisample() const;

		//窗口大小更新时调用
		static void UpdateWinSize(sint width, sint height);
	};

}