// Key Spam

#include <hl_sdk/engine/APIProxy.h>

#include <convar.h>
#include <dbg.h>

#include "keyspam.h"

#include "../game/utils.h"
#include "../config.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CKeySpam g_KeySpam;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CON_COMMAND_EXTERN(sc_spam_hold_mode, ConCommand_SpamHoldMode, "Toggle spam hold mode")
{
	Msg(g_Config.cvars.keyspam_hold_mode ? "Hold Mode disabled\n" : "Hold Mode enabled\n");
	g_Config.cvars.keyspam_hold_mode = !g_Config.cvars.keyspam_hold_mode;
}

CON_COMMAND_EXTERN(sc_spam_use, ConCommand_UseSpam, "Toggle E spam")
{
	g_pEngineFuncs->ClientCmd("-use");

	Msg(g_Config.cvars.keyspam_e ? "Use Spam disabled\n" : "Use Spam enabled\n");
	g_Config.cvars.keyspam_e = !g_Config.cvars.keyspam_e;
}

CON_COMMAND_EXTERN(sc_spam_forward, ConCommand_ForwardSpam, "Toggle W spam")
{
	g_pEngineFuncs->ClientCmd("-forward");

	Msg(g_Config.cvars.keyspam_w ? "Forward Spam disabled\n" : "Forward Spam enabled\n");
	g_Config.cvars.keyspam_w = !g_Config.cvars.keyspam_w;
}

CON_COMMAND_EXTERN(sc_spam_back, ConCommand_BackSpam, "Toggle S spam")
{
	g_pEngineFuncs->ClientCmd("-back");

	Msg(g_Config.cvars.keyspam_s ? "Back Spam disabled\n" : "Back Spam enabled\n");
	g_Config.cvars.keyspam_s = !g_Config.cvars.keyspam_s;
}

CON_COMMAND_EXTERN(sc_spam_q, ConCommand_SnarkSpam, "Toggle Q spam")
{
	Msg(g_Config.cvars.keyspam_q ? "Snark Spam disabled\n" : "Snark Spam enabled\n");
	g_Config.cvars.keyspam_q = !g_Config.cvars.keyspam_q;
}

CON_COMMAND_EXTERN(sc_spam_ctrl, ConCommand_CtrlSpam, "Toggle CTRL spam")
{
	g_pEngineFuncs->ClientCmd("-duck");

	Msg(g_Config.cvars.keyspam_ctrl ? "CTRL Spam disabled\n" : "CTRL Spam enabled\n");
	g_Config.cvars.keyspam_ctrl = !g_Config.cvars.keyspam_ctrl;
}

//-----------------------------------------------------------------------------

void CKeySpam::CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	KeySpam();
}

void CKeySpam::KeySpam()
{
	bool bSpam = !g_Config.cvars.keyspam_hold_mode;

	if (g_Config.cvars.keyspam_e)
	{
		static bool key_down = true;

		if (bSpam || GetAsyncKeyState(0x45)) // E
		{
			if (key_down)
				g_pEngineFuncs->ClientCmd("-use");
			else
				g_pEngineFuncs->ClientCmd("+use");

			key_down = !key_down;
		}
		else
		{
			g_pEngineFuncs->ClientCmd("-use");
			key_down = false;
		}
	}

	if (g_Config.cvars.keyspam_w)
	{
		static bool key_down = true;

		if (bSpam || GetAsyncKeyState(0x57)) // W
		{
			if (key_down)
				g_pEngineFuncs->ClientCmd("-forward");
			else
				g_pEngineFuncs->ClientCmd("+forward");

			key_down = !key_down;
		}
		else
		{
			g_pEngineFuncs->ClientCmd("-forward");
			key_down = false;
		}
	}

	if (g_Config.cvars.keyspam_s)
	{
		static bool key_down = true;

		if (bSpam || GetAsyncKeyState(0x53)) // S
		{
			if (key_down)
				g_pEngineFuncs->ClientCmd("-back");
			else
				g_pEngineFuncs->ClientCmd("+back");

			key_down = !key_down;
		}
		else
		{
			g_pEngineFuncs->ClientCmd("-back");
			key_down = false;
		}
	}
	
	if (g_Config.cvars.keyspam_ctrl)
	{
		static bool key_down = true;

		if (bSpam || GetAsyncKeyState(VK_LCONTROL)) // CTRL
		{
			if (key_down)
				g_pEngineFuncs->ClientCmd("-duck");
			else
				g_pEngineFuncs->ClientCmd("+duck");

			key_down = !key_down;
		}
		else
		{
			g_pEngineFuncs->ClientCmd("-duck");
			key_down = false;
		}
	}

	if (g_Config.cvars.keyspam_q && (bSpam || GetAsyncKeyState(0x51))) // Q
	{
		g_pEngineFuncs->ClientCmd("lastinv");
	}
}