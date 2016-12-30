/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
#define NUMCOLORS         4             // need at least 4
static const char colors[NUMCOLORS][ColLast][8] = {
	// border   foreground  background
	{ "#000000", "#aaaaaa", "#000000" },  // 0 = normal
	{ "#000000", "#000000", "#aaaaaa" },  // 1 = selected
	{ "#0066ff", "#ff0000", "#000000" },  // 2 = urgent/warning
	{ "#000000", "#000000", "#000000" },  // 3 = hidden
	// add more here
};
static const char font[]            = "-*-fixed-*-*-*-*-20-*-*-*-*-*-*-*";
static unsigned int borderpx        = 1;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static Bool showbar                 = True;     /* False means no bar */
static Bool topbar                  = True;     /* False means bottom bar */

/* tagging */
static const char tags[][MAXTAGLEN] =
	{ "1", "2", "3", "Q", "W", "E", "A", "S", "D" };
static unsigned int tagset[] = {1, 1}; /* after start, first tag is selected */

static Rule rules[] = {
	/* class      instance    title       tags mask     isfloating */
	{ NULL,       NULL,       " (@1)",    (1<<0),       False },
	{ NULL,       NULL,       " (@2)",    (1<<1),       False },
	{ NULL,       NULL,       " (@3)",    (1<<2),       False },
	{ NULL,       NULL,       " (@Q)",    (1<<3),       False },
	{ NULL,       NULL,       " (@W)",    (1<<4),       False },
	{ NULL,       NULL,       " (@E)",    (1<<5),       False },
	{ NULL,       NULL,       " (@A)",    (1<<6),       False },
	{ NULL,       NULL,       " (@S)",    (1<<7),       False },
	{ NULL,       NULL,       " (@D)",    (1<<8),       False },
	{ NULL,       NULL,       " (@0)",    ~0,           False },
	{ NULL,       NULL,       " (@F)",    0,            True },
// Floating apps.
	{ "feh",      NULL,       NULL,       0,            True },
	{ NULL,       NULL,       "plugin-container", 0,    True },
// Terminal apps.
	{ "Rxvt",     NULL,       NULL,       0,            TERMINAL },
	{ "terminal", NULL,       NULL,       0,            TERMINAL },
	{ "Terminal", NULL,       NULL,       0,            TERMINAL },
	{ "xterm",    NULL,       NULL,       0,            TERMINAL },
	{ "XTerm",    NULL,       NULL,       0,            TERMINAL },
};

/* layout(s) */
static float mfact      = 0.50; /* factor of master area size [0.05..0.95] */
static Bool resizehints = True; /* False means respect size hints in tiled resizals */

static Layout layouts[] = {
	/* symbol     arrange function */
	{ "",      tile },    /* first entry is default */
	//{ "",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *dmenucmd[] = { "dmenu_run", "-fn", font, "-nb", colors[0][ColBG], "-nf", colors[0][ColFG], "-sb", colors[1][ColBG], "-sf", colors[1][ColFG], NULL };
static const char *termcmd[] = { "xterm", NULL };
static const char *firefoxcmd[] = { "firefox", NULL };
static const char *thunarcmd[] = { "thunar", NULL };
static const char *volumeupcmd[] = { "volume", "up", NULL };
static const char *volumedowncmd[] = { "volume", "down", NULL };
static const char *volumemutecmd[] = { "volume", "toggle", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ Mod1Mask|ShiftMask,           XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_x,      spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,		XK_z,      spawn,          {.v = firefoxcmd } },
	{ MODKEY,			XK_z,      spawn,          {.v = thunarcmd } },

	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_m,      setmfact,       {.f = 1+0.50} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	//{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	//{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	//{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	TAGKEYS(                        XK_q,                      3)
	TAGKEYS(                        XK_w,                      4)
	TAGKEYS(                        XK_e,                      5)
	TAGKEYS(                        XK_a,                      6)
	TAGKEYS(                        XK_s,                      7)
	TAGKEYS(                        XK_d,                      8)
	{ MODKEY|ShiftMask,             XK_Escape, quit,           {0} },
	{ 0,            XF86XK_AudioRaiseVolume,   spawn,          {.v = volumeupcmd } },
	{ 0,            XF86XK_AudioLowerVolume,   spawn,          {.v = volumedowncmd } },
	{ 0,            XF86XK_AudioMute,          spawn,          {.v = volumemutecmd } },
};

/* button definitions */
/* click can be a tag number (starting at 0),
 * ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	//{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	//{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

// vim:noexpandtab:shiftwidth=8:softtabstop=8
