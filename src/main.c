#define _GNU_SOURCE

#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "gamedefs.h"
#include "particle.h"
#include "background.h"
#include "resources.h"
#include "player.h"

/* Player data */
enum { OPP_COMPUTER, OPP_NETWORK } opponent_type;
player_t player;		/* the player sitting at the local keyboard */
player_t opponent;		/* scripted or networked opponent */

/* The current camera position */
static int camera_x, camera_y;	/* position of 640x480 viewport within world */

SDL_Surface *screen;		/* global for convenience */

/* Other misc. global variables */
int fullscreen = 0;
int hwsurface = 0;
int doublebuf = 0;

double time_scale = 0;

/* ==========
   Prototypes
   ========== */
static unsigned int getrandom(void);
static void initrandom(void);
static void DrawPlayer(player_p p);
static void InitPlayer(void);
static void UpdatePlayer(player_p p);
static void PlayGame(void);
static char * script_name(char *path);
static void play_computer(void);
static void attack(void);
static void evade(void);
static double dist_to_player(void);
static double angle_to_player(void);
static void fire_weapon(void);

/* This is a simple custom pseudorandom number generator. It's not a very
   good one, but it's sufficient for our purposes. Never trust the rand()
   included with the C library. Its quality varies between implementations,
   and it's easy to run into patterns within the generated numbers. At least
   this one is consistent. */
static Sint32 seed = 0;

static void initrandom(void)
{
	 seed = time(NULL);
}

static unsigned int getrandom(void)
{
	 Sint32 p1 = 1103515245;
	 Sint32 p2 = 12345;
	 seed = (seed*p1+p2) % 2147483647;
	 return (unsigned)seed/3;
}

/* =======
   Drawing
   ======= */

/* Draws the given player to the screen. */
static void DrawPlayer(player_p p)
{
	 SDL_Rect src, dest;
	 int angle;

	 /* Calculate the player's new screen coordinates. */
	 p->screen_x = (int)p->world_x - camera_x;
	 p->screen_y = (int)p->world_y - camera_y;

	 /* If the player isn't on the screen, don't draw anything. */
	 if (p->screen_x < -PLAYER_WIDTH/2 ||
		 p->screen_x >= SCREEN_WIDTH+PLAYER_WIDTH/2)
		  return;

	 if (p->screen_y < -PLAYER_HEIGHT/2 ||
		 p->screen_y >= SCREEN_HEIGHT+PLAYER_HEIGHT/2)
		  return;

	 /* Calculate drawing coordinates. */
	 angle = p->angle;
	 if (angle < 0) angle += 360;
	 src.x = PLAYER_WIDTH * (angle / 4);
	 src.y = 0;
	 src.w = PLAYER_WIDTH;
	 src.h = PLAYER_HEIGHT;
	 dest.x = p->screen_x - PLAYER_WIDTH/2;
	 dest.y = p->screen_y - PLAYER_HEIGHT/2;
	 dest.w = PLAYER_WIDTH;
	 dest.h = PLAYER_HEIGHT;

	 /* Draw the sprite. */
	 SDL_BlitSurface(ship_strip,&src,screen,&dest);
}

/* =============
   Player Update
   ============= */

/* Initializes the player. */
static void InitPlayer(void)
{
	 player.world_x = getrandom() % WORLD_WIDTH;
	 player.world_y = getrandom() % WORLD_HEIGHT;
	 player.accel = 0;
	 player.velocity = 0;
	 player.angle = 0;
	 UpdatePlayer(&player);
}

/* Initializes the opponent (either computer- or network-controlled). */
static void InitOpponent(void)
{
	 if (opponent_type == OPP_COMPUTER) {
		  opponent.world_x = getrandom() % WORLD_WIDTH;
		  opponent.world_y = getrandom() % WORLD_HEIGHT;
		  opponent.accel = 0;
		  opponent.velocity = 0;
		  opponent.angle = 0;
	 } else {	/* network opponent */
	 }
}

