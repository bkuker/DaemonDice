// AnimatedMD2.cpp: implementation of the AnimatedMD2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AnimatedMD2.h"
#include <fstream>
#include "DXSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnimatedMd2::AnimatedMd2(DXSystem* dxs, std::string ani, std::string md2, std::string tex):
	Md2(dxs),
	time(0)
{
	load( ani, md2, tex );
}

AnimatedMd2::AnimatedMd2(DXSystem* dxs):
	Md2(dxs),
	time(0)
{
	curAni = NULL;
	CurrentFrame = 0;
}

AnimatedMd2::~AnimatedMd2()
{

}

void AnimatedMd2::load( std::string ani, std::string md2, std::string tex )
{
	Md2::load( md2, tex );
	loadAnimationSet(ani);
	curAni = byNum;
	CurrentFrame = curAni->start;
}

void AnimatedMd2::tick(int milliseconds)
{
	time += milliseconds;
	int nextF;

	if ( time > curAni->ms ){
		time = 0;
		CurrentFrame++;
		if (CurrentFrame > curAni->end)
			CurrentFrame = curAni->start;
	}

	nextF = CurrentFrame + 1;
	if (nextF > curAni->end)
		nextF = curAni->start;

	float i = (float)time / (float)curAni->ms;
	//dxs->debugprintf("Frame %i -> %i by %f", CurrentFrame, nextF, i);
	setFrame( CurrentFrame, nextF, i );
}

void AnimatedMd2::setAnimation(int n)
{
	ANIMATION* newA;
	if ( n >= 0 && n < numAni){
		newA = byNum + n;
		if ( newA != curAni)
			CurrentFrame = newA->start;
		curAni = newA;
	}
}

void AnimatedMd2::setAnimation(std::string n)
{
	ANIMATION* newA;
	std::map<std::string, ANIMATION*>::iterator i = byName.find(n);
	if ( i != byName.end() ){
		newA = i->second;
		if ( newA != curAni){
			CurrentFrame = newA->start;
			//dxs->debugprintf("Player %i changed to animation %s", this, n.c_str());
		}
		curAni = newA;
	} else {
		dxs->debugprintf("Invalid animation '%s' selected", n.c_str() );
	}
}

void AnimatedMd2::loadAnimationSet(std::string file)
{
	std::ifstream in(file.c_str());
	if(!in){
		dxs->debugprintf("Animation %s not found!", file.c_str());
		return;
	}
	std::string header;
	in >> header;
	dxs->debugprintf("Read animation header %s", header.c_str() );
	in >> numAni;
	dxs->debugprintf("Expecting %i animations", numAni);
	byNum = new ANIMATION[numAni];

	for (int i = 0; i < numAni; i++)
	{
		int num;
		float f;
		in >> num;
		if (num != i)
			dxs->debugprintf("Error in ani file");
		in >> byNum[i].name;
		in >> f;
		byNum[i].ms = (1.0f / f) * 1000;
		in >> byNum[i].start;
		in >> byNum[i].end;
		byName[byNum[i].name] = &byNum[i];
	}
}