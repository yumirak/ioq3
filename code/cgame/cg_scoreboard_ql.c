#ifndef BASEQZ
#error This file only used for QL.
#endif

#include "cg_local.h"
#include "cg_utils.h"
#include "../ui/ui_shared.h"


const char *CG_FeederItemText_NonPremium (float feederID, int index, int column, qhandle_t *handle)
{
    int scoreIndex = 0;
    clientInfo_t *info = NULL;
    int team = -1;
    score_t *sp = NULL;

    *handle = -1;

    // return va("%d", column); // debug
#if BASEQZ >= 730
    column -= 1;
#endif
    switch ( (int)feederID ) {
        case FEEDER_REDTEAM_LIST: team = TEAM_RED; break;
        case FEEDER_BLUETEAM_LIST: team = TEAM_BLUE; break;
        case FEEDER_SCOREBOARD: team = TEAM_FREE; break;
        default: return "";
    }

    info = CG_InfoFromScoreIndex(index, team, &scoreIndex);
    sp = &cg.scores[scoreIndex];
    if ( !info || !info->infoValid )
        return "";

    if( CG_IsTeamGame( cgs.gametype ) ) {
        switch (column) {
            case 0:
                *handle = cg_weapons[sp->bestWeapon].weaponIcon;
                return "";
            case 1: {
                qboolean ready = qfalse;
                if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << sp->client ) ) {
                    ready = qtrue;
                }
                if (cg.warmup || cg.snap->ps.pm_type == PM_INTERMISSION) {
                    if (ready) {
                        *handle = trap_R_RegisterShader("ui/assets/score/arrowg");
                    } else {
                        *handle = trap_R_RegisterShader("ui/assets/score/arrowr");
                    }
                }
                return "";
            }
            case 2: return va("%s", info->name);
            case 3: return va("%d", sp->score);
            case 4: return va("%d", sp->kills);
            case 5: return va("%d", sp->deaths);
            case 6:
                if(info->botSkill) return "BOT";
                return va("%d", sp->ping);
            default:
                break;
        }
    } else {
        switch (column) {
            case 0: *handle = cgs.clientinfo[sp->client].modelIcon; return "";
            case 1: {
                qboolean ready = qfalse;
                if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << sp->client ) ) {
                       ready = qtrue;
                }
                if (cg.warmup || cg.snap->ps.pm_type == PM_INTERMISSION) {
                    if (ready) {
                        *handle = trap_R_RegisterShader("ui/assets/score/arrowg");
                    } else {
                        *handle = trap_R_RegisterShader("ui/assets/score/arrowr");
                    }
                }
                return "";
            }
            case 2: return va("%s", info->name);
            case 3:
                if (cgs.gametype == GT_TOURNAMENT) {
                    return va("%i/%i", info->wins, info->losses);
                }
                return "";
            case 4: return va("%d", sp->score);
            case 5: return va("%d", sp->kills);
            case 6: return va("%d", sp->deaths);
            case 7: return va("%d", sp->time);
            case 8:
                if(info->botSkill) return "BOT";
                return va("%d", sp->ping);
            default:
                return "";
        }
    }

    return "";
}
const char *CG_FeederItemText_Premium (float feederID, int index, int column, qhandle_t *handle) {
   return CG_FeederItemText_NonPremium(feederID, index, column, handle);
}
