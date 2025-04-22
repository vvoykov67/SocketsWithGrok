#pragma once

// Role IDs for access control
enum class RoleId {
    Guest = 0, // Limited access (e.g., HelloModule only) 
    User = 1, // File operations 
    Admin = 2 // All operations (process and file) 
};