#include <dme/graph/mesh/ui_mesh.h>
#include <dme/ui/ui.h>
namespace dme::graph {
	IUIMesh::IUIMesh(const VertexType& vertexType, uint vertexSize_, uint indexSize_, const bool isFixed_, MeshDrawMode drawMode_)
	{
		stride = vertexType.stride;
		vertexSize = vertexSize_;
		indexSize = indexSize_;
		drawMode = drawMode_;

		data = Malloc<uchar>(tost(vertexSize) * stride);
		index = Malloc<uint>(indexSize);
		if (!isFixed_) {
			drawSize = 8;
			drawInfo = Malloc<uchar>(tost(drawSize) * (int_size + ptr_size));

			freeVertex = ArrFreeSpan(vertexSize);
			freeIndex = ArrFreeSpan(indexSize);
		}

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
		resizeBuffer();
		vertexType.bindVertexAttr();
	}
	void IUIMesh::resizeBuffer() const {
		//绑定顶点数组对象
		glBindVertexArray(vao);
		//顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, tost(vertexSize) * stride, null, GL_DYNAMIC_DRAW); //这里设置为null让GPU仅预留内存，glBufferSubData才是真正将顶点数据拷贝过去
		//索引缓冲
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tost(indexSize) * int_size, null, GL_DYNAMIC_DRAW);
	}
	void IUIMesh::extendVertexBuffer(uint newVertexCount) {
		if (newVertexCount <= indexSize) {
			return;
		}
		uint newSize = vertexSize * 2;
		while (newSize < newVertexCount) {
			newSize *= 2;
		}
		size_t byte = tost(newSize) * stride;
		uchar* newData = Malloc<uchar>(byte);
		Memcpy<uchar>(newData, data, tost(vertexSize) * stride);
		Free(data);
		data = newData;
		vertexSize = newSize;

		//绑定顶点数组对象
		glBindVertexArray(vao);
		//顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, tost(vertexSize) * stride, null, GL_DYNAMIC_DRAW); //这里设置为null让GPU仅预留内存，glBufferSubData才是真正将顶点数据拷贝过去
	}
	void IUIMesh::extendIndexBuffer(uint newIndexCount) {
		if (newIndexCount <= indexSize) {
			return;
		}
		uint newSize = indexSize * 2;
		while (newSize < newIndexCount) {
			newSize *= 2;
		}
		uint* newIndex = Malloc<uint>(newSize);
		Memcpy<uint>(newIndex, index, indexSize);
		Free(index);
		index = newIndex;
		indexSize = newSize;

		//绑定顶点数组对象
		glBindVertexArray(vao);
		//索引缓冲
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tost(indexSize) * int_size, null, GL_DYNAMIC_DRAW);
	}
	void IUIMesh::updateBuffer() const {
		//绑定顶点数组对象
		glBindVertexArray(vao);
		//顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, tost(vertexSize) * stride, data);
		//索引缓冲
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, tost(indexSize) * int_size, index);
	}

	void IUIMesh::addDrawInfo(uint count, size_t offset) noexcept {
		uint* indexCounts = reinterpret_cast<uint*>(drawInfo);
		size_t* indexOffsets = reinterpret_cast<size_t*>(drawInfo + tost(drawSize) * int_size);
		uint i = binarySearchDrawInfo(indexCounts, indexOffsets, offset);
		size_t end = offset + count;
		if (i && indexOffsets[i - 1] + tost(indexCounts[i - 1]) == offset) {
			if (indexOffsets[i] == end) {
				goto merge_head_tail;
			}
			else {
				goto merge_head;
			}
		}
		else {
			if (indexOffsets[i] == end) {
				goto merge_tail;
			}
			else {
				goto no_merge;
			}
		}
	merge_head:
		indexCounts[i - 1] += count;
		return;
	merge_tail:
		indexCounts[i] += count;
		indexOffsets[i] -= count;
		return;
	merge_head_tail:
		indexCounts[i - 1] += count + indexCounts[i];
		removeDrawInfo(indexCounts, indexOffsets, i);
		return;
	no_merge:
		insertDrawInfo(indexCounts, indexOffsets, i, count, offset);
	}
	void IUIMesh::removeDrawInfo(size_t offset) noexcept {
		uint* indexCounts = reinterpret_cast<uint*>(drawInfo);
		size_t* indexOffsets = reinterpret_cast<size_t*>(drawInfo + tost(drawSize) * int_size);
		uint i = binarySearchDrawInfo(indexCounts, indexOffsets, offset);
		removeDrawInfo(indexCounts, indexOffsets, i);
	}
	void IUIMesh::removeDrawInfo(uint* indexCounts, size_t* indexOffsets, uint i) noexcept {
		drawCount--;
		if (drawCount) {
			Memmove<uint>(indexCounts + i, indexCounts + i + 1, tost(drawCount) - i);
			Memmove<size_t>(indexOffsets + i, indexOffsets + i + 1, tost(drawCount) - i);
		}
	}
	void IUIMesh::splitDrawInfo(uint* indexCounts, size_t* indexOffsets, uint i, uint start, uint end, uint count) noexcept {
		if (start == indexOffsets[i]) {
			if (count == indexCounts[i]) { //完全覆盖
				removeDrawInfo(indexCounts, indexOffsets, i);
			}
			else { //在开头
				indexCounts[i] -= count;
			}
		}
		else {
			uint indexEnd = toui(indexOffsets[i]) + indexCounts[i];
			if (end == indexEnd) { //在尾部
				indexCounts[i] -= count;
				indexOffsets[i] += count;
			}
			else { //在中间
				indexCounts[i] = start - toui(indexOffsets[i]);
				insertDrawInfo(indexCounts, indexOffsets, i + 1, indexEnd - end, end);
			}
		}
	}

	void IUIMesh::insertDrawInfo(uint* indexCounts, size_t* indexOffsets, uint i, uint count, size_t offset) noexcept {
		if (drawCount == drawSize) {
			drawSize *= 2;
			uchar* newDrawInfo = Malloc<uchar>(tost(int_size + ptr_size) * drawSize);
			uint* newIndexCounts = reinterpret_cast<uint*>(newDrawInfo);
			size_t* newIndexOffsets = reinterpret_cast<size_t*>(newDrawInfo + tost(drawSize) * int_size);

			if (i != 0) {
				Memcpy<uint>(newIndexCounts, indexCounts, i);
				Memcpy<size_t>(newIndexOffsets, indexOffsets, i);
			}
			if (i != drawCount) {
				Memcpy<uint>(newIndexCounts + i, indexCounts + i + 1, tost(drawCount) - i);
				Memcpy<size_t>(newIndexOffsets + i, indexOffsets + i + 1, tost(drawCount) - i);
			}

			Free(drawInfo);
			drawInfo = newDrawInfo;
			indexCounts = newIndexCounts;
			indexOffsets = newIndexOffsets;
		}
		else {
			if (i != drawCount) {
				Memmove<uint>(indexCounts + i, indexCounts + i + 1, tost(drawCount) - i);
				Memmove<size_t>(indexOffsets + i, indexOffsets + i + 1, tost(drawCount) - i);
			}
		}
		drawCount++;
		indexCounts[i] = count;
		indexOffsets[i] = offset;
	}
	bool IUIMesh::binarySearchDrawInfo(uint* indexCounts, size_t* indexOffsets, uint start, uint& result) const noexcept {
		uint i1 = 0;
		uint i2 = drawCount;
		uint i = 0;
		while (i1 < i2) { //二分查找
			i = i1 + ((i2 - i1) / 2);
			if (start < indexOffsets[i]) {
				i2 = i;
			}
			else if (indexOffsets[i] + indexCounts[i] < start) {
				i1 = i + 1;
			}
			else {
				result = i;
				return true;
			}
		}
		return false;
	}
	uint IUIMesh::binarySearchDrawInfo(uint* indexCounts, size_t* indexOffsets, size_t offset) const noexcept {
		uint i1 = 0;
		uint i2 = drawCount;
		while (i1 < i2) { //二分查找
			uint i = i1 + ((i2 - i1) / 2);
			if (indexOffsets[i] < offset) {
				i1 = i + 1;
			}
			else {
				i2 = i;
			}
		}
		return i1;
	}

	void IUIMesh::removeFixed(ui::UI& u, VertexIndexCount useVertexIndexCount) noexcept {
		uint vertexIndex = u.getUIArrayIndex() * useVertexIndexCount.vertexCount;
		uint lastUIStart = vertexCount - useVertexIndexCount.vertexCount;
		if (vertexIndex != lastUIStart) {
			needUpdateBuffer = true;
			//将最后的顶点换到被删除的位置
			MemcpyByte(data + tost(stride) * vertexIndex, data + tost(stride) * lastUIStart, tost(stride) * useVertexIndexCount.vertexCount);
		}
		vertexCount -= useVertexIndexCount.vertexCount;
		indexCount -= useVertexIndexCount.indexCount;
	}
	void IUIMesh::removeNotFixedIndex(ui::UI& u) noexcept {
		uint* indexCounts = reinterpret_cast<uint*>(drawInfo);
		size_t* indexOffsets = reinterpret_cast<size_t*>(drawInfo + tost(drawSize) * int_size);
		auto& segment = u.meshSegment;
		uint icount = segment.indexCount();

		uint i;
		if (!binarySearchDrawInfo(indexCounts, indexOffsets, segment.indexStart, i)) {
			DME_ASSERT(false, "cannot find ui's vertex index, ui name: " + u.getName().str() );
			return;
		}
		splitDrawInfo(indexCounts, indexOffsets, i, segment.indexStart, segment.indexEnd, icount);
		freeIndex.dealloc(segment.indexStart, segment.indexEnd);
		segment.indexEnd = segment.indexStart;
	}
	void IUIMesh::removeNotFixedVertex(ui::UI& u) noexcept {
		auto& segment = u.meshSegment;
		freeVertex.dealloc(segment.vertexStart, segment.vertexEnd);
		segment.vertexEnd = segment.vertexStart;
	}

	void IUIMesh::alloc(ui::UI& u, MeshEditorBase& editor, VertexIndexCount uiNewCount) {
		auto& segment = u.meshSegment;
		uint icount = segment.indexCount();
		ArrSpan r = ArrSpan(segment.indexStart, segment.indexEnd);
		if (icount < uiNewCount.indexCount) {
			if (icount) {
				removeNotFixedIndex(u);
			}
			uint newSize = indexSize;
			r = freeIndex.alloc(uiNewCount.indexCount, newSize);
			extendIndexBuffer(newSize);
			addDrawInfo(uiNewCount.indexCount, r.start);
			if (icount && r.start != segment.indexStart) {
				Memmove<uint>(index + r.start, index + segment.indexStart, icount);
			}
			indexCount += uiNewCount.indexCount - icount;
		}
		else if (icount > uiNewCount.indexCount) {
			uint* indexCounts = reinterpret_cast<uint*>(drawInfo);
			size_t* indexOffsets = reinterpret_cast<size_t*>(drawInfo + tost(drawSize) * int_size);
			uint i;
			if (!binarySearchDrawInfo(indexCounts, indexOffsets, segment.indexStart, i)) {
				DME_ASSERT(false, "cannot find ui's vertex index, ui name: " + u.getName().str());
				return;
			}
			splitDrawInfo(indexCounts, indexOffsets, i, segment.indexStart, segment.indexEnd, icount);
			if (uiNewCount.indexCount) { // TODO 可以将split和add优化为1个函数，只需要分割一次
				addDrawInfo(uiNewCount.indexCount, segment.indexStart);
			}
			r.end = r.start + uiNewCount.indexCount;
			freeIndex.dealloc(r.end, icount - uiNewCount.indexCount);
			indexCount -= icount - uiNewCount.indexCount;
		}
		segment.indexStart = r.start;
		segment.indexEnd = r.end;

		uint newSize = vertexSize;
		uint vcount = segment.vertexCount();
		r = freeVertex.realloc(segment.vertexStart, segment.vertexEnd, uiNewCount.vertexCount, newSize);
		extendIndexBuffer(newSize);
		vertexCount = vertexCount - vcount + uiNewCount.vertexCount;

		editor.start = r.start;
		if (vcount && r.start != segment.vertexStart) {
			//移动旧顶点
			MemmoveByte(data + tost(stride) * r.start, data + tost(stride) * segment.vertexStart, tost(stride) * vcount);
			//修改索引值
			uint indexDiff = r.start - segment.vertexStart;
			for (uint i = segment.indexStart; i < segment.indexEnd; ++i) {
				index[i] += indexDiff;
			}
		}
		segment.vertexStart = r.start;
		segment.vertexEnd = r.end;
	}

	void IUIMesh::update(ui::UI& u, bool isAdd, const bool isFixedCount, VertexIndexCount useVertexIndexCount) {
		if (isAdd) {
			if (isFixedCount) {
				u.meshSegment.vertexStart = useVertexIndexCount.vertexCount * u.uiArrayIndex;
				u.meshSegment.vertexEnd = u.meshSegment.vertexStart + useVertexIndexCount.vertexCount;
				u.meshSegment.indexStart = useVertexIndexCount.indexCount * u.uiArrayIndex;
				u.meshSegment.indexEnd = u.meshSegment.indexStart + useVertexIndexCount.indexCount;

				vertexCount += useVertexIndexCount.vertexCount;
				indexCount += useVertexIndexCount.indexCount;
				extendVertexBuffer(vertexCount);
				extendIndexBuffer(indexCount);
			}
		}
		needUpdateBuffer = true;
		editMesh(u, MeshEditorBase(this, &u, isFixedCount), isAdd);
	}

	void IUIMesh::remove(ui::UI& u, const bool isFixedCount, VertexIndexCount useVertexIndexCount) noexcept {
		if (isFixedCount) {
			removeFixed(u, useVertexIndexCount);
		}
		else {
			removeNotFixedIndex(u);
			removeNotFixedVertex(u);
		}
		u.meshSegment.setEmpty();
	}

	void IUIMesh::draw(const bool isFixedCount) {
		if (indexCount == 0) {
			return;
		}
		if (needUpdateBuffer) {
			updateBuffer();
			needUpdateBuffer = false;
		}
		else {
			glBindVertexArray(vao);
		}

		if (isFixedCount) {
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		}
		else {
			glMultiDrawElements(static_cast<uint>(drawMode), reinterpret_cast<sint*>(drawInfo), GL_UNSIGNED_INT,
				reinterpret_cast<void**>(drawInfo + tost(drawSize) * int_size), drawCount);
		}
	}
	void IUIMesh::release() {
		if (data) {
			Free(data);
			Free(index);
			Free(drawInfo);
			freeIndex.release();
			freeVertex.release();
			
			data = null;
			index = null;
			drawInfo = null;
			ObjPool<IUIMesh>::Remove(this);
		}
	}
	void IUIMesh::editMesh(ui::UI& u, MeshEditorBase editor, const bool isAdd) {
		u.editMesh(editor, isAdd);
	}



	void UIMeshGroup::update(ui::UI& u) {
		Pair<ui::UIMeshKey, NodeVal>* pair;
		bool isAdd;
		if (u.uiArrayIndex.isNull()) {
			isAdd = true;
			pair = map.addIfAbsent(u.getUIMeshKey(), u.meshType);
			u.uiArrayIndex = pair->val.uiArray.count();
			pair->val.uiArray.add(&u);
		}
		else {
			isAdd = false;
			pair = map.get(u.getUIMeshKey());
		}
		pair->val.mesh->update(u, isAdd, pair->val.isFixedCount, pair->val.func.vertexIndexCount);
	}
	void UIMeshGroup::remove(ui::UI& u) {
		if (u.uiArrayIndex.isNull()) {
			return;
		}
		Pair<ui::UIMeshKey, NodeVal>* pair = map.get(u.getUIMeshKey());
		pair->val.uiArray.last()->uiArrayIndex = u.uiArrayIndex;
		pair->val.uiArray.removeSwapTail(u.uiArrayIndex);
		pair->val.mesh->remove(u, pair->val.isFixedCount, pair->val.func.vertexIndexCount);
		u.uiArrayIndex.setNull();
	}
	void UIMeshGroup::setGroupZIndex(ui::UI& u, const sint zIndex) {
		if (u.zIndex == zIndex) return;
		Pair<ui::UIMeshKey, NodeVal>* pair = map.replaceKey(u.getUIMeshKey(), ui::UIMeshKey(zIndex, u.meshType));
		if (u.uiArrayIndex.isNull()) {
			u.zIndex = zIndex;
			ui::UITree::NeedUpdate(u);
		}
		if (!pair) return;
		auto& uiArray = pair->val.uiArray;
		for (uint i = 0; i < uiArray.count(); ++i) {
			uiArray[i]->zIndex = zIndex;
			ui::UITree::NeedUpdate(*uiArray[i]);
		}
	}
	void UIMeshGroup::setZIndex(ui::UI& u, const sint zIndex) {
		if (u.zIndex == zIndex) return;
		if (u.uiArrayIndex.isNull()) {
			u.zIndex = zIndex;
			ui::UITree::NeedUpdate(u);
			return;
		}
		//删除
		Pair<ui::UIMeshKey, NodeVal>* pair = map.get(u.getUIMeshKey());
		pair->val.uiArray.last()->uiArrayIndex = u.uiArrayIndex;
		pair->val.uiArray.removeSwapTail(u.uiArrayIndex);
		pair->val.mesh->remove(u, pair->val.isFixedCount, pair->val.func.vertexIndexCount);
		//添加
		u.zIndex = zIndex;
		pair = map.addIfAbsent(u.getUIMeshKey(), u.meshType);
		u.uiArrayIndex = pair->val.uiArray.count();
		pair->val.uiArray.add(&u);
		pair->val.mesh->update(u, true, pair->val.isFixedCount, pair->val.func.vertexIndexCount);
		ui::UITree::NeedUpdate(u);
	}
	void UIMeshGroup::draw() {
		void (*shaderReadyFunc) (const ShaderBase&) = null;
		const ShaderBase* shader = null;
		bool is3d = false;
		map.each([&](const Pair<ui::UIMeshKey, NodeVal>& pair) {
			if (!pair.val.uiArray.empty()) {
				const ShaderBase* nowShader = &pair.val.func.getShaderFunc();
				if (nowShader != shader) {
					shader = nowShader;
					shader->use();
					shaderReadyFunc = pair.val.func.shaderReadyFunc;
					shaderReadyFunc(*shader);
				}
				else if (pair.val.func.shaderReadyFunc != shaderReadyFunc) {
					shaderReadyFunc = pair.val.func.shaderReadyFunc;
					shaderReadyFunc(*shader);
				}
				bool nowIs3d = pair.val.func.is3D;
				if (is3d != nowIs3d) {
					is3d = nowIs3d;
					if (nowIs3d) {
						Content::ClearDepthStencilBuffer();
						Content::EnableDepthTest();
						Content::EnableCullFace();
					}
					else {
						Content::DisableDepthTest();
						Content::DisableCullFace();
					}
				}
				pair.val.mesh->draw(pair.val.isFixedCount);
			}
		});
	}
	void UIMeshGroup::release() {
		map.each([](const Pair<ui::UIMeshKey, NodeVal>& pair) {
			pair.val.mesh->release();
		});
	}
}