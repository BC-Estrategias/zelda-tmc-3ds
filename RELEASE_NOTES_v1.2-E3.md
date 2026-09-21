# The Minish Cap 3DS v1.2-E3 — Soft-slot item fix

Third experimental build of the 1.2 series.

## Fix

- Fixed mapped **Item** actions not working on Nintendo 3DS hardware.
- The 3DS native frame path now updates soft-slot state before committing KEYINPUT.
- When a mapped Item input is held, the port now exposes it through the regular B-button dispatch path on 3DS.
- The effective B item is still overridden only transiently; the visible A/B equipment and save data are not changed.
- Held/charged item behavior continues to use the original Minish Cap item logic.

## Still included from E2

- Explicit action picker.
- Dedicated item picker showing only owned equippable items.
- X/Y on Old 3DS / Nintendo 2DS.
- X/Y/ZL/ZR/C-Stick on New 3DS / New 2DS XL.
- Configurable Turbo, Save State and Load State.
- Load State confirmation.
- Roll Attack removed from Y.

This is a prerelease for hardware validation before v1.2 stable.

The original ROM is not included.
