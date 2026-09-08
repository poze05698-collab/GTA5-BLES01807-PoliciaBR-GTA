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
        MENU_MAIN=0, MENU_VIATURAS, MENU_EQUIPAMENTOS, MENU_OPERACOES,
        MENU_PROCURADO, MENU_OCORRENCIA, MENU_UNIFORMES, MENU_DELEGACIAS,
        MENU_AREAS, MENU_CARREIRA, MENU_EMERGENCIA, MENU_PERSEGUICAO
    };

    static const char* mainItems[] = {
        "Entrar / sair de servico", "Viaturas", "Equipamentos", "Operacoes",
        "Abordagem de NPC", "Prender suspeito", "Aplicar multa", "Perseguicao",
        "Procurado", "Chamados / central", "Radio policial", "Backup policial",
        "Emergencia / sirene", "Fardamentos", "Delegacias", "Areas operacionais",
        "Carreira / patente"
    };
    static const int mainCount=17;
    static const char* vehicleItems[]={"PM - Viatura","PM - Interceptor","PM - Buffalo","PM - Sheriff","PRF - Sheriff","PF - FBI","PF - SUV","Policia - Moto"};
    static const char* equipmentItems[]={"Pistola","Pistola de combate","SMG","Escopeta","Carabina"};
    static const char* operationItems[]={"Patrulhamento","Blitz","Apoio a ocorrencia","Cerco policial"};
    static const char* wantedItems[]={"Nivel 0","Nivel 1","Nivel 2","Nivel 3","Nivel 4","Nivel 5"};
    static const char* uniformItems[]={"Policia Militar","Policia Civil","PRF","Policia Federal","Policia Cientifica"};
    static const char* stationItems[]={"Delegacia central","Delegacia norte","Delegacia Sandy / interior"};
    static const char* areaItems[]={"Area operacional 1","Area operacional 2","Area operacional 3"};
    static const char* careerItems[]={"Avancar patente","Ver estatisticas"};
    static const char* emergencyItems[]={"Ligar / desligar sirene","Radio de emergencia"};
    static const char* pursuitItems[]={"Iniciar perseguicao","Encerrar perseguicao","Solicitar backup"};

    static int count()
    {
        switch(currentMenu){
            case MENU_MAIN:return mainCount; case MENU_VIATURAS:return 8; case MENU_EQUIPAMENTOS:return 5;
            case MENU_OPERACOES:return 4; case MENU_PROCURADO:return 6; case MENU_UNIFORMES:return 5;
            case MENU_DELEGACIAS:return 3; case MENU_AREAS:return 3; case MENU_CARREIRA:return 2;
            case MENU_EMERGENCIA:return 2; case MENU_PERSEGUICAO:return 3; default:return 1;
        }
    }
    static void title(const char* t){ DrawRect(0.73f,0.16f,0.35f,0.055f,Colors::Common::Blue,235); DrawText(t,6,0.73f,0.145f,0.43f,0.43f,Colors::Common::White,255,true); }
    static void item(const char* t,int i,bool s){ float y=0.205f+0.034f*i; if(s) DrawRect(0.73f,y+0.010f,0.35f,0.031f,Colors::Common::White,225); ColorSet c=s?Colors::Common::Blue:Colors::Common::White; DrawText(t,6,0.575f,y,0.29f,0.29f,c,255,false); }
    static void status(){ char line[192]; snprintf(line,sizeof(line),"%s | %s | P:%d | M:%d | C:%d",PoliciaBR::GetUnitName(),PoliciaBR::GetRankName(),PoliciaBR::GetPoints(),PoliciaBR::GetFines(),PoliciaBR::GetCalls()); DrawText(line,6,0.73f,0.785f,0.23f,0.23f,Colors::Common::White,255,true); }

    static void select()
    {
        switch(currentMenu){
            case MENU_MAIN:
                switch(currentOption){
                    case 0:PoliciaBR::ToggleService(0);break; case 1:currentMenu=MENU_VIATURAS;currentOption=0;break;
                    case 2:currentMenu=MENU_EQUIPAMENTOS;currentOption=0;break; case 3:currentMenu=MENU_OPERACOES;currentOption=0;break;
                    case 4:PoliciaBR::ApproachNearest(0);break; case 5:PoliciaBR::ArrestNearest(0);break; case 6:PoliciaBR::Fine(0);break;
                    case 7:currentMenu=MENU_PERSEGUICAO;currentOption=0;break; case 8:currentMenu=MENU_PROCURADO;currentOption=0;break;
                    case 9:PoliciaBR::PoliceCall(0);break; case 10:PoliciaBR::Radio(0);break; case 11:PoliciaBR::Backup(0);break;
                    case 12:currentMenu=MENU_EMERGENCIA;currentOption=0;break; case 13:currentMenu=MENU_UNIFORMES;currentOption=0;break;
                    case 14:currentMenu=MENU_DELEGACIAS;currentOption=0;break; case 15:currentMenu=MENU_AREAS;currentOption=0;break;
                    case 16:currentMenu=MENU_CARREIRA;currentOption=0;break;
                } break;
            case MENU_VIATURAS:PoliciaBR::SpawnVehicle(currentOption);break;
            case MENU_EQUIPAMENTOS:PoliciaBR::Equipment(currentOption);break;
            case MENU_OPERACOES:PoliciaBR::Operation(currentOption);break;
            case MENU_PROCURADO:PoliciaBR::SetWanted(currentOption);break;
            case MENU_UNIFORMES:PoliciaBR::Uniform(currentOption);break;
            case MENU_DELEGACIAS:PoliciaBR::Station(currentOption);break;
            case MENU_AREAS:PoliciaBR::Favela(currentOption);break;
            case MENU_CARREIRA:if(currentOption==0)PoliciaBR::Rank(0);else PoliciaBR::Radio(0);break;
            case MENU_EMERGENCIA:if(currentOption==0)PoliciaBR::ToggleEmergencyLights(0);else PoliciaBR::Radio(0);break;
            case MENU_PERSEGUICAO:if(currentOption==0)PoliciaBR::StartPursuit(0);else if(currentOption==1)PoliciaBR::StopPursuit(0);else PoliciaBR::Backup(0);break;
        }
    }
    static const char** items(){
        switch(currentMenu){case MENU_MAIN:return mainItems;case MENU_VIATURAS:return vehicleItems;case MENU_EQUIPAMENTOS:return equipmentItems;case MENU_OPERACOES:return operationItems;case MENU_PROCURADO:return wantedItems;case MENU_UNIFORMES:return uniformItems;case MENU_DELEGACIAS:return stationItems;case MENU_AREAS:return areaItems;case MENU_CARREIRA:return careerItems;case MENU_EMERGENCIA:return emergencyItems;case MENU_PERSEGUICAO:return pursuitItems;default:return mainItems;}
    }
    void createMenuStruct(){for(int i=0;i<32;i++)subOptionCount[i]=0;}
    void closeMenu(){isOpen=false;currentMenu=MENU_MAIN;currentOption=0;ENABLE_ALL_CONTROL_ACTIONS();}
    void updateInput(){
        if(!isOpen){if(isJustPressed(Button_SQUARE)&&isJustPressed(Dpad_Left)){isOpen=true;currentMenu=MENU_MAIN;currentOption=0;sleep(120);}return;}
        DisableControlsAll(); int n=count();
        if(isJustPressed(Dpad_Up)){--currentOption;if(currentOption<0)currentOption=n-1;sleep(80);}
        else if(isJustPressed(Dpad_Down)){++currentOption;if(currentOption>=n)currentOption=0;sleep(80);}
        if(isJustPressed(Button_CROSS)){select();sleep(120);} if(isJustPressed(Button_CIRCLE)){if(currentMenu==MENU_MAIN)closeMenu();else{currentMenu=MENU_MAIN;currentOption=0;sleep(100);}}
    }
    void drawMenu(){if(!isOpen)return; title(currentMenu==MENU_MAIN?"POLICIA BR":"POLICIA BR - SUBMENU"); const char** a=items(); for(int i=0;i<count();++i)item(a[i],i,i==currentOption); status(); DrawText("L1/R1 categoria | X selecionar | O voltar",6,0.73f,0.825f,0.22f,0.22f,Colors::Common::Yellow,255,true);}
}
