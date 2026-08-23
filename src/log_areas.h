#pragma once

/**
 * @file log_areas.h
 * @brief Centralized registry for log area IDs
 * 
 * This file defines all log area IDs used throughout the application.
 * Using integer IDs instead of strings provides O(1) lookup performance
 * in the AreaFilter, eliminating hash map overhead.
 * 
 * USAGE:
 * 1. Add new area IDs here (sequential integers starting from 0)
 * 2. Use the IDs in LOG/SLOG macros: LOG(LogLevel::INFO, areas::NETWORK) << "msg";
 * 3. Configure filters using these IDs: filter->SetFilter(areas::NETWORK, LogLevel::DEBUG);
 * 
 * IMPORTANT:
 * - Area IDs must be unique across the entire application
 * - Once assigned, do not change an area's ID (breaks log file compatibility)
 * - New areas should be added at the end to maintain backward compatibility
 */

namespace asynclog {
namespace areas {

/**
 * @brief Log area identifiers
 * 
 * Each area represents a logical component or subsystem of the application.
 * Areas can be independently filtered to control log verbosity per component.
 */
enum AreaId : int {
    // Core system areas
    DEFAULT = 0,      ///< Default area (used when no area specified)
    
    // Application components
    NETWORK,          ///< Network communication (sockets, HTTP, etc.)
    DATABASE,         ///< Database operations (queries, connections)
    UI,               ///< User interface events
    AUTH,             ///< Authentication and authorization
    FILE_IO,          ///< File system operations
    
    // Testing and diagnostics
    TEST,             ///< Test code
    DEBUG,            ///< Debug-only logging
    PERFORMANCE,      ///< Performance measurements
    
    // Add new areas here (before AREA_COUNT)
    
    /**
     * @brief Total number of defined areas
     * 
     * Used internally for bounds checking. Automatically tracks the count.
     */
    AREA_COUNT
};

/**
 * @brief Get area name string for display purposes
 * 
 * @param areaId The area ID to convert
 * @return const char* The area name, or "UNKNOWN" if not found
 */
inline const char* getAreaName(int areaId) {
    switch (areaId) {
        case DEFAULT:     return "DEFAULT";
        case NETWORK:     return "NETWORK";
        case DATABASE:    return "DATABASE";
        case UI:          return "UI";
        case AUTH:        return "AUTH";
        case FILE_IO:     return "FILE_IO";
        case TEST:        return "TEST";
        case DEBUG:       return "DEBUG";
        case PERFORMANCE: return "PERFORMANCE";
        default:          return "UNKNOWN";
    }
}

} // namespace areas
} // namespace asynclog
