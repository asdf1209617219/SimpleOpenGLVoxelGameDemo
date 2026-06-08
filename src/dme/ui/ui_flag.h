#pragma once
#include <dme/core.h>
namespace dme::ui {
	enum class UIFlag : uint {
		none			= 0,
		isInRootTree	= 1,					//是否在Root所在的树中
		isHidden		= isInRootTree << 1,	//当前UI或祖级UI隐藏
		isTriggerable	= isHidden << 1,		//是否会触发各种事件，是则会加入bvh
		isRendering		= isTriggerable << 1,	//是否渲染
		needUpdateBox	= isRendering << 1,		//是否需要更新box
		minSideRatio	= needUpdateBox << 1,	//true时宽高以父节点最小边长为基准计算
		widthIsPixel	= minSideRatio << 1,	//宽是否为像素（如果不是则为父节点的width的比例，root永远为比例1即winWidth）
		heightIsPixel	= widthIsPixel << 1,	//高是否为像素（如果不是则为父节点的height的比例，root永远为比例1即winHeight）
		anchorsXIsPixel	= heightIsPixel << 1,	//anchors的x分量是否为像素
		anchorsYIsPixel	= anchorsXIsPixel << 1,	//anchors的y分量是否为像素
		anchorsOnRoot	= anchorsYIsPixel << 1,	//anchors是否在整个窗口上
		pivotXIsPixel	= anchorsOnRoot << 1,	//pivot的x分量是否为像素
		pivotYIsPixel	= pivotXIsPixel << 1,	//pivot的y分量是否为像素
		disabled		= pivotYIsPixel << 1,	//是否禁用
		readonly		= disabled << 1,		//是否只读
		occlusion		= readonly << 1,		//是否会将在它zIndex之下的UI遮挡
	};

	inline bool operator ==(UIFlag a, UIFlag b) noexcept {
		return static_cast<uint>(a) == static_cast<uint>(b);
	}
	inline UIFlag operator |(UIFlag a, UIFlag b) noexcept {
		return static_cast<UIFlag>(static_cast<uint>(a) | static_cast<uint>(b));
	}
	inline UIFlag operator &(UIFlag a, UIFlag b) noexcept {
		return static_cast<UIFlag>(static_cast<uint>(a) & static_cast<uint>(b));
	}
	inline UIFlag operator ^(UIFlag a, UIFlag b) noexcept {
		return static_cast<UIFlag>(static_cast<uint>(a) ^ static_cast<uint>(b));
	}
	inline UIFlag operator ~(UIFlag a) noexcept {
		return static_cast<UIFlag>(~static_cast<uint>(a));
	}
	inline UIFlag& operator |=(UIFlag& a, UIFlag b) noexcept {
		a = a | b;
		return a;
	}
	inline UIFlag& operator &=(UIFlag& a, UIFlag b) noexcept {
		a = a & b;
		return a;
	}
	inline UIFlag& operator ^=(UIFlag& a, UIFlag b) noexcept {
		a = a ^ b;
		return a;
	}

	enum class UIDisplayType : uchar {
		display	= 0, //正常显示
		hidden	= 1, //隐藏，包括子节点
		layout	= 2, //仅用于布局，子节点可以显示
	};
}