# Heisenberg F4VR - Developer API

Public API for Heisenberg (Physical Interactions VR), a Fallout 4 VR mod that adds hand-based object grabbing, throwing, equipping, consuming, and physics interaction.

This repository contains everything mod developers need to integrate with Heisenberg from their own F4SE plugins.

## Quick Start

### Simple: Check if player is grabbing

No API object needed. Just include the header and call:

```cpp
#include "API/HeisenbergInterface001.h"

if (HeisenbergPluginAPI::IsHeisenbergGrabbing()) {
    // Either hand is holding an object - skip your grab/holster logic
}
```

Per-hand variants: `IsHeisenbergGrabbingLeft()`, `IsHeisenbergGrabbingRight()`

### Full API Access

```cpp
#include "API/HeisenbergInterface001.h"

// Get the API (call once, cache the pointer)
auto* api = HeisenbergPluginAPI::GetHeisenbergInterface001_DLLExport();
if (!api) return; // Heisenberg not installed

// Now use any API function
RE::TESObjectREFR* held = api->GetGrabbedObject(false); // right hand
bool inMouth = api->IsInMouthZone(true);                 // left hand near mouth?
api->DisableHand(true);                                   // disable left hand grabbing
```

## API Reference

### DLL Exports (No Interface Required)

| Function | Returns | Description |
|----------|---------|-------------|
| `IsHeisenbergGrabbing()` | `bool` | Either hand holding an object |
| `IsHeisenbergGrabbingLeft()` | `bool` | Left hand holding |
| `IsHeisenbergGrabbingRight()` | `bool` | Right hand holding |
| `GetHeisenbergAPI(1)` | `void*` | Get full `IHeisenbergInterface001*` |

### IHeisenbergInterface001 Methods

#### Grab State

| Method | Returns | Description |
|--------|---------|-------------|
| `IsHoldingObject(isLeft)` | `bool` | Hand is holding an object (thread-safe) |
| `IsPulling(isLeft)` | `bool` | Object being pulled toward hand |
| `CanGrabObject(isLeft)` | `bool` | Hand ready to grab |
| `GetGrabbedObject(isLeft)` | `TESObjectREFR*` | Held object or nullptr (thread-safe) |
| `GetGrabbedNodeName(isLeft)` | `const char*` | Grabbed collision node name |

#### ViewCaster / Selection

| Method | Returns | Description |
|--------|---------|-------------|
| `GetViewCasterTarget(isLeft)` | `TESObjectREFR*` | What hand is pointing at |
| `GetPrimaryWandTarget()` | `TESObjectREFR*` | Primary wand target |
| `GetSecondaryWandTarget()` | `TESObjectREFR*` | Secondary wand target |
| `GetSelectedObject(isLeft)` | `TESObjectREFR*` | Heisenberg's physics-based selection |

#### Grab Control

| Method | Returns | Description |
|--------|---------|-------------|
| `GrabObject(refr, isLeft)` | `bool` | Programmatically grab an object |
| `DropObject(isLeft, velocity)` | `void` | Drop/throw held object |
| `ForceEndGrab(isLeft)` | `void` | Force release (for object deletion) |

#### Hand State

| Method | Returns | Description |
|--------|---------|-------------|
| `DisableHand(isLeft)` | `void` | Disable grabbing for a hand |
| `EnableHand(isLeft)` | `void` | Re-enable grabbing |
| `IsHandDisabled(isLeft)` | `bool` | Check if disabled |

#### Finger Tracking

| Method | Description |
|--------|-------------|
| `GetFingerCurls(isLeft, float[5])` | Get curl values [thumb..pinky], 0=open, 1=curled |
| `SetFingerCurls(isLeft, float[5])` | Override finger positions |

#### Zone Detection

