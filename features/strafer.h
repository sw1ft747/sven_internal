#ifndef STRAFER_H
#define STRAFER_H

#ifdef _WIN32
#pragma once
#endif

#include <base_feature.h>

#include "../strafe/strafe.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

extern void UpdateStrafeData(Strafe::StrafeData &strafeData, bool bStrafe, Strafe::StrafeDir dir, Strafe::StrafeType type, float flYaw, float flPointX, float flPointY);

//-----------------------------------------------------------------------------
// Strafer
//-----------------------------------------------------------------------------

class CStrafer : public CBaseFeature
{
public:
	virtual bool Load();
	virtual void PostLoad();

	virtual void Unload();

public:
	void CreateMove(float frametime, struct usercmd_s *cmd, int active);

private:
	void StrafeVectorial(struct usercmd_s *cmd);
};

extern CStrafer g_Strafer;

#endif // STRAFER_H