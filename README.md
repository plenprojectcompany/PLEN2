PLEN2 | PLEN Project Company
================================================================================

The world's first printable open-source humanoid.


## Getting Started
1. Download this repository.
2. Get control boards and servo motors. [Buy all!](https://plen.jp/shop/)
3. Install firmware. (This is placed "/arduino/firmware/" directory.)
   [See also...](https://plen.jp/playground/wiki/tutorials/plen2/firmware)
4. Print and assemble all *.stl files. (There are in the "/stl/" directory.)
   [See also...](https://plen.jp/playground/wiki/tutorials/index.html)
5. Configure home positions of joints. (This step is done by using
   [PLENUtilities](https://github.com/plenproject/plen__control_server/releases).)
   [See also...](https://plen.jp/playground/wiki/tutorials/plen2/tune)
6. Write all motions. (Please check "/motion/*/" directory and use MotionInstaller
   ([win](https://github.com/plenproject/plen__motion_installer_gui_win/releases) or
   [mac](https://github.com/plenproject/plen__motion_installer_gui_mac/releases)).)
   [See also...](https://plen.jp/playground/wiki/tutorials/motion_installer/simple_usage)
7. Hello, world where PLEN is!


## Enjoy More, with PLEN :)
- Let moving PLEN as you like! [Check it out.](https://plen.jp/playground/motion-editor/)
- Learn about creativity thinking, structuring logics, or programing. [Check it out.](https://plen.jp/playground/scenography/)
- Communicate other users. [Check it out.](https://plen.jp/playground/forum/)
- Get more details of PLEN. [Check it out.](https://plen.jp/playground/wiki/)
- etc...


## Development Repositories
### Solid Works
SolidWorks is used for to build [mechanism elements](https://github.com/plenproject/plen__3DModel_for_SolidWorks).

- Body
- Foot
- Hand
- Neck
- Servo bracket
- Servo horn

### Blender
Blender is used for to build [high design quality parts](https://github.com/plenproject/plen__3DModel_for_Blender).
In the future, all parts will be implemented by SolidWorks.

- 3D Model (the whole body)
- Face (head and barb)
- Chest

### Arduino
We define here as the default repository of the firmware,
and it's already installed on a control-board.

If you are interested deeply in the firmware, please check
[a development repository (branch) the manager has](https://github.com/Guvalif/plen-Firmware_Arduino/tree/v1.x).

### Android
- [Controller and visual programming language](https://github.com/plenproject/plen__Scenography_for_Android)

### iOS
- [Controller and visual programming language](https://github.com/plenproject/plen-Scenography_iOS)

### Web Application
- [Motion editor](https://github.com/plenproject/plen__motion_editor_for_web)
- [Control server](https://github.com/plenproject/plen__control_server), A communication tool between HTTP and Serial

### Eagle
> EAGLE PCB Design Software is the tool of choice for thousands of engineers worldwide.
> With 3 modules and a common interface, EAGLE offers a variety of product combinations
> and allows every user to choose the configuration that meets their individual requirements.
> (origin: http://www.cadsoftusa.com/)

- Printed circuit board CAD for [control-board](https://github.com/plenproject/plen-ControlBoard)
- Printed circuit board CAD for [head-board](https://github.com/plenproject/plen-HeadBoard)


## Laws of PLEN
You could use PLEN for every imaginable things, but please observe the following laws strictly.

1. PLEN must not fight.
2. PLEN must not give someone a feeling of discomfort.
3. PLEN must be able to inherit its characteristics.


## Copyright
- [PLEN Project Company](https://plen.jp/), and all contributors.