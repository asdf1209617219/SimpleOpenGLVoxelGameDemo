#pragma once
#include <dme/input/input.h>

#include <dme/ui/ui_bvh.h>
#include <dme/ui/ui_flag.h>
#include <dme/ui/ui_unit.h>
#include <dme/graph/mesh/ui_mesh.h>
#include <dme/graph/mesh/mesh_editor.h>
#include <dme/graph/texture/tex_atlas.h>
namespace dme {
	namespace graph {
		class UIMeshGroup;
		class IUIMesh;
		class MeshEditorBase;
	}

	namespace ui {
		enum class UIEventType : uchar {
			Click,
			Hold,
			Release,
			Hover,
			Leave,
		};
		inline std::ostream& operator<<(std::ostream& os, const UIEventType& a) {
			switch (a) {
			case UIEventType::Click:
				return os << "Click";
			case UIEventType::Hold:
				return os << "Hold";
			case UIEventType::Release:
				return os << "Release";
			case UIEventType::Hover:
				return os << "Hover";
			case UIEventType::Leave:
				return os << "Leave";
			default:
				return os;
			}
		}

		class UI;
		class UITree;
		class UIBVH;
		class UIBVHBak;

		//用于点击等事件的传入参数
		class UIEvent {
		private:
			UI* source;
			Float2 triggerPos;
			UIEventType eventType;
			input::InputAction inputAction;
		public:
			UIEvent(UI& source, const Float2& triggerPos, UIEventType eventType, input::InputAction inputAction = input::InputAction::None) noexcept;

			UI& getUI() const noexcept {
				return *source;
			}
			const Float2& getTriggerPos() const noexcept {
				return triggerPos;
			}
			UIEventType getEventType() const noexcept {
				return eventType;
			}
			input::InputAction getInputAction() const noexcept {
				return inputAction;
			}
		};

		//基础UI类，默认为矩形布局，使用矩形顶点
		class UI : private ObjPool<UI> {
			friend Game;
			friend ObjPool<UI>;
			friend UITree;
			friend UIBVH;
			friend UIBVHBak;
			friend graph::UIMeshGroup;
			friend graph::IUIMesh;
			friend graph::MeshEditorBase;
		private:
			//父ui
			UI* parent = null;
			//子ui
			DynArr<UI*> children;
			//在父节点的子节点数组的下标
			uid childrenIndex;
			//uibvh中节点的下标
			uid bvhIndex;
			//显示层级
			sint zIndex = 0;
			//ui的meshType
			uint meshType = 0;
			//在meshGroup的node中数组的下标
			uid uiArrayIndex;
			//保存各类状态，默认可渲染和触发事件
			UIFlag flags = UIFlag::isTriggerable | UIFlag::isRendering;

			//顶点和索引
			graph::MeshSegment meshSegment;

			//宽高（宽高为父布局的比率或为具体像素，其中minSideRatio为true时，宽高为窗口最小边的比率）
			Float2 widthHeight;
			//父布局上的锚点（单位为比例，左下角为(0, 0)，右上角为(1, 1)，可超出布局内）
			Float2 anchors;
			//当前布局的支点，将pivot固定在anchors上（单位为比例，规则同anchors）
			Float2 pivot;

			//左下角和左上角构成的包围盒（物理像素）
			Abox2 box;

			strid name;

			//显示类型
			UIDisplayType displayType = UIDisplayType::display;

			//被移除时释放资源的逻辑，移除前会与父节点断开，不要直接调用此方法，外部删除UI请使用Remove方法
			virtual void removeImpl() noexcept {
				ObjPool<UI>::Remove(this);
			}

			//是参数的孙子节点
			bool isGrandchildOf(const UI& u) const noexcept;
			//当前子树的根节点
			UI* getThisTreeRoot() noexcept;
			//当前子树的根节点
			const UI* getThisTreeRoot() const noexcept;

			void updateBoxPosAndSize(const Abox2& parentBox) noexcept;
			void updateBoxImpl(const Abox2& parentBox) noexcept;
			//根据当前的widthHeight、anchors、docking和父节点的包围盒计算包围盒
			void updateBox() noexcept;

			void updateBoxIfNeedImpl(const Abox2& parentBox) noexcept;
			//更新包围盒如果flags里有标记的话（如果祖级节点也有标记则也要更新）
			void updateBoxIfNeed() noexcept;

