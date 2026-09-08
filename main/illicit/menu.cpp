#include "menu.h"
#include "buttons.h"
#include "hud.h"
#include "natives.h"
#include "police.h"

using namespace Buttons;
using namespace Hud;
using namespace Natives;

namespace Menu_f
{
    bool isOpen = false;
    int currentOption = 0;
    int currentMenu = 0;
    int subOptionCount[32] = {0};

    enum MenuId
    {
        MENU_MAIN = 0,
        MENU_VIATURAS,
        MENU_EQUIPAMENTOS,
        MENU_OPERACOES,
        MENU_PROCURADO,
        MENU_CHAMADOS,
        MENU_RADIO,
        MENU_UNIFORMES,
        MENU_DELEGACIAS,
        MENU_FAVELAS,
        MENU_PATENTE
    };

    static int counts[] = { 10, 8, 5, 4, 6, 1, 1, 5, 3, 3, 1 };

    static const char* mainItems[] = {
        "Entrar em servico",
        "Spawn de viaturas",
        "Equipamentos policiais",
        "Operacoes policiais",
        "Sistema de procurado",
        "Chamados policiais",
        "Radio policial",
        "Fardamentos",
        "Delegacias",
        "Favelas / areas operacionais"
    };

    static const char* vehicleItems[] = {
        "PM - Police", "PM - Buffalo", "PM - Interceptor", "PM - Sheriff",
        "PRF - Sheriff", "PF - FBI", "PF - SUV", "Policia - Moto"
    };

    static const char* equipmentItems[] = {
        "Pistola", "Pistola de combate", "SMG", "Escopeta", "Carabina"
    };

    static const char* operationItems[] = {
        "Patrulhamento", "Blitz", "Apoio a ocorrencia", "Cerco policial"
    };

    static const char* wantedItems[] = {
        "Nivel 0", "Nivel 1", "Nivel 2", "Nivel 3", "Nivel 4", "Nivel 5"
    };

    static const char* uniformItems[] = {
        "Policia Militar", "Policia Civil", "PRF", "Policia Federal", "Policia Cientifica"
    };

    static const char* stationItems[] = {
        "Delegacia central", "Delegacia norte", "Delegacia Sandy / interior"
    };

    static const char* favelaItems[] = {
        "Area operacional 1", "Area operacional 2", "Area operacional 3"
    };

    static void title(const char* text)
    {
        DrawRect(0.73f, 0.16f, 0.35f, 0.055f, Colors::Common::Blue, 235);
        DrawText(text, 6, 0.73f, 0.145f, 0.43f, 0.43f, Colors::Common::White, 255, true);
    }

    static void item(const char* text, int index, bool selected)
    {
        float y = 0.205f + (0.043f * index);
        if (selected)
            DrawRect(0.73f, y + 0.012f, 0.35f, 0.039f, Colors::Common::White, 225);

        ColorSet color = selected ? Colors::Common::Blue : Colors::Common::White;
        DrawText(text, 6, 0.575f, y, 0.34f, 0.34f, color, 255, false);
    }

    static void drawStatus()
    {
        char status[128];
        snprintf(status, sizeof(status), "Servico: %s | Patente: %s",
                 PoliciaBR::IsOnDuty() ? "ATIVO" : "INATIVO",
                 PoliciaBR::GetRankName());
        DrawText(status, 6, 0.73f, 0.70f, 0.28f, 0.28f,
                 Colors::Common::White, 255, true);
        DrawText("X Selecionar   O Voltar", 6, 0.73f, 0.735f,
                 0.27f, 0.27f, Colors::Common::Yellow, 255, true);
    }

    static int itemCount()
    {
        return counts[currentMenu];
    }

