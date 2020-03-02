## Quake3 Autoshoot - Using a Neural Net

### Notes
- Tested using ioQuake3 ([https://ioquake3.org/](https://ioquake3.org/))
- All modes continue to train the perceptrons apart from the DeepAim mode.
- Plays a simple beep tone from the console every time a key is pressed,
  although it may be desired to replace this with a better tone function.
- Precompiled binary provided `aim`.
- Videos on [the YouTube Channel.](https://www.youtube.com/channel/UCYe31SWsmK3X9H3r1WM6OKQ)
- I would suppose this would work on any game, I just play Quake 3.

### Linux Compile
```gcc aimbot.c -Ofast -O3 -lX11 -lm -o aim```

#### Key Mapping
```
Left CTRL - Enable/Disable Auto-Shoot

&& The following hotkey functions become enabled also;

F10 - Preset High Tollerance
UP - Preset Medium Tollerance
DOWN - Preset Low Tollerence

LEFT - Manual Lower Tollerance
RIGHT - Manual Higher Tollerance

F1 - Target Aqua Blue
F2 - Target Blue
F3 - Target Red
F4 - Target Red #2

H - Retrain/Target on current center screen colour
G - Same as H but uses an average of 9 surrounding colours

B - Deep Aim Only
N - Neural Aim Only (trains Neural Net)
M - Colour Aim Only (trains Neural Net)

K - Reduce Neural Firing Probability
L - Increase Neural Firing Probability
```

#### Preferred Quake3 Client Settings
```
cg_oldRail "1"
cg_noProjectileTrail "1"
cg_forceModel "1"
cg_railTrailTime "100"
cg_crosshairSize "24"
cg_drawFPS "1"
cg_draw2D "1"
cg_gibs "1"
cg_fov "160"
cg_zoomfov "90"
cg_drawGun "1"
cg_brassTime "0"
cg_drawCrosshair "7"
cg_drawCrosshairNames "1"
cg_marks "0"
cg_crosshairPulse "1"
cg_stepTime "100"
cg_centertime "3"
xp_noParticles "1"
xp_noShotgunTrail "1"
xp_noMip "2047"
xp_ambient "1"
xp_modelJump "0"
xp_corpse "3"
xp_crosshairColor "7"
xp_improvePrediction "1"
cm_playerCurveClip "1"
com_maxfps "600"
com_blood "0"
cg_autoswitch "0"
rate "25000"
snaps "40"
model "bones/default"
headmodel "bones/default"
team_model "bones/default"
team_headmodel "bones/default"
color1 "6"
color2 "5"
cg_predictItems "1"
r_picmip "16"
r_overBrightBits "1"
r_mode "-2"
r_fullscreen "1"
r_customwidth "1920"
r_customheight "1080"
r_simpleMipMaps "1"
r_railWidth "16"
r_railCoreWidth "6"
r_railSegmentLength "32"
cg_shadows "0"
com_zoneMegs "24"
com_hunkmegs "512"
```

#### Licence
Distributed under the MIT software license, see the accompanying
file COPYING or http://www.opensource.org/licenses/mit-license.php.

