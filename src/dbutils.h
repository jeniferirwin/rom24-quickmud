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

int next_free_room(AREA_DATA *pArea);
int next_free_obj(AREA_DATA *pArea);
int next_free_mob(AREA_DATA *pArea);
int next_free_mprog(AREA_DATA *pArea);
int get_random_area();
MOB_INDEX_DATA *get_random_mob_area(AREA_DATA *pArea);
MOB_INDEX_DATA *get_random_mob_global(void);
MPROG_CODE *get_random_mprog_area(AREA_DATA *pArea);
MPROG_CODE *get_random_mprog_global(void);
OBJ_INDEX_DATA *get_random_obj_area(AREA_DATA *pArea);
OBJ_INDEX_DATA *get_random_obj_global(void);
ROOM_INDEX_DATA *get_random_room_area(AREA_DATA *pArea);
ROOM_INDEX_DATA *get_random_room_global(void);