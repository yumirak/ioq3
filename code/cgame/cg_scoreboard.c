/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"


#define	SCOREBOARD_X		(0)

#define SB_HEADER			86
#define SB_TOP				(SB_HEADER+32)

// Where the status bar starts, so we don't overwrite it
#define SB_STATUSBAR		420

#define SB_NORMAL_HEIGHT	40
#define SB_INTER_HEIGHT		16 // interleaved height

#define SB_MAXCLIENTS_NORMAL  ((SB_STATUSBAR - SB_TOP) / SB_NORMAL_HEIGHT)
#define SB_MAXCLIENTS_INTER   ((SB_STATUSBAR - SB_TOP) / SB_INTER_HEIGHT - 1)

// Used when interleaved



#define SB_LEFT_BOTICON_X	(SCOREBOARD_X+0)
#define SB_LEFT_HEAD_X		(SCOREBOARD_X+32)
#define SB_RIGHT_BOTICON_X	(SCOREBOARD_X+64)
#define SB_RIGHT_HEAD_X		(SCOREBOARD_X+96)
// Normal
#define SB_BOTICON_X		(SCOREBOARD_X+32)
#define SB_HEAD_X			(SCOREBOARD_X+64)

#define SB_SCORELINE_X		112

#define SB_RATING_WIDTH	    (6 * BIGCHAR_WIDTH) // width 6
#define SB_SCORE_X			(SB_SCORELINE_X + BIGCHAR_WIDTH) // width 6
#define SB_RATING_X			(SB_SCORELINE_X + 6 * BIGCHAR_WIDTH) // width 6
#define SB_PING_X			(SB_SCORELINE_X + 12 * BIGCHAR_WIDTH + 8) // width 5
#define SB_TIME_X			(SB_SCORELINE_X + 17 * BIGCHAR_WIDTH + 8) // width 5
#define SB_NAME_X			(SB_SCORELINE_X + 22 * BIGCHAR_WIDTH) // width 15

// The new and improved score board
//
// In cases where the number of clients is high, the score board heads are interleaved
// here's the layout

//
//	0   32   80  112  144   240  320  400   <-- pixel position
//  bot head bot head score ping time name
//  
//  wins/losses are drawn on bot icon now

