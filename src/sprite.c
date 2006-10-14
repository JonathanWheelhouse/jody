#include <SDL_image.h>
#include <string.h>
#include "sprite.h"

struct CSpriteBase *base_init(char *dir)
{
	char buffer[255];
	char filename[255];
	char name[255];
	int pause=0, r=0, g=0, b=0;
	FILE *fp;

	sprintf(filename, "%s/info", dir);

	if ((fp=fopen(filename, "r")) == NULL) {
		printf("ERROR opening file %s\n\n", filename);
		return NULL;
	}


	struct CSpriteBase *base = malloc(sizeof(struct CSpriteBase));
	base->mBuilt = base->mNumframes = base->mW = base->mH = 0;

	fgets(buffer, 255, fp);
	sscanf(buffer, "FILES: %d", &base->mNumframes);

	struct CSpriteFrame *mAnim = malloc(base->mNumframes * sizeof(struct CSpriteFrame));
  

	base->mBuilt = 1;
	int count = 0;

	while (!feof(fp) && count < base->mNumframes) {
		fgets(buffer, 255, fp);
		if (buffer[0] != '#' && buffer[0] != '\r' && buffer[0] != '\0' && buffer[0] != '\n' && strlen(buffer) != 0) {

			int match = sscanf(buffer, "%s %d %d %d %d", name, &pause, &r, &g, &b);
			if (match != 5)
				return NULL;

			sprintf(filename, "%s/%s", dir, name);

			SDL_Surface *temp;
			if((temp = IMG_Load(filename)) == NULL)
				return NULL;
			if(r >= 0)
				SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, r, g, b));

/* 			mAnim[count].image = SDL_DisplayFormat(temp); */
			mAnim[count].image = temp;
/* 			SDL_FreeSurface(temp); */

			mAnim[count].pause = pause;
			if (!base->mW)
				base->mW = mAnim[count].image->w;
			if (!base->mH)
				base->mH = mAnim[count].image->w;

			count++;
		}
	}
	fclose(fp);

	base->mAnim = mAnim;
	return base;
}

struct CSprite *init(struct CSpriteBase *base, SDL_Surface *screen)
{
	struct CSprite *sprite = malloc(sizeof(struct CSprite));
	sprite->mFrame = 0;
	sprite->mX = 0;
	sprite->mY = 0;
	sprite->mOldX = 0;
	sprite->mOldY = 0;
	sprite->mAnimating = 0;
	sprite->mDrawn = 0;
	sprite->mSpeed = 0;
	sprite->mLastupdate = 0;

	sprite->mSpriteBase = base;

	if (sprite->mSpriteBase->mBuilt) {
		if (sprite->mSpriteBase->mNumframes > 1)
			sprite->mAnimating = 1;
		sprite->mBackreplacement = SDL_DisplayFormat(sprite->mSpriteBase->mAnim[0].image);
	}
	sprite->mScreen = screen;
	return sprite;
}

void draw(struct CSprite *sprite)
{
	if (sprite->mAnimating == 1) {
		if (sprite->mLastupdate + sprite->mSpriteBase->mAnim[sprite->mFrame].pause * sprite->mSpeed < SDL_GetTicks()) {
			sprite->mFrame++;
			if (sprite->mFrame > sprite->mSpriteBase->mNumframes - 1)
				sprite->mFrame = 0;
			sprite->mLastupdate = SDL_GetTicks();
		}
	}

	if(sprite->mDrawn == 0)
		sprite->mDrawn = 1;

	SDL_Rect dest;
	dest.x = sprite->mX; dest.y = sprite->mY;
	SDL_BlitSurface(sprite->mSpriteBase->mAnim[sprite->mFrame].image, NULL, sprite->mScreen, &dest);
}

void clearBG(struct CSprite *sprite)
{
	if (sprite->mDrawn == 1) {
		SDL_Rect dest;
		dest.x = sprite->mOldX;
		dest.y = sprite->mOldY;
		dest.w = sprite->mSpriteBase->mW;
		dest.h = sprite->mSpriteBase->mH;
		SDL_BlitSurface(sprite->mBackreplacement, NULL, sprite->mScreen, &dest);
	}
}

void updateBG(struct CSprite *sprite)
{
	SDL_Rect srcrect;
	srcrect.w = sprite->mSpriteBase->mW;
	srcrect.h = sprite->mSpriteBase->mH;
	srcrect.x = sprite->mX;
	srcrect.y = sprite->mY;
	sprite->mOldX = sprite->mX;
	sprite->mOldY = sprite->mY;
	SDL_BlitSurface(sprite->mScreen, &srcrect, sprite->mBackreplacement, NULL);
}

void setFrame(struct CSprite *sprite, int nr) { sprite->mFrame = nr; }
int getFrame(struct CSprite *sprite) { return sprite->mFrame; }

void setSpeed(struct CSprite *sprite, float nr) { sprite->mSpeed = nr; }
float getSpeed(struct CSprite *sprite) { return sprite->mSpeed; }

void toggleAnim(struct CSprite *sprite) { sprite->mAnimating = !sprite->mAnimating; }
void startAnim(struct CSprite *sprite) { sprite->mAnimating = 1; }
void stopAnim(struct CSprite *sprite) { sprite->mAnimating = 0; }
void rewind_frame(struct CSprite *sprite) { sprite->mFrame = 0; }

void xadd(struct CSprite *sprite, int nr) { sprite->mX += nr; }
void yadd(struct CSprite *sprite, int nr) { sprite->mY += nr; }
void xset(struct CSprite *sprite, int nr) { sprite->mX = nr; }
void yset(struct CSprite *sprite, int nr) { sprite->mY = nr; }
void set(struct CSprite *sprite, int xx, int yy) { sprite->mX = xx; sprite->mY = yy; }
