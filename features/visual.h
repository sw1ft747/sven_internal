#ifndef VISUAL_H
#define VISUAL_H

#ifdef _WIN32
#pragma once
#endif

#include <base_feature.h>
#include <IHooks.h>

#include "../game/class_table.h"

typedef float bone_matrix3x4_t[MAXSTUDIOBONES][3][4];

//-----------------------------------------------------------------------------
// Visuals
//-----------------------------------------------------------------------------

class CVisual : public CBaseFeature
{
public:
	CVisual();

	virtual bool Load();
	virtual void PostLoad();

	virtual void Unload();

public:
	void Process();
	bool StudioRenderModel();

	void OnVideoInit();
	void OnHUDRedraw(float flTime);

	void ResetJumpSpeed();

private:
	void ESP();
	void DrawCrosshair();
	void ShowSpeed();
	void Lightmap();

	void DrawPlayerInfo_Default(int index, int iHealth, bool bIsEntityFriend, float top_mid_x, float top_mid_y, float bottom_mid_x, float bottom_mid_y);
	void DrawEntityInfo_Default(int index, class_info_t classInfo, float bottom_mid_x, float bottom_mid_y, int r, int g, int b);

	void DrawPlayerInfo_SAMP(int index, int iHealth, bool bDucking, bool bIsEntityFriend, Vector vecTop);
	void DrawEntityInfo_SAMP(int index, class_info_t classInfo, Vector vecTop, int r, int g, int b);
	
	void DrawPlayerInfo_L4D(int index, int iHealth, bool bDucking, bool bIsEntityFriend, Vector vecTop);
	void DrawEntityInfo_L4D(int index, class_info_t classInfo, Vector vecTop, int r, int g, int b);

	void DrawBox(bool bPlayer, bool bItem, int iHealth, int x, int y, int w, int h, int r, int g, int b);
	void DrawBones(int index, studiohdr_t *pStudioHeader);

	void ProcessBones();

private:
	float m_flTime;

	float m_flPrevTime;
	float m_flFadeTime;
	float m_flJumpSpeed;

	int m_clFadeFrom[3];

	bool m_bOnGround;

	DetourHandle_t m_hUserMsgHook_ScreenShake;
	DetourHandle_t m_hUserMsgHook_ScreenFade;

	int m_iScreenWidth;
	int m_iScreenHeight;
};

extern CVisual g_Visual;

#endif // VISUAL_H