static qboolean localClient; // true if local client has been displayed


							 /*
=================
CG_DrawScoreboard
=================
*/
static void CG_DrawClientScore( int y, score_t *score, float *color, float fade ) {
	char	string_score[5];
	char	string_acc[5];
	char	string_ping[5];
	char	string_time[5];
	char	string_name[128];
	char	string_wl[8];
	vec3_t	headAngles;
	clientInfo_t	*ci;
	int iconx, headx;

	if ( score->client < 0 || score->client >= cgs.maxclients ) {
		Com_Printf( "Bad score->client: %i\n", score->client );
		return;
	}
	
	ci = &cgs.clientinfo[score->client];

	iconx = SB_BOTICON_X + (SB_RATING_WIDTH / 2);
	headx = SB_HEAD_X + (SB_RATING_WIDTH / 2);


	if ( ci->botSkill > 0 && ci->botSkill <= 5 ) {
		if ( cg_drawIcons.integer ) {
				CG_DrawPic( iconx, y, 16, 16, cgs.media.botSkillShaders[ ci->botSkill - 1 ] );
		}
	}
	// draw the wins / losses
	if ( cgs.gametype == GT_TOURNAMENT ) {
		Com_sprintf( string_wl, sizeof( string_wl ), "%i/%i", ci->wins, ci->losses );
		CG_DrawSmallStringColor( iconx, y, string_wl, color );
	}

	// draw the face
	VectorClear( headAngles );
	headAngles[YAW] = 180;
	CG_DrawHead( headx, y, 16, 16, score->client, headAngles );

#ifdef MISSIONPACK
	// draw the team task
	if ( ci->teamTask != TEAMTASK_NONE ) {
		if ( ci->teamTask == TEAMTASK_OFFENSE ) {
			CG_DrawPic( headx + 48, y, 16, 16, cgs.media.assaultShader );
		}
		else if ( ci->teamTask == TEAMTASK_DEFENSE ) {
			CG_DrawPic( headx + 48, y, 16, 16, cgs.media.defendShader );
		}
	}
#endif
	// draw the score line
	switch(ci->team)
	{
		case TEAM_SPECTATOR:
			Com_sprintf(string_ping, sizeof(string_ping),
				"%i", score->ping);
			Com_sprintf(string_name, sizeof(string_name),
				"%s(s)", ci->name);
			break;
		default:
			Com_sprintf(string_score, sizeof(string_score),
				"%i", score->score);
			Com_sprintf(string_acc, sizeof(string_acc),
				"%i%%", score->accuracy);
			Com_sprintf(string_ping, sizeof(string_ping),
				"%i", score->ping);
			Com_sprintf(string_time, sizeof(string_time),
				"%i", score->time);
			Com_sprintf(string_name, sizeof(string_name),
				"%s", ci->name);
			break;
	}

	if ( score->ping == -1 ) {
		Com_sprintf(string_name, sizeof(string_name),
			" connecting    %s", ci->name);
	}

	// highlight your position
	if ( score->client == cg.snap->ps.clientNum ) {
		float	hcolor[4];
		int		rank;

		localClient = qtrue;

		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR 
			|| cgs.gametype >= GT_TEAM ) {
			rank = -1;
		} else {
			rank = cg.snap->ps.persistant[PERS_RANK] & ~RANK_TIED_FLAG;
		}
		switch(rank)
		{
			case 0:
				hcolor[0] = 0;
				hcolor[1] = 0;
				hcolor[2] = 0.7f;
				break;
			case 1:
				hcolor[0] = 0.7f;
				hcolor[1] = 0;
				hcolor[2] = 0;
				break;
			case 2:
				hcolor[0] = 0.7f;
				hcolor[1] = 0.7f;
				hcolor[2] = 0;
				break;
			default:
				hcolor[0] = 0.7f;
				hcolor[1] = 0.7f;
				hcolor[2] = 0;
				break;
		}

		hcolor[3] = fade * 0.7;
		CG_FillRect( SB_SCORELINE_X +  (SB_RATING_WIDTH / 2), y,
			640 - SB_SCORELINE_X - BIGCHAR_WIDTH - (SB_RATING_WIDTH/2),
			BIGCHAR_HEIGHT+1, hcolor );
	}

	//score_str = va("%d",cg.snap->ps.persistant[PERS_SCORE]);
	CG_DrawString( SB_SCORELINE_X + 30 + 10 + (SB_RATING_WIDTH / 2), y, string_score, UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 70 + (SB_RATING_WIDTH / 2), y, string_acc, UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 120 + (SB_RATING_WIDTH / 2), y, string_ping, UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 170 + (SB_RATING_WIDTH / 2), y, string_time, UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 200 + (SB_RATING_WIDTH / 2), y, string_name, UI_DROPSHADOW| UI_BIGFONT,colorWhite );

	// add the "ready" marker for intermission exiting
	if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << score->client ) ) {
		CG_DrawBigStringColor( iconx, y, "READY", color );
	}
}

/*
=================
CG_TeamScoreboard
=================
*/
static int CG_TeamScoreboard( int y, team_t team, float fade, int maxClients, int lineHeight ) {
	int		i;
	score_t	*score;
	float	color[4];
	int		count;
	clientInfo_t	*ci;

	color[0] = color[1] = color[2] = 1.0;
	color[3] = fade;

	count = 0;
	for ( i = 0 ; i < cg.numScores && count < maxClients ; i++ ) {
		score = &cg.scores[i];
		ci = &cgs.clientinfo[ score->client ];

		if ( team != ci->team ) {
			continue;
		}

		CG_DrawClientScore( y + lineHeight * count, score, color, fade );

		count++;
	}

	return count;
}

