# The Minish Cap 3DS v1.2-E2 — Button Mapping UI

Second experimental build of the 1.2 series.

## What changed since E1

- Button mapping now uses an explicit action picker instead of cycling through actions by repeated taps.
- Choosing **Item** opens a dedicated item picker.
- The item picker shows only equippable items currently owned by the active save.
- Item selection is paginated for the 3DS bottom screen.
- The selected action/item is highlighted.
- Old 3DS / Nintendo 2DS still exposes only X and Y.
- New 3DS / New 2DS XL exposes X, Y, ZL, ZR and C-Stick.

## Mapping actions

- None
- Cycle bottom-screen tabs
- Turbo
- Item
- Save State
- Load State

## Defaults

- X → Tabs
- Y → None
- ZL → None
- ZR → Turbo
- C-Stick → Turbo

## Important behavior

- Roll Attack remains removed from Y.
- Fixed ZL+X / ZL+Y state shortcuts remain removed.
- Turbo is held while its mapped input is held.
- Save State fires once per press and is executed through the existing safe frame-boundary path.
- Load State fires once per press and still opens the existing confirmation dialog.
- C-Stick is treated as one logical input with the existing deadzone.
- Item shortcuts reuse the soft-slot/B-dispatch path and do not overwrite the visible A/B equipment.
- Held/charged item behavior remains delegated to the original Minish Cap item logic.

This is a prerelease for hardware validation before v1.2 stable.

The original ROM is not included.
