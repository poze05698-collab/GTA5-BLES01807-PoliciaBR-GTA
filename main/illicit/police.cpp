#include "police.h"
#include "natives.h"
#include "hud.h"
#include "misc.h"
#include "colors.h"

using namespace Natives;
using namespace Hud;
using namespace Misc;

namespace PoliciaBR
{
    static bool g_onDuty = false;
    static bool g_emergencyLights = false;
    static int g_rank = 0;
    static Vehicle g_lastVehicle = 0;
    static Ped g_lastSuspect = 0;
    static int g_callState = 0;

    static const char* rankNames[] = {
        "Recruta", "Soldado", "Cabo", "Sargento", "Tenente",
        "Capitao", "Major", "Coronel"
    };

    static const char* vehicleModels[] = {
        "police", "police2", "police3", "police4", "sheriff", "fbi", "fbi2", "policeb"
    };

    static const char* vehicleNames[] = {
        "PM - Police", "PM - Buffalo", "PM - Interceptor", "PM - Sheriff",
        "PRF - Sheriff", "PF - FBI", "PF - SUV", "Policia - Moto"
    };

    static void message(const char* text)
    {
        PrintText(text, 1800);
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
        float spawn[3];
        spawn[0] = pos.x + forward.x * 5.0f;
        spawn[1] = pos.y + forward.y * 5.0f;
        spawn[2] = pos.z + 0.5f;

        Vehicle vehicle = CREATE_VEHICLE(hash, spawn[0], spawn[1], spawn[2],
                                         GET_ENTITY_HEADING(PlayerSelf()), false, false);
        if (vehicle)
        {
            SET_ENTITY_AS_MISSION_ENTITY(vehicle, true, true);
            SET_VEHICLE_ON_GROUND_PROPERLY(vehicle);
            SET_PED_INTO_VEHICLE(PlayerSelf(), vehicle, -1);
            SET_VEHICLE_NUMBER_PLATE_TEXT(vehicle, (char*)"POLICIA");
            g_lastVehicle = vehicle;
        }

        SET_MODEL_AS_NO_LONGER_NEEDED(hash);
        return vehicle;
    }

    void Init()
    {
        g_onDuty = false;
        g_emergencyLights = false;
        g_rank = 0;
        g_lastVehicle = 0;
        g_lastSuspect = 0;
        g_callState = 0;
    }

    void Tick()
    {
        if (!g_onDuty)
            return;

        if (g_lastVehicle)
        {
            SET_VEHICLE_INDICATOR_LIGHTS(g_lastVehicle, 0, g_emergencyLights);
            SET_VEHICLE_INDICATOR_LIGHTS(g_lastVehicle, 1, g_emergencyLights);
            SET_VEHICLE_SIREN(g_lastVehicle, g_emergencyLights);
        }
    }

    void ToggleService(int)
    {
        g_onDuty = !g_onDuty;
        if (g_onDuty)
            message("~b~POLICIA BR~w~: em servico.");
        else
            message("~w~POLICIA BR: fora de servico.");
    }

    void SpawnVehicle(int param)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        if (param < 0 || param >= 8)
            return;

