#ifndef CONTINUITY_H
#define CONTINUITY_H

#include <stdint.h>

typedef enum {
    ContinuityTypeAirDrop,
    ContinuityTypeProximityPair,
    ContinuityTypeAirplayTarget,
    ContinuityTypeHandoff,
    ContinuityTypeTetheringSource,
    ContinuityTypeNearbyAction,
    ContinuityTypeNearbyInfo,
    ContinuityTypeCustomCrash,
    ContinuityTypeCount
} ContinuityType;

typedef struct {
    uint8_t flags;
    uint8_t type;
} ContinuityNearbyAction;

typedef struct {
    uint8_t prefix;
    uint16_t model;
} ContinuityProximityPair;

typedef union {
    struct {} custom_crash;
    ContinuityNearbyAction nearby_action;
    ContinuityProximityPair proximity_pair;
} ContinuityData;

typedef struct {
    ContinuityType type;
    ContinuityData data;
} ContinuityMsg;

const char* continuity_get_type_name(ContinuityType type);
uint8_t continuity_get_packet_size(ContinuityType type);
void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet);

#endif // CONTINUITY_H