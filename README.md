# Heavy Artillery

## Copyright (c) 2002, All rights reserved

### Damian Coventry

#### Sunday, 7 Januray 2018
- Updated the camera's fly by mode to use Cubic Bézier Curves.

#### Saturday, 6 Januray 2018
- Code now builds using VS2017
- Added a 64 bit build
- Increased font size in dialogs, which in turn required all dialogs to be re-laid out
- Replaced fixed sized buffer strings with std::string
- Replaced almost all sprintf_s code with std::ostringstream

#### Monday, 12 August 2002
- released demo version 1.10, it includes all the above bug fixes and a few more

#### Sunday, 11 August 2002
- the bug causing the game to crash at startup on a few select computers has been squished.

#### Saturday, 10 August 2002
- fixed some more cosmetic bugs with the UI
- fixed the 3D Sound wierdness
- added DirectShow code so the game will now play MP3s, although none are distributed in this zip

#### Friday, 9 August 2002
- fixed some cosmetic bugs with the UI
- made the difference between each bot personality larger eg. inept bots really are dumb now :)

#### Thursday, 8 August 2002
- fixed a bug causing the properties/auto defense dialog to reflect the wrong state of shields
- added more logging to hopefully catch why some video cards cause the game to crash during startup
- fixed a bug causing tanks from previous games to be placed on landscape

#### Monday, 5 August 2002
- released demo version 1.00

#### TODO

+ **DONE** Update the CameraShake routine to not block any established line of sight
+ **DONE** Implement routines: IsLineOfSight, IsConeOfSight, IsCylinderOfSight
+ **DONE** Save points above the flight path of shot projectiles
+ **DONE** Using flight path projectile locations as last resort when placing camera
+ _TODO_ The game crahes when de-selecting a weapon or item in the shop
+ _TODO_ Implement all of the new camera tracking routines
+ _TODO_ Check which routines from StateGetShotInfo.h/.c are actually used
+ _TODO_ Check which routines from StateGetShotInfo.h/.c are actually used
+ _TODO_ Check which routines from Camera.h/.c are actually used
+ _TODO_ Implement an action tracking priority queue to handle "what" to look at "when"
+ _TODO_ Change maths/matrix/vector library to glm
+ _TODO_ Change to an OpenGL 4.5 shader based pipeline
+ _TODO_ Create a blender mesh -> custom mesh converter
+ _TODO_ Use vertex buffer objects
+ _TODO_ Implement per-pixel dynamic lighting. Start with a directional light.
+ _TODO_ Change the world to use a disk based structure
+ _TODO_ Use level of detail on the world object
+ _TODO_ Create 15-20 world meshes in blender
+ _TODO_ Used the Bullet 3.8 physics engine to implement rigid body dynamics
+ _TODO_ Implement a mini map feature
+ _TODO_ Implement du/dv based water
+ _TODO_ Re-create the tank meshes with 5k - 10k polygons
+ _TODO_ Re-create the weapon meshes with 5k - 10k polygons
+ _TODO_ Re-create the item meshes with 5k - 10k polygons
+ _TODO_ Re-create the projectile meshes with 5k - 10k polygons
+ _TODO_ Implement landscape items: trees, rocks, buildings, fences, etc
+ _TODO_ Create better looking explosions
+ _TODO_ Create better looking particle systems
+ _TODO_ Create better looking napalm
+ _TODO_ Stop tanks taking damaged when initially placed on the landscape
+ _TODO_ Fix the font creation/drawing routines
+ _TODO_ Implement forward rendered positional lights
+ _TODO_ Use background textures in the GUI
+ _TODO_ Implement dynamic shadows
+ _TODO_ Implement bump mapping
+ _TODO_ Make driving tanks around suck less i.e. use rigid bodies
+ _TODO_ Make driving tanks around a much more common part of the game play
+ _TODO_ Implement a weather system

#### Known bugs

Last Updated: Saturday, 3rd August 2002

+ **Fixed** fix camera following shot outside of landscape
+ **Fixed** fix camera moving from 1st person -> mouselook
+ **Fixed** fix object perspective in dialogs
+ _TODO_ tweek 3d sound parameters
+ **Fixed** make missile projectiles correctly face the way they're heading
+ **Fixed** can't remove bot status once set
+ _TODO_ check object -> object collisions for missile -> tank
+ **Fixed** make the camera track tanks moved by explosions
+ _TODO_ lighting is incorrect on projectiles
+ _TODO_ make dynamic/static lighting option in game options dialog
+ _TODO_ actually implement dynamic/static lighting toggle
+ _TODO_ missiles sometimes sit above the landscape
+ **Fixed** reduce the number of particles in explosions
+ **Fixed** disallow explosion particles to collide with players?
+ _TODO_ replace placeholder textures
+ _TODO_ tweek explosion sizes
+ _TODO_ setting 3d listener position kills performance
+ _TODO_ balance weapons eg. small missile kills in one hit when it shouldn't
+ _TODO_ put player halos back in
+ _TODO_ use bezier curve for path in flyby mode
+ **Fixed** tweek bouncing projectile bounce reduction parameter
+ **Fixed** make a #define DEMO facility
+ **Fixed** players are active during shop, can hear them hit ground
+ **Fixed** put camera shake module back in
+ **Fixed** the splash screen per landscape type is not working
+ **Fixed** make the actual per landscape splash screens
+ **Fixed** can't see the gdi bitmap during a full screen startup
+ **Fixed** when the player suicides the game doesn't wait
+ **Fixed** properties dialog reflects wrong state
+ **Fixed** fix camera tracking objects within dead zone and edge of world
+ _TODO_ game sometimes gets into funny state after airstrike planes leave

#### Ideas for Heavy Artillery 2

- put landscape deformation code back in
- make slider gui control and use it for sound and music volume
- make some sort of radar for the HUD
- rigid body physics
- network support, chat window etc