        if (spawnVehicleModel(vehicleModels[param]))
        {
            char text[96];
            snprintf(text, sizeof(text), "~b~Viatura: ~w~%s", vehicleNames[param]);
            message(text);
        }
    }

    void Equipment(int param)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        Ped player = PlayerSelf();
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

        GIVE_DELAYED_WEAPON_TO_PED(player, weapon, 1, true);
        message("~b~Equipamento policial entregue.");
    }

    void ToggleEmergencyLights(int)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        g_emergencyLights = !g_emergencyLights;
        if (g_emergencyLights)
            message("~b~Emergencia: ~g~LIGADA");
        else
            message("~b~Emergencia: ~r~DESLIGADA");
    }

    void ApproachNearest(int)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        Vector3 pos = GET_ENTITY_COORDS(PlayerSelf(), true);
        Ped suspect = 0;
        if (GET_CLOSEST_PED(pos.x, pos.y, pos.z, 20.0f, true, false,
                            &suspect, false, false, 0) && suspect)
        {
            g_lastSuspect = suspect;
            message("~y~Abordagem: ~w~suspeito localizado.");
        }
        else
        {
            message("~r~Nenhum suspeito proximo.");
        }
    }

    void ArrestNearest(int)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        if (!g_lastSuspect)
            ApproachNearest(0);

        if (g_lastSuspect)
        {
            TASK_ARREST_PED(g_lastSuspect, PlayerSelf());
            message("~b~Suspeito: ~w~prisao iniciada.");
        }
        else
        {
            message("~r~Nenhum suspeito selecionado.");
        }
    }

    void SetWanted(int level)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

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
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        g_callState = (g_callState + 1) % 3;
        if (g_callState == 1)
            message("~b~Central: ~w~chamado recebido.");
        else if (g_callState == 2)
            message("~y~Central: ~w~ocorrencia em andamento.");
        else
            message("~g~Central: ~w~ocorrencia encerrada.");
    }

    void Backup(int)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        Vehicle vehicle = spawnVehicleModel("police2");
        if (vehicle)
            message("~b~Backup: ~w~unidade policial enviada.");
    }

    void Radio(int)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }
        message("~b~RADIO POLICIA~w~: QAP, QSL, aguardando instrucoes.");
    }

    void Operation(int param)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        switch (param)
        {
            case 0: message("~b~Operacao: ~w~patrulhamento iniciado."); break;
            case 1: message("~b~Operacao: ~w~blitz iniciada."); break;
            case 2: message("~b~Operacao: ~w~apoio a ocorrencia solicitado."); break;
            case 3: message("~b~Operacao: ~w~cerco policial iniciado."); break;
        }
    }

    void Favela(int param)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        Vector3 coords;
        switch (param)
        {
            case 0: coords.x = 103.7f; coords.y = -1940.0f; coords.z = 20.8f; break;
            case 1: coords.x = 1365.0f; coords.y = -1720.0f; coords.z = 65.0f; break;
            case 2: coords.x = -215.0f; coords.y = -1600.0f; coords.z = 34.0f; break;
            default: return;
        }

        float p[3] = { coords.x, coords.y, coords.z };
        SET_ENTITY_COORDS(PlayerSelf(), p, 1, 0, 0, 1);
        message("~b~Deslocamento operacional realizado.");
    }

    void Station(int param)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        Vector3 coords;
        switch (param)
        {
            case 0: coords.x = 425.1f; coords.y = -979.5f; coords.z = 30.7f; break;
            case 1: coords.x = 1850.0f; coords.y = 3680.0f; coords.z = 34.2f; break;
            case 2: coords.x = -448.0f; coords.y = 6012.0f; coords.z = 31.7f; break;
            default: return;
        }

        float p[3] = { coords.x, coords.y, coords.z };
        SET_ENTITY_COORDS(PlayerSelf(), p, 1, 0, 0, 1);
        message("~b~Delegacia: ~w~deslocamento realizado.");
    }

    void Uniform(int param)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }

        int drawable = 0;
        switch (param)
        {
            case 0: drawable = 0; message("~b~Fardamento: ~w~Policia Militar."); break;
            case 1: drawable = 1; message("~b~Fardamento: ~w~Policia Civil."); break;
            case 2: drawable = 2; message("~b~Fardamento: ~w~PRF."); break;
            case 3: drawable = 3; message("~b~Fardamento: ~w~Policia Federal."); break;
            case 4: drawable = 4; message("~b~Fardamento: ~w~Policia Cientifica."); break;
            default: return;
        }
        // Primeiro preset visual. Modelos brasileiros 3D exigem assets adicionais.
        SET_PED_COMPONENT_VARIATION(PlayerSelf(), 11, drawable, 0, 0);
    }

    void Rank(int)
    {
        if (g_rank < 7)
            ++g_rank;
        else
            g_rank = 0;

        char text[96];
        snprintf(text, sizeof(text), "~b~Patente: ~w~%s", rankNames[g_rank]);
        message(text);
    }

    void Fine(int)
    {
        if (!g_onDuty)
        {
            message("~r~Entre em servico primeiro.");
            return;
        }
        message("~b~Multa: ~w~registro de infracao emitido.");
    }

    bool IsOnDuty()
    {
        return g_onDuty;
    }

    int GetRank()
    {
        return g_rank;
    }

    const char* GetRankName()
    {
        return rankNames[g_rank];
    }
}
