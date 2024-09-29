/* See LICENSE file for copyright and license details. */

/* Constants */
#include <X11/X.h>
#include <X11/Xutil.h>
#define TERMINAL "st"
#define TERMCLASS "St"
#define BROWSER "firefox"

/* appearance */
static unsigned int borderpx = 3; /* border pixel of windows */
static unsigned int snap = 32;    /* snap pixel */
static unsigned int gappih = 20;  /* horiz inner gap between windows */
static unsigned int gappiv = 10;  /* vert inner gap between windows */
static unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static unsigned int gappov =
    30; /* vert outer gap between windows and screen edge */
static int swallowfloating =
    0; /* 1 means swallow floating windows by default */
static int smartgaps =
    0;                  /* 1 means no outer gap when there is only one window */
static int showbar = 1; /* 0 means no bar */
static int topbar = 1;  /* 0 means bottom bar */
static char *fonts[] = {"ZedMono NerdFont:style:bold:pixelsize=16"};
static char normbgcolor[] = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[] = "#bbbbbb";
static char selfgcolor[] = "#eeeeee";
static char selbordercolor[] = "#770000";
static char selbgcolor[] = "#005577";
static char bluebgcolor[] = "#005577";
static char *colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
    [SchemeBlue] = {normbgcolor, bluebgcolor, bluebgcolor}};

typedef struct {
  const char *name;
  const void *cmd;
} Sp;
const char *spcmd1[] = {TERMINAL, "-n", "spterm", "-g", "120x34", NULL};
const char *spcmd2[] = {TERMINAL,
                        "-n",
                        "spcalc",
                        "-f",
                        "ZedMono NerdFont:style:bold:pixelsize=16",
                        "-g",
                        "50x20",
                        "-e",
                        "bc",
                        "-lq",
                        NULL};
static Sp scratchpads[] = {
    /* name          cmd  */
    {"spterm", spcmd1},
    {"spcalc", spcmd2},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class    instance      title       	 tags mask    isfloating
       isterminal  noswallow  monitor */
    {"firefox", NULL, NULL, 1, 0, 0, 0, 1},
    {"vesktop", NULL, NULL, 1 << 1, 0, 0, 0, 0},
    {"Spotify", NULL, NULL, 1 << 3, 0, 0, 0, 1},
    {"pavucontrol", NULL, NULL, 1 << 6, 1, 0, 0, -1},
    {NULL, NULL, "Carla - CarlaConfig.carxp", 64, 0, 0, 0, 1},
};

/* layout(s) */
static float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static int nmaster = 1;     /* number of clients in master area */
static int resizehints = 0; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */
#define FORCE_VSPLIT                                                           \
  1 /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"
static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile},   /* Default: Master on left, slaves on right */
    {"TTT", bstack}, /* Master on top, slaves on bottom */

    {"[@]", spiral},   /* Fibonacci spiral */
    {"[\\]", dwindle}, /* Decreasing in size right and leftward */

    {"[D]", deck},    /* Master on left, slaves in monocle-like mode on right */
    {"[M]", monocle}, /* All windows on top of eachother */

    {"|M|", centeredmaster},         /* Master in middle, slaves on sides */
    {">M>", centeredfloatingmaster}, /* Same but master floats */

    {"><>", NULL}, /* no layout function means floating behavior */
    {NULL, NULL},
};

