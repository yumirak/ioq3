#ifndef MISSIONPACK
#error This file not be used for classic Q3A.
#endif
#include "cg_local.h"
#include "cg_utils.h"
#include "cg_newdraw.h"
#include "cg_newownerdraw.h"

void CG_DrawHorizontalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num, qboolean lefttoright)
{
    float value;
    float s1, t1, s2, t2;
    value = Com_Clamp(0, max_num, num);
    CG_SetTeamColor(1);
    CG_AdjustFrom640( &rect->x, &rect->y, &rect->w, &rect->h );

    // drain from left to right
    if( lefttoright ) {
        s1 = 1.0 - (value / 100.0);
        t1 = 0;
        s2 = 1; t2 = 1;
        trap_R_DrawStretchPic( rect->x + rect->w * (1 - value / 100.0), rect->y, rect->w * (value / 100.0), rect->h, s1, t1, s2, t2, shader );
    } else {
        s1 = 0; t1 = 0;
        s2 = (value / 100.0);
        t2 = 1;
        trap_R_DrawStretchPic( rect->x, rect->y, rect->w * (value / 100.0), rect->h, s1, t1, s2, t2, shader );
    }
}

// The current ql image (a200.png) has spacing for more than 200 health so that needs to be taken into account.
// Also, 145 (not 150) matches ql behavior.
void CG_DrawVerticalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num)
{
    float value, max_value;
    float s1, t1, s2, t2;
    max_value = max_num * 1.45;
    value = Com_Clamp(0, max_value, num);

    CG_SetTeamColor(1);
    CG_AdjustFrom640( &rect->x, &rect->y, &rect->w, &rect->h );
    s1 = 0;
    t1 = 1.0 - (value / (float)max_value);
    s2 = 1;
    t2 = 1;
    trap_R_DrawStretchPic( rect->x, rect->y + rect->h * (1 - value / (float)max_value), rect->w, rect->h * (value / (float)max_value), s1, t1, s2, t2, shader );
}

static void CG_DrawPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle, char *s, int score )
{
    int      spacing;
    int      teamSpacing;
    int      maxNameWidth;
    char     *endOfName;

    teamSpacing = CG_IsTeamGame( cgs.gametype ) ? rect->h / 2 : 0;
    spacing = CG_Text_Width( "00", scale, 0 );

    maxNameWidth = rect->w - spacing - 1;
    endOfName = s + strlen( s );

    while( maxNameWidth > 0 && CG_Text_Width( s, scale, 0 ) > maxNameWidth ) {
        endOfName--;
        *endOfName = '\0';
    }

    // FIXME spacing for scores
    CG_Text_Paint( rect->x + teamSpacing, rect->y, scale, color, s, 0, rect->w - teamSpacing - spacing - 6, textStyle );

    s = va( "%d  ", score );
    if( score == SCORE_NOT_PRESENT ) {
        s = "";
    }

    CG_Text_Paint( rect->x + rect->w - spacing, rect->y, scale, color, s, 0, 0, textStyle );
}

void CG_Draw1stPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    char *s;
    int  score;
    int  team;

    if( !CG_IsTeamGame( cgs.gametype ) && !cgs.clientinfo[ cgs.cs[CS_CLIENTNUM1STPLAYER].integer ].infoValid )
        return;

    team = cg.snap->ps.persistant[PERS_TEAM];

    if( CG_IsTeamGame( cgs.gametype )) {
        if( cgs.scores1 > cgs.scores2 ) {
            score = cgs.scores1;
            s = va( "1.%s", CG_GetTeamName( TEAM_RED ));
        } else if( cgs.scores1 < cgs.scores2 ) {
            score = cgs.scores2;
            s = va( "1.%s", CG_GetTeamName( TEAM_BLUE ));
        } else {
            s = va( "1.%s", CG_GetTeamName( team ));
            score = cgs.scores1;
        }
    } else {
        s = va( "1.%s", cgs.cs[CS_FIRSTPLACE].string );
        score = cgs.scores1;
    }

    if( s )
        CG_DrawPlaceScore( rect, scale, color, textStyle, s, score );

}