/* Calculates a player's new world coordinates based on the camera
   and the player's velocity. Adds acceleration to velocity. Uses simple
   trigonometry to update the world coordinates. */
static void UpdatePlayer(player_p p)
{
	 float angle;

	 angle = (float)p->angle;

	 p->velocity += p->accel * time_scale;
	 if (p->velocity > PLAYER_MAX_VELOCITY) p->velocity = PLAYER_MAX_VELOCITY;
	 if (p->velocity < PLAYER_MIN_VELOCITY) p->velocity = PLAYER_MIN_VELOCITY;

	 p->world_x += p->velocity * cos(angle*PI/180.0) * time_scale;
	 p->world_y += p->velocity * -sin(angle*PI/180.0) * time_scale;

	 /* Make sure the player doesn't slide off the edge of the world. */
	 if (p->world_x < 0) p->world_x = 0;
	 if (p->world_x >= WORLD_WIDTH) p->world_x = WORLD_WIDTH-1;
	 if (p->world_y < 0) p->world_y = 0;
	 if (p->world_y >= WORLD_HEIGHT) p->world_y = WORLD_HEIGHT-1;

}

/* ==============
   Main Game Loop
   ============== */

static void PlayGame(void)
{
	 Uint8 *keystate;
	 int quit = 0;
	 int turn;
	 int prev_ticks = 0, cur_ticks = 0; /* for keeping track of timing */

	 /* framerate counter variables */
	 int start_time, end_time;
	 int frames_drawn = 0;

	 prev_ticks = SDL_GetTicks();

	 start_time = time(NULL);
	 while (quit == 0) {

		  /* Determine how many milliseconds have passed since
			 the last frame, and update our motion scaling. */

		  prev_ticks = cur_ticks;
		  cur_ticks = SDL_GetTicks();
		  time_scale = (double)(cur_ticks-prev_ticks)/30.0;

		  /* Update SDL's internal input state information. */
		  SDL_PumpEvents();

		  /* Grab a snapshot of the keyboard. */
		  keystate = SDL_GetKeyState(NULL);

		  /* Respond to input. */
		  if (keystate[SDLK_q] || keystate[SDLK_ESCAPE]) quit = 1;

		  /* Left and right arrow keys control turning. */
		  turn = 0;
		  if (keystate[SDLK_LEFT]) turn += 15;
		  if (keystate[SDLK_RIGHT]) turn -= 15;

		  /* Forward and back arrow keys activate thrusters. */
		  player.accel = 0;
		  if (keystate[SDLK_UP]) player.accel = PLAYER_FORWARD_THRUST;
		  if (keystate[SDLK_DOWN]) player.accel = PLAYER_REVERSE_THRUST;

		  /* Spacebar slows the ship down. */
		  if (keystate[SDLK_SPACE]) {
			   player.velocity *= 0.8;
		  }

		  /* Just an amusing way to test the particle system. */
		  if (keystate[SDLK_e]) {
			   CreateParticleExplosion(
					player.world_x, player.world_y, 255, 255, 255, 10, 300);
			   CreateParticleExplosion(
					player.world_x, player.world_y, 255, 0, 0, 5, 100);
			   CreateParticleExplosion(
					player.world_x, player.world_y, 255, 255, 0, 2, 50);
		  }

		  /* Allow a turn of four degrees per frame. */
		  player.angle += turn * time_scale;
		  if (player.angle < 0) player.angle += 360;
		  if (player.angle >= 360) player.angle -= 360;

		  /* If this is a player vs. computer game, give the computer a chance. */
		  if (opponent_type == OPP_COMPUTER)
			   play_computer();

		  /* Update the player's position. */
		  UpdatePlayer(&player);
		  UpdatePlayer(&opponent);

		  /* Make the camera follow the player (but impose limits). */
		  camera_x = player.world_x - SCREEN_WIDTH/2;
		  camera_y = player.world_y - SCREEN_HEIGHT/2;

		  if (camera_x < 0) camera_x = 0;
		  if (camera_x >= WORLD_WIDTH-SCREEN_WIDTH)
			   camera_x = WORLD_WIDTH-SCREEN_WIDTH-1;
		  if (camera_y < 0) camera_y = 0;
		  if (camera_y >= WORLD_HEIGHT-SCREEN_HEIGHT)
			   camera_y = WORLD_HEIGHT-SCREEN_HEIGHT-1;

		  /* Update the particle system. */
		  UpdateParticles();

		  /* Redraw everything. */
		  DrawBackground(screen, camera_x, camera_y);
		  DrawParallax(screen, camera_x, camera_y);
		  DrawParticles(screen, camera_x, camera_y);
		  DrawPlayer(&player);
		  DrawPlayer(&opponent);

		  /* Flip the page. */
		  SDL_Flip(screen);

		  frames_drawn++;

	 }

	 end_time = time(NULL);
	 if (start_time == end_time) end_time++;

	 /* Display the average framerate. */
	 printf("Drew %i frames in %i seconds, for a framerate of %.2f fps.\n",
			frames_drawn,
			end_time-start_time,
			(float)frames_drawn/(float)(end_time-start_time));

}

