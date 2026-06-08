#include <dme/graph/texture/tex_atlas.h>
namespace dme::graph {

	TexAtlas::TexAtlas() noexcept : Tex(), depth(0), size(0) {}

	sint TexAtlas::getSize() const noexcept {
		return size;
	}

	TexIndex TexAtlas::addIfAbsent(strid path) {
		auto ptr = texIndexMap.get(path);
		if (!ptr) {
			auto data = Tex::LoadImage(path, TexTransform::N, TexChannel::RGBA);
			if (!data.data()) {
				return TexIndex();
			}
			if (data.width() > default_max_size || data.height() > default_max_size) {
				Log::Error("Texture: image size exceeds the texture atlas max size at path: ", path,
					"\nwidth: ", data.width(), ", height: ", data.height(), ", max size: ", default_max_size);
				return TexIndex();
			}
			ptr = texIndexMap.add(path, TexIndex(toui(texIndexMap.count())));
			auto d = texDataToIndex.add(std::move(data), ptr->val);
		}
		return ptr->val;
	}

	TexPos TexAtlas::getPos(strid path) const noexcept {
		auto texIndexPtr = texIndexMap.get(path);
		if (texIndexPtr == null) {
			return TexPos();
		}
		return texPosArr[texIndexPtr->val];
	}
	TexPos TexAtlas::getPos(TexIndex index) const noexcept {
		return texPosArr[index];
	}
	TexIndex TexAtlas::getIndex(strid path) const noexcept {
		auto texIndexPtr = texIndexMap.get(path);
		if (texIndexPtr == null) {
			return TexIndex();
		}
		return texIndexPtr->val;
	}
	