void CG_Draw2ndPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    char *s;
    int  team;
    int  score;
    int  rank;

    if( !CG_IsTeamGame( cgs.gametype ) && !cgs.clientinfo[ cgs.cs[CS_CLIENTNUM2NDPLAYER].integer ].infoValid )
        return;

    team = cg.snap->ps.persistant[PERS_TEAM];
    rank = cg.snap->ps.persistant[PERS_RANK] & ~RANK_TIED_FLAG;
    rank++;

    if( CG_IsTeamGame( cgs.gametype )) {
        if( cgs.scores1 < cgs.scores2 ) {
            score = cgs.scores1;
            s = va( "2.%s", CG_GetTeamName( TEAM_RED ));
        } else if( cgs.scores1 > cgs.scores2 ) {
            score = cgs.scores2;
            s = va( "2.%s", CG_GetTeamName( TEAM_BLUE ));
        } else {
            s = va( "2.%s", CG_GetTeamName( team == TEAM_BLUE ? TEAM_RED : TEAM_BLUE ));
            score = cgs.scores1;
        }
    } else {
        if( cgs.scores1 == cg.snap->ps.persistant[PERS_SCORE] ) {
            if( cgs.scores2 == cgs.scores1 ) {
                s = va( "%d.%s", 1, cgs.cs[CS_SECONDPLACE].string );
            } else {
                s = va( "%d.%s", cgs.scores2 == cgs.scores1 ? 1 : 2, cgs.cs[CS_SECONDPLACE].string );
            }
            score = cgs.scores2;
        } else {
            s = va( "%d.^7%s", rank, cgs.clientinfo[cg.snap->ps.clientNum].name );
            score = cg.snap->ps.persistant[PERS_SCORE];
        }
    }

    if( s )
        CG_DrawPlaceScore( rect, scale, color, textStyle, s, score );
}

void CG_DrawObit( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    int i, w, h;
    int picWidth, picHeight, picScale;
    int spacing;
    int yOffset;
    qhandle_t icon;

    picScale = 2.0;
    spacing = 4;
    h = CG_Text_Height( "T", scale, 0 );

    for( i = 0; i < MAX_OBITUARY; i++ ) {
        if( i >= cg_obituaryRowSize.integer )
            return;

        if( cg.time - cg.obituary[i].time > cg_obituaryRowTime.integer || !strlen( cg.obituary[i].victim ) ) {
            memset(&cg.obituary[i], 0, sizeof(cg.obituary[i]));
            break;
        }

        w = CG_Text_Width( cg.obituary[i].killer, scale, 0 );

        picWidth = picHeight = h * picScale;
        yOffset = ( h * 2 ) * i;
        icon = cg.obituary[i].icon;

        if( icon < 1 )
            icon = cgs.media.worldDeathShader;

        CG_Text_Paint( rect->x, rect->y + yOffset, scale, color, cg.obituary[i].killer, 0, 0, textStyle );
        CG_DrawPic( rect->x + w + spacing, rect->y + yOffset - h - ( picHeight - h ) / 2, picWidth, picHeight, icon );
        CG_Text_Paint( rect->x + w + spacing + picWidth + spacing, rect->y + yOffset, scale, color, cg.obituary[i].victim, 0, 0, textStyle );
    }
}

void CG_DrawAreaNewChat( rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    int i;
    int yOffset;
    int count = 0;
    float textscale = 0.2f;
    int h = CG_Text_Height("T", textscale, 0);

    for( i = 0; i < MAX_CHAT_LINES; i++ ) {
        if (!cg.scoreBoardShowing) {
            if( !cg.forceDrawChat  && cg.time - cg.chatArea[i].time > 5000 )
                break;
        }

        yOffset = ( h * 2 ) * i;
        CG_Text_Paint( rect->x, rect->y + rect->h - yOffset, textscale, color, cg.chatArea[i].message, 0, 0, ITEM_TEXTSTYLE_SHADOWED /* textStyle */ );
        count++;
    }

    cg.numChatLinesVisible = count;
}