    static void select()
    {
        switch (currentMenu)
        {
            case MENU_MAIN:
                switch (currentOption)
                {
                    case 0: PoliciaBR::ToggleService(0); break;
                    case 1: currentMenu = MENU_VIATURAS; currentOption = 0; break;
                    case 2: currentMenu = MENU_EQUIPAMENTOS; currentOption = 0; break;
                    case 3: currentMenu = MENU_OPERACOES; currentOption = 0; break;
                    case 4: currentMenu = MENU_PROCURADO; currentOption = 0; break;
                    case 5: PoliciaBR::PoliceCall(0); break;
                    case 6: PoliciaBR::Radio(0); break;
                    case 7: currentMenu = MENU_UNIFORMES; currentOption = 0; break;
                    case 8: currentMenu = MENU_DELEGACIAS; currentOption = 0; break;
                    case 9: currentMenu = MENU_FAVELAS; currentOption = 0; break;
                }
                break;

            case MENU_VIATURAS: PoliciaBR::SpawnVehicle(currentOption); break;
            case MENU_EQUIPAMENTOS: PoliciaBR::Equipment(currentOption); break;
            case MENU_OPERACOES: PoliciaBR::Operation(currentOption); break;
            case MENU_PROCURADO: PoliciaBR::SetWanted(currentOption); break;
            case MENU_CHAMADOS: PoliciaBR::PoliceCall(0); break;
            case MENU_RADIO: PoliciaBR::Radio(0); break;
            case MENU_UNIFORMES: PoliciaBR::Uniform(currentOption); break;
            case MENU_DELEGACIAS: PoliciaBR::Station(currentOption); break;
            case MENU_FAVELAS: PoliciaBR::Favela(currentOption); break;
            case MENU_PATENTE: PoliciaBR::Rank(0); break;
        }
    }

    void createMenuStruct()
    {
        for (int i = 0; i < 32; ++i)
            subOptionCount[i] = 0;

        for (unsigned int i = 0; i < sizeof(counts) / sizeof(counts[0]); ++i)
            subOptionCount[i] = counts[i];
    }

    void closeMenu()
    {
        isOpen = false;
        currentMenu = MENU_MAIN;
        currentOption = 0;
        ENABLE_ALL_CONTROL_ACTIONS();
    }

    void updateInput()
    {
        if (!isOpen)
        {
            if (isJustPressed(Button_SQUARE) && isJustPressed(Dpad_Left))
            {
                isOpen = true;
                currentMenu = MENU_MAIN;
                currentOption = 0;
                sleep(120);
            }
            return;
        }

        DisableControlsAll();

        if (isJustPressed(Dpad_Up))
        {
            --currentOption;
            if (currentOption < 0)
                currentOption = itemCount() - 1;
            sleep(80);
        }
        else if (isJustPressed(Dpad_Down))
        {
            ++currentOption;
            if (currentOption >= itemCount())
                currentOption = 0;
            sleep(80);
        }

        if (isJustPressed(Button_CROSS))
        {
            select();
            sleep(120);
        }

        if (isJustPressed(Button_CIRCLE))
        {
            if (currentMenu == MENU_MAIN)
                closeMenu();
            else
            {
                currentMenu = MENU_MAIN;
                currentOption = 0;
                sleep(100);
            }
        }
    }

    void drawMenu()
    {
        if (!isOpen)
            return;

        title("POLICIA BR");

        if (currentMenu == MENU_MAIN)
        {
            for (int i = 0; i < counts[MENU_MAIN]; ++i)
                item(mainItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_VIATURAS)
        {
            title("VIATURAS");
            for (int i = 0; i < counts[MENU_VIATURAS]; ++i)
                item(vehicleItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_EQUIPAMENTOS)
        {
            title("EQUIPAMENTOS");
            for (int i = 0; i < counts[MENU_EQUIPAMENTOS]; ++i)
                item(equipmentItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_OPERACOES)
        {
            title("OPERACOES");
            for (int i = 0; i < counts[MENU_OPERACOES]; ++i)
                item(operationItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_PROCURADO)
        {
            title("PROCURADO");
            for (int i = 0; i < counts[MENU_PROCURADO]; ++i)
                item(wantedItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_UNIFORMES)
        {
            title("FARDAMENTOS");
            for (int i = 0; i < counts[MENU_UNIFORMES]; ++i)
                item(uniformItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_DELEGACIAS)
        {
            title("DELEGACIAS");
            for (int i = 0; i < counts[MENU_DELEGACIAS]; ++i)
                item(stationItems[i], i, i == currentOption);
        }
        else if (currentMenu == MENU_FAVELAS)
        {
            title("AREAS OPERACIONAIS");
            for (int i = 0; i < counts[MENU_FAVELAS]; ++i)
                item(favelaItems[i], i, i == currentOption);
        }
        else
        {
            title("POLICIA BR");
            item("Acao disponivel", 0, true);
        }

        drawStatus();
    }
}
