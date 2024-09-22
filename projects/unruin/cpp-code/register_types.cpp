#include "register_types.h"

#include "classes/player.h"
#include "classes/actor.h"
#include "classes/player_movement.h"
#include "classes/projectile.h"
#include "classes/pistol.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#ifdef _DEBUG
#ifdef _WIN32
#include <windows.h>
#endif // _DEBUG
#endif // _WIN32

using namespace godot;
using namespace unruin;

void unruin_init_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;

    // The order of class registration matters. Base classes (such as actor)
    // should be registered first before inherited classes.
    ClassDB::register_class<EntityStats>();
    ClassDB::register_class<Actor>();   // TODO: Make Actor an abstract class
    ClassDB::register_class<PlayerMovement>();
    ClassDB::register_class<Player>();
    ClassDB::register_class<Projectile>();
    ClassDB::register_class<Pistol>();
}

void unruin_terminate_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
}

extern "C"
{
    GDExtensionBool GDE_EXPORT unruin_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
#ifdef _DEBUG
#ifdef _WIN32
        if (IsDebuggerPresent())
        {
            DebugBreak(); //Break on dll load so we can set breakpoints at runtime.
        }        
#endif // _DEBUG
#endif // _WIN32

        GDExtensionBinding::InitObject init_object(p_get_proc_address, p_library, r_initialization);

        init_object.register_initializer(unruin_init_module);
        init_object.register_terminator(unruin_terminate_module);
        init_object.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_object.init();
    }
};