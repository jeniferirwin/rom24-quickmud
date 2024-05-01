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
sh_int get_random_room_area(int area_vnum);

void do_test_portals(CHAR_DATA *ch, char *argument)
{
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *pRoomIndex;
    OBJ_INDEX_DATA *pObjIndex;
    MPROG_CODE *code;
    int vnum;
    int area;
    int count;

    for (area = 0; area < top_area; area++)
    {
        pArea = get_area_data(area);
        count = 0;
        while (count < 3)
        {
            vnum = rand() % top_room;
            if (!(pRoomIndex = get_room_index(vnum)))
                continue;
            if ((pObjIndex = get_obj_index(vnum)))
                continue;
            if ((code = get_mprog_index(vnum)))
                continue;
            count++;
            if (count > 3)
                break;
            srand(time(0) + area + vnum);
            int type = rand() % 6 + 1;
            type = 1;
            switch (type)
            {
            case 1:
                create_password_portal(ch, vnum, TRUE);
                break;
            case 2:
                create_password_portal(ch, vnum, FALSE);
                break;
            case 3:
                create_normal_portal(ch, vnum, TRUE);
                break;
            case 4:
                create_normal_portal(ch, vnum, FALSE);
                break;
            case 5:
                create_verb_portal(ch, vnum, TRUE);
                break;
            case 6:
                create_verb_portal(ch, vnum, FALSE);
                break;
            }
        }
    }
}

void create_password_portal(CHAR_DATA *ch, int vnum, bool can_leave)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    RESET_DATA *pReset;

    if (!(pRoomIndex = get_room_index(vnum)))
    {
        send_to_char("Portal tester: Can't find room.\n\r", ch);
        return;
    }
    pObjIndex = new_obj_index();
    pObjIndex->area = pRoomIndex->area;
    pObjIndex->vnum = vnum;
    pObjIndex->material = str_dup("ether");
    pObjIndex->name = "xyzzy portal";
    pObjIndex->short_descr = "a password portal";
    pObjIndex->description = "You hear a whisper from a password portal... '\e[1;32mxyzzy\e[0m'.\n\r";
    pObjIndex->condition = 100;
    pObjIndex->weight = 0;
    pObjIndex->cost = 0;
    pObjIndex->level = 0;
    send_to_char("Portal tester: Password portal created. About to try to set destination....\n\r", ch);
    if (can_leave)
        pObjIndex->value[3] = get_random_room_area(0);
    else
        pObjIndex->value[3] = get_random_room_area(pRoomIndex->area->vnum);
    pObjIndex->value[2] = GATE_PASSWORD;
    pObjIndex->ospec_fun = ospec_lookup("spec_password");
    pReset = new_reset_data();
    pReset->command = 'O';
    pReset->arg1 = pObjIndex->vnum;
    pReset->arg2 = 1;
    pReset->arg3 = pRoomIndex->vnum;
    pReset->arg4 = 1;
    add_reset(pRoomIndex, pReset, 0);
    reset_room(pRoomIndex);
    sprintf(buf, "Portal %d created at room %d.\n\r", pObjIndex->vnum, pRoomIndex->vnum);
    send_to_char(buf, ch);
}

void create_verb_portal(CHAR_DATA *ch, int vnum, bool can_leave)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    MPROG_CODE *code;
    RESET_DATA *pReset;
    MPROG_LIST *list;
    int target;

    if (!(pRoomIndex = get_room_index(vnum)))
    {
        send_to_char("Portal tester: Can't find room.\n\r", ch);
        return;
    }
    pObjIndex = new_obj_index();
    pObjIndex->vnum = vnum;
    pObjIndex->material = str_dup("ether");
    pObjIndex->name = "leap portal";
    pObjIndex->short_descr = "a verb portal";
    pObjIndex->description = "You could \e[1;32mleap\e[0m through this portal.\n\r";
    pObjIndex->condition = 100;
    pObjIndex->weight = 0;
    pObjIndex->cost = 0;
    pObjIndex->level = 0;
    if (can_leave)
        target = get_random_room_area(0);
    else
        target = get_random_room_area(pRoomIndex->area->vnum);
    pObjIndex->ospec_fun = ospec_lookup("spec_verb");

    pReset = new_reset_data();
    pReset->command = 'O';
    // pReset->arg1 = pRoomIndex->reset_last + 1;
    pReset->arg2 = 1;
    pReset->arg4 = 1;
    add_reset(pRoomIndex, pReset, -1);

    code = new_mpcode();
    code->vnum = vnum;
    sprintf(code->code, "mob echoat $n You leap through!\n\rmob trans $n %d\n\r", target);
    code->next = mprog_list;
    mprog_list = code;

    list = new_mprog();
    list->vnum = vnum;
    list->trig_type = TRIG_VERB;
    list->trig_phrase = str_dup("leap");
    list->code = code->code;
    SET_BIT(pObjIndex->mprog_flags, list->trig_type);
    list->next = pObjIndex->mprogs;
    pObjIndex->mprogs = list;

    reset_room(pRoomIndex);
    sprintf(buf, "Portal %d created at room %d.\n\r", pObjIndex->vnum, pRoomIndex->vnum);
    send_to_char(buf, ch);
}

void create_normal_portal(CHAR_DATA *ch, int vnum, bool can_leave)
{
}

sh_int get_random_room_area(int area_vnum)
{
    ROOM_INDEX_DATA *pRoomIndex;
    int count = 0;
    int vnum;

    for (vnum = 0; vnum <= top_room; vnum++)
    {
        if ((pRoomIndex = get_room_index(vnum)) == NULL)
            continue;

        if (area_vnum > 0)
        {
            if (pRoomIndex->area->vnum == area_vnum)
            {
                count++;
                if (count == number_mm() % count)
                    return pRoomIndex->vnum;
            }
        }
        else
        {
            count++;
            if (count == number_mm() % count)
                return pRoomIndex->vnum;
        }
    }

    return 0;
}
