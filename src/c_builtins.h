#include <guile/gh.h>

SCM fire_weapon(void);
SCM player_x(void);
SCM player_y(void);
SCM player_angle(void);
SCM player_accel(void);
SCM computer_x(void);
SCM computer_y(void);
SCM computer_angle(void);
SCM computer_accel(void);

SCM set_player_x(SCM p_x);
SCM set_player_y(SCM p_y);
SCM set_player_angle(SCM p_an);
SCM set_player_accel(SCM p_ac);
SCM set_computer_x(SCM c_x);
SCM set_computer_y(SCM c_y);
SCM set_computer_angle(SCM c_an);
SCM set_computer_accel(SCM c_ac);

SCM world_width(void);
SCM world_height(void);
SCM player_forward_thrust(void);
SCM player_reverse_thrust(void);

SCM scm_atan2(SCM y, SCM x);
