#include "raylib.h"
#include "raymath.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h>

#include <vector>

// Disable common warnings triggered by Jolt, you can use #pragma warning(disable: ...) or -Wno-...
#ifdef JPH_DISABLE_warnings
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
#endif

// Jolt layer settings
namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer COUNT = 2;
};

// Jolt broadphase layer interface
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broadphase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = JPH::BroadPhaseLayer(0);
        mObjectToBroadPhase[Layers::MOVING] = JPH::BroadPhaseLayer(1);
    }

    virtual JPH::uint GetNumBroadPhaseLayers() const override
    {
        return 2;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::COUNT);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type)inLayer)
        {
        case 0:
            return "NON_MOVING";
        case 1:
            return "MOVING";
        default:
            JPH_ASSERT(false);
            return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::COUNT];
};

// Jolt object vs broadphase layer filter
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case Layers::NON_MOVING:
            return inLayer2 == JPH::BroadPhaseLayer(1); // Non moving only collides with moving
        case Layers::MOVING:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// Jolt object vs object layer filter
class ObjectVsObjectLayerFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case Layers::NON_MOVING:
            return inLayer2 == Layers::MOVING; // Non moving only collides with moving
        case Layers::MOVING:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// Jolt object layer filter for raycasting
class ObjectLayerFilterImpl : public JPH::ObjectLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer) const override
    {
        return inLayer == Layers::MOVING;
    }
};

// Jolt broadphase layer filter for raycasting
class BroadPhaseLayerFilterImpl : public JPH::BroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::BroadPhaseLayer inLayer) const override
    {
        return true;
    }
};


void CreateBodies(JPH::BodyInterface &body_interface, std::vector<JPH::BodyID>& body_ids);

void ResetSimulation(JPH::BodyInterface &body_interface, std::vector<JPH::BodyID>& body_ids)
{
    for (const auto& body_id : body_ids)
    {
        body_interface.RemoveBody(body_id);
        body_interface.DestroyBody(body_id);
    }
    body_ids.clear();
    CreateBodies(body_interface, body_ids);
}


void CreateBodies(JPH::BodyInterface &body_interface, std::vector<JPH::BodyID>& body_ids)
{
    JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::Vec3(0.0f, 10.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
    body_ids.push_back(body_interface.CreateAndAddBody(sphere_settings, JPH::EActivation::Activate));

    JPH::BodyCreationSettings box_settings(new JPH::BoxShape(JPH::Vec3(0.5f, 0.5f, 0.5f)), JPH::Vec3(0.5f, 12.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
    body_ids.push_back(body_interface.CreateAndAddBody(box_settings, JPH::EActivation::Activate));

    for (int i = 0; i < 5; ++i)
    {
        JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::Vec3(i * 1.5, 15.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
        body_ids.push_back(body_interface.CreateAndAddBody(sphere_settings, JPH::EActivation::Activate));
    }
}


int main(void)
{
    // Raylib initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Raylib Jolt Demo");
    SetTargetFPS(60);

    // Jolt initialization
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
    JPH::TempAllocatorImpl temp_allocator(10 * 1024 * 1024);
    JPH::JobSystemThreadPool job_system(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    // Physics settings
    const JPH::uint cMaxBodies = 1024;
    const JPH::uint cNumBodyMutexes = 0;
    const JPH::uint cMaxBodyPairs = 1024;
    const JPH::uint cMaxContactConstraints = 1024;

    BPLayerInterfaceImpl broad_phase_layer_interface;
    ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;
    ObjectVsObjectLayerFilterImpl object_vs_object_layer_filter;

    JPH::PhysicsSystem physics_system;
    physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
        broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

    JPH::BodyInterface &body_interface = physics_system.GetBodyInterface();

    // Create floor
    JPH::BodyCreationSettings floor_settings(new JPH::BoxShape(JPH::Vec3(100.0f, 1.0f, 100.0f)), JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    body_interface.CreateAndAddBody(floor_settings, JPH::EActivation::DontActivate);

    // Create bodies
    std::vector<JPH::BodyID> body_ids;
    CreateBodies(body_interface, body_ids);


    // Camera setup
    Camera3D camera = {0};
    camera.position = (Vector3){15.0f, 15.0f, 15.0f};
    camera.target = (Vector3){0.0f, 5.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Mouse picking
    JPH::BodyID picked_body_id;
    float picked_body_dist = 0.0f;


    // Main loop
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_R))
        {
            ResetSimulation(body_interface, body_ids);
        }

        // Mouse picking
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            JPH::RRayCast ray_cast(JPH::RVec3(ray.position.x, ray.position.y, ray.position.z), JPH::Vec3(ray.direction.x, ray.direction.y, ray.direction.z) * 1000.0f);
            JPH::RayCastResult result;
            
            ObjectLayerFilterImpl object_layer_filter;
            BroadPhaseLayerFilterImpl broad_phase_layer_filter;

            if (physics_system.GetNarrowPhaseQuery().CastRay(ray_cast, result, broad_phase_layer_filter, object_layer_filter))
            {
                picked_body_id = result.mBodyID;
                body_interface.ActivateBody(picked_body_id);
                JPH::RVec3 pick_pos = ray_cast.mOrigin + result.mFraction * ray_cast.mDirection;
                picked_body_dist = Vector3Length(Vector3Subtract(camera.position, Vector3{(float)pick_pos.GetX(),(float)pick_pos.GetY(),(float)pick_pos.GetZ()}));
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !picked_body_id.IsInvalid())
        {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            JPH::RVec3 new_pos = JPH::RVec3(ray.position.x, ray.position.y, ray.position.z) + JPH::Vec3(ray.direction.x, ray.direction.y, ray.direction.z) * picked_body_dist;
            JPH::Vec3 force = (new_pos - body_interface.GetCenterOfMassPosition(picked_body_id)) * 500.0f;
            body_interface.AddForce(picked_body_id, force);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !picked_body_id.IsInvalid())
        {
            picked_body_id = JPH::BodyID();
        }


        // Physics update
        physics_system.Update(1.0f / 60.0f, 1, &temp_allocator, &job_system);

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw floor
        DrawCube(Vector3{0.0f, -1.0f, 0.0f}, 200.0f, 2.0f, 200.0f, LIGHTGRAY);

        // Draw bodies
        for (const auto& body_id : body_ids)
        {
            JPH::Vec3 pos = body_interface.GetCenterOfMassPosition(body_id);
            JPH::Quat rot = body_interface.GetRotation(body_id);
            const JPH::Shape *shape = body_interface.GetShape(body_id);


            if (shape->GetSubType() == JPH::EShapeSubType::Sphere)
            {
                DrawSphere(Vector3{(float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ()}, ((JPH::SphereShape*)shape)->GetRadius(), RED);
            }
            else if (shape->GetSubType() == JPH::EShapeSubType::Box)
            {
                JPH::Vec3 extent = ((JPH::BoxShape*)shape)->GetHalfExtent();
                DrawCube(Vector3{(float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ()}, extent.GetX() * 2, extent.GetY() * 2, extent.GetZ() * 2, BLUE);
            }
        }
        EndMode3D();
        DrawText("Press 'R' to reset", 10, 30, 20, BLACK);
        DrawText("Click and drag to move objects", 10, 50, 20, BLACK);
        DrawFPS(10, 10);


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
