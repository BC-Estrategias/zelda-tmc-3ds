# The Minish Cap 3DS v1.1-E2 - File Select Heart Fix

> Pre-release test build for validating the in-app updater from v1.1-E1 to v1.1-E2.

## Changes

- Restores the original file-select heart palette/tile behavior used by the GBA game and Esteban's 3DS port.
- Removes the custom file-select heart palette override introduced in the fork.
- Keeps the updater/TLS fixes from the latest E1 branch.
- CIA revision bumped to 120.
- Version bumped to v1.1-E2.

## Update test

Expected flow:

~~~text
v1.1-E1
-> Settings > Update
-> Channel: Pre-release
-> v1.1-E2
-> Update available
-> Download and install
-> App closes
-> Reopen as v1.1-E2
~~~

After updating, open the save-file selection screen and compare the heart colors with the original GBA game / Esteban port.

The original ROM is not included.
