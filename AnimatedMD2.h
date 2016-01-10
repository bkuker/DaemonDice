// AnimatedMD2.h: interface for the AnimatedMD2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATEDMD2_H__3CE94D18_0F2A_4B1F_BAC1_409F409C6E81__INCLUDED_)
#define AFX_ANIMATEDMD2_H__3CE94D18_0F2A_4B1F_BAC1_409F409C6E81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Md2.h"
#include "Animated.h"



typedef struct
{	
	std::string name;
	int ms;
	int start;
	int end;
} ANIMATION;


class AnimatedMd2 : 
	public Animated, 
	public Md2  
{
public:
	AnimatedMd2(DXSystem* dxs, std::string ani, std::string md2, std::string tex);
	AnimatedMd2(DXSystem* dxs);
	virtual ~AnimatedMd2();
	void tick(int milliseconds);
	void setAnimation(int);
	void setAnimation(std::string);

protected:
	void load( std::string ani, std::string md2, std::string tex );
	void loadAnimationSet(std::string file);

protected:
	int numAni;
	int CurrentFrame;
	int time;
	ANIMATION* curAni;
	std::map<std::string, ANIMATION*> byName;
	ANIMATION* byNum;
};

#endif // !defined(AFX_ANIMATEDMD2_H__3CE94D18_0F2A_4B1F_BAC1_409F409C6E81__INCLUDED_)
