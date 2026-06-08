#include <dme/graph/buffer/frame_buffer.h>

namespace dme::graph {

	FBO::FBO() : target(0), result(0), rbo(0), texMultisample(), tex(), width(0), height(0) {}
	FBO::FBO(FBO&& a) noexcept : target(a.target), result(a.result), rbo(a.rbo),
		texMultisample(std::move(a.texMultisample)), tex(std::move(a.tex)),
		width(a.width), height(a.height) {
		a.target = 0;
		a.result = 0;
		a.rbo = 0;
		a.width = 0;
		a.height = 0;
	}
	FBO::~FBO() {
		if (target) {
			glDeleteRenderbuffers(1, &rbo);
			glDeleteFramebuffers(2, &target);

			target = 0;
			result = 0;
			rbo = 0;
			width = 0;
			height = 0;
			texMultisample.release();
			tex.release();
		}
	}

	void FBO::genTexRenderbuffer(sint width_, sint height_) {
		width = width_;
		height = height_;
		//创建纹理
		tex.init();
		texMultisample.init();
		glBindFramebuffer(GL_FRAMEBUFFER, target);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texMultisample.getId());
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Content::MSAALevel, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		//将创建的纹理附加到帧缓冲上
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texMultisample.getId(), 0);

		//创建渲染缓冲对象
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, Content::MSAALevel, GL_DEPTH24_STENCIL8, width, height); //启用抗锯齿
		//渲染缓冲对象附加到FBO的深度和模板附件上
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		//验证完整性
		check();
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, result);
		glBindTexture(GL_TEXTURE_2D, tex.getId());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.getId(), 0);

		//启用深度测试与混合
		Content::EnableDepthTest();
		Content::EnableBlend();
		//设置清除颜色
		Content::SetDefaultClearColor();
	}

	void FBO::resize(sint width_, sint height_) {
		if (tex.notNull()) {
			if (width != width_ || height != height_) {
				//删除纹理
				tex.release();
				//删除渲染缓冲对象
				glDeleteRenderbuffers(1, &rbo);
				genTexRenderbuffer(width_, height_);
			}
		}
		else {
			init();
		}
	}
	bool FBO::check() const {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Log::Error("Framebuffer: framebuffer is not complete!");
			return false;
		}
		return true;
	}

	Hive<FBO> _pool = Hive<FBO>();

	FBO& FBO::Allocate() {
		Hive<FBO>::comp_ptr ptr = _pool.add(FBO()); // TODO 由于返回的是压缩指针，所以这里将压缩指针变为原始指针后无法通过Hive删除对象了，只能Hive自动析构时回收内存
		return *ptr;
	}
	FBO& FBO::Create() {
		Hive<FBO>::comp_ptr ptr = _pool.add(FBO());
		ptr->init();
		return *ptr;
	}

	void FBO::init() {
		if (tex.isNull()) {
			//创建帧缓冲
			glGenFramebuffers(2, &target);
			//创建纹理与Renderbuffer
			sint width = Game::GetWinWidth();
			sint height = Game::GetWinHeight();
			genTexRenderbuffer(width, height);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
	void FBO::startRender() const {
		glBindFramebuffer(GL_FRAMEBUFFER, target);
		Content::ClearColorDepthStencilBuffer(); //清除fbo的缓冲
	}
	void FBO::endRender() const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, target);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, result);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	Tex2d FBO::getTex() const {
		return tex;
	}
	Tex2dMultisample FBO::getTexMultisample() const {
		return texMultisample;
	}
	void FBO::UpdateWinSize(sint width_, sint height_) {
		for (auto& obj : _pool) {
			obj.resize(width_, height_);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}