| Method | Returns | Description |
|--------|---------|-------------|
| `IsInStorageZone(isLeft)` | `bool` | Hand behind head (storage area) |
| `IsInEquipZone(isLeft)` | `bool` | Hand at head/chest (equip area) |
| `IsInMouthZone(isLeft)` | `bool` | Hand near mouth (consume area) |
| `IsInVHZone(isLeft)` | `bool` | Hand in a [Virtual Holsters](https://www.nexusmods.com/fallout4/mods/88386) zone |
| `GetCurrentZoneName(isLeft)` | `const char*` | Zone name or empty string |
| `GetVHZoneIndex(isLeft)` | `int` | [Virtual Holsters](https://www.nexusmods.com/fallout4/mods/88386) zone: 0=None, 1-7=zone |

> **Note:** VH (Virtual Holsters) is a separate mod that adds body-mounted weapon holster zones. These functions require Virtual Holsters to be installed — they return false/0 if it is not present.

#### Inventory

| Method | Returns | Description |
|--------|---------|-------------|
| `DropToHand(form, isLeft)` | `bool` | Spawn inventory item to hand |

#### Activator Proximity

| Method | Returns | Description |
|--------|---------|-------------|
| `IsInActivationZone(isLeft)` | `bool` | Hand near an interactive activator (button, switch, door, terminal) |

#### Transform

| Method | Description |
|--------|-------------|
| `GetGrabTransform(isLeft)` | Get hand-to-object offset |
| `SetGrabTransform(isLeft, transform)` | Change how object is held |

#### Hand Collision (Work in Progress)

> **Note:** Hand collision is not yet active in the current release. These functions are reserved for a future update and will return false/nullptr until then.

| Method | Returns | Description |
|--------|---------|-------------|
| `IsHandCollisionEnabled()` | `bool` | Physics hands active? |
| `GetHandRigidBody(isLeft)` | `void*` | bhkNPCollisionObject* |
| `IsHandInContact(isLeft)` | `bool` | Hand touching something? |
| `GetHandContactObject(isLeft)` | `TESObjectREFR*` | What hand is touching |

#### Settings

| Method | Returns | Description |
|--------|---------|-------------|
| `GetSettingDouble(name, &out)` | `bool` | Read a Heisenberg INI setting |
| `SetSettingDouble(name, val)` | `bool` | Write a setting (some need reload) |

### Callbacks

Register callbacks to react to Heisenberg events:

```cpp
api->AddGrabbedCallback([](bool isLeft, RE::TESObjectREFR* refr) {
    // Object was grabbed
});
```

| Callback | Signature |
|----------|-----------|
| `AddGrabbedCallback` | `void(bool isLeft, TESObjectREFR*)` |
| `AddDroppedCallback` | `void(bool isLeft, TESObjectREFR*)` |
| `AddStashedCallback` | `void(bool isLeft, TESForm*)` |
| `AddConsumedCallback` | `void(bool isLeft, TESForm*)` |
| `AddPulledCallback` | `void(bool isLeft, TESObjectREFR*)` |
| `AddCollisionCallback` | `void(bool isLeft, float mass, float velocity)` |
| `AddPrePhysicsCallback` | `void(void* bhkWorld)` |
| `AddPostPhysicsCallback` | `void(void* bhkWorld)` |
| `AddViewCasterTargetChangedCallback` | `void(bool isLeft, TESObjectREFR* new, TESObjectREFR* old)` |

## MCM Settings Reference

Heisenberg's settings are managed via MCM VR. Other mods can read/write these via the Settings API, or users can configure them in-game.

See [`MCM/settings.ini`](MCM/settings.ini) for all available settings with their defaults, and [`MCM/config.json`](MCM/config.json) for the MCM UI layout.

## File Structure

```
API/
  HeisenbergInterface001.h   -- Include this in your plugin
examples/
  BasicExample.cpp           -- Usage examples
MCM/
  settings.ini               -- All settings with defaults
  config.json                -- MCM UI configuration
```

## Requirements

- Fallout 4 VR (1.2.72)
- F4SE VR
- Heisenberg_F4VR.dll installed (the API gracefully returns nullptr if not present)

## Notes

- The `isLeft` parameter refers to the player's hand. `true` = left hand, `false` = right hand.
- The API pointer is stable for the lifetime of the game session. Cache it once.
- All callbacks fire on the main game thread.
- The interface uses a vtable-based design. Do not modify the virtual method order in the header.
