# NXQuake
Switch port of [TyrQuake](https://disenchant.net/tyrquake/).
Only contains NetQuake with software rendering.

## Building
You need:
* latest versions of devkitA64 and libnx;
* switch-sdl2 (you can install this with `(dkp-)pacman`).

Run `make` in this directory to build `nxquake.nro`.

## Running
Place the NRO into `/switch/nxquake/` on your SD card.

If you only have the [shareware version of Quake](ftp://ftp.idsoftware.com/idstuff/quake/quake106.zip), copy `pak0.pak` to `/switch/nxquake/id1/`.
If you have the full version, copy both `pak0.pak` and `pak1.pak` to `/switch/nxquake/id1/`.
Make sure the pak files and the `id1` directory have all-lowercase names, just in case.

You can now run NXQuake using Homebrew Launcher.

If it crashes, look for `console.log` and `error.log` in `/switch/nxquake/`.

Mission Packs and mods are technically supported, but there is currently no user-friendly way to run them.

## Credits
* Kevin Shanahan (aka Tyrann) for TyrQuake;
* Rinnegatamante for some input-related code I copypasted from vitaQuake;
* Drakia/TheDgtl for testing;
* id Software for Quake.