			void addFlag(UIFlag flag) noexcept;
			void addThisAndChildrenFlag(UIFlag flag) noexcept;
			void setThisAndChildrenFlag(UIFlag mask, UIFlag flag) noexcept;
			void setThisAndChildrenFlagUntilHasAnyFlag(UIFlag mask, UIFlag flag, UIFlag untilFlag) noexcept;
			void removeFlag(UIFlag flag) noexcept;
			void removeThisAndChildrenFlag(UIFlag flag) noexcept;
			bool hasAnyFlag(UIFlag flag) const noexcept;
			bool hasAllFlag(UIFlag flag) const noexcept;
			UIFlag getFlag(UIFlag flag) const noexcept;
			void setFlag(UIFlag mask, bool isTrue) noexcept;
			void setFlag(UIFlag mask, UIFlag flag) noexcept;

			void addThisAndChildrenFlagIfNotHas(UIFlag flag) noexcept;

			void addThisAndChildrenRenderingFlagUntilTypeIsHidden() noexcept;

			void changeParentSetFlagsImpl(UIFlag flag) noexcept;
			//修改完parent后设置flags
			void changeParentSetFlags() noexcept;

			bool isInNeedUpdateSet() const noexcept;
			//等于getIsRendering() && getIsTriggerable()
			bool needBVH() const noexcept;

			void needUpdate() noexcept;

			static Abox2 _ParentBox(const UI& u) noexcept;
			static void _RemoveChildren(bool parentIsInNeedUpdateSet, UI& u);

			static void _RemoveThisAndChildrenMesh(UI& u);
			static void _RemoveMesh(UI& u);
			static void _UpdateThisAndChildrenMesh(UI& u);
			static void _UpdateMesh(UI& u);

			static void _RemoveBVH(UI& u) noexcept; //彻底删除UI在BVH中的节点，包括子节点
			static void _UpdateBVH(UI& u) noexcept; //更新或添加到BVH
			static void _UpdateThisAndChildrenBVH(UI& u) noexcept; //更新或添加当前节点以及子节点到BVH
			static void _DisconnectBVH(UI& u) noexcept; //将UI从BVH中断开连接

			//窗口大小更新时调用
			static void _UpdateWinSize(sint width, sint height) noexcept;

			static UI& _InitRoot() noexcept {
				UI& r = UI::Create();
				r.name = "Root";
				r.flags = UIFlag::isInRootTree;
				r.displayType = UIDisplayType::layout; // TODO 目前Root只用于布局
				r.widthHeight = Float2(1, 1);
				r.anchors = Float2();
				r.pivot = Float2();
				r.box = Abox2(Float2(0, 0), Float2(tof(Game::GetWinWidth()), tof(Game::GetWinHeight())));
				return r;
			}

			//管理UI的渲染
			static inline graph::UIMeshGroup _meshGroup;
			//管理包围盒
			static inline UIBVH _bvh;
			//缩放系数
			static inline float _scaleFactor = 1.0f;

		protected:
			//需要传入UIMeshBase<T>::GetMeshId()
			//默认是会触发事件的
			explicit UI(uint meshType) noexcept : meshType(meshType) {}
		public:

			static constexpr size_t obj_pool_size = 32;
			static void ShaderReadyFunc(const graph::Shader<graph::RectUIVertex>& shader) {
				shader.setTexAtlas("aTexture", graph::TexAtlas::Default(), 0);
				shader.setInt("aTextureSize", graph::TexAtlas::Default().getSize());
				shader.setMatrix("projection", UI::GetProjection());
			}
			static const auto& GetShader() {
				return graph::Shaders::UI();
			}

			static inline const graph::VertexIndexCount UseVertexIndexCount = graph::VertexIndexCount(); //0则表示顶点和索引数量不固定
			static graph::IUIMesh* CreateMesh() {
				return graph::IUIMesh::Create(GetShader().getType(), 64, 96, !UseVertexIndexCount.empty());
			}

			//UI类型的meshType
			static uint GetMeshType() noexcept {
				return graph::UIMeshBase<UI>::GetMeshType();
			}

			//负责将UI回收，从UITree中移除（递归移除子节点），然后UI需重写removeImpl方法，将自己的内存归还给对象池
			//不允许移除root
			static void Remove(UI& u);
			//负责将UI回收，从UITree中移除（递归移除子节点），然后UI需重写removeImpl方法，将自己的内存归还给对象池
			//不允许移除root
			void removeThis();

			//创建UI
			static UI& Create() noexcept {
				return *ObjPool<UI>::Add(GetMeshType());
			}

			//获取在mesh的node中数组的下标
			uid getUIArrayIndex() const noexcept;
			//获取UI在mesh中的key
			UIMeshKey getUIMeshKey() const noexcept;

			//设置与自己属于同一顶点数组的UI的zIndex
			UI& setGroupZIndex(const sint zIndex);
			//设置zIndex
			UI& setZIndex(const sint zIndex);
			//获取zIndex
			sint getZIndex() const noexcept;

