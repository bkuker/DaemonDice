// Animated.h: interface for the Animated class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATED_H__B4A8F1F6_F549_41A8_A9CA_7BA5C50830AD__INCLUDED_)
#define AFX_ANIMATED_H__B4A8F1F6_F549_41A8_A9CA_7BA5C50830AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Animated
{
public:
	Animated();
	virtual ~Animated();
	virtual void tick(int milliseconds)=0;
};

#endif // !defined(AFX_ANIMATED_H__B4A8F1F6_F549_41A8_A9CA_7BA5C50830AD__INCLUDED_)
