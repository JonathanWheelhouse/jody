#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <guile/gh.h>
#include "c_builtins.h"
#include "scripting.h"
#include "player.h"

void InitScripting(void)
{
    gh_new_procedure("fire_weapon", fire_weapon,0,0,0);
    gh_new_procedure("player_x", player_x,0,0,0);
    gh_new_procedure("player_y", player_y,0,0,0);
    gh_new_procedure("player_angle", player_angle,0,0,0);
    gh_new_procedure("player_accel", player_accel,0,0,0);
    gh_new_procedure("computer_x", computer_x,0,0,0);
    gh_new_procedure("computer_y", computer_y,0,0,0);
    gh_new_procedure("computer_angle", computer_angle,0,0,0);
    gh_new_procedure("computer_accel", computer_accel,0,0,0);

    gh_new_procedure("set_player_x", set_player_x,1,0,0);
    gh_new_procedure("set_player_y", set_player_y,1,0,0);
    gh_new_procedure("set_player_angle", set_player_angle,1,0,0);
    gh_new_procedure("set_player_accel", set_player_accel,1,0,0);
    gh_new_procedure("set_computer_x", set_computer_x,1,0,0);
    gh_new_procedure("set_computer_y", set_computer_y,1,0,0);
    gh_new_procedure("set_computer_angle", set_computer_angle,1,0,0);
    gh_new_procedure("set_computer_accel", set_computer_accel,1,0,0);

    gh_new_procedure("world_width", world_width,0,0,0);
    gh_new_procedure("world_height", world_height,0,0,0);
    gh_new_procedure("player_forward_thrust", player_forward_thrust,0,0,0);
    gh_new_procedure("player_reverse_thrust", player_reverse_thrust,0,0,0);

    gh_new_procedure("scm_atan2",scm_atan2,2,0,0);
}

/* Cleans up after our scripting system. */
void CleanupScripting(void)
{
}

int LoadGameScript(char *filename)
{
    gh_load(filename);
    return 0;
}

/* Runs the game script's update function (named "playComputer").
   Returns 0 on success, -1 on failure. */
int RunGameScript()
{
    SCM obj;
    obj = gh_eval_str("(play_computer)");

    /* Enforce limits on the script. It can still "cheat" by turning its ship
       more quickly than the player or by directly modifying its position
       variables, but that's not too much of a problem. We can more or less
       trust the script (it's part of the game). */
    if (opponent.accel > PLAYER_FORWARD_THRUST)
		opponent.accel = PLAYER_FORWARD_THRUST;
    if (opponent.accel < PLAYER_REVERSE_THRUST)
		opponent.accel = PLAYER_REVERSE_THRUST;
    while (opponent.angle >= 360)
		opponent.angle -= 360;
    while (opponent.angle < 0)
		opponent.angle += 360;

    return 0;
}
