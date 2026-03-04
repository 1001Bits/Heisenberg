/**
 * BasicExample.cpp - Example F4SE plugin using the Heisenberg API
 *
 * Demonstrates:
 *   1. Simple grab checks via DLL export (no API object needed)
 *   2. Full API access via DLL export
 *   3. Callback registration
 */

#include "../API/HeisenbergInterface001.h"
#include <cstdio>

// ============================================================================
// Example 1: Simple grab check (minimal - no interface needed)
// ============================================================================
// Use this when you only need to know if the player is holding something.
// Common use case: prevent your mod's actions while Heisenberg is grabbing.

void CheckGrabState()
{
    if (HeisenbergPluginAPI::IsHeisenbergGrabbing()) {
        // Player is holding something - skip your holster/grab logic
        return;
    }

    // Safe to proceed with your mod's action
}

// Per-hand check
void CheckPerHandGrab()
{
    if (HeisenbergPluginAPI::IsHeisenbergGrabbingLeft()) {
        // Left hand is holding something
    }
    if (HeisenbergPluginAPI::IsHeisenbergGrabbingRight()) {
        // Right hand is holding something
    }
}

// ============================================================================
// Example 2: Full API access
// ============================================================================
// Use this for advanced integration: programmatic grabs, zone detection,
// ViewCaster queries, transform control, etc.

HeisenbergPluginAPI::IHeisenbergInterface001* g_heisenberg = nullptr;

void InitializeAPI()
{
    g_heisenberg = HeisenbergPluginAPI::GetHeisenbergInterface001_DLLExport();
    if (g_heisenberg) {
        printf("[MyMod] Heisenberg API v%u connected\n", g_heisenberg->GetBuildNumber());
    }
}

void UseFullAPI()
{
    if (!g_heisenberg) return;

    // Check what the player's right hand is pointing at
    RE::TESObjectREFR* target = g_heisenberg->GetViewCasterTarget(false);

    // Check if left hand is in the storage zone (behind head)
    if (g_heisenberg->IsInStorageZone(true)) {
        // Player's left hand is behind their head
    }

    // Spawn an item to the player's right hand
    // RE::TESForm* myItem = ...;
    // g_heisenberg->DropToHand(myItem, false);

    // Disable left hand grabbing temporarily
    g_heisenberg->DisableHand(true);
    // ... do your thing ...
    g_heisenberg->EnableHand(true);
}

// ============================================================================
// Example 3: Callbacks
// ============================================================================

void OnObjectGrabbed(bool isLeft, RE::TESObjectREFR* refr)
{
    const char* hand = isLeft ? "left" : "right";
    printf("[MyMod] Player grabbed something with %s hand\n", hand);
}

void OnObjectDropped(bool isLeft, RE::TESObjectREFR* refr)
{
    printf("[MyMod] Player dropped object\n");
}

void RegisterCallbacks()
{
    if (!g_heisenberg) return;

    g_heisenberg->AddGrabbedCallback(OnObjectGrabbed);
    g_heisenberg->AddDroppedCallback(OnObjectDropped);
}