void CG_DrawMatchWinnerString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	vec4_t ourColor;
	float x;
	char *s;

	Vector4Copy(colorWhite, ourColor);
	// 2018-07-13 only used in end_scoreboard* but ql shows 'player leads with a score of 2' if it isn't match end
	// 2018-07-12 ql uses forecolor to set the player name but the rest of the message is forced to white
	// 2018-09-25 if name is large uses '...'

	if (!CG_IsTeamGame(cgs.gametype)) {
		const char *playerName = "";
		char endText[128];
		const char *fullText = "";
		float w;

		playerName = cgs.cs[CS_FIRSTPLACE].string;

		if (cg.snap->ps.pm_type == PM_INTERMISSION) {
			Q_strncpyz(endText, " WINS", sizeof(endText));
		} else {
			Com_sprintf(endText, sizeof(endText), " leads with a score of %d", cgs.scores1);
		}

		fullText = va("%s ^7%s", playerName, endText);
		w = CG_Text_Width(fullText, scale, 0);

		switch(align) {
			case ITEM_ALIGN_CENTER: x = rect->x - (w / 2); break;
			case ITEM_ALIGN_RIGHT : x = rect->x - w; break;
			default: x = rect->x; break;
		}

		// we have to split text painting since the first part (player name) might be using a different alpha value for color
		CG_Text_Paint(x, rect->y, scale, color, playerName, 0, 0, textStyle);
		x += CG_Text_Width(playerName, scale, 0);
		CG_Text_Paint(x, rect->y, scale, ourColor, endText, 0, 0, textStyle);
	} else {  // team game
		ourColor[3] = color[3];
		if (CG_ScoresEqual(cgs.scores1, cgs.scores2)) {  // shouldn't happen during intermission
			s = va("Teams are tied with a score of %d", cgs.scores1);
		} else if (cgs.scores1 > cgs.scores2) {
			if (cg.snap->ps.pm_type == PM_INTERMISSION) {
				s = va("^1Red Team ^7WINS by a score of %d to %d", cgs.scores1, cgs.scores2);
			} else {
				s = va("^1Red Team ^7leads with a score of %d", cgs.scores1);
			}
		} else {
			if (cg.snap->ps.pm_type == PM_INTERMISSION) {
				s = va("^4Blue Team ^7WINS by a score of %d to %d", cgs.scores2, cgs.scores1);
			} else {
				s = va("^4Blue Team ^7leads with a score of %d", cgs.scores2);
			}
		}

		CG_Text_Paint_Align(rect, scale, color, s, 0, 0, textStyle, align);
	}
}

void CG_MatchEndCondition( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	char *s = "Highest score at the end of the game";
	if (cgs.gametype == GT_CTF) {
		s = "First to reach the mercy limit";
	} else if (cgs.gametype == GT_CLAN_ARENA) {
		s = "First to reach the round limit";
	} else if (cgs.gametype == GT_CTFS  ||  cgs.gametype == GT_DOMINATION) {
		s = "First to reach the score limit";
	}

	CG_Text_Paint_Align(rect, scale, color, s, 0, 0, textStyle, align);
}

void CG_EndGameScoreString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	// chopping off color code like ql
	if (cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR  &&  cgs.clientinfo[cg.clientNum].infoValid) {
		int i;
		const score_t *sc;
		int assists = 0;
		int captures = 0;
		int defends = 0;
		char placeString[32];
		char *s;
		char *fullStr;

		for (i = 0;  i < cg.numScores;  i++) {
			sc = &cg.scores[i];
			if (sc->client == cg.snap->ps.clientNum) {
				assists = sc->assistCount;
				captures = sc->captures;
				defends = sc->defendCount;
				break;
			}
		}

		// 2018-07-28 ignore RANK_TIED_FLAG to match quake live
		//s = CG_PlaceString((cg.snap->ps.persistant[PERS_RANK] &= ~RANK_TIED_FLAG) + 1);
		Q_strncpyz(placeString, CG_PlaceString((cg.snap->ps.persistant[PERS_RANK] &= ~RANK_TIED_FLAG) + 1), sizeof(placeString));
		s = placeString;

		// ignore colorized '1st', '2nd', etc.
		if (s[0] == '^') {
			s += 2;
		}
		if (strlen(s) > 1) {
			if (s[strlen(s) - 2] == '^') {
				s[strlen(s) - 2] = '\0';
			}
		}

		if ( CG_IsTeamGame(cgs.gametype) && cgs.gametype != GT_TEAM ) {  //FIXME OBELISK like quakelive -- even if wrong
			if (captures) {
				if (cgs.gametype == GT_HARVESTER) {
					fullStr = va("You captured %d skull%s", captures, captures == 1 ? "." : "s.");
				} else {
					fullStr = va("You had %d flag capture%s", captures, captures == 1 ? "." : "s.");
				}
			} else if (assists) {
				fullStr = va("You had %d assist%s", assists, assists == 1 ? "." : "s.");
			} else if (defends) {
				fullStr = va("You had %d defend%s", defends, defends == 1 ? "." : "s.");
			} else {
				fullStr = va("You finished with a score of %d.", cg.snap->ps.persistant[PERS_SCORE]);
			}
		} else {  // other gametypes
			fullStr = va("You finished %s with a score of %d.", s, cg.snap->ps.persistant[PERS_SCORE]);
		}

		CG_Text_Paint_Align(rect, scale, color, fullStr, 0, 0, textStyle, align);
	}
}
void CG_PlayerCountsString( int team, int type, rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	int count = 0;
	int maxPlayers = cgs.maxclients;
	char *s;

	//FIXME don't do it every time
	count = CG_GetPlayerCount( team );
	switch(type)
	{
		case 1:
			s = va("%d/%d Players", count, maxPlayers);
			break;
		case 2:
			s = va("%d Players", count);
			break;
		case 3:
			s = va("(%d/%d)", count, maxPlayers);
			break;
		case 4:
			s = va("(%d)", count);
			break;
		default:
			return;
	}

	CG_Text_Paint_Align(rect, scale, color, s, 0, 0, textStyle, align);
}

