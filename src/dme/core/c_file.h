#pragma once
#include <dme/core/utils.h>
namespace dme {
	//文件类相关操作
	struct CFile {
	public:
		FILE* file;
		errno_t err;

		CFile() noexcept : file(null), err(0) {}
		CFile(FILE* file, errno_t err = 0) noexcept : file(file), err(err) {}

		//关闭文件
		void close() noexcept {
			::fclose(file);
			file = null;
		}
		//错误信息
		stdstr errStr() const noexcept {
			#if defined(DME_VC)
			stdstr msg;
			msg.reserve(100);
			strerror_s(msg.data(), 100, err);
			return msg;
			#else
			return ::strerror(err);
			#endif
		}
		//获取上一个错误信息
		stdstr lastErrStr() const noexcept {
			#if defined(DME_VC)
			stdstr msg;
			msg.reserve(100);
			strerror_s(msg.data(), 100, errno);
			return msg;
			#else
			return ::strerror(errno);
			#endif
		}

		//打开文件
		CFile Open(const char* filename, const char* mode) {
			CFile f;
			#if defined(DME_VC)
			f.err = ::fopen_s(&f.file, filename, mode);
			#else
			f.file = ::fopen(filename, mode);
			if(!f.file) {
				f.err = errno;
			}
			#endif
			return f;
		}
	};
}