int main(int argc, char *argv[])
{
	 enum { GAME_COMPUTER, GAME_NETWORK, GAME_UNKNOWN } game_type = GAME_UNKNOWN;
	 char *remote_address;
	 int i;

	 if (argc < 2) {
		  printf("jody\n");
		  printf("Usage: jody [ mode ] [ option ... ]\n");
		  printf("  Game modes (choose one):\n");
		  printf("    --computer\n");
		  printf("    --net <remote ip address>\n");
		  printf("  Display options, for tweaking and experimenting:\n");
		  printf("    --fullscreen\n");
		  printf("    --hwsurface  (use an SDL hardware surface if possible)\n");
		  printf("    --doublebuf  (use SDL double buffering if possible)\n");
		  printf("  The display options default to a windowed, software buffer.\n");
		  exit(1);
	 }

	 /* Process command line arguments. There are plenty of libraries for command
		line processing, but our needs are simple in this case. */
	 for (i = 0; i < argc; i++) {

		  /* Networked or scripted opponent? */
		  if (!strcmp(argv[i], "--computer")) {
			   game_type = GAME_COMPUTER;
			   continue;
		  } else if (!strcmp(argv[i], "--net")) {
			   game_type = GAME_NETWORK;
			   if (++i >= argc) {
					printf("You must supply an IP address for --net.\n");
					exit(1);
			   }
			   remote_address = argv[i];
			   continue;

			   /* Display-related options */
		  } else if (!strcmp(argv[i], "--hwsurface")) {
			   hwsurface = 1;
		  } else if (!strcmp(argv[i], "--doublebuf")) {
			   doublebuf = 1;
		  } else if (!strcmp(argv[i], "--fullscreen")) {
			   fullscreen = 1;
		  }
	 }

	 switch (game_type) {
	 case GAME_COMPUTER:
		  opponent_type = OPP_COMPUTER;
		  printf("Playing against the computer.\n");
		  break; 

	 case GAME_NETWORK:
		  printf("Sorry, network play is not implemented... yet!\n");
		  exit(1);

	 default:
		  printf("You must select a game type.\n");
		  exit(1);
	 }

	 /* Initialize our random number generator. */
	 initrandom();

	 /* Fire up SDL. */
	 if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		  printf("Unable to initialize SDL: %s\n", SDL_GetError());
		  exit(1);
	 }
	 atexit(SDL_Quit);

	 /* Set an appropriate 16-bit double buffered video mode. */
	 if (SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16,
						  (hwsurface ? SDL_HWSURFACE : SDL_SWSURFACE) |
						  (doublebuf ? SDL_DOUBLEBUF : 0) |
						  (fullscreen ? SDL_FULLSCREEN : 0)) == NULL) {
		  printf("Unable to set video mode: %s\n", SDL_GetError());
		  exit(1);
	 }

	 /* Save the screen pointer for later use. */
	 screen = SDL_GetVideoSurface();

	 /* Set the window caption to the name of the game. */
	 SDL_WM_SetCaption("Jody", "Jody");

	 /* Load the game's data into globals. */
	 LoadGameData();

	 /* Initialize the background starfield. */
	 InitBackground();

	 /* Play! */
	 InitPlayer();
	 InitOpponent();
	 PlayGame();

	 /* Unload data. */
	 UnloadGameData();

	 exit(0);
}

