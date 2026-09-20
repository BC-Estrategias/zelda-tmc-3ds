# The Minish Cap 3DS v1.1-E4 - Retail File-Select Heart Palette

> Pre-release test build focused on restoring the original file-select heart colors.

## Changes

- Fixes the incorrect yellow hearts introduced by the E3 palette experiment.
- Restores the retail file-select heart palette by explicitly loading gPalette_12 into BG palette 15.
- Matches the original GBA title-to-file-select palette state instead of copying OBJ palette 15.
- Keeps the updater improvements introduced in E3, including the installed-version display.
- Version bumped to v1.1-E4.
- CIA revision bumped to 122.

## Why this change

The file-select heart tilemap uses BG palette 15. In the original game, title-screen palette groups load gPalette_12 into that slot before entering file select, and the file-select code relies on that state being preserved.

Ports can enter the file-select path with different palette state, so E4 loads the same retail palette explicitly.

## Test

After updating to v1.1-E4:

1. Open the save-file selection screen.
2. Check full and empty heart colors.
3. Confirm the hearts no longer render black or yellow.
4. Confirm the rest of the file-select screen has no palette regressions.

The original ROM is not included.
