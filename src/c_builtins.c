#include <stdio.h>
#include <math.h>
     
#include <guile/gh.h>
     
#include "c_builtins.h"
#include "gamedefs.h"
#include "player.h"

SCM fire_weapon(void)
{
    static int i = 1;
    fprintf(stderr, "%d Computer is firing weapon. This doesn't work yet.\n", i++);
    return 0;
}
SCM player_x(void)
{
    return gh_double2scm(player.world_x);
}
SCM player_y(void)
{
    return gh_double2scm(player.world_y);
}
SCM player_angle(void)
{
    return gh_double2scm(player.angle);
}
SCM player_accel(void)
{
    return gh_double2scm(player.accel);
}
SCM computer_x(void)
{
    return gh_double2scm(opponent.world_x);
}
SCM computer_y(void)
{
    return gh_double2scm(opponent.world_y);
}
SCM computer_angle(void)
{
    return gh_double2scm(opponent.angle);
}
SCM computer_accel(void)
{
    return gh_double2scm(opponent.accel);
}

SCM set_player_x(SCM p_x)
{
    double x = gh_scm2double(p_x);
    player.world_x = x;
    return 0;
}
SCM set_player_y(SCM p_y)
{
    double x = gh_scm2double(p_y);
    player.world_x = x;
    return 0;
}
SCM set_player_angle(SCM p_an)
{
    double an = gh_scm2double(p_an);
    player.angle = an;
    return 0;
}
SCM set_player_accel(SCM p_ac)
{
    double ac = gh_scm2double(p_ac);
    player.accel = ac;
    return 0;
}
SCM set_computer_x(SCM c_x)
{
    double x = gh_scm2double(c_x);
    opponent.world_x = x;
    return 0;
}
SCM set_computer_y(SCM c_y)
{
    double y = gh_scm2double(c_y);
    opponent.world_y = y;
    return 0;
}
SCM set_computer_angle(SCM c_an)
{
    double an = gh_scm2double(c_an);
    opponent.angle = an;
    return 0;
}
SCM set_computer_accel(SCM c_ac)
{
    double ac = gh_scm2double(c_ac);
    opponent.accel = ac;
    return 0;
}

SCM world_width(void)
{
    long i = WORLD_WIDTH;
    return gh_long2scm(i);
}
SCM world_height(void)
{
    long i = WORLD_HEIGHT;
    return gh_long2scm(i);
}
SCM player_forward_thrust(void)
{
    long i = PLAYER_FORWARD_THRUST;
    return gh_long2scm(i);
}
SCM player_reverse_thrust(void)
{
    long i = PLAYER_REVERSE_THRUST;
    return gh_long2scm(i);
}

SCM scm_atan2(SCM y, SCM x)
{
    double Y = gh_scm2double(y);
    double X = gh_scm2double(x);
    double res;
    res = atan2(Y, X);
    return gh_double2scm(res);
}
