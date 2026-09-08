#ifndef POLICIA_BR_H
#define POLICIA_BR_H

#include "includes.h"
#include "types.h"

namespace PoliciaBR
{
    enum PoliceUnit
    {
        UNIT_PM = 0,
        UNIT_CIVIL,
        UNIT_PRF,
        UNIT_PF,
        UNIT_CIENTIFICA
    };

    void Init();
    void Tick();

    void ToggleService(int param);
    void SpawnVehicle(int param);
    void Equipment(int param);
    void ToggleEmergencyLights(int param);
    void ApproachNearest(int param);
    void StartPursuit(int param);
    void StopPursuit(int param);
    void ArrestNearest(int param);
    void Fine(int param);
    void SetWanted(int level);
    void PoliceCall(int param);
    void Backup(int param);
    void Radio(int param);
    void Operation(int param);
    void Favela(int param);
    void Station(int param);
    void Uniform(int param);
    void Rank(int param);

    bool IsOnDuty();
    bool IsPursuitActive();
    bool IsEmergencyActive();
    int GetRank();
    int GetPoints();
    int GetFines();
    int GetCalls();
    const char* GetRankName();
    const char* GetUnitName();
}

#endif
