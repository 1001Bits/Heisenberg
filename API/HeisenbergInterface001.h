#pragma once

/**
 * HeisenbergInterface001.h - Public API for Heisenberg F4VR
 *
 * This interface allows other F4SE plugins to interact with Heisenberg's
 * grabbing, selection, and physics systems.
 *
 * Two access methods are provided:
 *   Method A (DLL Export - simplest):
 *     auto* api = GetHeisenbergInterface001_DLLExport();
 *
 *   Method B (F4SE Messaging):
 *     auto* api = GetHeisenbergInterface001(pluginHandle, messagingInterface);
 *
 * Thread Safety:
 *   - Most functions should only be called from the main game thread
 *   - Callbacks are invoked from the main game thread
 *   - IsHoldingObject/GetGrabbedObject are safe to call from any thread
 */

// Forward declarations - replace with your RE header includes
namespace RE {
    class TESObjectREFR;
    class TESForm;
    struct NiPoint3;
    struct NiTransform;
}

// Forward declarations for F4SE types used by the messaging method
namespace F4SE {
    using PluginHandle = uint32_t;
    class MessagingInterface;
}

namespace HeisenbergPluginAPI {

    // Forward declaration
    struct IHeisenbergInterface001;

    // =========================================================================
    // MESSAGE STRUCTURE (for F4SE messaging system - Method B)
    // =========================================================================

    /**
     * Message structure for requesting the Heisenberg API via F4SE messaging.
     *
     * Protocol:
     * 1. Your plugin dispatches kMessage_GetInterface to "Heisenberg_F4VR"
     * 2. Heisenberg fills in the GetApiFunction callback
     * 3. Call GetApiFunction(1) to get IHeisenbergInterface001*
     */
    struct HeisenbergMessage
    {
        enum { kMessage_GetInterface = 0xF4D3B7A2 };
        void* (*GetApiFunction)(unsigned int revisionNumber) = nullptr;
    };

    // =========================================================================
    // CONVENIENCE FUNCTIONS
    // =========================================================================

    /**
     * Method A: Get the Heisenberg interface via DLL export (simplest).
     * No F4SE messaging needed. Can be called at any time after game loads.
     *
     * @return Pointer to IHeisenbergInterface001, or nullptr if Heisenberg is not loaded
     */
    inline IHeisenbergInterface001* GetHeisenbergInterface001_DLLExport()
    {
        typedef void* (*GetHeisenbergAPI_t)(unsigned int);
        HMODULE hMod = GetModuleHandleA("Heisenberg_F4VR.dll");
        if (!hMod) return nullptr;
        auto getApi = reinterpret_cast<GetHeisenbergAPI_t>(GetProcAddress(hMod, "GetHeisenbergAPI"));
        if (!getApi) return nullptr;
        return static_cast<IHeisenbergInterface001*>(getApi(1));
    }

    /**
     * Method B: Get the Heisenberg interface via F4SE messaging.
     * Call this after F4SE sends kGameLoaded or kPostLoadGame.
     *
     * @param pluginHandle Your plugin's handle from F4SE
     * @param messagingInterface The F4SE messaging interface
     * @return Pointer to IHeisenbergInterface001, or nullptr if Heisenberg is not loaded
     */
    inline IHeisenbergInterface001* GetHeisenbergInterface001(
        const F4SE::PluginHandle& pluginHandle,
        F4SE::MessagingInterface* messagingInterface)
    {
        if (!messagingInterface) return nullptr;

        HeisenbergMessage msg{};
        // Note: Replace with your F4SE version's Dispatch signature
        // messagingInterface->Dispatch(HeisenbergMessage::kMessage_GetInterface,
        //     &msg, sizeof(HeisenbergMessage), "Heisenberg_F4VR");
        //
        // if (!msg.GetApiFunction) return nullptr;
        // return static_cast<IHeisenbergInterface001*>(msg.GetApiFunction(1));

        // The DLL export method is simpler and equivalent:
        return GetHeisenbergInterface001_DLLExport();
    }

    // =========================================================================
    // SIMPLE GRAB QUERIES (DLL exports, no interface needed)
    // =========================================================================

    /**
     * Quick check: is either hand grabbing? Uses direct DLL export.
     * Returns false if Heisenberg is not loaded.
     */
    inline bool IsHeisenbergGrabbing()
    {
        typedef bool (*Fn)();
        HMODULE hMod = GetModuleHandleA("Heisenberg_F4VR.dll");
        if (!hMod) return false;
        auto fn = reinterpret_cast<Fn>(GetProcAddress(hMod, "IsHeisenbergGrabbing"));
        return fn ? fn() : false;
    }

