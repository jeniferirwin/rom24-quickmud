 /***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 **************************************************************************/

/***************************************************************************
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                            *
 *   ROM has been brought to you by the ROM consortium                     *
 *       Russ Taylor (rtaylor@hypercube.org)                               *
 *       Gabrielle Taylor (gtaylor@hypercube.org)                          *
 *       Brian Moore (zump@rom.org)                                        *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ROM license, in the file Rom24/doc/rom.license                        *
 **************************************************************************/

/***************************************************************************
 * dbutils.c
 * Utility functions for getting 'next free vnum', random vnums, etc. a
 * bit easier. Has options for global searches or area-based searches.
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"
#include "dbutils.h"
#include "tables.h"
#include "db.h"
#include "olc.h"

/*
 * Command to test all of the database functions. If you want to get rid
 * of this command, search for `testutils` in interp.h and interp.c and
 * remove it.
 */

void do_testutils(CHAR_DATA *ch, char *argument)
{
    ROOM_INDEX_DATA *pRoomIndex;
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    MPROG_CODE *pMprogIndex;
    AREA_DATA *pArea;
    char arg[MAX_INPUT_LENGTH];
    arg[0] = '\0';
    int openRoom, openObj, openMob, openMprog;
    char buf[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg);

    if (!str_cmp(arg, "random"))
        for (pArea = area_first; pArea; pArea = pArea->next)
        {
            for (int i = 0; i < 3; i++)
            {
                if ((pRoomIndex = get_random_room_area(pArea)))
                {
                    sprintf(buf, "get_random_room_area: room %d in area %s (%d)\n\r", pRoomIndex->vnum, pArea->name, pArea->vnum);
                    send_to_char(buf, ch);
                }
            }
            for (int i = 0; i < 3; i++)
            {
                if ((pObjIndex = get_random_obj_area(pArea)))
                {
                    sprintf(buf, "get_random_obj_area: obj %d in area %s (%d)\n\r", pObjIndex->vnum, pArea->name, pArea->vnum);
                    send_to_char(buf, ch);
                }
            }
            for (int i = 0; i < 3; i++)
            {
                if ((pMobIndex = get_random_mob_area(pArea)))
                {
                    sprintf(buf, "get_random_mob_area: mob %d in area %s (%d)\n\r", pMobIndex->vnum, pArea->name, pArea->vnum);
                    send_to_char(buf, ch);
                }
            }
            for (int i = 0; i < 3; i++)
            {
                if ((pMprogIndex = get_random_mprog_area(pArea)))
                {
                    sprintf(buf, "get_random_mprog_area: mprog %d in area %s (%d)\n\r", pMprogIndex->vnum, pArea->name, pArea->vnum);
                    send_to_char(buf, ch);
                }
            }
        }
    else if (!str_cmp(arg, "open"))
        for (pArea = area_first; pArea; pArea = pArea->next)
        {
            if ((openRoom = next_free_room(pArea)))
            {
                sprintf(buf, "next_free_room: vnum %d in area %s (%d)\n\r", openRoom, pArea->name, pArea->vnum);
                send_to_char(buf, ch);
            }
            if ((openObj = next_free_obj(pArea)))
            {
                sprintf(buf, "next_free_obj: vnum %d in area %s (%d)\n\r", openObj, pArea->name, pArea->vnum);
                send_to_char(buf, ch);
            }
            if ((openMob = next_free_mob(pArea)))
            {
                sprintf(buf, "next_free_mob: vnum %d in area %s (%d)\n\r", openMob, pArea->name, pArea->vnum);
                send_to_char(buf, ch);
            }
            if ((openMprog = next_free_mprog(pArea)))
            {
                sprintf(buf, "next_free_mprog: vnum %d in area %s (%d)\n\r", openMprog, pArea->name, pArea->vnum);
                send_to_char(buf, ch);
            }
        }
    else if (!str_cmp(arg, "global"))
    {
        for (int i = 0; i < 50; i++)
        {
            if ((pMobIndex = get_random_mob_global()))
            {
                sprintf(buf, "get_random_mob_global: mob %d (%s)\n\r", pMobIndex->vnum, pMobIndex->short_descr);
                send_to_char(buf, ch);
            }
        }
        for (int i = 0; i < 50; i++)
        {
            if ((pMprogIndex = get_random_mprog_global()))
            {
                sprintf(buf, "get_random_mprog_global: mprog %d\n\r", pMprogIndex->vnum);
                send_to_char(buf, ch);
            }
        }
        for (int i = 0; i < 50; i++)
        {
            if ((pObjIndex = get_random_obj_global()))
            {
                sprintf(buf, "get_random_obj_global: obj %d (%s)\n\r", pObjIndex->vnum, pObjIndex->short_descr);
                send_to_char(buf, ch);
            }
        }
        for (int i = 0; i < 50; i++)
        {
            if ((pRoomIndex = get_random_room_global()))
            {
                sprintf(buf, "get_random_room_global: room %d (%s)\n\r", pRoomIndex->vnum, pRoomIndex->name);
                send_to_char(buf, ch);
            }
        }
    }
    else if (!str_cmp(arg, "area"))
    {
        if ((pArea = get_area_data(get_random_area())))
        {
            sprintf(buf, "get_random_area: vnum %d (%s)\n\r", pArea->vnum, pArea->name);
            send_to_char(buf, ch);
        }
    }
    else
    {
        send_to_char("Usage: testutils random | open | global | area\n\r", ch);
    }
}

