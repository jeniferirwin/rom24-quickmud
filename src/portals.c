/*
 * Test portals
 *
 * Create test portals all over the MUD. This is for my scraper
 * program that may become public once it's finished.
 *
 * This is not quite functional yet.
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "db.h"
#include "olc.h"

void create_password_portal(CHAR_DATA *ch, int vnum, bool can_leave);
void create_verb_portal(CHAR_DATA *ch, int vnum, bool can_leave);
void create_normal_portal(CHAR_DATA *ch, int vnum, bool can_leave);
ROOM_INDEX_DATA *get_random_room_area(AREA_DATA *pArea);
int find_testable_vnum(AREA_DATA *pArea);
OBJ_INDEX_DATA *generate_portal_base(int vnum);
void create_testportal_reset(OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pRoomIndex);
void create_testportal_code(OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pTargetRoomIndex);
ROOM_INDEX_DATA *get_portal_dest(CHAR_DATA *ch, OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pRoomIndex, int can_leave);

void create_normal_portal(CHAR_DATA *ch, int vnum, bool can_leave)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex, *pTargetRoomIndex;

    if (!(pRoomIndex = get_room_index(vnum)))
    {
        bug("Create_normal_portal: bad room %d.", vnum);
        return;
    }

    if (!(pObjIndex = generate_portal_base(vnum)))
        return;

    if (!(pTargetRoomIndex = get_portal_dest(ch, pObjIndex, pRoomIndex, can_leave)))
        return;

    pObjIndex->name = str_dup("normal portal");
    pObjIndex->short_descr = str_dup("a normal portal");
    pObjIndex->description = str_dup("You could \e[0;36menter\e[0m this normal portal.");
    pObjIndex->item_type = ITEM_PORTAL;
    pObjIndex->value[3] = pTargetRoomIndex->vnum;
    pObjIndex->material = str_dup("ether");

    create_testportal_reset(pObjIndex, pRoomIndex);

    sprintf(buf, "Normal portal %d created at room %d in area %s (%d). Goes to room %d.\n\r",
            pObjIndex->vnum,
            pRoomIndex->vnum,
            pRoomIndex->area->name,
            pRoomIndex->area->vnum,
            pObjIndex->value[3]);
    send_to_char(buf, ch);
}

void create_password_portal(CHAR_DATA *ch, int vnum, bool can_leave)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex, *pTargetRoomIndex;

    if (!(pRoomIndex = get_room_index(vnum)))
    {
        bug("Create_password_portal: bad room %d.", vnum);
        return;
    }

    if (!(pObjIndex = generate_portal_base(vnum)))
        return;

    if (!(pTargetRoomIndex = get_portal_dest(ch, pObjIndex, pRoomIndex, can_leave)))
        return;

    pObjIndex->area = pRoomIndex->area;
    pObjIndex->material = str_dup("ether");
    pObjIndex->name = str_dup("xyzzy portal");
    pObjIndex->short_descr = str_dup("a password portal");
    pObjIndex->description = str_dup("You hear a whisper from a password portal... '\e[1;32mxyzzy\e[0m'.");
    pObjIndex->item_type = ITEM_PORTAL;
    pObjIndex->value[2] = GATE_PASSWORD;
    pObjIndex->value[3] = pTargetRoomIndex->vnum;
    pObjIndex->ospec_fun = ospec_lookup("spec_password");

    create_testportal_reset(pObjIndex, pRoomIndex);

    sprintf(buf, "Password portal %d created at room %d in area %s (%d). Goes to room %d.\n\r",
            pObjIndex->vnum,
            pRoomIndex->vnum,
            pRoomIndex->area->name,
            pRoomIndex->area->vnum,
            pObjIndex->value[3]);
    send_to_char(buf, ch);
};

void create_testportal_code(OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pTargetRoomIndex)
{
    MPROG_CODE *code;
    MPROG_LIST *list;
    char buf[MAX_STRING_LENGTH];

    if (pTargetRoomIndex == NULL)
    {
        bug("Create_testportal_code: bad room %d.", pTargetRoomIndex->vnum);
        return;
    }

    if (pObjIndex == NULL)
    {
        bug("Create_testportal_code: bad object %d.", pObjIndex->vnum);
        return;
    }

    pObjIndex->extra_descr = new_extra_descr();
    pObjIndex->extra_descr->keyword = str_dup("@verb");
    pObjIndex->extra_descr->description = str_dup("leap");
    pObjIndex->extra_descr->next = NULL;

    code = new_mpcode();
    code->vnum = pObjIndex->vnum;
    sprintf(buf, "mob echoat $n You leap through!\n\rmob trans $n %d\n\r", pTargetRoomIndex->vnum);
    code->code = str_dup(buf);
    code->next = mprog_list;
    mprog_list = code;

    list = new_mprog();
    list->vnum = pObjIndex->vnum;
    list->trig_type = TRIG_VERB;
    list->trig_phrase = str_dup("leap");
    list->code = code->code;
    SET_BIT(pObjIndex->mprog_flags, list->trig_type);
    list->next = pObjIndex->mprogs;
    pObjIndex->mprogs = list;
}

void create_testportal_reset(OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pRoomIndex)
{
    RESET_DATA *pReset;
    pReset = new_reset_data();
    pReset->command = 'O';
    pReset->arg1 = pObjIndex->vnum;
    pReset->arg3 = pRoomIndex->vnum;
    add_reset(pRoomIndex, pReset, 0);
    reset_room(pRoomIndex);
}

void create_verb_portal(CHAR_DATA *ch, int vnum, bool can_leave)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex, *pTargetRoomIndex;

    if (!(pRoomIndex = get_room_index(vnum)))
    {
        sprintf(buf, "Create_verb_portal: Can't find room %d.\n\r", vnum);
        send_to_char(buf, ch);
        return;
    }

    if (!(pObjIndex = generate_portal_base(pRoomIndex->vnum)))
    {
        bug("Create_verb_portal: bad object %d.", pRoomIndex->vnum);
        return;
    }

    if (!(pTargetRoomIndex = get_portal_dest(ch, pObjIndex, pRoomIndex, can_leave)))
        return;

    pObjIndex->area = pRoomIndex->area;
    pObjIndex->material = str_dup("ether");
    pObjIndex->name = str_dup("leap portal");
    pObjIndex->short_descr = str_dup("a verb portal");
    pObjIndex->description = str_dup("You could \e[0;36mleap\e[0m through this portal.");
    pObjIndex->item_type = ITEM_FURNITURE;
    pObjIndex->ospec_fun = ospec_lookup("spec_verb");

    create_testportal_code(pObjIndex, pTargetRoomIndex);
    create_testportal_reset(pObjIndex, pRoomIndex);

    sprintf(buf, "Verb portal %d created at room %d in area %s (%d). Goes to room %d.\n\r",
            pObjIndex->vnum,
            pRoomIndex->vnum,
            pRoomIndex->area->name,
            pRoomIndex->area->vnum,
            pTargetRoomIndex->vnum);
    send_to_char(buf, ch);
}

void do_test_portals(CHAR_DATA *ch, char *argument)
{
    AREA_DATA *pArea;
    int vnum, area;
    int type;

    for (area = 0; area < top_area; area++)
    {
        if (!(pArea = get_area_data(area)))
        {
            bug("do_test_portals: bad area %d.", area);
            return;
        }
        if ((vnum = find_testable_vnum(pArea)))
        {
            srand(time(0) + vnum + area);
            type = rand() % 6;
            switch (type)
            {
            case 0:
                create_verb_portal(ch, vnum, TRUE);
                break;
            case 1:
                create_verb_portal(ch, vnum, FALSE);
                break;
            case 2:
                create_password_portal(ch, vnum, FALSE);
                break;
            case 3:
                create_password_portal(ch, vnum, TRUE);
                break;
            case 4:
                create_normal_portal(ch, vnum, FALSE);
                break;
            case 5:
                create_normal_portal(ch, vnum, TRUE);
                break;
            default:
                break;
            }
        }
    }
}

int find_testable_vnum(AREA_DATA *pArea)
{
    int vnum;
    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if (!get_room_index(vnum))
            continue;
        if (get_obj_index(vnum))
            continue;
        if (get_mprog_index(vnum))
            continue;
        return vnum;
    }
    return 0;
}

OBJ_INDEX_DATA *generate_portal_base(int vnum)
{
    OBJ_INDEX_DATA *pObjIndex;

    if ((pObjIndex = get_obj_index(vnum)))
    {
        bug("generate_portal_base: vnum %d already exists.", vnum);
        return NULL;
    }
    pObjIndex = alloc_perm(sizeof(*pObjIndex));
    pObjIndex->vnum = vnum;
    pObjIndex->new_format = TRUE;
    pObjIndex->reset_num = 0;
    newobjs++;
    int iHash = vnum % MAX_KEY_HASH;
    pObjIndex->next = obj_index_hash[iHash];
    obj_index_hash[iHash] = pObjIndex;
    top_obj_index++;
    top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
    return pObjIndex;
}

ROOM_INDEX_DATA *get_random_room_area(AREA_DATA *pArea)
{
    ROOM_INDEX_DATA *pRoomIndex;
    int vnum;
    int tries = 5;

    while (tries > 0)
    {
        tries--;
        vnum = rand() % ((pArea->max_vnum - pArea->min_vnum) + pArea->min_vnum);
        if ((pRoomIndex = get_room_index(vnum)))
            return pRoomIndex;
    }
    return NULL;
}

ROOM_INDEX_DATA *next_free_room(AREA_DATA *pArea)
{
    ROOM_INDEX_DATA *pRoomIndex;
    int vnum;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
        if (!(pRoomIndex = get_room_index(vnum)))
            return pRoomIndex;
    }
    return 0;
}

ROOM_INDEX_DATA *get_portal_dest(CHAR_DATA *ch, OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pRoomIndex, int can_leave)
{
    ROOM_INDEX_DATA *pRandomRoomIndex;

    if (can_leave)
    {
        if ((pRandomRoomIndex = get_random_room(ch)))
        {
            return pRandomRoomIndex;
        }
        else
        {
            bug("Create_password_portal: random room failed.", 0);
            return NULL;
        }
    }
    else
    {
        if ((pRandomRoomIndex = get_random_room_area(pRoomIndex->area)))
        {
            return pRandomRoomIndex;
        }
        else
        {
            bug("Create_password_portal: random room area failed.", 0);
            return NULL;
        }
    }
}
