# The Minish Cap 3DS v1.2-E1 — Button Mapping

First experimental build of the 1.2 series.

## Button Mapping

A new **Controls / Controles** page lets the player configure the extra 3DS inputs.

### Available inputs

**New Nintendo 3DS / New Nintendo 2DS XL**
- X
- Y
- ZL
- ZR
- C-Stick

**Old Nintendo 3DS / Nintendo 2DS**
- X
- Y

ZL, ZR and C-Stick are hidden automatically on hardware that does not provide them.

### Available actions

- None
- Cycle bottom-screen tabs
- Turbo
- Item
- Save State
- Load State

When **Item** is selected, the item value can be changed independently and only owned equippable items are offered.

### Default mapping

- X → Tabs
- Y → None
- ZL → None
- ZR → Turbo
- C-Stick → Turbo

### Changes from v1.1

- Removed the non-working Roll Attack shortcut from Y.
- Removed fixed ZL+X / ZL+Y save-state shortcuts.
- ZR and C-Stick Turbo are no longer hardcoded; both are configurable.
- Load State still uses the existing confirmation screen before restoring state.
- Item shortcuts use the existing soft-slot path and do not overwrite the items equipped on A/B.
- Charged/held items continue to behave as held buttons through the soft-slot system.
- Button mapping is persisted in `tmc3ds.ini`.
- Existing soft-slot assignments are migrated to the expanded five-input format.

This is a prerelease intended for hardware validation before v1.2 stable.

The original ROM is not included.
