/*
Copyright (C) 2011 azazello and ezQuake team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// HUD: clock, democlock, gameclock etc

#include "quakedef.h"
#include "common_draw.h"
#include "hud.h"
#include "hud_common.h"
#include "utils.h"

extern cvar_t scr_newHud;

static const char *SCR_HUD_ClockFormat(int format)
{
	switch (format) {
		case 1: return "%I:%M %p";
		case 2: return "%I:%M:%S %p";
		case 3: return "%H:%M";
		default: case 0: return "%H:%M:%S";
	}
}

//---------------------
//
// draw HUD clock
//
static void SCR_HUD_DrawClock(hud_t *hud)
{
	int width, height;
	int x, y;
	const char *t;

	static cvar_t
		*hud_clock_big = NULL,
		*hud_clock_style,
		*hud_clock_blink,
		*hud_clock_scale,
		*hud_clock_format;

	if (hud_clock_big == NULL) {
		// first time
		hud_clock_big = HUD_FindVar(hud, "big");
		hud_clock_style = HUD_FindVar(hud, "style");
		hud_clock_blink = HUD_FindVar(hud, "blink");
		hud_clock_scale = HUD_FindVar(hud, "scale");
		hud_clock_format = HUD_FindVar(hud, "format");
	}

	t = SCR_GetTimeString(TIMETYPE_CLOCK, SCR_HUD_ClockFormat(hud_clock_format->integer));
	width = SCR_GetClockStringWidth(t, hud_clock_big->integer, hud_clock_scale->value);
	height = SCR_GetClockStringHeight(hud_clock_big->integer, hud_clock_scale->value);

	if (HUD_PrepareDraw(hud, width, height, &x, &y)) {
		if (hud_clock_big->value) {
			SCR_DrawBigClock(x, y, hud_clock_style->value, hud_clock_blink->value, hud_clock_scale->value, t);
		}
		else {
			SCR_DrawSmallClock(x, y, hud_clock_style->value, hud_clock_blink->value, hud_clock_scale->value, t);
		}
	}
}

//---------------------
//
// draw HUD gameclock
//
static void SCR_HUD_DrawGameClock(hud_t *hud)
{
	int width, height;
	int x, y;
	int timetype;
	const char *t;

	static cvar_t
		*hud_gameclock_big = NULL,
		*hud_gameclock_style,
		*hud_gameclock_blink,
		*hud_gameclock_countdown,
		*hud_gameclock_scale,
		*hud_gameclock_offset;

	if (hud_gameclock_big == NULL)    // first time
	{
		hud_gameclock_big = HUD_FindVar(hud, "big");
		hud_gameclock_style = HUD_FindVar(hud, "style");
		hud_gameclock_blink = HUD_FindVar(hud, "blink");
		hud_gameclock_countdown = HUD_FindVar(hud, "countdown");
		hud_gameclock_scale = HUD_FindVar(hud, "scale");
		hud_gameclock_offset = HUD_FindVar(hud, "offset");
		gameclockoffset = &hud_gameclock_offset->integer;
	}

	timetype = (hud_gameclock_countdown->value) ? TIMETYPE_GAMECLOCKINV : TIMETYPE_GAMECLOCK;
	t = SCR_GetTimeString(timetype, NULL);
	width = SCR_GetClockStringWidth(t, hud_gameclock_big->integer, hud_gameclock_scale->value);
	height = SCR_GetClockStringHeight(hud_gameclock_big->integer, hud_gameclock_scale->value);

	if (HUD_PrepareDraw(hud, width, height, &x, &y)) {
		if (hud_gameclock_big->value)
			SCR_DrawBigClock(x, y, hud_gameclock_style->value, hud_gameclock_blink->value, hud_gameclock_scale->value, t);
		else
			SCR_DrawSmallClock(x, y, hud_gameclock_style->value, hud_gameclock_blink->value, hud_gameclock_scale->value, t);
	}
}

//---------------------
//
// draw HUD democlock
//
static void SCR_HUD_DrawDemoClock(hud_t *hud)
{
	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;
	const char *t;
	static cvar_t
		*hud_democlock_big = NULL,
		*hud_democlock_style,
		*hud_democlock_blink,
		*hud_democlock_scale;

	if (!cls.demoplayback || cls.mvdplayback == QTV_PLAYBACK) {
		HUD_PrepareDraw(hud, width, height, &x, &y);
		return;
	}

	if (hud_democlock_big == NULL) {
		// first time
		hud_democlock_big = HUD_FindVar(hud, "big");
		hud_democlock_style = HUD_FindVar(hud, "style");
		hud_democlock_blink = HUD_FindVar(hud, "blink");
		hud_democlock_scale = HUD_FindVar(hud, "scale");
	}

	t = SCR_GetTimeString(TIMETYPE_DEMOCLOCK, NULL);
	width = SCR_GetClockStringWidth(t, hud_democlock_big->integer, hud_democlock_scale->value);
	height = SCR_GetClockStringHeight(hud_democlock_big->integer, hud_democlock_scale->value);

	if (HUD_PrepareDraw(hud, width, height, &x, &y)) {
		if (hud_democlock_big->value) {
			SCR_DrawBigClock(x, y, hud_democlock_style->value, hud_democlock_blink->value, hud_democlock_scale->value, t);
		}
		else {
			SCR_DrawSmallClock(x, y, hud_democlock_style->value, hud_democlock_blink->value, hud_democlock_scale->value, t);
		}
	}
}

///
/// cl_screen.c clock module: to be merged
///
static void OnChange_scr_clock_format(cvar_t *var, char *value, qbool *cancel);

// non-static for menu
cvar_t scr_clock = { "cl_clock", "0" };
static cvar_t scr_clock_format = { "cl_clock_format", "%H:%M:%S", 0, OnChange_scr_clock_format };
static cvar_t scr_clock_x = { "cl_clock_x", "0" };
static cvar_t scr_clock_y = { "cl_clock_y", "-1" };

// non-static for menu
cvar_t scr_gameclock = { "cl_gameclock", "0" };
static cvar_t scr_gameclock_x = { "cl_gameclock_x", "0" };
static cvar_t scr_gameclock_y = { "cl_gameclock_y", "-3" };
static cvar_t scr_gameclock_offset = { "cl_gameclock_offset", "0" };

static cvar_t scr_democlock = { "cl_democlock", "0" };
static cvar_t scr_democlock_x = { "cl_democlock_x", "0" };
static cvar_t scr_democlock_y = { "cl_democlock_y", "-2" };

static void OnChange_scr_clock_format(cvar_t *var, char *value, qbool *cancel)
{
	if (!host_initialized) {
		return; // we in progress of initialization, allow
	}

	// MEAG: You what now?
	if (cls.state == ca_active) {
		Com_Printf("Can't change %s while connected\n", var->name);
		*cancel = true; // prevent stick notes
		return;
	}
}

static void SCR_DrawClock(void)
{
	int x, y;
	time_t t;
	struct tm *ptm;
	char str[64];

	if (!scr_clock.value || scr_newHud.value == 1) {
		// newHud has its own clock
		return;
	}

	if (scr_clock.value == 2) {
		time(&t);
		if ((ptm = localtime(&t))) {
			strftime(str, sizeof(str) - 1, scr_clock_format.string[0] ? scr_clock_format.string : "%H:%M:%S", ptm);
		}
		else {
			strlcpy(str, "#bad date#", sizeof(str));
		}
	}
	else {
		float time = (cl.servertime_works) ? cl.servertime : cls.realtime;
		strlcpy(str, SecondsToHourString((int)time), sizeof(str));
	}

	x = ELEMENT_X_COORD(scr_clock);
	y = ELEMENT_Y_COORD(scr_clock);
	Draw_String(x, y, str);
}

static void SCR_DrawGameClock(void)
{
	int x, y;
	char str[80], *s;
	float timelimit;

	if (!scr_gameclock.value || scr_newHud.value == 1) {
		// newHud has its own gameclock
		return;
	}

	if (scr_gameclock.value == 2 || scr_gameclock.value == 4) {
		timelimit = 60 * Q_atof(Info_ValueForKey(cl.serverinfo, "timelimit")) + 1;
	}
	else {
		timelimit = 0;
	}

	if (cl.countdown || cl.standby) {
		strlcpy(str, SecondsToHourString(timelimit), sizeof(str));
	}
	else {
		strlcpy(str, SecondsToHourString((int)abs(timelimit - cl.gametime + scr_gameclock_offset.value)), sizeof(str));
	}

	if ((scr_gameclock.value == 3 || scr_gameclock.value == 4) && (s = strstr(str, ":"))) {
		// or just use SecondsToMinutesString() ...
		s++;
	}
	else {
		s = str;
	}

	x = ELEMENT_X_COORD(scr_gameclock);
	y = ELEMENT_Y_COORD(scr_gameclock);
	Draw_String(x, y, s);
}

static void SCR_DrawDemoClock(void)
{
	int x, y;
	char str[80];

	if (!cls.demoplayback || cls.mvdplayback == QTV_PLAYBACK || !scr_democlock.value || scr_newHud.value == 1) {
		// newHud has its own democlock
		return;
	}

	if (scr_democlock.value == 2) {
		strlcpy(str, SecondsToHourString((int)(cls.demotime)), sizeof(str));
	}
	else {
		strlcpy(str, SecondsToHourString((int)(cls.demotime - demostarttime)), sizeof(str));
	}

	x = ELEMENT_X_COORD(scr_democlock);
	y = ELEMENT_Y_COORD(scr_democlock);
	Draw_String(x, y, str);
}

void SCR_DrawClocks(void)
{
	SCR_DrawClock();
	SCR_DrawGameClock();
	SCR_DrawDemoClock();
}

/// Initialisation
void Clock_HudInit(void)
{
	Cvar_Register(&scr_clock_x);
	Cvar_Register(&scr_clock_y);
	Cvar_Register(&scr_clock_format);
	Cvar_Register(&scr_clock);

	Cvar_Register(&scr_gameclock_offset);
	Cvar_Register(&scr_gameclock_x);
	Cvar_Register(&scr_gameclock_y);
	Cvar_Register(&scr_gameclock);

	Cvar_Register(&scr_democlock_x);
	Cvar_Register(&scr_democlock_y);
	Cvar_Register(&scr_democlock);

	// init clock
	HUD_Register(
		"clock", NULL, "Shows current local time (hh:mm:ss).",
		HUD_PLUSMINUS, ca_disconnected, 8, SCR_HUD_DrawClock,
		"0", "top", "right", "console", "0", "0", "0", "0 0 0", NULL,
		"big", "1",
		"style", "0",
		"scale", "1",
		"blink", "1",
		"format", "0",
		NULL
	);

	// init democlock
	HUD_Register(
		"democlock", NULL, "Shows current demo time (hh:mm:ss).",
		HUD_PLUSMINUS, ca_disconnected, 7, SCR_HUD_DrawDemoClock,
		"1", "top", "right", "console", "0", "8", "0", "0 0 0", NULL,
		"big", "0",
		"style", "0",
		"scale", "1",
		"blink", "0",
		NULL
	);

	// init gameclock
	HUD_Register(
		"gameclock", NULL, "Shows current game time (hh:mm:ss).",
		HUD_PLUSMINUS, ca_disconnected, 8, SCR_HUD_DrawGameClock,
		"1", "top", "right", "console", "0", "0", "0", "0 0 0", NULL,
		"big", "1",
		"style", "0",
		"scale", "1",
		"blink", "1",
		"countdown", "0",
		"offset", "0",
		NULL
	);
}