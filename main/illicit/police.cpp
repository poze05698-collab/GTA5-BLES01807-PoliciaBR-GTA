#include "police.h"
#include "natives.h"
#include "hud.h"
#include "misc.h"
#include "colors.h"
#include "addresses.h"

using namespace Natives;
using namespace Hud;
using namespace Misc;

namespace PoliciaBR
{
    static bool g_onDuty = false;
    static bool g_emergencyLights = false;
    static bool g_pursuit = false;
    static int g_rank = 0;
    static int g_points = 0;
    static int g_fines = 0;
    static int g_calls = 0;
    static int g_unit = UNIT_PM;
    static Vehicle g_lastVehicle = 0;
    static Ped g_lastSuspect = 0;
    static Ped g_backupDriver = 0;
    static int g_callState = 0;
    static int g_tick = 0;

    static const char* rankNames[] = {
        "Recruta", "Soldado", "Cabo", "Sargento",
        "Tenente", "Capitao", "Major", "Coronel"
    };

    static const char* unitNames[] = {
        "Policia Militar", "Policia Civil", "PRF",
        "Policia Federal", "Policia Cientifica"
    };

    static const char* vehicleModels[] = {
        "police", "police2", "police3", "police4",
        "sheriff", "fbi", "fbi2", "policeb"
    };

    static const char* vehicleNames[] = {
        "PM - Viatura", "PM - Interceptor", "PM - Buffalo",
        "PM - Sheriff", "PRF - Sheriff", "PF - FBI",
        "PF - SUV", "Policia - Moto"
    };

    static void message(const char* text)
    {
        PrintText(text, 1800);
    }

    static void addPoints(int amount)
    {
        if (amount <= 0) return;
        g_points += amount;
        int newRank = g_points / 100;
        if (newRank > 7) newRank = 7;
        if (newRank != g_rank)
        {
            g_rank = newRank;
            char text[96];
            snprintf(text, sizeof(text), "~b~Promocao! ~w~Patente: %s", rankNames[g_rank]);
            message(text);
        }
    }

    static bool loadModel(const char* modelName, uint& hash)
    {
        hash = GET_HASH_KEY(modelName);
        REQUEST_MODEL(hash);
        int tries = 0;
        while (!HAS_MODEL_LOADED(hash) && tries < 100)
        {
            wait(10);
            ++tries;
        }
        return HAS_MODEL_LOADED(hash);
    }

    static Vehicle spawnVehicleModel(const char* modelName)
    {
        uint hash = 0;
        if (!loadModel(modelName, hash))
        {
            message("~r~Policia BR: modelo nao carregado.");
            return 0;
        }

        Vector3 pos = GET_ENTITY_COORDS(PlayerSelf(), true);
        Vector3 forward = GET_ENTITY_FORWARD_VECTOR(PlayerSelf());
        float x = pos.x + forward.x * 6.0f;
        float y = pos.y + forward.y * 6.0f;
        float z = pos.z + 0.5f;

        Vehicle vehicle = CREATE_VEHICLE(hash, x, y, z,
                                         GET_ENTITY_HEADING(PlayerSelf()), false, false);
        if (vehicle)
        {
            SET_ENTITY_AS_MISSION_ENTITY(vehicle, true, true);
            SET_VEHICLE_ON_GROUND_PROPERLY(vehicle);
            SET_PED_INTO_VEHICLE(PlayerSelf(), vehicle, -1);
            SET_VEHICLE_NUMBER_PLATE_TEXT(vehicle, (char*)"POLICIA");
            SET_VEHICLE_INDICATOR_LIGHTS(vehicle, 0, g_emergencyLights);
            SET_VEHICLE_INDICATOR_LIGHTS(vehicle, 1, g_emergencyLights);
            SET_VEHICLE_SIREN(vehicle, g_emergencyLights);
            g_lastVehicle = vehicle;
        }

        SET_MODEL_AS_NO_LONGER_NEEDED(hash);
        return vehicle;
    }

    static Ped spawnBackupUnit()
    {
        uint vehicleHash = 0;
        uint pedHash = 0;
        if (!loadModel("police2", vehicleHash)) return 0;
        if (!loadModel("s_m_y_cop_01", pedHash)) return 0;

        Vector3 pos = GET_ENTITY_COORDS(PlayerSelf(), true);
        Vector3 forward = GET_ENTITY_FORWARD_VECTOR(PlayerSelf());
        Vehicle vehicle = CREATE_VEHICLE(vehicleHash,
            pos.x + forward.x * 12.0f, pos.y + forward.y * 12.0f, pos.z + 0.5f,
            GET_ENTITY_HEADING(PlayerSelf()), false, false);
        if (!vehicle) return 0;

        SET_ENTITY_AS_MISSION_ENTITY(vehicle, true, true);
        SET_VEHICLE_ON_GROUND_PROPERLY(vehicle);
        SET_VEHICLE_SIREN(vehicle, true);
        SET_VEHICLE_INDICATOR_LIGHTS(vehicle, 0, true);
        SET_VEHICLE_INDICATOR_LIGHTS(vehicle, 1, true);

        Ped driver = CREATE_PED_INSIDE_VEHICLE(vehicle, 6, pedHash, -1, false, false);
        if (driver)
        {
            SET_ENTITY_AS_MISSION_ENTITY(driver, true, true);
            g_backupDriver = driver;
            if (g_lastSuspect)
                TASK_VEHICLE_CHASE(driver, g_lastSuspect);
            else
                TASK_VEHICLE_DRIVE_WANDER(driver, vehicle, 18.0f, 786603);
        }

        SET_MODEL_AS_NO_LONGER_NEEDED(vehicleHash);
        SET_MODEL_AS_NO_LONGER_NEEDED(pedHash);
        return driver;
    }