    inline bool IsHeisenbergGrabbingLeft()
    {
        typedef bool (*Fn)();
        HMODULE hMod = GetModuleHandleA("Heisenberg_F4VR.dll");
        if (!hMod) return false;
        auto fn = reinterpret_cast<Fn>(GetProcAddress(hMod, "IsHeisenbergGrabbingLeft"));
        return fn ? fn() : false;
    }

    inline bool IsHeisenbergGrabbingRight()
    {
        typedef bool (*Fn)();
        HMODULE hMod = GetModuleHandleA("Heisenberg_F4VR.dll");
        if (!hMod) return false;
        auto fn = reinterpret_cast<Fn>(GetProcAddress(hMod, "IsHeisenbergGrabbingRight"));
        return fn ? fn() : false;
    }

    // =========================================================================
    // MAIN INTERFACE
    // =========================================================================

    /**
     * Main Heisenberg interface.
     * Provides access to grabbing, selection, zones, callbacks, and physics.
     *
     * IMPORTANT: This is a pure virtual interface backed by a vtable in
     * Heisenberg_F4VR.dll. Do not change the order of virtual methods.
     */
    struct IHeisenbergInterface001
    {
        // =================================================================
        // VERSION
        // =================================================================

        /** Get the Heisenberg build number for compatibility checks. */
        virtual unsigned int GetBuildNumber() = 0;

        // =================================================================
        // GRAB STATE QUERIES
        // =================================================================

        /** Check if a hand is holding an object. Thread-safe. */
        virtual bool IsHoldingObject(bool isLeft) = 0;

        /** Check if a hand is pulling an object toward it. */
        virtual bool IsPulling(bool isLeft) = 0;

        /** Check if a hand can currently grab (not disabled, not holding, not in menu). */
        virtual bool CanGrabObject(bool isLeft) = 0;

        /** Get the held object reference, or nullptr. Thread-safe. */
        virtual RE::TESObjectREFR* GetGrabbedObject(bool isLeft) = 0;

        /** Get the name of the grabbed collision node, or empty string. */
        virtual const char* GetGrabbedNodeName(bool isLeft) = 0;

        // =================================================================
        // VIEWCASTER / SELECTION
        // =================================================================

        /** Get what a hand's ViewCaster is pointing at (activation target). */
        virtual RE::TESObjectREFR* GetViewCasterTarget(bool isLeft) = 0;

        /** Get primary wand target. Same as GetViewCasterTarget(true). */
        virtual RE::TESObjectREFR* GetPrimaryWandTarget() = 0;

        /** Get secondary wand target. Same as GetViewCasterTarget(false). */
        virtual RE::TESObjectREFR* GetSecondaryWandTarget() = 0;

        /** Get Heisenberg's selected object (may differ from ViewCaster due to physics raycasts). */
        virtual RE::TESObjectREFR* GetSelectedObject(bool isLeft) = 0;

        // =================================================================
        // GRAB CONTROL
        // =================================================================

        /** Programmatically grab an object. Returns true on success. */
        virtual bool GrabObject(RE::TESObjectREFR* object, bool isLeft) = 0;

        /** Drop/release held object. Pass throwVelocity for throwing, or nullptr. */
        virtual void DropObject(bool isLeft, const RE::NiPoint3* throwVelocity = nullptr) = 0;

        /** Force end grab without physics cleanup (use when object is being deleted). */
        virtual void ForceEndGrab(bool isLeft) = 0;

        // =================================================================
        // HAND STATE
        // =================================================================

        /** Disable grabbing/selection for a hand. */
        virtual void DisableHand(bool isLeft) = 0;

        /** Re-enable grabbing/selection. */
        virtual void EnableHand(bool isLeft) = 0;

        /** Check if hand is disabled. */
        virtual bool IsHandDisabled(bool isLeft) = 0;

        // =================================================================
        // FINGER TRACKING
        // =================================================================

        /** Get finger curls [thumb,index,middle,ring,pinky]. 0=open, 1=curled. */
        virtual void GetFingerCurls(bool isLeft, float values[5]) = 0;

        /** Override finger curls. */
        virtual void SetFingerCurls(bool isLeft, const float values[5]) = 0;