static int not_alerted_tags[] = {1 << 6};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD,  XK_v,   ACTION##stack,  {.i = 0 } }, \
	/* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = {TERMINAL, NULL};

/* autostart */
static const char *autostartcmds[] = {
  "xrdb merge ~/.Xresources &",
  "autorandr multi &",
  "nvidia-settings --load-config-only &",
  "systemctl --user start opentabletdriver.service &",
  "/usr/lib/xfce-polkit/xfce-polkit &",
  "dunst &",
  "spotify &",
  "firefox &",
  "vesktop &",
  "/home/radsteve/NowPlaying.sh &",
  "syncthing serve --no-browser &",
  "feh --bg-fill /home/radsteve/Pictures/wallpapers/Montains.png &",
  "/home/radsteve/IdeaProjects/arduino-goxlr-rs/target/debug &",
  "qpwgraph -x -m /home/radsteve/Patchbay.qpwgraph &",
  "PIPEWIRE_LATENCY='512/96000' carla /home/radsteve/CarlaConfig.carxp &",
  "dwmblocks &",
  "/home/radsteve/.config/dwmblocks/updater.sh &",
  "companion-headless &"
};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
  {"normbordercolor", STRING, &normbordercolor},
  {"selbordercolor", STRING, &selbordercolor},
  {"normbgcolor", STRING, &normbgcolor},
  {"normfgcolor", STRING, &normfgcolor},
  {"selfgcolor", STRING, &selfgcolor},
  {"selbgcolor", STRING, &selbgcolor},
  {"borderpx", INTEGER, &borderpx},
  {"snap", INTEGER, &snap},
  {"showbar", INTEGER, &showbar},
  {"topbar", INTEGER, &topbar},
  {"nmaster", INTEGER, &nmaster},
  {"resizehints", INTEGER, &resizehints},
  {"mfact", FLOAT, &mfact},
  {"gappih", INTEGER, &gappih},
  {"gappiv", INTEGER, &gappiv},
  {"gappoh", INTEGER, &gappoh},
  {"gappov", INTEGER, &gappov},
  {"swallowfloating", INTEGER, &swallowfloating},
  {"smartgaps", INTEGER, &smartgaps},
  {"bluebgcolor", STRING, &bluebgcolor}
};

#include "shiftview.c"
#include <X11/XF86keysym.h>