void CG_GameLimitString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	// 2018-07-06 depends on game type (Round Limit, Frag Limit, Cap Limit, ...
	// 2018-07-06 ql only supports left and center align, missing right align is probably a bug
	char *s;
	int type;

	if (align == ITEM_ALIGN_RIGHT) {
		align = ITEM_ALIGN_LEFT;
	}

	if (cgs.fraglimit > 0)
		type = 1;
	if (cgs.gametype == GT_CTF  ||  cgs.gametype == GT_1FCTF  ||  cgs.gametype == GT_HARVESTER)
		type = 2;

	switch(type)
	{
		case 1:
			s = va("Frag Limit: %d", cgs.fraglimit);
			break;
		case 2:
			s = va("Cap Limit: %d", cgs.capturelimit);
			break;
		default:
			s = va("Time Limit: %d", cgs.timelimit);
			break;
	}

	CG_Text_Paint_Align(rect, scale, color, s, 0, 0, textStyle, align);
}

const char* GetMatchState( void )
{
	const char *state = "MATCH IN PROGRESS";

	if (cg.warmup) {
		state = "MATCH WARMUP";
	} else if (cg.snap->ps.pm_type == PM_INTERMISSION) {
		state = "MATCH SUMMARY";
	}

	return state;
}

void CG_MatchStateString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	CG_Text_Paint_Align(rect, scale, color, GetMatchState(), 0, 0, textStyle, align);
}

void CG_Match_Details( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	const char *s = va("%s - %s", GetMatchState(), CG_ConfigString(CS_MESSAGE));
	CG_Text_Paint_Align(rect, scale, color, s, 0, 0, textStyle, align);
}

void CG_ServerOwnerString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	const char *ownerName = "Quake Live";
	// ownerName = Info_ValueForKey(CG_ConfigString(CS_SERVERINFO), "sv_owner");
	CG_Text_Paint_Align(rect, scale, color, ownerName, 0, 0, textStyle, align);
}

void CG_PlayerBestWeaponName( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	int weapon;
	gitem_t *item;

	if( cg.selectedScore < 0 || cg.selectedScore > MAX_CLIENTS )
		return;

	weapon = Com_Clamp( WP_MACHINEGUN, WP_NUM_WEAPONS, cg.scores[cg.selectedScore].bestWeapon );
	item = BG_FindItemForWeapon( weapon );
	if( !item )
		return;

	CG_Text_Paint_Align( rect, scale, color, item->pickup_name, 0, 0, textStyle, align );
}

void CG_SelectedPlayerAccuracy( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	if( cg.selectedScore < 0 || cg.selectedScore > MAX_CLIENTS )
		return;

	CG_Text_Paint( rect->x, rect->y, scale, color, va( "%d%%", cg.scores[cg.selectedScore].accuracy ), 0, 0, textStyle );
	// CG_Text_Paint_Align( rect, scale, color, va( "%d%%", cg.scores[cg.selectedScore].accuracy ), 0, 0, textStyle, align );
}


void CG_DrawFollowNameString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align, qboolean drawfollowing )
{
	char *s = "";
	char *full;

	if( CG_IsTeamGame( cgs.gametype ) )
		s = va(" (^1%s^7)", CG_GetTeamName( cgs.clientinfo[cg.snap->ps.clientNum].team ));

	full = va("%s%s%s", drawfollowing ? "Following - " : "", cgs.clientinfo[cg.snap->ps.clientNum].name, s );

	CG_Text_Paint_Align(rect, scale, color, full, 0, 0, textStyle, align);
}