	//TODO 目前是采取直接对纹理数组增加一层的方法扩容
	TexAtlas::FreeRectX::value_type* TexAtlas::extend() {
		auto ptr = freeRect.add({ size, FreeRectY({{size, Int3(0, 0, depth)}}) });
		depth++;
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		constexpr sint glFormat = ChannelToGLFormat(TexChannel::RGBA);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, glFormat, size, size, depth, 0, glFormat, GL_UNSIGNED_BYTE, NULL);
		for (uint i = 0; i < dataArr.count(); ++i) {
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, size, size, 1, glFormat, GL_UNSIGNED_BYTE, dataArr[i].data());
		}
		dataArr.add(TexData::AllocTexData(size, size, TexChannel::RGBA));
		return ptr;
	}

	void TexAtlas::merge() {
		if (!size) {
			//size = math::Min(Content::GetMaxTextureSize(), 8192); //目前太大了导出需要太久了

			size = default_max_size;

			//申请纹理
			glGenTextures(1, &id.v);
			glBindTexture(GL_TEXTURE_2D_ARRAY, id);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			extend();
		}
		if (texDataToIndex.empty()) {
			return;
		}

		constexpr sint channel = static_cast<sint>(TexChannel::RGBA);
		constexpr sint glFormat = ChannelToGLFormat(TexChannel::RGBA);
		texPosArr.recount(toui(texIndexMap.count()));

		auto dataIt = texDataToIndex.end();
		auto dataStartIt = texDataToIndex.begin();
		while (dataIt != dataStartIt) {
			--dataIt;
			auto& pair = *dataIt;
			auto& subTexData = pair.key;
			Int2 wh = Int2(subTexData.width(), subTexData.height());
			FreeRectX::value_type* x = freeRect.getRangeMin(wh.x, sint_max);
			FreeRectY::value_type* y = null;
			auto it = freeRect.it(x);
			while (!it.isend()) {
				y = x->val.getRangeMin({ wh.y, Int3() }, { sint_max, Int3(sint_max) });
				if (y) {
					break;
				}
				++it;
				x = it.ptr();
			}
			if (!y) { //没有宽高足够的空闲矩形了
				x = extend();
				y = &x->val.first();
			}

			sint z = y->val.z;
			Int2 min = y->val.vxy;
			Int2 max = min + wh;
			auto& texData = dataArr[z];
			//写入显存
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, min.x, min.y, z, wh.x, wh.y, 1, glFormat, GL_UNSIGNED_BYTE, subTexData.data());
			//将像素写入到dataArr中
			for (sint i = 0; i < wh.y; ++i) {
				size_t line = tost(wh.x) * channel;
				MemcpyByte(texData.data() + (tost(min.y + i) * size + min.x) * channel,
					subTexData.data() + i * line, line);
			}

			if (x->key == wh.x) { //宽度覆盖
				if (y->key == wh.y) { //完全覆盖
					x->val.remove(*y);
					if (x->val.empty()) {
						freeRect.remove(x->key);
					}
				}
				else { //宽度覆盖高度没覆盖
					x->val.modify(*y, [&](FreeRectY::value_type& v) {
						v.val.vxy.y += wh.y;
						v.key -= wh.y;
					});
				}
			}
			else { //宽度没覆盖
				if (y->key == wh.y) { //宽度没覆盖高度覆盖
					sint oldWidth = x->key;
					sint newWidth = oldWidth - wh.x;
					x->val.remove(*y);
					if (x->val.empty()) {
						freeRect.remove(oldWidth);
					}
					x = freeRect.get(newWidth);
					if (x) {
						x->val.add({ wh.y, Int3(max.x, min.y, z) });
					}
					else {
						freeRect.add({ newWidth, {{ wh.y, Int3(max.x, min.y, z) }} });
					}
				}
				else { //宽度没覆盖高度没覆盖
					sint oldWidth = x->key;
					sint newWidth = oldWidth - wh.x;
					sint oldHeight = y->key;
					sint newHeight = oldHeight - wh.y;
					size_t area1 = tost(oldWidth) * tost(newHeight);
					size_t area2 = tost(newWidth) * tost(oldHeight);
					if (area1 >= area2) { //横向切分
						x->val.modify(*y, [&](FreeRectY::value_type& v) {
							v.val.vxy.y += wh.y;
							v.key = newHeight;
						});
						x = freeRect.get(newWidth);
						if (x) {
							x->val.add({ wh.y, Int3(max.x, min.y, z) });
						}
						else {
							freeRect.add({ newWidth, {{ wh.y, Int3(max.x, min.y, z) }} });
						}
					}
					else { //纵向切分
						x->val.remove(*y);
						if (x->val.empty()) {
							freeRect.remove(oldWidth);
						}
						x = freeRect.get(newWidth);
						if (x) {
							x->val.add({ oldHeight, Int3(max.x, min.y, z) });
						}
						else {
							freeRect.add({ newWidth, {{ oldHeight, Int3(max.x, min.y, z) }} });
						}
						x = freeRect.get(wh.x);
						if (x) {
							x->val.add({ newHeight, Int3(min.x, max.y, z) });
						}
						else {
							freeRect.add({ wh.x, {{ newHeight, Int3(min.x, max.y, z) }} });
						}
					}
				}
			}
			texPosArr[pair.val] = TexPos(min, max, z);
		}
		texDataToIndex.clear();
	}

	bool TexAtlas::exportToFile(strid path) const {
		Log::Info("TexAtlas: start exporting to file");
		bool success = true;
		for (uint i = 0; i < dataArr.count(); ++i) {
			strid layerPath = strid(path.str() + "_" + std::to_string(i) + ".png");
			if (!Tex::ExportToFile(layerPath, dataArr[i])) {
				success = false;
				break;
			}
		}
		Log::Info("TexAtlas: exporting end");
		return success;
	}

	void TexAtlas::bind(sint location) const {
		glActiveTexture(GL_TEXTURE0 + location);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	}

	TexAtlas& TexAtlas::Default() {
		static TexAtlas _texAtlas = TexAtlas();
		return _texAtlas;
	}
}