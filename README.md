# NXQuake
Switch port of [TyrQuake](https://disenchant.net/tyrquake/) (only the NetQuake part).

## Building
You need:
* latest versions of devkitA64 and libnx;
* latest versions of switch-sdl2, switch-mesa, switch-libdrm_nouveau.

Run `make` in this directory to build `nxquake.nro`.

## Running
Place the NRO into `/switch/nxquake/` on your SD card.

If you only have the [shareware version of Quake](ftp://ftp.idsoftware.com/idstuff/quake/quake106.zip), copy `pak0.pak` to `/switch/nxquake/id1/`.
If you have the full version, copy both `pak0.pak` and `pak1.pak` to `/switch/nxquake/id1/`.

If you want to play Scourge of Armagon, copy `pak0.pak` from `hipnotic` to `/switch/nxquake/hipnotic/`.
If you want to play Dissolution of Eternity, copy `pak0.pak` from `rogue` to `/switch/nxquake/rogue/`.

Make sure the pak files and the `id1` directory have all-lowercase names, just in case.

You can run NXQuake using Homebrew Launcher.

If it crashes, look for `console.log` and `error.log` in `/switch/nxquake/`.

Mission Packs and mods are supported. If you have more game folders than just `id1` in `/switch/nxquake/`, a mod select menu will pop up when you launch NXQuake.

## Credits
* Kevin Shanahan (aka Tyrann) for TyrQuake;
* Rinnegatamante for some input-related code I copypasted from vitaQuake;
* some nice people from the ReiSwitched Discord for testing (especially Drakia, Crusatyr and Patrick);
* id Software for Quake.
