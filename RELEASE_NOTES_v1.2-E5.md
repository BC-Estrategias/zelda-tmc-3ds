# The Minish Cap 3DS v1.2-E5 — Input persistence + CIA version fix

Fifth experimental build of the 1.2 series.

## Fixes

- Fixed mapped Item actions on native Nintendo 3DS input path.
- Fixed mapped item assignments not surviving a restart.
  - Soft-slot assignments now load during app startup.
  - Soft-slot storage is initialized before every assignment read/write.
- Fixed internal updater installs being rejected with AM result 0xD8E08027.
  - CIA/TMD title version is now explicitly passed to makerom with -major/-minor/-micro.
  - Pre-release title versions are reserved below the next stable release so v1.2 stable remains an upgrade over every v1.2-E build.
- The updater still recognizes the already-installed title-version case instead of reporting a misleading generic failure.

## Controls

New 3DS / New 2DS XL:
- X
- Y
- ZL
- ZR
- C-Stick

Old 3DS / 2DS:
- X
- Y

Available actions:
- None
- Cycle bottom-screen tabs
- Turbo
- Item
- Save State
- Load State

Item shortcuts use the game's original item-use path and do not change the visible A/B equipment.

This is a prerelease for hardware validation before v1.2 stable.

The original ROM is not included.