			//设置父节点
			UI& setParent(UI& u);
			//设置父节点为根节点
			UI& setParentIsRoot();
			//与父节点断开
			UI& disconnectParent();
			//获取父节点
			UI* getParent() noexcept;
			//获取父节点
			const UI* getParent() const noexcept;

			//添加子节点
			UI& addChild(UI& child);
			//删除子节点
			UI& removeChild(UI& child);
			//获取子节点
			const DynArr<const UI*>& getChildren() const noexcept;

			//设置是否宽高以父节点最小边长为基准计算，为true时宽高均为比例计算
			UI& setMinSideRatio(const bool minSideRatio) noexcept;
			//true时宽高以父节点最小边长为基准计算
			bool getMinSideRatio() const noexcept;

			//显示类型
			UIDisplayType getDisplayType() const noexcept;
			//设置显示类型
			UI& setDisplayType(UIDisplayType displayType) noexcept;
			//是否渲染（父节点隐藏当前节点不隐藏或者不在Root所处的树也会返回false）
			bool getIsRendering() const noexcept;

			//设置是否可触发事件
			UI& setIsTriggerable(bool isTriggerable) noexcept;
			//是否可触发事件
			bool getIsTriggerable() const noexcept;


			//宽
			UIut getWidth() const noexcept;
			//高
			UIut getHeight() const noexcept;
			//引用宽高的值，注意这里是不带单位的
			Float2& refWidthHeightVal() noexcept;
			//设置宽（宽高为父布局的比率或为具体像素，其中minSideRatio为true时，宽高为窗口最小边的比率）
			UI& setWidth(UIut width) noexcept;
			//设置高（宽高为父布局的比率或为具体像素，其中minSideRatio为true时，宽高为窗口最小边的比率）
			UI& setHeight(UIut height) noexcept;
			//设置宽高（宽高为父布局的比率或为具体像素，其中minSideRatio为true时，宽高为窗口最小边的比率）
			UI& setWidthHeight(UIut width, UIut height) noexcept;
			//宽是否为像素
			bool getWidthIsPixel() const noexcept;
			//设置宽是否为像素
			UI& setWidthIsPixel(const bool widthIsPixel) noexcept;
			//高是否为像素
			bool getHeightIsPixel() const noexcept;
			//设置高是否为像素
			UI& setHeightIsPixel(const bool heightIsPixel) noexcept;

			//父布局上的锚点X分量（单位为比例，左下角为(0, 0)，右上角为(1, 1)，可超出布局内）
			UIut getAnchorsX() const noexcept;
			//父布局上的锚点Y分量（单位为比例，左下角为(0, 0)，右上角为(1, 1)，可超出布局内）
			UIut getAnchorsY() const noexcept;
			//引用锚点的值，注意这里是不带单位的
			Float2& refAnchorsVal() noexcept;
			//设置父布局上的锚点X分量（单位为比例，左下角为(0, 0)，右上角为(1, 1)，可超出布局内）
			UI& setAnchorsX(UIut anchorsX) noexcept;
			//设置父布局上的锚点Y分量（单位为比例，左下角为(0, 0)，右上角为(1, 1)，可超出布局内）
			UI& setAnchorsY(UIut anchorsY) noexcept;
			//设置父布局上的锚点（单位为比例，左下角为(0, 0)，右上角为(1, 1)，可超出布局内）
			UI& setAnchors(UIut anchorsX, UIut anchorsY) noexcept;
			//父布局上的锚点X分量是否为像素
			bool getAnchorsXIsPixel() const noexcept;
			//设置父布局上的锚点X分量是否为像素
			UI& setAnchorsXIsPixel(const bool anchorsXIsPixel) noexcept;
			//父布局上的锚点Y分量是否为像素
			bool getAnchorsYIsPixel() const noexcept;
			//设置父布局上的锚点Y分量是否为像素
			UI& setAnchorsYIsPixel(const bool anchorsYIsPixel) noexcept;

			//位置以整个窗口为基准
			bool getAnchorsOnRoot() const noexcept;
			//设置位置以整个窗口为基准
			UI& setAnchorsOnRoot(const bool anchorsOnRoot) noexcept;

			//固定在父节点左下角
			UI& setPivotAndAnchorsOnLowerLeft() noexcept;
			//固定在父节点左上角
			UI& setPivotAndAnchorsOnUpperLeft() noexcept;
			//固定在父节点右下角
			UI& setPivotAndAnchorsOnLowerRight() noexcept;
			//固定在父节点右上角
			UI& setPivotAndAnchorsOnUpperRight() noexcept;
			//固定在父节点中心点
			UI& setPivotAndAnchorsOnCenter() noexcept;
			//固定在父节点左侧
			UI& setPivotAndAnchorsOnLeft() noexcept;
			//固定在父节点右侧
			UI& setPivotAndAnchorsOnRight() noexcept;
			//固定在父节点下侧
			UI& setPivotAndAnchorsOnLower() noexcept;
			//固定在父节点上侧
			UI& setPivotAndAnchorsOnUpper() noexcept;