/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawOldScoreboard( void ) {
	int		x, y, w, i, n1, n2;
	float	fade;
	float	*fadeColor;
	char	*s;
	int maxClients;
	int lineHeight;
	int topBorderSize, bottomBorderSize;

	// don't draw amuthing if the menu or console is up
	if ( cg_paused.integer ) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	if ( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if ( cg.warmup && !cg.showScores ) {
		return qfalse;
	}

	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
		 cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
		
		if ( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return qfalse;
		}
		fade = *fadeColor;
	}


	// fragged by ... line
	if ( cg.killerName[0] ) {
		s = va("Fragged by %s", cg.killerName );
		w = CG_DrawStrlen( s , UI_BIGFONT);

		x = ( SCREEN_WIDTH - w ) / 2;
		y = 40;
		CG_DrawBigString( x, y, s, fade );
	}

	// current rank
	if ( cgs.gametype < GT_TEAM) {
		if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
			s = va("%s place with %i",
				CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),
				cg.snap->ps.persistant[PERS_SCORE] );
			w = CG_DrawStrlen( s , UI_BIGFONT);

			x = ( SCREEN_WIDTH - w ) / 2;
			y = 60;
			CG_DrawBigString( x, y, s, fade );
		}
	} else {
		if ( cg.teamScores[0] == cg.teamScores[1] ) {
			s = va("Teams are tied at %i", cg.teamScores[0] );
		} else if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			s = va("Red leads %i to %i",cg.teamScores[0], cg.teamScores[1] );
		} else {
			s = va("Blue leads %i to %i",cg.teamScores[1], cg.teamScores[0] );
		}

		w = CG_DrawStrlen( s , UI_BIGFONT);

		x = ( SCREEN_WIDTH - w ) / 2;
		y = 60;
		CG_DrawBigString( x, y, s, fade );
	}

	// scoreboard
	y = SB_HEADER;

	CG_DrawString( SB_SCORELINE_X + 30 + 10 + (SB_RATING_WIDTH / 2), y, "Score", UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 70 + (SB_RATING_WIDTH / 2), y, "Acc", UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 120 + (SB_RATING_WIDTH / 2), y, "Ping", UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 170 + (SB_RATING_WIDTH / 2), y, "Time", UI_RIGHT | UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	CG_DrawString( SB_SCORELINE_X + 30 + 200 + (SB_RATING_WIDTH / 2), y, "Name", UI_DROPSHADOW| UI_BIGFONT,colorWhite );
	y = SB_TOP;

	maxClients = SB_MAXCLIENTS_INTER;
	lineHeight = SB_INTER_HEIGHT;
	topBorderSize = 8;
	bottomBorderSize = 16;

	localClient = qfalse;

	if ( cgs.gametype >= GT_TEAM ) {
		//
		// teamplay scoreboard
		//
		y += lineHeight/2;

		if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			n1 = CG_TeamScoreboard( y, TEAM_RED, fade, maxClients, lineHeight );
			CG_DrawTeamBackground( 0, y - topBorderSize, 640, n1 * lineHeight + bottomBorderSize, 0.33f, TEAM_RED );
			y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n1;
			n2 = CG_TeamScoreboard( y, TEAM_BLUE, fade, maxClients, lineHeight );
			CG_DrawTeamBackground( 0, y - topBorderSize, 640, n2 * lineHeight + bottomBorderSize, 0.33f, TEAM_BLUE );
			y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n2;
		} else {
			n1 = CG_TeamScoreboard( y, TEAM_BLUE, fade, maxClients, lineHeight );
			CG_DrawTeamBackground( 0, y - topBorderSize, 640, n1 * lineHeight + bottomBorderSize, 0.33f, TEAM_BLUE );
			y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n1;
			n2 = CG_TeamScoreboard( y, TEAM_RED, fade, maxClients, lineHeight );
			CG_DrawTeamBackground( 0, y - topBorderSize, 640, n2 * lineHeight + bottomBorderSize, 0.33f, TEAM_RED );
			y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n2;
		}
		n1 = CG_TeamScoreboard( y, TEAM_SPECTATOR, fade, maxClients, lineHeight );
		y += (n1 * lineHeight) + BIGCHAR_HEIGHT;

	} else {
		//
		// free for all scoreboard
		//
		n1 = CG_TeamScoreboard( y, TEAM_FREE, fade, maxClients, lineHeight );
		y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
		n2 = CG_TeamScoreboard( y, TEAM_SPECTATOR, fade, maxClients - n1, lineHeight );
		y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
	}

	if (!localClient) {
		// draw local client at the bottom
		for ( i = 0 ; i < cg.numScores ; i++ ) {
			if ( cg.scores[i].client == cg.snap->ps.clientNum ) {
				CG_DrawClientScore( y, &cg.scores[i], fadeColor, fade );
				break;
			}
		}
	}

	// load any models that have been deferred
	if ( ++cg.deferredPlayerLoading > 10 ) {
		CG_LoadDeferredPlayers();
	}

	return qtrue;
}