/* Get random area vnum */

int get_random_area(void)
{
    return rand() % top_area;
}

/* Get random mob index in an area */

MOB_INDEX_DATA *get_random_mob_area(AREA_DATA *pArea)
{
    MOB_INDEX_DATA *pMobIndex;
    MOB_INDEX_DATA **pRandomMobIndex = malloc(sizeof(MOB_INDEX_DATA *) * (pArea->max_vnum - pArea->min_vnum + 1));
    MOB_INDEX_DATA *pMobPickedIndex;
    int count = 0;

    for (int vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if ((pMobIndex = get_mob_index(vnum)))
        {
            pRandomMobIndex[count] = pMobIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pMobPickedIndex = pRandomMobIndex[rand() % count];
        free(pRandomMobIndex);
        return pMobPickedIndex;
    }
    else
    {
        return 0;
    }
}

/* Get random mob index globally */

MOB_INDEX_DATA *get_random_mob_global(void)
{
    MOB_INDEX_DATA *pMobIndex;
    MOB_INDEX_DATA **pRandomMobIndex = malloc(sizeof(MOB_INDEX_DATA *) * (top_mob_index + 1));
    MOB_INDEX_DATA *pMobPickedIndex;
    int count = 0;

    for (int vnum = 0; vnum <= MAX_VNUM; vnum++)
    {
        if ((pMobIndex = get_mob_index(vnum)))
        {
            pRandomMobIndex[count] = pMobIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pMobPickedIndex = pRandomMobIndex[rand() % count];
        free(pRandomMobIndex);
        return pMobPickedIndex;
    }
    return NULL;
}

/* Get random mprog index in an area */
MPROG_CODE *get_random_mprog_area(AREA_DATA *pArea)
{
    MPROG_CODE *pMprogIndex;
    MPROG_CODE **pRandomMprogIndex = malloc(sizeof(MPROG_CODE *) * (pArea->max_vnum - pArea->min_vnum + 1));
    MPROG_CODE *pMprogPickedIndex;
    int count = 0;

    for (int vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if ((pMprogIndex = get_mprog_index(vnum)))
        {
            pRandomMprogIndex[count] = pMprogIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pMprogPickedIndex = pRandomMprogIndex[rand() % count];
        free(pRandomMprogIndex);
        return pMprogPickedIndex;
    }
    else
    {
        return 0;
    }
}

/* Get random mprog index globally */

MPROG_CODE *get_random_mprog_global(void)
{
    MPROG_CODE *pMprogIndex;
    MPROG_CODE **pRandomMprogIndex = malloc(sizeof(MPROG_CODE *) * (top_mprog_index + 1));
    MPROG_CODE *pMprogPickedIndex;
    int count = 0;

    for (int vnum = 0; vnum <= MAX_VNUM; vnum++)
    {
        if ((pMprogIndex = get_mprog_index(vnum)))
        {
            pRandomMprogIndex[count] = pMprogIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pMprogPickedIndex = pRandomMprogIndex[rand() % count];
        free(pRandomMprogIndex);
        return pMprogPickedIndex;
    }
    return NULL;
}

/* Get random obj index in an area */
OBJ_INDEX_DATA *get_random_obj_area(AREA_DATA *pArea)
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_INDEX_DATA **pRandomObjIndex = malloc(sizeof(OBJ_INDEX_DATA *) * (pArea->max_vnum - pArea->min_vnum + 1));
    OBJ_INDEX_DATA *pObjPickedIndex;
    int count = 0;

    for (int vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if ((pObjIndex = get_obj_index(vnum)))
        {
            pRandomObjIndex[count] = pObjIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pObjPickedIndex = pRandomObjIndex[rand() % count];
        free(pRandomObjIndex);
        return pObjPickedIndex;
    }
    else
    {
        return 0;
    }
}

/* Get random obj index globally */
OBJ_INDEX_DATA *get_random_obj_global(void)
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_INDEX_DATA **pRandomObjIndex = malloc(sizeof(OBJ_INDEX_DATA *) * (top_obj_index + 1));
    OBJ_INDEX_DATA *pObjPickedIndex;
    int count = 0;

    for (int vnum = 0; vnum <= MAX_VNUM; vnum++)
    {
        if ((pObjIndex = get_obj_index(vnum)))
        {
            pRandomObjIndex[count] = pObjIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pObjPickedIndex = pRandomObjIndex[rand() % count];
        free(pRandomObjIndex);
        return pObjPickedIndex;
    }
    return NULL;
}

/* Get random room index in an area */
ROOM_INDEX_DATA *get_random_room_area(AREA_DATA *pArea)
{
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA **pRandomRoomIndex = malloc(sizeof(ROOM_INDEX_DATA *) * (pArea->max_vnum - pArea->min_vnum + 1));
    ROOM_INDEX_DATA *pRoomPickedIndex;
    int count = 0;

    for (int vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if ((pRoomIndex = get_room_index(vnum)))
        {
            pRandomRoomIndex[count] = pRoomIndex;
            count++;
        }
    }

    if (count > 0)
    {
        pRoomPickedIndex = pRandomRoomIndex[rand() % count];
        free(pRandomRoomIndex);
        return pRoomPickedIndex;
    }
    else
    {
        return 0;
    }
}

/* Get random room index globally */

ROOM_INDEX_DATA *get_random_room_global(void)
{
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA **pRandomRoomIndex = malloc(sizeof(ROOM_INDEX_DATA *) * (top_vnum_room + 1));
    ROOM_INDEX_DATA *pRoomPickedIndex;
    int count = 0;

    for (int vnum = 0; vnum <= MAX_VNUM; vnum++)
    {
        if ((pRoomIndex = get_room_index(vnum)))
        {
            pRandomRoomIndex[count] = pRoomIndex;
            count++;
        }
    }
    if (count > 0)
    {
        pRoomPickedIndex = pRandomRoomIndex[rand() % count];
        free(pRandomRoomIndex);
        return pRoomPickedIndex;
    }
    return NULL;
}

/* Get next free mob vnum in an area */

int next_free_mob(AREA_DATA *pArea)
{
    MOB_INDEX_DATA *pMobIndex;
    int vnum;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if (!(pMobIndex = get_mob_index(vnum)))
            return vnum;
    }
    return 0;
}

/* Get next free mprog vnum in an area */

int next_free_mprog(AREA_DATA *pArea)
{
    MPROG_CODE *pMprog;
    int vnum;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if (!(pMprog = get_mprog_index(vnum)))
            return vnum;
    }
    return 0;
}

/* Get next free obj vnum in an area */

int next_free_obj(AREA_DATA *pArea)
{
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if (!(pObjIndex = get_obj_index(vnum)))
            return vnum;
    }
    return 0;
}

/* Get next free room vnum in an area */

int next_free_room(AREA_DATA *pArea)
{
    ROOM_INDEX_DATA *pRoomIndex;
    int vnum;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if (!(pRoomIndex = get_room_index(vnum)))
            return vnum;
    }
    return 0;
}
