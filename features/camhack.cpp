// Cam Hack

#include <hl_sdk/engine/APIProxy.h>

#include <ISvenModAPI.h>
#include <client_state.h>
#include <convar.h>
#include <dbg.h>
#include <keydefs.h>

#include "camhack.h"

#include "../game/utils.h"
#include "../config.h"

extern Vector g_oldviewangles;
extern Vector g_newviewangles;
extern Vector g_vecSpinAngles;

//-----------------------------------------------------------------------------
// Vars
//-----------------------------------------------------------------------------

CCamHack g_CamHack;

cvar_t *hud_draw = NULL;

static usercmd_t dummy_cmd;

static bool keydown_w = false;
static bool keydown_s = false;
static bool keydown_a = false;
static bool keydown_d = false;
static bool keydown_space = false;
static bool keydown_ctrl = false;
static bool keydown_shift = false;
static bool keydown_mouse1 = false;
static bool keydown_mouse2 = false;

//-----------------------------------------------------------------------------
// Utils
//-----------------------------------------------------------------------------

static inline void user_PM_NoClip(Vector &origin, Vector &va, float frametime, struct usercmd_s *cmd)
{
	Vector		wishvel;
	Vector		forward;
	Vector		right;
	float		fmove, smove;

	AngleVectors(va, &forward, &right, NULL);

	fmove = cmd->forwardmove;
	smove = cmd->sidemove;

	for (int i = 0; i < 3; ++i)
	{
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	}

	wishvel[2] += cmd->upmove;

	if (g_Config.cvars.camhack_speed_factor >= 0.0f)
		wishvel = wishvel * g_Config.cvars.camhack_speed_factor;

	VectorMA(origin, frametime, wishvel, origin);
}

static void ClampViewAngles(Vector &viewangles)
{
	if (viewangles[0] > 89.0f)
		viewangles[0] = 89.0f;

	if (viewangles[0] < -89.0f)
		viewangles[0] = -89.0f;

	if (viewangles[2] > 89.0f)
		viewangles[2] = 89.0f;

	if (viewangles[2] < -89.0f)
		viewangles[2] = -89.0f;
}

//-----------------------------------------------------------------------------
// Console Commands
//-----------------------------------------------------------------------------

CON_COMMAND_EXTERN_NO_WRAPPER(sc_camhack, ConCommand_CamHack, "Toggle CamHack")
{
	if ( !Client()->IsInGame() || Client()->IsSpectating() )
		return;

	if (g_CamHack.IsEnabled())
	{
		Msg("CamHack disabled\n");
		g_CamHack.Disable();
	}
	else
	{
		Msg("CamHack enabled\n");
		g_CamHack.Enable();
	}
}

CON_COMMAND_EXTERN_NO_WRAPPER(sc_camhack_reset_roll, ConCommand_CamHackResetRoll, "Reset camera's roll axis to zero")
{
	if (g_CamHack.IsEnabled())
	{
		g_CamHack.ResetRollAxis();
	}
}

CON_COMMAND_EXTERN_NO_WRAPPER(sc_camhack_reset, ConCommand_CamHackReset, "Teleport to your original position")
{
	if (g_CamHack.IsEnabled())
	{
		g_CamHack.ResetOrientation();
	}
}

//-----------------------------------------------------------------------------
// Cam Hack Callbacks
//-----------------------------------------------------------------------------

bool CCamHack::StudioRenderModel()
{
	if (m_bEnabled && !g_pClientFuncs->CL_IsThirdPerson() && g_pStudioRenderer->m_pCurrentEntity == g_pEngineFuncs->GetViewModel())
		return true;

	return false;
}

void CCamHack::OnVideoInit()
{
	if (m_bEnabled)
	{
		Disable();
	}
}

