#ifndef BASEQZ
#error This file only used for QL.
#endif

#include "cg_local.h"
#include "cg_utils.h"
#include "../ui/ui_shared.h"

enum {
    SBC_NAME,
    SBC_SCORE,
    SBC_KILL,
    SBC_DEATH,
    SBC_KILLDEATH,
    SBC_TIME,
    SBC_PING,
    SBC_WINLOSE,
    SBC_WEAPON,
    SBC_WEAPON_ACC,
    SBC_DAMAGE,
    SBC_NETKD,
    SBC_FLAG_CAPTURE,
    SBC_FLAG_ASSIST,
    SBC_FLAG_DEFENSE,
    SBC_COUNTRY,
    SBC_PREMIUM,
    SBC_READY,
    SBC_MODELICON,
    SBC_NUM_MAX
};

static int cg_scoreboard_column_default[2][SBC_NUM_MAX] = {
//  NAME   S   K   D  KD TIMEPING  WL  WP WPA DMG NET CAP  A   D   C   P  R ICON
#if BASEQZ >= 730 // FFA / Duel
    {  3,  5,  6,  7, -1,  8,  9,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1,  2,  0 },
#else
    {  2,  4,  5,  6, -1,  7,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  0 },
#endif

#if BASEQZ >= 730 // Team
    {  3,  4,  5,  6, -1, -1,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  2,  0 },
#else
    {  2,  3,  4,  5, -1, -1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  0 },
#endif

};

static int cg_scoreboard_column_premium[3][SBC_NUM_MAX] = {
//  NAME   S   K   D  KD TIMEPING  WL  WP WPA DMG NET CAP  A   D   C   P  R ICON
#if BASEQZ >= 1069 // FFA - 0
    {  5,  7, -1, -1,  8, 12, 13, -1, 10, 11,  9, -1, -1, -1, -1,  4,  2,  3,  0 },
#elif BASEQZ >= 934
    {  4,  6,  7,  8, -1,  9, 10, -1, -1, -1, -1, -1, -1, -1, -1,  4,  2,  3,  0 },
#elif BASEQZ >= 730
    {  3,  5,  6,  7, -1,  8,  9, -1, -1, -1, -1, -1, -1, -1, -1,  3,  1,  2,  0 },
#else
    {  2,  4,  5,  6, -1,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  0 },
#endif

#if BASEQZ >= 730 // Team - 1
    {  5,  6, -1, -1, -1, -1,  9, -1, -1, -1,  8,  7, -1, -1, -1, -1,  1,  3,  0 },
#else
    {  4,  5, -1, -1, -1, -1,  8, -1, -1, -1,  7,  6, -1, -1, -1, -1,  1,  2,  0 }, // -1 from 730
#endif

#if BASEQZ >= 730 // CTF - 2
    {  5,  6, -1, -1,  7, -1, 11, -1, -1, -1, -1, -1,  8,  9, 10, -1,  2,  4,  0 },
#else
    {  4,  5, -1, -1,  6, -1, 10, -1, -1, -1, -1, -1,  7,  8,  9, -1,  1,  3,  0 }, // -1 from 730
#endif
};

static const char *Get_GametypeColumnValue ( clientInfo_t *info, score_t *sp, teamscores_t *tsp, int findcolumn, int column, qhandle_t *handle ) {
    int n;
    qboolean b;
    int resultcolumn;
    int table;
    const char *s = "";

    if( cg.scoreboardPremium ) {
        switch ( cgs.gametype ) {
            case GT_FFA:
            case GT_SINGLE_PLAYER:
            case GT_TOURNAMENT:
                table = 0; break;
            case GT_CTF:
            case GT_1FCTF:
                table = 2; break;
            default: table = 1; break;
        }
        resultcolumn = cg_scoreboard_column_premium[table][findcolumn];
    } else {
        table = cgs.gametype >= GT_TEAM ? 1 : 0;
        resultcolumn = cg_scoreboard_column_default[table][findcolumn];
    }

    if ( resultcolumn != column ) {
        return s;
    }

    switch ( findcolumn ) {
        case SBC_NAME: s = va("%s", info->name); break;
        case SBC_SCORE: s = va("%d", sp->score); break;
        case SBC_KILL: s = va("%d", sp->kills); break;
        case SBC_DEATH: s = va("%d", sp->deaths); break;
        case SBC_KILLDEATH: s = va("%d/%d", sp->kills, sp->deaths); break;
        case SBC_TIME: s = va("%d", sp->time); break;
        case SBC_PING: s = va("%d", sp->ping); break;
        case SBC_WEAPON: *handle = cg_weapons[sp->bestWeapon].weaponIcon; break;
        case SBC_WEAPON_ACC: s = va("%d%%", sp->bestWeaponAcc); break;
        case SBC_NETKD: s = va("%d", sp->kills - sp->deaths); break;
        case SBC_FLAG_CAPTURE: s = va("%d", sp->captures); break;
        case SBC_FLAG_ASSIST: s = va("%d", sp->assistCount); break;
        case SBC_FLAG_DEFENSE: s = va("%d", sp->defendCount); break;
        case SBC_MODELICON: *handle = cgs.clientinfo[sp->client].modelIcon; break;
        case SBC_DAMAGE:
            n = sp->damageGiven;
            if ( n > 0 ) {
                if (n >= 1000) {
                    s = va("%.1fk", (float)n / 1000.0);
                } else {
                    s = va("%d", n);
                }
            }
            break;
        case SBC_WINLOSE:
            if(cgs.gametype == GT_TOURNAMENT)
                s = va("%d/%d", info->wins, info->losses);
            break;
        case SBC_READY:
            b = info->ready;
            if ( cg.warmup || cg.snap->ps.pm_type == PM_INTERMISSION ) {
                if (b) {
                    *handle = trap_R_RegisterShader("ui/assets/score/arrowg");
                } else {
                    *handle = trap_R_RegisterShader("ui/assets/score/arrowr");
                }
            }
            break;
        default: break;
    }
    // return va("%d", column); // debug
    return s;
}

const char *CG_ScoreboardFeederItemText (float feederID, int index, int column, qhandle_t *handle) {
    int scoreIndex = 0;
    clientInfo_t *info = NULL;
    int team = -1;
    score_t *sp = NULL;
    teamscores_t *tsp = NULL;
    const char *s = "";
    int i;

    *handle = -1;

    switch ( (int)feederID ) {
        case FEEDER_REDTEAM_LIST: team = TEAM_RED; break;
        case FEEDER_BLUETEAM_LIST: team = TEAM_BLUE; break;
        case FEEDER_SCOREBOARD: team = TEAM_FREE; break;
        default: return "";
    }

    info = CG_InfoFromScoreIndex(index, team, &scoreIndex);
    sp = &cg.scores[scoreIndex];
    tsp = &cg.teamscore[team];

    if ( !info || !info->infoValid )
        return "";

    for ( i = 0; i < SBC_NUM_MAX; i++ ) {
        s = Get_GametypeColumnValue( info, sp, tsp, i, column, handle );
        if ( strlen( s ) > 0 )
            return s;
    }

    return s;
}
