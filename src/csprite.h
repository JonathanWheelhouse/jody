#ifndef __CSPRITE_H__
#define __CSPRITE_H__

#include <SDL/SDL.h>

struct CSpriteFrame
{
  SDL_Surface *image;
  int pause;
};

struct CSpriteBase
{
  struct CSpriteFrame *mAnim;
  int mBuilt, mNumframes, mW, mH;
};

struct CSprite
{
	int mFrame;
	int mX, mY, mOldX, mOldY;
	int mAnimating;
	int mDrawn;
	float mSpeed;
	long mLastupdate;
	struct CSpriteBase *mSpriteBase;
	SDL_Surface *mBackreplacement;
	SDL_Surface *mScreen;
};

extern struct CSpriteBase *base_init(char *dir);
extern struct CSprite *init(struct CSpriteBase *base, SDL_Surface *screen);

extern void draw(struct CSprite *sprite);
extern void clearBG(struct CSprite *sprite);
extern void updateBG(struct CSprite *sprite);

extern void setFrame(struct CSprite *sprite, int nr);
extern int getFrame();

extern void setSpeed(struct CSprite *sprite, float nr);
extern float getSpeed(struct CSprite *sprite);

extern void toggleAnim(struct CSprite *sprite);
extern void startAnim(struct CSprite *sprite);
extern void stopAnim(struct CSprite *sprite);
extern void rewind_frame(struct CSprite *sprite);

extern void xadd(struct CSprite *sprite, int nr);
extern void yadd(struct CSprite *sprite, int nr);
extern void xset(struct CSprite *sprite, int nr);
extern void yset(struct CSprite *sprite, int nr);
extern void set(struct CSprite *sprite, int xx, int yy);

#endif