        // =================================================================
        // ZONE DETECTION
        // =================================================================

        /** Is hand in the behind-head storage zone? */
        virtual bool IsInStorageZone(bool isLeft) = 0;

        /** Is hand in an equip zone (head/chest)? */
        virtual bool IsInEquipZone(bool isLeft) = 0;

        /** Is hand in the mouth/consume zone? */
        virtual bool IsInMouthZone(bool isLeft) = 0;

        /** Is hand in a VirtualHolsters zone? */
        virtual bool IsInVHZone(bool isLeft) = 0;

        /** Get current zone name ("HEAD","CHEST","LEFT_HIP",etc.) or empty string. */
        virtual const char* GetCurrentZoneName(bool isLeft) = 0;

        /** Get VH zone index. 0=None,1=LShoulder,2=RShoulder,3=LHip,4=RHip,5=LowerBack,6=LChest,7=RChest */
        virtual int GetVHZoneIndex(bool isLeft) = 0;

        // =================================================================
        // INVENTORY INTEGRATION
        // =================================================================

        /** Spawn an item from inventory to hand. Returns true on success. */
        virtual bool DropToHand(RE::TESForm* form, bool isLeft) = 0;

        /** Trigger smart grab (context-aware item retrieval). Returns true if item retrieved. */
        virtual bool SmartGrab(bool isLeft) = 0;

        // =================================================================
        // TRANSFORM CONTROL
        // =================================================================

        /** Get the hand-to-object offset transform. */
        virtual RE::NiTransform GetGrabTransform(bool isLeft) = 0;

        /** Set the hand-to-object offset transform. */
        virtual void SetGrabTransform(bool isLeft, const RE::NiTransform& transform) = 0;

        // =================================================================
        // CALLBACKS
        // =================================================================

        typedef void(*GrabbedCallback)(bool isLeft, RE::TESObjectREFR* grabbedRefr);
        typedef void(*DroppedCallback)(bool isLeft, RE::TESObjectREFR* droppedRefr);
        typedef void(*StashedCallback)(bool isLeft, RE::TESForm* stashedForm);
        typedef void(*ConsumedCallback)(bool isLeft, RE::TESForm* consumedForm);
        typedef void(*PulledCallback)(bool isLeft, RE::TESObjectREFR* pulledRefr);
        typedef void(*CollisionCallback)(bool isLeft, float mass, float separatingVelocity);
        typedef void(*PrePhysicsCallback)(void* bhkWorld);
        typedef void(*PostPhysicsCallback)(void* bhkWorld);
        typedef void(*ViewCasterTargetChangedCallback)(bool isLeft, RE::TESObjectREFR* newTarget, RE::TESObjectREFR* oldTarget);

        virtual void AddGrabbedCallback(GrabbedCallback callback) = 0;
        virtual void AddDroppedCallback(DroppedCallback callback) = 0;
        virtual void AddStashedCallback(StashedCallback callback) = 0;
        virtual void AddConsumedCallback(ConsumedCallback callback) = 0;
        virtual void AddPulledCallback(PulledCallback callback) = 0;
        virtual void AddCollisionCallback(CollisionCallback callback) = 0;
        virtual void AddPrePhysicsCallback(PrePhysicsCallback callback) = 0;
        virtual void AddPostPhysicsCallback(PostPhysicsCallback callback) = 0;
        virtual void AddViewCasterTargetChangedCallback(ViewCasterTargetChangedCallback callback) = 0;

        // =================================================================
        // SETTINGS
        // =================================================================

        /** Get a Heisenberg setting by name. Returns true if found. */
        virtual bool GetSettingDouble(const char* name, double& out) = 0;

        /** Set a Heisenberg setting. Some require reload. Returns true if found. */
        virtual bool SetSettingDouble(const char* name, double val) = 0;

        // =================================================================
        // HAND COLLISION
        // =================================================================

        /** Is hand collision enabled? */
        virtual bool IsHandCollisionEnabled() = 0;

        /** Get hand physics body (bhkNPCollisionObject*), or nullptr. */
        virtual void* GetHandRigidBody(bool isLeft) = 0;

        /** Is hand touching an object? */
        virtual bool IsHandInContact(bool isLeft) = 0;

        /** Get the object the hand is touching, or nullptr. */
        virtual RE::TESObjectREFR* GetHandContactObject(bool isLeft) = 0;
    };

} // namespace HeisenbergPluginAPI
