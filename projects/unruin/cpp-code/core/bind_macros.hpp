// ----------------------------------------------------------
// Macros for defining Godot Properties with GDExt using C++.
//
// Removes the need to manually define and bind setters and 
// getters for your properties inside your Godot class.
//
// Made for Godot 4.2.1
// ----------------------------------------------------------

#ifndef WOODENSPONGE_BIND_MACROS_HPP_INCLUDED
#define WOODENSPONGE_BIND_MACROS_HPP_INCLUDED

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>

// Binds the getter and setter for the property.
#define BIND_PROPERTY_METHODS(m_class, m_name)                                                                  \
    ::godot::ClassDB::bind_method(D_METHOD(_STR(set_##m_name), _STR(p_##m_name)), &##m_class::set_##m_name);    \
    ::godot::ClassDB::bind_method(D_METHOD(_STR(get_##m_name)), &##m_class::get_##m_name);

// Binds the getter and setter for the property before adding it.
#define BIND_ADD_PROPERTY_METHODS(m_class, m_name, m_property_info)         \
    BIND_PROPERTY_METHODS(m_class, m_name)                                  \
    ADD_PROPERTY(m_property_info, _STR(set_##m_name), _STR(get_##m_name));

// Defines the setter and getters for a class member.
// Make sure they're publically accessible.
#define PROPERTY_METHODS(m_type, m_name)                                    \
    void set_##m_name(const m_type p_##m_name) { ##m_name = p_##m_name; }   \
    m_type get_##m_name() const { return m_name; }

#endif // WOODENSPONGE_BIND_MACROS_HPP_INCLUDED