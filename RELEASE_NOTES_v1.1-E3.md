# The Minish Cap 3DS v1.1-E3 - Retail Heart Palette

> Pre-release test build for validating the in-app updater from v1.1-E2 to v1.1-E3.

## Changes

- Fixes black hearts on the file-select screen.
- Uses the original retail heart palette already loaded by palette group 0x9.
- Mirrors OBJ palette 15 into BG palette 15 for the 3DS/PC file-card renderer.
- Removes the need for the old custom red palette ramp.
- Shows the currently installed version on the bottom-screen updater page.
- Version bumped to v1.1-E3.
- CIA revision bumped to 121.

## Update test

Expected flow:

~~~text
v1.1-E2
-> Settings > Update
-> Channel: Pre-release
-> v1.1-E3
-> Download update
-> Install
-> App closes
-> Reopen as v1.1-E3
~~~

After updating, open the save-file selection screen and confirm that the hearts no longer render black and match the original game palette more closely.

The original ROM is not included.
