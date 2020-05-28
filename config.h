/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int gappx     = 20;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "hack:size=10" };
static const char dmenufont[]       = "hack:size=10";
static const char col_gray1[]       = "#2e3440";
static const char col_gray2[]       = "#434c5e";
static const char col_gray3[]       = "#e5e9f0";
static const char col_gray4[]       = "#d8dee9";
static const char col_cyan[]        = "#88c0d0";
static const char *colors[][3]      = {
  /*               fg         bg         border   */
  [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
  [SchemeSel]  = { col_gray1, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
  /* xprop(1):
   *  WM_CLASS(STRING) = instance, class
   *  WM_NAME(STRING) = title
   */
  /* class      instance    title       tags mask     isfloating   monitor */
  { "Gimp",     NULL,       NULL,       0,            1,           -1 },
  { "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

#include "fibonacci.c"
static const Layout layouts[] = {
  /* symbol     arrange function */
  { "[]=",      tile },    /* first entry is default */
  { "><>",      NULL },    /* no layout function means floating behavior */
  { "[M]",      monocle },
  { "[@]",      spiral },
  { "[\\]",     dwindle },
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
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray1, NULL };
static const char *menucmd[]  = { "rofi", "-show", NULL };
static const char *termcmd[]  = { "urxvt", NULL };
/* volume */
static const char *upvol[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%",     NULL };
static const char *downvol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%",     NULL };
static const char *mutevol[] = { "pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle",  NULL };
static const char *dwmblocksvol[] = { "pkill", "-RTMIN+1", "dwmblocks", NULL };
/* multimedia keys */
static const char *mmplay[] = { "playerctl", "play-pause", NULL };
static const char *mmnext[] = { "playerctl", "next",       NULL };
static const char *mmprev[] = { "playerctl", "previous",   NULL };
/* brightness */
static const char *upbright[]   = {"light", "-A", "5", NULL};
static const char *downbright[] = {"light", "-U", "5", NULL};

static Key keys[] = {
  /* modifier                     key                        function        argument */
  { MODKEY,                       XK_p,                      spawn,          {.v = dmenucmd } },
  { MODKEY|ShiftMask,             XK_Return,                 spawn,          {.v = termcmd } },
  { MODKEY,                       XK_b,                      togglebar,      {0} },
  { MODKEY,                       XK_j,                      focusstack,     {.i = +1 } },
  { MODKEY,                       XK_k,                      focusstack,     {.i = -1 } },
  { MODKEY,                       XK_i,                      incnmaster,     {.i = +1 } },
  { MODKEY,                       XK_d,                      incnmaster,     {.i = -1 } },
  { MODKEY,                       XK_h,                      setmfact,       {.f = -0.05} },
  { MODKEY,                       XK_l,                      setmfact,       {.f = +0.05} },
  { MODKEY,                       XK_Return,                 zoom,           {0} },
  { MODKEY,                       XK_Tab,                    view,           {0} },
  { MODKEY|ShiftMask,             XK_q,                      killclient,     {0} },
  { MODKEY,                       XK_t,                      setlayout,      {.v = &layouts[0]} },
  { MODKEY,                       XK_f,                      setlayout,      {.v = &layouts[1]} },
  { MODKEY,                       XK_m,                      setlayout,      {.v = &layouts[2]} },
  { MODKEY,                       XK_space,                  spawn,          {.v = menucmd} },
  { MODKEY|ShiftMask,             XK_space,                  togglefloating, {0} },
  { MODKEY,                       XK_0,                      view,           {.ui = ~0 } },
  { MODKEY|ShiftMask,             XK_0,                      tag,            {.ui = ~0 } },
  { MODKEY,                       XK_comma,                  focusmon,       {.i = -1 } },
  { MODKEY,                       XK_period,                 focusmon,       {.i = +1 } },
  { MODKEY|ShiftMask,             XK_comma,                  tagmon,         {.i = -1 } },
  { MODKEY|ShiftMask,             XK_period,                 tagmon,         {.i = +1 } },
  /* fibonacci */
  { MODKEY,                       XK_r,                      setlayout,      {.v = &layouts[3]} },
  { MODKEY|ShiftMask,             XK_r,                      setlayout,      {.v = &layouts[4]} },
  /* gaps */
  { MODKEY,                       XK_minus,                  setgaps,        {.i = -1 } },
  { MODKEY,                       XK_equal,                  setgaps,        {.i = +1 } },
  { MODKEY|ShiftMask,             XK_minus,                  setgaps,        {.i = 0  } },
  /* volume keys */
  { 0,                            XF86XK_AudioLowerVolume,   spawn,          {.v = downvol } },
  { 0,                            XF86XK_AudioMute,          spawn,          {.v = mutevol } },
  { 0,                            XF86XK_AudioRaiseVolume,   spawn,          {.v = upvol   } },
  { 0,                            XF86XK_AudioLowerVolume,   spawn,          {.v = dwmblocksvol } },
  { 0,                            XF86XK_AudioMute,          spawn,          {.v = dwmblocksvol } },
  { 0,                            XF86XK_AudioRaiseVolume,   spawn,          {.v = dwmblocksvol } },
  /* multimedia keys */
  { 0,                            XF86XK_AudioPlay,          spawn,          {.v = mmplay } },
  { 0,                            XF86XK_AudioNext,          spawn,          {.v = mmnext } },
  { 0,                            XF86XK_AudioPrev,          spawn,          {.v = mmprev } },
  /* brightness keys */
  { 0,                            XF86XK_MonBrightnessUp,    spawn,          {.v = upbright   } },
  { 0,                            XF86XK_MonBrightnessDown,  spawn,          {.v = downbright } },
  /* screenshot */
  { 0,                            XK_Print,                  spawn,          SHCMD("maim -su | xclip -sel clip -t image/png") },
  { ShiftMask,                    XK_Print,                  spawn,          SHCMD("maim -u | xclip -sel clip -t image/png")  },
  /* quit */
  { MODKEY|ShiftMask,             XK_e,                      quit,           {0} },
  TAGKEYS(                        XK_1,                                      0)
  TAGKEYS(                        XK_2,                                      1)
  TAGKEYS(                        XK_3,                                      2)
  TAGKEYS(                        XK_4,                                      3)
  TAGKEYS(                        XK_5,                                      4)
  TAGKEYS(                        XK_6,                                      5)
  TAGKEYS(                        XK_7,                                      6)
  TAGKEYS(                        XK_8,                                      7)
  TAGKEYS(                        XK_9,                                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
  /* click                event mask      button          function        argument */
  { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
  { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
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