bool CCamHack::OnKeyPress(int down, int keynum)
{
	bool bKeyDown = (down != 0);
	
	switch (keynum)
	{
	case K_SPACE:
		keydown_space = bKeyDown;
		break;

	case 'w':
		keydown_w = bKeyDown;
		break;
		
	case 's':
		keydown_s = bKeyDown;
		break;
		
	case 'a':
		keydown_a = bKeyDown;
		break;
		
	case 'd':
		keydown_d = bKeyDown;
		break;
		
	case K_CTRL:
		keydown_ctrl = bKeyDown;
		break;

	case K_SHIFT:
		keydown_shift = bKeyDown;
		break;

	case K_MOUSE1:
		keydown_mouse1 = bKeyDown;
		break;

	case K_MOUSE2:
		keydown_mouse2 = bKeyDown;
		break;

	default:
		return false;
	}

	return true;
}

void CCamHack::CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	if (m_bEnabled)
	{
		float flMaxSpeed = Client()->GetMaxSpeed();

		bool bAnglesChanged = false;

		if (g_Config.cvars.spin_yaw_angle)
		{
			bAnglesChanged = true;
		}
		else if (g_Config.cvars.lock_yaw)
		{
			bAnglesChanged = true;
		}

		if (g_Config.cvars.spin_pitch_angle)
		{
			bAnglesChanged = true;
		}
		else if (g_Config.cvars.lock_pitch)
		{
			bAnglesChanged = true;
		}

		dummy_cmd.forwardmove = 0.f;
		dummy_cmd.sidemove = 0.f;
		dummy_cmd.upmove = 0.f;

		if (keydown_shift)
			flMaxSpeed /= 2;

		if (keydown_w)
			dummy_cmd.forwardmove += flMaxSpeed;
		
		if (keydown_s)
			dummy_cmd.forwardmove -= flMaxSpeed;
		
		if (keydown_d)
			dummy_cmd.sidemove += flMaxSpeed;
		
		if (keydown_a)
			dummy_cmd.sidemove -= flMaxSpeed;

		if (keydown_space)
			dummy_cmd.upmove += flMaxSpeed;
		
		if (keydown_ctrl)
			dummy_cmd.upmove -= flMaxSpeed;

		if (keydown_mouse1)
			g_CamHack.m_vecCameraAngles.z -= 0.2f;

		if (keydown_mouse2)
			g_CamHack.m_vecCameraAngles.z += 0.2f;

		dummy_cmd.upmove *= 0.75f;

		Vector va_delta = g_newviewangles - g_oldviewangles;

		// ToDo: for better rotation, use quaternions when the camera is tilted
		g_CamHack.m_vecCameraAngles += va_delta;

		NormalizeAngles(g_CamHack.m_vecCameraAngles);
		ClampViewAngles(g_CamHack.m_vecCameraAngles);

		user_PM_NoClip(g_CamHack.m_vecCameraOrigin, g_CamHack.m_vecCameraAngles, Client()->Frametime(), &dummy_cmd);

		cmd->viewangles = bAnglesChanged ? g_vecSpinAngles : m_vecViewAngles;
	}
}

void CCamHack::V_CalcRefdef(struct ref_params_s *pparams)
{
	if (m_bEnabled)
	{
		if ( !Client()->IsSpectating() )
		{
			cl_entity_t *pLocal = g_pEngineFuncs->GetLocalPlayer();

			*reinterpret_cast<Vector *>(pparams->vieworg) = g_CamHack.m_vecCameraOrigin;
			*reinterpret_cast<Vector *>(pparams->viewangles) = g_CamHack.m_vecCameraAngles;

			pLocal->angles.x = g_CamHack.m_flSavedPitchAngle;
			pLocal->curstate.angles.x = g_CamHack.m_flSavedPitchAngle;
			pLocal->prevstate.angles.x = g_CamHack.m_flSavedPitchAngle;
			pLocal->latched.prevangles.x = g_CamHack.m_flSavedPitchAngle;
		}
		else
		{
			g_CamHack.Disable();
		}
	}
}