    void Init()
    {
        g_onDuty = false;
        g_emergencyLights = false;
        g_pursuit = false;
        g_rank = 0;
        g_points = 0;
        g_fines = 0;
        g_calls = 0;
        g_unit = UNIT_PM;
        g_lastVehicle = 0;
        g_lastSuspect = 0;
        g_backupDriver = 0;
        g_callState = 0;
        g_tick = 0;
    }

    void Tick()
    {
        if (!g_onDuty) return;
        ++g_tick;

        if (g_lastVehicle)
        {
            SET_VEHICLE_INDICATOR_LIGHTS(g_lastVehicle, 0, g_emergencyLights);
            SET_VEHICLE_INDICATOR_LIGHTS(g_lastVehicle, 1, g_emergencyLights);
            SET_VEHICLE_SIREN(g_lastVehicle, g_emergencyLights);
        }

        if (g_pursuit && g_lastSuspect && g_lastVehicle && (g_tick % 80) == 0)
        {
            // Reaplica a tarefa periodicamente para manter a perseguição ativa.
            Ped player = PlayerSelf();
            if (player && IS_PED_IN_ANY_VEHICLE(player, false))
                message("~r~PERSEGUICAO ~w~ativa - mantenha distancia segura.");
        }
    }

    void ToggleService(int)
    {
        g_onDuty = !g_onDuty;
        if (!g_onDuty)
        {
            g_emergencyLights = false;
            g_pursuit = false;
            message("~w~POLICIA BR: fora de servico.");
            return;
        }
        message("~b~POLICIA BR~w~: em servico.");
    }