//================================================================================

/*
================
CG_CenterGiantLine
================

static void CG_CenterGiantLine( float y, const char *string ) {
	float		x;
	vec4_t		color;

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	x = 0.5 * ( 640 - GIANT_WIDTH * CG_DrawStrlen( string , UI_GIANTFONT) );

	CG_DrawStringExt( x, y, string, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );
}
*/
static void CG_CenterGiantLine( float y, const char *string ) {
	CG_DrawString( 320, y, string, UI_CENTER|UI_DROPSHADOW|UI_GIANTFONT|UI_NOSCALE, NULL );
}
/*
=================
CG_DrawTourneyScoreboard

Draw the oversize scoreboard for tournements
=================
*/
void CG_DrawTourneyScoreboard( void ) {
	const char		*s;
	vec4_t			color;
	int				min, tens, ones;
	clientInfo_t	*ci;
	int				y;
	int				i;

	// request more scores regularly
	if ( cg.scoresRequestTime + 2000 < cg.time ) {
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );
	}

	// draw the dialog background
	color[0] = color[1] = color[2] = 0;
	color[3] = 1;
	CG_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color );

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	// print the mesage of the day
	s = CG_ConfigString( CS_MOTD );
	if ( !s[0] ) {
		s = "Scoreboard";
	}

	// print optional title
	CG_CenterGiantLine( 8, s );

	// print server time
	ones = cg.time / 1000;
	min = ones / 60;
	ones %= 60;
	tens = ones / 10;
	ones %= 10;
	s = va("%i:%i%i", min, tens, ones );

	CG_CenterGiantLine( 64, s );


	// print the two scores

	y = 160;
	if ( cgs.gametype >= GT_TEAM ) {
		//
		// teamplay scoreboard
		//
		//CG_DrawStringExt( 8, y, "Red Team", color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );
		CG_DrawString( 8, y, "Red Team", UI_LEFT|UI_DROPSHADOW|UI_GIANTFONT|UI_NOSCALE, NULL );
		s = va("%i", cg.teamScores[0] );
		//CG_DrawStringExt( 632 - GIANT_WIDTH * strlen(s), y, s, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );
		CG_DrawString( 632, y, s, UI_RIGHT|UI_DROPSHADOW|UI_GIANTFONT|UI_NOSCALE, NULL );
		
		y += GIANTCHAR_HEIGHT + 16;

		CG_DrawString( 8, y, "Blue Team", UI_LEFT|UI_DROPSHADOW|UI_GIANTFONT|UI_NOSCALE, NULL );
		s = va("%i", cg.teamScores[1] );
		CG_DrawString( 632, y, s, UI_RIGHT|UI_DROPSHADOW|UI_GIANTFONT|UI_NOSCALE, NULL );
	} else {
		//
		// free for all scoreboard
		//
		for ( i = 0 ; i < MAX_CLIENTS ; i++ ) {
			ci = &cgs.clientinfo[i];
			if ( !ci->infoValid ) {
				continue;
			}
			if ( ci->team != TEAM_FREE ) {
				continue;
			}

			CG_DrawString( 8, y, ci->name, UI_LEFT|UI_DROPSHADOW|UI_NOSCALE|UI_BIGFONT, NULL );
			s = va("%i", ci->score );
			CG_DrawString( 632, y, s, UI_RIGHT|UI_DROPSHADOW|UI_NOSCALE|UI_BIGFONT, NULL );
			y += 64;
		}
	}


}

