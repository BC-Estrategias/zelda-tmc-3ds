# The Minish Cap 3DS v1.2-E4 — Updater title-version fix

Fourth experimental build of the 1.2 series.

## Fixes

- Keeps the E3 soft-slot item fix for mapped X/Y/ZL/ZR/C-Stick item actions.
- Bumps the CIA internal title-version to 129 so E4 can overwrite E3 normally.
- The internal updater now recognizes AM error 0xD8E08027 when the downloaded CIA is already installed at the same or a newer internal title-version.
- In that already-installed case, the updater no longer reports a false installation failure; restarting the app loads the installed binary.

## Included from E2/E3

- Explicit action picker.
- Dedicated owned-item picker.
- X/Y on Old 3DS / Nintendo 2DS.
- X/Y/ZL/ZR/C-Stick on New 3DS / New 2DS XL.
- Configurable Turbo, Save State and Load State.
- Load State confirmation.
- Roll Attack removed from Y.

This is a prerelease for hardware validation before v1.2 stable.

The original ROM is not included.
