// ----------------------------------------------------------
// Implementation for Godot Properties with GDExt using C++.
//
// Removes the need to use _bind_methods for properties in
// Godot classes. The overhead shouldn't be too big but if
// you don't want it, use bind_macros.hpp.
//
// Made for Godot 4.2.1.
// Should work for later releases, maybe.
// ----------------------------------------------------------

// === USAGE EXAMPLE ===
//
// float player_speed;
// float gravity;
//
// SP_BEGIN_PROPERTIES
//
//     // Simply state SP_PROPERTY, the name of the variable, and it's info:
//     SP_PROPERTY(player_speed, PropertyInfo(Variant::FLOAT, "player_speed"))
//
//     // If you want, you can also add groups:
//     SP_PROPERTY_GROUP("Physics", "physics_")
//     SP_PROPERTY(gravity, PropertyInfo(Variant::FLOAT, "physics_gravity"))
//
// SP_END_PROPERTIES
//
// ======================

#ifndef WOODENSPONGE_SMART_PROPERTIES_HPP_INCLUDED
#define WOODENSPONGE_SMART_PROPERTIES_HPP_INCLUDED

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <unordered_map>
#include <functional>

namespace sprop
{
class PropertyFunctions
{
    std::function<void(godot::Variant)> setter;
    std::function<godot::Variant()> getter;

public:
    template <class G, class S>
    PropertyFunctions(godot::List<::godot::PropertyInfo> &p_list, 
        godot::PropertyInfo p_info, G p_setter_method, S p_getter_method)
        : setter(p_setter_method), getter(p_getter_method)
    {
        p_list.push_back(p_info);
    }
    void get(godot::Variant &r_property) const { r_property = getter(); }
    void set(const godot::Variant &r_property) { setter(r_property); }
};
}

// Needed for SP_PROPERTY to work.
// SP_END_PROPERTIES needs to be under it.
#define SP_BEGIN_PROPERTIES                                                                             \
private:                                                                                                \
    ::godot::List<::godot::PropertyInfo> _sp_property_info_list;                                        \
    ::std::unordered_map<::godot::StringName, ::sprop::PropertyFunctions> _sp_property_functions = {

// The bottom of the SP_PROPERTY list.
// Skips implementation of the smart properties. Either use SP_SET_FUNC_BODY, SP_GET_FUNC_BODY, and 
// SP_GET_PLIST_FUNC_BODY in their respective functions, or consider using SP_END_PROPERTIES.
//
// Make sure SP_BEGIN_PROPERTIES is above it.
#define SP_END_PROPERTIES_NO_IMPLEMENT };

#define SP_SET_FUNC_BODY(p_name, p_property)                                                            \
    if (_sp_property_functions.find(p_name) != _sp_property_functions.end())                            \
    {                                                                                                   \
        _sp_property_functions.at(p_name).set(p_property);                                              \
        return true;                                                                                    \
    }

#define SP_GET_FUNC_BODY(p_name, r_property)                                                            \
    if (_sp_property_functions.find(p_name) != _sp_property_functions.end())                            \
    {                                                                                                   \
        _sp_property_functions.at(p_name).get(r_property);                                              \
        return true;                                                                                    \
    }

#define SP_GET_PLIST_FUNC_BODY(p_list)                                                                      \
    for (::godot::PropertyInfo info : _sp_property_info_list)                                               \
    {                                                                                                       \
        p_list->push_back(info);                                                                            \
    }

// The bottom of the SP_PROPERTY list.
// Defines functionality for _set, _get, and _get_property_list to load all the PROPERTY macros. Be warned
// that the macro ends with "private:".
//
// Make sure BEGIN_PROPERTIES is above it.
#define SP_END_PROPERTIES                                                                                   \
    };                                                                                                      \
protected:                                                                                                  \
    bool _set(const ::godot::StringName &p_name, const ::godot::Variant &p_property)                        \
    {                                                                                                       \
        SP_SET_FUNC_BODY(p_name, p_property)                                                                \
        return false;                                                                                       \
    }                                                                                                       \
	bool _get(const ::godot::StringName &p_name, ::godot::Variant &r_property) const                        \
    {                                                                                                       \
        SP_GET_FUNC_BODY(p_name, r_property)                                                                \
        return false;                                                                                       \
    }                                                                                                       \
	void _get_property_list(::godot::List<::godot::PropertyInfo> *p_list) const                             \
    {                                                                                                       \
        SP_GET_PLIST_FUNC_BODY(p_list)                                                                      \
    }                                                                                                       \
private:

// Defines a private member and their accessor methods, automatically binding and adding them as a property.
// Use between SP_BEGIN_PROPERTIES and SP_END_PROPERTIES.
#define SP_PROPERTY(m_name, m_property_info)                                                                            \
    {m_property_info.name, ::sprop::PropertyFunctions(_sp_property_info_list, m_property_info,                          \
        [=](const ::godot::Variant p_##m_name){ m_name = p_##m_name; }, [=]() -> ::godot::Variant { return m_name; })},

// Declares an inspector group. Cannot take functions. Returns nothing if, somehow, got or set.
// Use between SP_BEGIN_PROPERTIES and SP_END_PROPERTIES.
#define SP_PROPERTY_GROUP(group_name_string, hint_string)                                                 \
    {group_name_string, ::sprop::PropertyFunctions(_sp_property_info_list, ::godot::PropertyInfo(Variant::NIL, group_name_string, ::godot::PROPERTY_HINT_NONE, hint_string, ::godot::PROPERTY_USAGE_GROUP | ::godot::PROPERTY_USAGE_SCRIPT_VARIABLE),                        \
        [=](const ::godot::Variant parameter){ return; }, [=]() -> ::godot::Variant { return Variant::NIL; })},

#endif