static const Key keys[] = {
    /* modifier                     key        function        argument */
    STACKKEYS(MODKEY,                          focus)
    STACKKEYS(MODKEY | ShiftMask,              push)
    {MODKEY,                        XK_grave,  spawn,          {.v = (const char *[]){"dmenuunicode", NULL}}},
    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)
    {MODKEY,                        XK_0,      view,           {.ui = ~0}},
    {MODKEY | ShiftMask,            XK_0,      tag,            {.ui = ~0}},
    {MODKEY,                        XK_minus,  spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-")},
    {MODKEY | ShiftMask,            XK_minus,  spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 15%-")},
    {MODKEY,                        XK_equal,  spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+")},
    {MODKEY | ShiftMask,            XK_equal,  spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 15%+")},
    {MODKEY,                        XK_Tab,    view,           {0}},
    {MODKEY,                        XK_q,      killclient,     {0}},
    {MODKEY | ShiftMask,            XK_w,      spawn,          {.v = (const char *[]){BROWSER, NULL}}},
    {MODKEY,                        XK_t,      setlayout,      {.v = &layouts[0]}}, /* tile */
    {MODKEY | ShiftMask,            XK_t,      setlayout,      {.v = &layouts[1]}}, /* bstack */
    {MODKEY,                        XK_y,      setlayout,      {.v = &layouts[2]}}, /* spiral */
    {MODKEY | ShiftMask,            XK_y,      setlayout,      {.v = &layouts[3]}}, /* dwindle */
    {MODKEY,                        XK_u,      setlayout,      {.v = &layouts[4]}}, /* deck */
    {MODKEY | ShiftMask,            XK_u,      setlayout,      {.v = &layouts[5]}}, /* monocle */
    {MODKEY,                        XK_i,      setlayout,      {.v = &layouts[6]}}, /* centeredmaster */
    {MODKEY | ShiftMask,            XK_i,      setlayout,      {.v = &layouts[7]}}, /* centeredfloatingmaster */
    {MODKEY,                        XK_o,      incnmaster,     {.i = +1}},
    {MODKEY | ShiftMask,            XK_o,      incnmaster,     {.i = -1}},
    {MODKEY,                        XK_backslash, view,        {0}},
    {MODKEY,                        XK_a,      togglegaps,     {0}},
    {MODKEY | ShiftMask,            XK_a,      defaultgaps,    {0}},
    {MODKEY,                        XK_s,      togglesticky,   {0}},
    {MODKEY,                        XK_d,      spawn,          SHCMD("/usr/local/bin/dmenu_run")},
    {MODKEY,                        XK_f,      togglefullscr,  {0}},
    {MODKEY | ShiftMask,            XK_f,      setlayout,      {.v = &layouts[8]}},
    {MODKEY,                        XK_g,      shiftview,      {.i = -1}},
    {MODKEY | ShiftMask,            XK_g,      shifttag,       {.i = -1}},
    {MODKEY,                        XK_h,      setmfact,       {.f = -0.05}},
    /* J and K are automatically bound above in STACKKEYS */
    {MODKEY,                        XK_l,      setmfact,       {.f = +0.05}},
    {MODKEY,                        XK_semicolon, shiftview,   {.i = 1}},
    {MODKEY | ShiftMask,            XK_semicolon, shifttag,    {.i = 1}},
    {MODKEY,                        XK_apostrophe, togglescratch, {.ui = 1}},
    {MODKEY | ShiftMask,            XK_apostrophe, togglesmartgaps, {0}},
    {MODKEY,                        XK_Return, spawn,          {.v = termcmd}},
    {MODKEY | ShiftMask,            XK_Return, togglescratch,  {.ui = 0}},
    {MODKEY,                        XK_z,      incrgaps,       {.i = +3}},
    {MODKEY,                        XK_x,      incrgaps,       {.i = -3}},
    /* V is automatically bound above in STACKKEYS */
    {MODKEY,                        XK_b,      togglebar,      {0}},
    {MODKEY | ShiftMask,            XK_m,      spawn,          SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle")},
    {MODKEY,                        XK_Left,   focusmon,       {.i = -1}},
    {MODKEY | ShiftMask,            XK_Left,   tagmon,         {.i = -1}},
    {MODKEY,                        XK_Right,  focusmon,       {.i = +1}},
    {MODKEY | ShiftMask,            XK_Right,  tagmon,         {.i = +1}},
    {MODKEY,                        XK_Page_Up, shiftview,     {.i = -1}},
    {MODKEY | ShiftMask,            XK_Page_Up, shifttag,      {.i = -1}},
    {MODKEY,                        XK_Page_Down, shiftview,   {.i = +1}},
    {MODKEY | ShiftMask,            XK_Page_Down, shifttag,    {.i = +1}},
    {MODKEY,                        XK_F5,     xrdb,           {.v = NULL}},
    {MODKEY,                        XK_space,  togglefloating, {0}},
    {MODKEY | ShiftMask,            XK_space,  zoom,           {0}},
    {MODKEY,                        XK_Scroll_Lock, spawn,     SHCMD("killall screenkey || screenkey &")},
    {0,                             XK_Print,  spawn,          {.v = (const char *[]){"/home/radsteve/.local/bin/screenshot", NULL}}},
    {0,                             XF86XK_AudioMute, spawn,   SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle")},
    {0,                             XF86XK_AudioRaiseVolume, spawn, SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 0%- && wpctl set-volume @DEFAULT_AUDIO_SINK@ 3%+")},
    {0,                             XF86XK_AudioLowerVolume, spawn, SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 0%+ && wpctl set-volume @DEFAULT_AUDIO_SINK@ 3%-")},
    {0,                             XF86XK_AudioPrev, spawn,   {.v = (const char *[]){"playerctl", "previous", NULL}}},
    {0,                             XF86XK_AudioNext, spawn,   {.v = (const char *[]){"playerctl", "next", NULL}}},
    {0,                             XF86XK_AudioPause, spawn,  {.v = (const char *[]){"playerctl", "pause", NULL}}},
    {0,                             XF86XK_AudioPlay, spawn,   {.v = (const char *[]){"playerctl", "play", NULL}}},
    {0,                             XF86XK_AudioStop, spawn,   {.v = (const char *[]){"playerctl", "stop", NULL}}},
    {MODKEY | ControlMask,          XK_q,      quit,           {.i = 23}},
    {MODKEY,                        XK_c,      spawn,          {.v = (const char *[]){"=", "--dmenu=dmenu", NULL}}}
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function          argument */
    {ClkClientWin,          MODKEY,         Button1,        movemouse,        {0}},
    {ClkClientWin,          MODKEY,         Button2,        defaultgaps,      {0}},
    {ClkClientWin,          MODKEY,         Button3,        resizemouse,      {0}},
    {ClkClientWin,          MODKEY,         Button4,        incrgaps,         {.i = +1}},
    {ClkClientWin,          MODKEY,         Button5,        incrgaps,         {.i = -1}},
    {ClkTagBar,             0,              Button1,        view,             {0}},
    {ClkTagBar,             0,              Button3,        toggleview,       {0}},
    {ClkTagBar,             MODKEY,         Button1,        tag,              {0}},
    {ClkTagBar,             MODKEY,         Button3,        toggletag,        {0}},
    {ClkTagBar,             0,              Button4,        shiftview,        {.i = -1}},
    {ClkTagBar,             0,              Button5,        shiftview,        {.i = 1}},
    {ClkRootWin,            0,              Button2,        togglebar,        {0}},
};