//-----------------------------------------------------------------------------
// Cam Hack implementations
//-----------------------------------------------------------------------------

CCamHack::CCamHack()
{
	m_bEnabled = false;

	m_flSavedPitchAngle = 0.0f;

	m_bChangeCameraState = false;
	m_bChangeToThirdPerson = false;

	m_vecCameraOrigin = { 0.0f, 0.0f, 0.0f };
	m_vecCameraAngles = { 0.0f, 0.0f, 0.0f };

	m_vecViewAngles = { 0.0f, 0.0f, 0.0f };

	memset(&dummy_cmd, 0, sizeof(usercmd_t));
}

void CCamHack::Init()
{
	hud_draw = CVar()->FindCvar("hud_draw");

	if ( !hud_draw )
		Sys_Error("[Sven Internal] Can't find cvar hud_draw");
}

void CCamHack::Enable()
{
	m_bEnabled = true;

	keydown_w = false;
	keydown_s = false;
	keydown_a = false;
	keydown_d = false;
	keydown_space = false;
	keydown_ctrl = false;
	keydown_shift = false;
	keydown_mouse1 = false;
	keydown_mouse2 = false;

	bool bAnglesChanged = false;

	if (g_Config.cvars.spin_yaw_angle)
	{
		bAnglesChanged = true;
	}
	else if (g_Config.cvars.lock_yaw)
	{
		bAnglesChanged = true;
	}

	if (g_Config.cvars.spin_pitch_angle)
	{
		bAnglesChanged = true;
	}
	else if (g_Config.cvars.lock_pitch)
	{
		bAnglesChanged = true;
	}

	if (bAnglesChanged)
	{
		m_vecViewAngles = g_vecSpinAngles;
	}
	else
	{
		g_pEngineFuncs->GetViewAngles(m_vecViewAngles);
	}

	m_vecCameraOrigin = g_pPlayerMove->origin + Client()->GetViewOffset();
	m_vecCameraAngles = m_vecViewAngles;

	m_flSavedPitchAngle = NormalizeAngle(m_vecViewAngles.x) / -3.0f;

	if (g_Config.cvars.camhack_hide_hud)
	{
		CVar()->SetValue(hud_draw, 0);
	}

	if (g_Config.cvars.camhack_show_model)
	{
		if ( !g_pClientFuncs->CL_IsThirdPerson() )
		{
			g_pEngineFuncs->ClientCmd("thirdperson\n");
			m_bChangeToThirdPerson = false;
		}
		else
		{
			m_bChangeToThirdPerson = true;
		}

		m_bChangeCameraState = true;
	}
	else
	{
		if ( g_pClientFuncs->CL_IsThirdPerson() )
			g_pEngineFuncs->ClientCmd("firstperson\n");

		m_bChangeCameraState = false;
	}
}

void CCamHack::Disable()
{
	m_bEnabled = false;

	keydown_w = false;
	keydown_s = false;
	keydown_a = false;
	keydown_d = false;
	keydown_space = false;
	keydown_ctrl = false;
	keydown_shift = false;
	keydown_mouse1 = false;
	keydown_mouse2 = false;

	if (g_Config.cvars.camhack_hide_hud)
	{
		CVar()->SetValue(hud_draw, 1);
	}

	if (m_bChangeCameraState)
	{
		if (m_bChangeToThirdPerson)
		{
			g_pEngineFuncs->ClientCmd("thirdperson\n");
		}
		else
		{
			g_pEngineFuncs->ClientCmd("firstperson\n");
		}
	}

	g_pEngineFuncs->SetViewAngles(m_vecViewAngles);

	m_bChangeCameraState = false;
}

void CCamHack::ResetRollAxis()
{
	m_vecCameraAngles.z = 0.0f;
}

void CCamHack::ResetOrientation()
{
	m_vecCameraOrigin = g_pPlayerMove->origin + Client()->GetViewOffset();
}