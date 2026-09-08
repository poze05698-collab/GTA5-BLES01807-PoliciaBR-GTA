#ifndef POLICIA_BR_H
#define POLICIA_BR_H

#include "includes.h"
#include "types.h"

namespace PoliciaBR
{
    void Init();
    void Tick();

    void ToggleService(int param);
    void SpawnVehicle(int param);
    void Equipment(int param);
    void ToggleEmergencyLights(int param);
    void ApproachNearest(int param);
    void ArrestNearest(int param);
    void SetWanted(int level);
    void PoliceCall(int param);
    void Backup(int param);
    void Radio(int param);
    void Operation(int param);
    void Favela(int param);
    void Station(int param);
    void Uniform(int param);
    void Rank(int param);
    void Fine(int param);

    bool IsOnDuty();
    int GetRank();
    const char* GetRankName();
}

#endif