    void SpawnVehicle(int param)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (param < 0 || param >= 8) return;
        if (spawnVehicleModel(vehicleModels[param]))
        {
            char text[96];
            snprintf(text, sizeof(text), "~b~Viatura: ~w~%s", vehicleNames[param]);
            message(text);
        }
    }

    void Equipment(int param)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        uint weapon = 0;
        switch (param)
        {
            case 0: weapon = GET_HASH_KEY("WEAPON_PISTOL"); break;
            case 1: weapon = GET_HASH_KEY("WEAPON_COMBATPISTOL"); break;
            case 2: weapon = GET_HASH_KEY("WEAPON_SMG"); break;
            case 3: weapon = GET_HASH_KEY("WEAPON_PUMPSHOTGUN"); break;
            case 4: weapon = GET_HASH_KEY("WEAPON_CARBINERIFLE"); break;
            default: return;
        }
        GIVE_DELAYED_WEAPON_TO_PED(PlayerSelf(), weapon, 999, true);
        message("~b~Equipamento policial entregue.");
        addPoints(5);
    }

    void ToggleEmergencyLights(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        g_emergencyLights = !g_emergencyLights;
        message(g_emergencyLights ? "~b~Emergencia: ~g~LIGADA" : "~b~Emergencia: ~r~DESLIGADA");
    }

    void ApproachNearest(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        Vector3 pos = GET_ENTITY_COORDS(PlayerSelf(), true);
        Ped suspect = 0;
        if (GET_CLOSEST_PED(pos.x, pos.y, pos.z, 20.0f, true, false,
                            &suspect, false, false, 0) && suspect && !IS_PED_A_PLAYER(suspect))
        {
            g_lastSuspect = suspect;
            message("~y~ABORDAGEM: ~w~suspeito selecionado.");
            addPoints(10);
        }
        else message("~r~Nenhum suspeito proximo.");
    }

    void StartPursuit(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (!g_lastSuspect) ApproachNearest(0);
        if (!g_lastSuspect) return;
        if (!g_lastVehicle)
        {
            message("~y~Spawn uma viatura antes da perseguicao.");
            return;
        }
        g_pursuit = true;
        g_emergencyLights = true;
        message("~r~PERSEGUICAO INICIADA~w~: suspeito em fuga.");
        addPoints(15);
    }

    void StopPursuit(int)
    {
        g_pursuit = false;
        if (g_backupDriver) CLEAR_PED_TASKS(g_backupDriver);
        message("~g~Perseguicao encerrada.");
    }

    void ArrestNearest(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (!g_lastSuspect) ApproachNearest(0);
        if (!g_lastSuspect) { message("~r~Nenhum suspeito selecionado."); return; }
        TASK_ARREST_PED(g_lastSuspect, PlayerSelf());
        g_pursuit = false;
        message("~b~PRISAO: ~w~procedimento iniciado.");
        addPoints(50);
    }

    void Fine(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (!g_lastSuspect) ApproachNearest(0);
        if (!g_lastSuspect) { message("~r~Nenhum suspeito selecionado."); return; }
        ++g_fines;
        addPoints(25);
        char text[96];
        snprintf(text, sizeof(text), "~b~MULTA REGISTRADA~w~: total %d | pontos %d", g_fines, g_points);
        message(text);
    }

    void SetWanted(int level)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (level < 0) level = 0;
        if (level > 5) level = 5;
        SET_PLAYER_WANTED_LEVEL(PlayerID(), level, false);
        SET_PLAYER_WANTED_LEVEL_NOW(PlayerID(), false);
        char text[64];
        snprintf(text, sizeof(text), "~b~Nivel de procurado: ~w~%d", level);
        message(text);
    }

    void PoliceCall(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        g_callState = (g_callState + 1) % 3;
        ++g_calls;
        if (g_callState == 1) { message("~b~CENTRAL: ~w~chamado recebido."); addPoints(15); }
        else if (g_callState == 2) message("~y~CENTRAL: ~w~ocorrencia em andamento.");
        else { message("~g~CENTRAL: ~w~ocorrencia encerrada."); addPoints(25); }
    }

    void Backup(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (!g_lastSuspect) ApproachNearest(0);
        if (spawnBackupUnit())
        {
            message("~b~BACKUP: ~w~unidade policial despachada.");
            addPoints(10);
        }
        else message("~r~Nao foi possivel criar a unidade de backup.");
    }

    void Radio(int)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        message("~b~RADIO POLICIA~w~: QAP, QSL. Unidade em deslocamento.");
    }

    void Operation(int param)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        switch (param)
        {
            case 0: message("~b~Operacao: ~w~patrulhamento iniciado."); break;
            case 1: message("~b~Operacao: ~w~blitz iniciada."); break;
            case 2: message("~b~Operacao: ~w~apoio a ocorrencia solicitado."); Backup(0); break;
            case 3: message("~b~Operacao: ~w~cerco policial iniciado."); g_emergencyLights = true; Backup(0); break;
        }
        addPoints(10);
    }

    void Favela(int param)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        Vector3 coords;
        switch (param)
        {
            case 0: coords.x=103.7f; coords.y=-1940.0f; coords.z=20.8f; break;
            case 1: coords.x=1365.0f; coords.y=-1720.0f; coords.z=65.0f; break;
            case 2: coords.x=-215.0f; coords.y=-1600.0f; coords.z=34.0f; break;
            default: return;
        }
        float p[3]={coords.x,coords.y,coords.z};
        SET_ENTITY_COORDS(PlayerSelf(), p, 1, 0, 0, 1);
        message("~b~AREA OPERACIONAL: ~w~deslocamento realizado.");
    }

    void Station(int param)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        Vector3 coords;
        switch (param)
        {
            case 0: coords.x=425.1f; coords.y=-979.5f; coords.z=30.7f; break;
            case 1: coords.x=1850.0f; coords.y=3680.0f; coords.z=34.2f; break;
            case 2: coords.x=-448.0f; coords.y=6012.0f; coords.z=31.7f; break;
            default: return;
        }
        float p[3]={coords.x,coords.y,coords.z};
        SET_ENTITY_COORDS(PlayerSelf(), p, 1, 0, 0, 1);
        message("~b~DELEGACIA: ~w~deslocamento realizado.");
    }

    void Uniform(int param)
    {
        if (!g_onDuty) { message("~r~Entre em servico primeiro."); return; }
        if (param < 0 || param > 4) return;
        g_unit = param;
        // Presets visuais temporarios. Modelos/roupas brasileiras reais exigem assets adicionais.
        SET_PED_COMPONENT_VARIATION(PlayerSelf(), 11, param, 0, 0);
        char text[96];
        snprintf(text, sizeof(text), "~b~FARDAMENTO: ~w~%s", unitNames[g_unit]);
        message(text);
    }

    void Rank(int)
    {
        g_rank = (g_rank + 1) % 8;
        g_points = g_rank * 100;
        char text[96];
        snprintf(text, sizeof(text), "~b~PATENTE: ~w~%s | pontos %d", rankNames[g_rank], g_points);
        message(text);
    }

    bool IsOnDuty() { return g_onDuty; }
    bool IsPursuitActive() { return g_pursuit; }
    bool IsEmergencyActive() { return g_emergencyLights; }
    int GetRank() { return g_rank; }
    int GetPoints() { return g_points; }
    int GetFines() { return g_fines; }
    int GetCalls() { return g_calls; }
    const char* GetRankName() { return rankNames[g_rank]; }
    const char* GetUnitName() { return unitNames[g_unit]; }
}