void CG_Draw1stPlacePlayerModel (float x, float y, float w, float h)
{
	refdef_t refdef;
	refEntity_t legs, torso, head;
	refEntity_t gun, barrel;
	vec3_t origin;
	int renderfx;
	float len;
	float xx;
	clientInfo_t *ci;
	int weaponNum;
	const weaponInfo_t *weapon;
	float xscale, yscale;
	int torsoAnim;
	int clientNum;
	int i;
	vec3_t legsAngles, torsoAngles, headAngles;

	memset(&refdef, 0, sizeof(refdef));
	memset(&legs, 0, sizeof(legs));
	memset(&torso, 0, sizeof(torso));
	memset(&head, 0, sizeof(head));

	clientNum = cg.snap->ps.clientNum;

	weaponNum = WP_MACHINEGUN;

	clientNum = cgs.cs[CS_CLIENTNUM1STPLAYER].integer;
	ci = &cgs.clientinfo[clientNum];

	// CG_CheckForModelChange(&cg_entities[clientNum], ci, &legs, &torso, &head);

	legs.hModel = ci->legsModel;
	legs.customSkin = ci->legsSkin;

	torso.hModel = ci->torsoModel;
	torso.customSkin = ci->torsoSkin;

	head.hModel = ci->headModel;
	head.customSkin = ci->headSkin;

	for (i = 0;  i < cg.numScores;  i++) {
		if (cg.scores[i].client == clientNum) {
			weaponNum = cg.scores[i].bestWeapon;
			break;
		}
	}

	if (weaponNum == WP_NONE  ||  weaponNum == WP_GAUNTLET) {
		torsoAnim = TORSO_ATTACK2;
	} else {
		torsoAnim = TORSO_ATTACK;
	}

	CG_AdjustFrom640(&x, &y, &w, &h);

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear(refdef.viewaxis);

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	// 2018-08-03  match quake live and use fixed values to prevent x or y stretching with change of screen dimensions, taking values calculated from screensize 1365 x 768
	xscale = 1365.0 / 640.0;
	// 2018-08-05 make a little bigger to widden models a bit
	xscale *= 1.05;
	yscale = 768.0 / 480.0;

	refdef.fov_x = (int)((float)refdef.width / xscale / 640.0f * 90.0f);
	xx = refdef.width / xscale / tan( refdef.fov_x / 360 * M_PI );
	refdef.fov_y = atan2( refdef.height / yscale, xx );
	refdef.fov_y *= ( 360 / M_PI );

	// 2018-08-03 match quake live, based on fixed values at 1365 x 768
	refdef.fov_y *= 0.7;

	// calculate distance so the player nearly fills the box
	len = 0.7 * ( ci->playerModelHeight );  // 2018-08-13 tested with sarge, xaero, keel, but orbb a little bigger compared to quake live

	// 2018-08-03 match quake live, based on fixed values at 1365 x 768
	len *= 2.0;
	len *= 0.78;

	origin[0] = len / tan( DEG2RAD(refdef.fov_x) * 0.5 );
	origin[1] = 0.5 * ( bg_playerMins[1] + bg_playerMaxs[1] );
	origin[2] = -0.5 * ( bg_playerMins[2] + bg_playerMaxs[2] );

	// 2018-08-05 match quake live
	origin[2] -= 3;

	refdef.time = cg.time;

	trap_R_ClearScene();

	headAngles[YAW] = 0;
	headAngles[PITCH] = 0;
	headAngles[ROLL] = 0;

	if (weaponNum == WP_NONE  ||  weaponNum == WP_GAUNTLET) {
		torsoAngles[YAW] = 0;
		torsoAngles[PITCH] = -10;
		torsoAngles[ROLL] = 0;
	} else {
		torsoAngles[YAW] = -5;
		torsoAngles[PITCH] = -10;
		torsoAngles[ROLL] = 0;
	}

	legsAngles[YAW] = 160;
	legsAngles[PITCH] = 10;
	legsAngles[ROLL] = 0;

	AnglesToAxis( legsAngles, legs.axis );
	AnglesToAxis( torsoAngles, torso.axis );
	AnglesToAxis( headAngles, head.axis );

	legs.oldframe = legs.frame = ci->animations[LEGS_IDLE].firstFrame + 0;
	torso.oldframe = torso.frame = ci->animations[torsoAnim].firstFrame + 0;

	renderfx = RF_NOSHADOW; // RF_LIGHTING_ORIGIN | RF_MINLIGHT

	// add the legs
	VectorCopy(origin, legs.origin);
	VectorCopy(origin, legs.lightingOrigin);
	legs.renderfx = renderfx;
	VectorCopy(legs.origin, legs.oldorigin);

	trap_R_AddRefEntityToScene(&legs);

	// if the model failed, allow the default nullmodel to be displayed
	if (!legs.hModel) {
		return;
	}

	// add the torso
	if (!torso.hModel) {
		return;
	}

	VectorCopy(origin, torso.lightingOrigin);
	CG_PositionRotatedEntityOnTag(&torso, &legs, ci->legsModel, "tag_torso");
	torso.renderfx = renderfx;

	trap_R_AddRefEntityToScene(&torso);

	// add the head
	if (!head.hModel) {
		return;
	}

	VectorCopy(origin, head.lightingOrigin);

	CG_PositionRotatedEntityOnTag(&head, &torso, ci->torsoModel, "tag_head");

	head.renderfx = renderfx;

	trap_R_AddRefEntityToScene(&head);

	// add the gun
	CG_RegisterWeapon(weaponNum);
	weapon = &cg_weapons[weaponNum];

	if (weaponNum != WP_NONE) {
		memset(&gun, 0, sizeof(gun));
		gun.hModel = weapon->weaponModel;
		//FIXME railgun shader color
		gun.shaderRGBA[0] = 255;
		gun.shaderRGBA[1] = 255;
		gun.shaderRGBA[2] = 255;
		gun.shaderRGBA[3] = 255;

		VectorCopy(origin, gun.origin);
		VectorCopy(origin, gun.lightingOrigin);

		CG_PositionEntityOnTag(&gun, &torso, ci->torsoModel, "tag_weapon");
		gun.renderfx = renderfx;
		trap_R_AddRefEntityToScene(&gun);
	}

	// add the spinning barrel
	if (weapon->barrelModel) {
		vec3_t angles;

		memset(&barrel, 0, sizeof(barrel));
		VectorCopy(origin, barrel.lightingOrigin);
		barrel.renderfx = renderfx;

		barrel.hModel = weapon->barrelModel;
		angles[YAW] = 0;
		angles[PITCH] = 0;
		angles[ROLL] = 60;
		AnglesToAxis(angles, barrel.axis);

		CG_PositionRotatedEntityOnTag(&barrel, &gun, weapon->weaponModel, "tag_barrel");

		trap_R_AddRefEntityToScene(&barrel);
	}

	// add an accent light
	origin[0] -= 100;       // + = behind, - = in front
	origin[1] += 100;       // + = left, - = right
	origin[2] += 100;       // + = above, - = below

	trap_R_AddLightToScene( origin, 400, 1.0, 1.0, 1.0 );  // 500

	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;

	trap_R_AddLightToScene( origin, 500, 1.0, 0.0, 0.0 );

	trap_R_RenderScene(&refdef);
}

