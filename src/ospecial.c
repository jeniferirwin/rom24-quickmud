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
 ***************************************************************************/

/***************************************************************************
 *  ROM 2.4 is copyright 1993-1998 Russ Taylor                             *
 *  ROM has been brought to you by the ROM consortium                      *
 *      Russ Taylor (rtaylor@hypercube.org)                                *
 *      Gabrielle Taylor (gtaylor@hypercube.org)                           *
 *      Brian Moore (zump@rom.org)                                         *
 *  By using this code, you have agreed to follow the terms of the         *
 *  ROM license, in the file Rom24/doc/rom.license                         *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"

/*
 * The following special functions are available for objects.
 */
DECLARE_OSPEC_FUN (ospec_verb);
DECLARE_OSPEC_FUN (ospec_password);

/* the function table */
const struct ospec_type ospec_table[] = {
    {"ospec_verb", ospec_verb},
    {"ospec_password", ospec_password},
    {NULL, NULL}
};

/*
 * Given a name, return the appropriate spec fun.
 */
OSPEC_FUN *ospec_lookup (const char *name)
{
    int i;

    for (i = 0; ospec_table[i].name != NULL; i++)
    {
        if (LOWER (name[0]) == LOWER (ospec_table[i].name[0])
            && !str_prefix (name, ospec_table[i].name))
            return ospec_table[i].function;
    }

    return 0;
}

char *ospec_name (OSPEC_FUN * function)
{
    int i;

    for (i = 0; ospec_table[i].function != NULL; i++)
    {
        if (function == ospec_table[i].function)
            return ospec_table[i].name;
    }

    return NULL;
}

bool ospec_verb (CHAR_DATA * ch, OBJ_DATA * obj)
{
    char *message;

    if (!IS_AWAKE (ch) || ch->in_room == NULL || IS_AFFECTED (ch, AFF_CHARM)
        || ch->fighting != NULL)
        return FALSE;

    message = "You have activated $p.";
    act (message, ch, obj, NULL, TO_CHAR);
    message = "$n has activated $p.";
    act (message, ch, obj, NULL, TO_ROOM);
    return TRUE;
}

bool ospec_password (CHAR_DATA * ch, OBJ_DATA * obj)
{
    ROOM_INDEX_DATA * pRoom = NULL;
    char *message;

    if (!IS_AWAKE (ch) || ch->in_room == NULL
        || IS_AFFECTED (ch, AFF_CHARM) || ch->fighting != NULL)
        return FALSE;

    if (obj->item_type != ITEM_PORTAL || !(pRoom = get_room_index(obj->value[3]))) {
        message = "Unfortunately, $p doesn't seem to go anywhere.";
        act (message, ch, obj, NULL, TO_CHAR);
        send_to_char("This is unintended behavior. If you have the time, please file a TYPO.",ch);
        return FALSE;
    }
    
    // TODO: Use extra description keywords to fill the messages
    if (obj->item_type != ITEM_PORTAL || obj->value[3])
    message = "You have activated the password on $p.";
    act (message, ch, obj, NULL, TO_CHAR);
    message = "$n has activated the password on $p.";
    act (message, ch, obj, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, pRoom);
    message = "$n has arrived through $p.";
    act (message, ch, obj, NULL, TO_ROOM);
    return TRUE;
}