static char * script_name(char *dir)
{
	 size_t dir_len = strlen(dir);
	 char *all = malloc(dir_len + 9);
	 char *start = all;
	 all = mempcpy(all, dir, dir_len);
	 all = mempcpy(all, "/jody.scm", 9);
	 *all = '\0';
	 return start;
}

enum STATE { ATTACK, EVADE };
enum STATE state = ATTACK;
int target_x = 0;
int target_y = 0;
static void play_computer(void)
{
	 if (state == ATTACK)
		  attack();
	 else
		  evade();

	 double arc = angle_to_player() - opponent.angle;
	 if (arc < 0)
		  arc += 360;
	 double angle = 0;
	 if (arc < 180)
		  angle = opponent.angle + 3;
	 else
		  angle = opponent.angle - 3;
	 opponent.angle = angle;

	 if (opponent.accel > PLAYER_FORWARD_THRUST)
		  opponent.accel = PLAYER_FORWARD_THRUST;
	 if (opponent.accel < PLAYER_REVERSE_THRUST)
		  opponent.accel = PLAYER_REVERSE_THRUST;
	 while (opponent.angle >= 360)
		  opponent.angle -= 360;
	 while (opponent.angle < 0)
		  opponent.angle += 360;
}
static void attack(void)
{
	 target_x = player.world_x;
	 target_y = player.world_y;
	 if (dist_to_player() < 30) {
		  state = EVADE;
		  target_x = -1;
	 } else {
		  if (dist_to_player() > 100) 
			   opponent.accel = PLAYER_FORWARD_THRUST;
		  else if (dist_to_player() > 50)
			   opponent.accel = PLAYER_FORWARD_THRUST / 3;
		  else
			   opponent.accel = 0;
		  if (dist_to_player() < 200)
			   fire_weapon();
	 }
}
static void evade(void)
{
	 double x2 = target_x - opponent.world_x;
	 double y2 = target_y - opponent.world_y;
 	 if ((fabs(x2) < 10) && ((fabs(y2) < 10))) {
		  printf("Going back into ATTACK mode.\n");
		  state = ATTACK;
	 } else {
		  if (target_x < 0) {
			   target_x = rand() * WORLD_WIDTH / RAND_MAX;
			   target_y = rand() * WORLD_HEIGHT / RAND_MAX;
			   printf("Selected new EVADE target.\n");
		  }
		  opponent.accel = PLAYER_FORWARD_THRUST;
	 }
}
static double dist_to_player(void)
{
	 double xdiff = opponent.world_x - player.world_x;
	 double ydiff = opponent.world_y - player.world_y;
	 return sqrt((xdiff * xdiff) + (ydiff * ydiff));
}
static double angle_to_player(void)
{
	 double xdiff = target_x - opponent.world_x;
	 double ydiff = target_y - opponent.world_y;
	 double theta = atan2(-ydiff, xdiff);
	 if (theta < 0)
		  theta += 2 * M_PI;
	 return theta * (180 / M_PI);
}
static void fire_weapon(void)
{
	 static int i = 1;
	 fprintf(stderr, "%d Computer is firing weapon. This doesn't work yet.\n", i++);
}