void CG_DrawWeaponVertical( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	int i;
	int offset = 0;

	for (i = WP_MACHINEGUN;  i < WP_NUM_WEAPONS;  i++) {
		if (!cg_weapons[i].registered)
			continue;

		CG_DrawPic(rect->x, rect->y + offset, rect->w * 1.0, rect->w * 1.0, cg_weapons[i].weaponIcon);
		offset += rect->h;
	}
}

void CG_DrawAccuracyVertical( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	int i, acc;
	int offset = 0;
	rectDef_t textRect;

	memcpy(&textRect, rect, sizeof(textRect));

	for (i = WP_MACHINEGUN;  i < WP_NUM_WEAPONS;  i++) {
		if (!cg_weapons[i].registered)
			continue;

		acc = cg.wpstats[i].accuracy[WP_ACC_PERCENT];
		textRect.y = rect->y + offset;
		CG_Text_Paint_Align( &textRect, scale, color, va("%d%%", acc), 0, 0, textStyle, align);
		offset += rect->h;
	}
}

void CG_DrawOvertime( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align )
{
	const char *s;
	int numOverTimes = cg.overtime.count;

	if ( cg.snap->ps.pm_type == PM_INTERMISSION || numOverTimes <= 0 ) {
		return;
	}

	s = numOverTimes > 1 ? va("Overtime x%d", numOverTimes) : "Overtime";

	CG_Text_Paint_Align(rect, scale, color, s, 0, 0, textStyle, align);
}

void CG_DrawPlayerKey( rectDef_t *rect )
{
	int value;

	if ( !cg.snap->ps.stats[STAT_MAP_KEYS] )
		return;

	value = cg.snap->ps.stats[STAT_PERSISTANT_POWERUP];
	if ( value ) {
		CG_RegisterItemVisuals( value );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].icon );
	}
}