			//当前布局的支点X分量，将pivot固定在anchors上（规则同anchors）
			UIut getPivotX() const noexcept;
			//当前布局的支点Y分量，将pivot固定在anchors上（规则同anchors）
			UIut getPivotY() const noexcept;
			//引用当前布局的支点的值，注意这里是不带单位的
			Float2& refPivotVal() noexcept;
			//设置当前布局的支点X分量，将pivot固定在anchors上（规则同anchors）
			UI& setPivotX(UIut pivotX) noexcept;
			//设置当前布局的支点Y分量，将pivot固定在anchors上（规则同anchors）
			UI& setPivotY(UIut pivotY) noexcept;
			//设置当前布局的支点，将pivot固定在anchors上（规则同anchors）
			UI& setPivot(UIut pivotX, UIut pivotY) noexcept;
			//当前布局的支点X分量是否为像素
			bool getPivotXIsPixel() const noexcept;
			//设置当前布局的支点X分量是否为像素
			UI& setPivotXIsPixel(const bool pivotXIsPixel) noexcept;
			//当前布局的支点Y分量是否为像素
			bool getPivotYIsPixel() const noexcept;
			//设置当前布局的支点Y分量是否为像素
			UI& setPivotYIsPixel(const bool pivotYIsPixel) noexcept;

			//左下角和左上角构成的包围盒（物理像素），注意在const和非const下调用逻辑不相同，非const下调用可能触发布局计算
			const Abox2& getBox() noexcept;
			//左下角和左上角构成的包围盒（物理像素），注意在const和非const下调用逻辑不相同，非const下调用可能触发布局计算
			const Abox2& getBox() const noexcept;

			strid getName() const noexcept {
				return name;
			}
			UI& setName(strid n) noexcept {
				name = n;
				return *this;
			}


			//判断点是否在UI内（调用前bvh会先初步判断是否在UI的box内，如果点不在包围盒内则不会调用此方法）
			virtual bool pointInUI(const Float2& point) const noexcept;

			virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept;

			static void TestEvent(const UIEvent& e) {
				UI& u = e.getUI();
				Log::Info("UI: ", "name: ", u.getName(), "; event type: ", e.getEventType(), "; ui zIndex: ", u.zIndex, "; ui mesh type: ", u.meshType,
					"; box: ", u.getBox(), "; cursor in UI pos: ", input::Input::GetCursorInUIPositionF()
				);
			}

			//鼠标点击，按住时，第一帧触发click，第二帧触发hold（如果第二帧没有立即松开的话）
			virtual void click(const UIEvent& e) {
				//TestEvent(e);
			}
			//鼠标按住
			virtual void hold(const UIEvent& e) {
				//TestEvent(e);
			}
			//鼠标松开，松开时，如果上一帧触发click，则不触发hold（或者通过设置参数来决定hold需要按住几帧来触发）
			virtual void release(const UIEvent& e) {
				//TestEvent(e);
			}
			//鼠标悬停
			virtual void hover(const UIEvent& e) {
				//TestEvent(e);
			}
			//鼠标离开悬停区域
			virtual void leave(const UIEvent& e) {
				//TestEvent(e);
			}

			//获取最近触发click的UI
			static UI* GetClickUI() noexcept;
			//获取最近触发hold的UI
			static UI* GetHoldUI() noexcept;
			//获取最近触发release的UI
			static UI* GetReleaseUI() noexcept;
			//获取最近触发hover的UI
			static UI* GetHoverUI() noexcept;
			//获取最近触发leave的UI
			static UI* GetLeaveUI() noexcept;

			//正交投影矩阵
			static const Matrix4& GetProjection() noexcept;

			//UI根节点
			static UI& Root() noexcept;
		};


		//管理所有UI之间的连接关系，以及UI的渲染，以及UI的包围盒
		class UITree {
		private:
			friend UI;


			static inline FlatSet<UI*> needUpdateUISet; //需要被更新的UI集合

			static void RemoveNeedUpdate(UI& u) noexcept;
			static void NeedUpdateChildren(UI* u) noexcept;
		public:
			//将UI添加到需要被更新的UI集合里
			static void NeedUpdate(UI& u) noexcept;
			//处理需要被更新的UI
			static void Update();

			//绘制全部UI
			static void Draw();
		};

	}
}
