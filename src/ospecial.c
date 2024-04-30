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
DECLARE_OSPEC_FUN(spec_verb);
DECLARE_OSPEC_FUN(spec_password);

/* the function table */
const struct ospec_type ospec_table[] = {
    {"spec_verb", spec_verb},
    {"spec_password", spec_password},
    {NULL, NULL}};

/*
 * Given a name, return the appropriate spec fun.
 */
OSPEC_FUN *ospec_lookup(const char *name)
{
    int i;

    for (i = 0; ospec_table[i].name != NULL; i++)
    {
        if (LOWER(name[0]) == LOWER(ospec_table[i].name[0]) && !str_prefix(name, ospec_table[i].name))
            return ospec_table[i].function;
    }

    return 0;
}

char *ospec_name(OSPEC_FUN *function)
{
    int i;

    for (i = 0; ospec_table[i].function != NULL; i++)
    {
        if (function == ospec_table[i].function)
            return ospec_table[i].name;
    }

    return NULL;
}

char *get_gate_password(OBJ_DATA *obj)
{
    char *password = strdup(obj->name);
    if (password == NULL)
        return NULL;
    one_argument(obj->name, password);
    return password;
}

bool spec_verb(CHAR_DATA *ch, OBJ_DATA *obj)
{
    char *message;

    if (!IS_AWAKE(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
        return FALSE;

    message = "You have activated $p.";
    act(message, ch, obj, NULL, TO_CHAR);
    message = "$n has activated $p.";
    act(message, ch, obj, NULL, TO_ROOM);
    return TRUE;
}

/*
 * ospec_password
 *
 * This is a shameless ripoff of do_enter. I was going to try to bake
 * passwords directly into do_enter but I couldn't do that elegantly
 * without modifying the list of parameters, which would have opened up
 * a whole other can of worms.
 *
 * So here we are. This mimics most of the behavior of do_enter, but
 * only works on passworded portals, and also doesn't prevent the player
 * from moving while cursed.
 *
 * Password-based portals are usually meant to represent taking a
 * physical action on an object, such as swinging on a rope or
 * squeezing through a tight space. As such, these should rarely be
 * 'portals' in the traditional sense of the word.
 *
 * Because of this, it is advised to avoid the use of doors, locks and
 * the GATE_NOCURSE flag on these types of objects. If someone has
 * already figured out the password to a portal, they probably shouldn't
 * need to find a key as well, unless the portal is an ultra-reinforced
 * final door on some epic hero zone quest.
 *
 * - technitaur
 */

bool spec_password(CHAR_DATA *ch, OBJ_DATA *obj)
{
    ROOM_INDEX_DATA *source = NULL;
    ROOM_INDEX_DATA *destination = NULL;
    CHAR_DATA *fch, *fch_next;

    source = ch->in_room;

    if (source == NULL || obj == NULL || ch->fighting != NULL || !IS_AWAKE(ch) || IS_AFFECTED(ch, AFF_CHARM))
        return FALSE;

    if (obj->item_type != ITEM_PORTAL)
    {
        send_to_char("OSPEC: Somehow activated a password on an object that", ch);
        send_to_char("       isn't a portal. Please report a BUG.", ch);
        return FALSE;
    }

    if (!IS_SET(obj->value[2], GATE_PASSWORD))
    {
        send_to_char("OSpec: Somehow activated a password on a gate that doesn't", ch);
        send_to_char("       have GATE_PASSWORD set. Please report a BUG.", ch);
        return FALSE;
    }

    if (!IS_TRUSTED(ch, ANGEL) && !IS_SET(obj->value[2], GATE_NOCURSE) && (IS_AFFECTED(ch, AFF_CURSE) || IS_SET(source->room_flags, ROOM_NO_RECALL)))
    {
        send_to_char("Something prevents you from leaving...\n\r", ch);
        return FALSE;
    }

    if (IS_SET(obj->value[1], EX_CLOSED) && !IS_TRUSTED(ch, ANGEL))
    {
        act("$p is closed.", ch, obj, NULL, TO_CHAR);
        return FALSE;
    }

    if (IS_SET(obj->value[2], GATE_RANDOM) || obj->value[3] == -1)
    {
        destination = get_random_room(ch);
        obj->value[3] = destination->vnum; /* for record keeping :) */
    }
    else if (IS_SET(obj->value[2], GATE_BUGGY) && (number_percent() < 5))
        destination = get_random_room(ch);
    else
        destination = get_room_index(obj->value[3]);

    if (destination == NULL || destination == source || !can_see_room(ch, destination) || (room_is_private(destination) && !IS_TRUSTED(ch, IMPLEMENTOR)))
    {
        act("$p doesn't seem to go anywhere.", ch, obj, NULL, TO_CHAR);
        return FALSE;
    }

    if (IS_NPC(ch) && IS_SET(ch->act, ACT_AGGRESSIVE) && IS_SET(destination->room_flags, ROOM_LAW))
    {
        send_to_char("Aggro mobs can't enter lawful zones.\n\r", ch);
        return FALSE;
    }

    // TODO: Use extra descs to make the messages for ACT
    act("You activate $p.", ch, obj, NULL, TO_CHAR);
    act("$n activates $p.", ch, obj, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, destination);

    if (IS_SET(obj->value[2], GATE_GOWITH))
    { /* take the gate along */
        obj_from_room(obj);
        obj_to_room(obj, destination);
    }

    act("$n has arrived through $p.", ch, obj, NULL, TO_ROOM);

    if (!IS_SET(obj->value[2], GATE_SILENT))
        do_function(ch, &do_look, "auto");

    if (obj->value[0] > 0)
    {
        obj->value[0]--;
        if (obj->value[0] == 0)
            obj->value[0] = -1;
    }

    if (source == destination)
        return FALSE;

    for (fch = source->people; fch != NULL; fch = fch_next)
    {
        fch_next = fch->next_in_room;

        if (obj == NULL || obj->value[0] == -1)
            /* no following through dead portals */
            continue;

        if (fch->master == ch && IS_AFFECTED(fch, AFF_CHARM) && fch->position < POS_STANDING)
            do_function(fch, &do_stand, "");

        if (fch->master == ch && fch->position == POS_STANDING)
        {

            if (IS_SET(ch->in_room->room_flags, ROOM_LAW) && (IS_NPC(fch) && IS_SET(fch->act, ACT_AGGRESSIVE)))
            {
                act("You can't bring $N into the city.",
                    ch, NULL, fch, TO_CHAR);
                act("You aren't allowed in the city.",
                    fch, NULL, NULL, TO_CHAR);
                continue;
            }

            act("You follow $N.", fch, NULL, ch, TO_CHAR);
            obj->ospec_fun(fch, obj);
        }
    }

    if (obj != NULL && obj->value[0] == -1)
    {
        act("$p fades out of existence.", ch, obj, NULL, TO_CHAR);
        if (ch->in_room == source)
            act("$p fades out of existence.", ch, obj, NULL, TO_ROOM);
        else if (source->people != NULL)
        {
            act("$p fades out of existence.",
                source->people, obj, NULL, TO_CHAR);
            act("$p fades out of existence.",
                source->people, obj, NULL, TO_ROOM);
        }
        extract_obj(obj);
    }

    /*
     * If someone is following the char, these triggers get activated
     * for the followers before the char, but it's safer this way...
     */
    if (IS_NPC(ch) && HAS_TRIGGER(ch, TRIG_ENTRY))
        mp_percent_trigger(ch, NULL, NULL, NULL, TRIG_ENTRY);
    if (!IS_NPC(ch))
        mp_greet_trigger(ch);

    return TRUE;
}
