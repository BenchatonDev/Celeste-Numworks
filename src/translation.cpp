#include "translation.hpp"

using namespace EADK;

// Emulator related variables
int palette[16];
bool screenShake = true;
bool pauseEmu = false;
void *gameState = NULL;
uint16_t emuBtnState = 0;
uint16_t lastEmuBtnState = 0;
int renderScale = 2;

// Input related variables :
EADK::Keyboard::State state = 0;
EADK::Keyboard::State lastState = 0;

// That's where we're handling the actual inputs
// for the game and OSD (It's here to !)
void emuInput() {
    lastState = state;
    state = Keyboard::scan();

    // Emulator input (functions such as pause etc)
    if (state.keyDown(Keyboard::Key::Backspace)
        && !lastState.keyDown(Keyboard::Key::Backspace)) { pauseEmu = !pauseEmu; }

    // TODO : Handle game reset, maybe Saving and loading too

    // Actual game input
    if (state.keyDown(Keyboard::Key::Left))  emuBtnState |= (1<<0);
	if (state.keyDown(Keyboard::Key::Right)) emuBtnState |= (1<<1);
	if (state.keyDown(Keyboard::Key::Up))    emuBtnState |= (1<<2);
	if (state.keyDown(Keyboard::Key::Down))  emuBtnState |= (1<<3);
	if (state.keyDown(Keyboard::Key::Back)) emuBtnState |= (1<<4);
	if (state.keyDown(Keyboard::Key::OK)) emuBtnState |= (1<<5);
}

// Renders a sprite from the given sprite sheet
// at the given coordinates on the actual screen
template <size_t sprites, size_t rows, size_t columns>
void emuSprtRender(int sprt, int x, int y, bool flipX, bool flipY, const uint8_t (&sheet)[sprites][rows][columns], int colorOverride) {
	#define pixelColor(x, y) ((colorOverride) != -1 ? palette[colorOverride%16] : palette[sheet[sprt][y][x]%16])
	
    // Don't render empty sprites or sprites outside the sheet
	static const int noSprite[8][8] = {0};
    if (!memcmp(sheet[sprt], noSprite, sizeof(noSprite)) || 
		sprt >= sprites || sprt < 0) { return; };

    // Setting the right values to flip or not the sprite
	// Better flipping tech from framebuffer-dev branch
	//(maybe not in performance but it fixes the way I flip sprites
	// when trying to render one edge outside the screen)
    #define posX(iX) flipX ? (8 - 1) - iX : iX
	#define posY(iY) flipY ? (8 - 1) - iY : iY

    // Actually rendering the sprite
    for (int iY = 0; iY != 8; iY++) {
		// Don't render pixels that are out of the emulated display
		// area on the Y axis or transparent rows (Idea from framebuffer-dev branch)
		if (y + iY < 0 || y + iY >= pico8Size ||
			!memcmp(sheet[sprt][posY(iY)], noSprite[0], sizeof(noSprite[0])))
			{ continue; }

        for (int iX = 0; iX != 8; iX++) {
            // I consider black pixels as transparent same with pixels
            // that are out of the emulated display area on the X axis
            if (palette[(sheet)[sprt][posY(iY)][posX(iX)]%16] == 0 || x + iX < 0 || x + iX >= pico8Size) { continue; }
			
			// Small change, I store the placing position to do one final bounds check
			int pX = pico8XOrgin + ((x + iX) * renderScale);
			int pY = pico8YOrgin + ((y + iY) * renderScale);

			if (pX < 0 || pY < 0 || pX > screenW || pY > screenH) { continue; }
            Display::pushRectUniform(Rect(pX, pY, renderScale, renderScale),
									 pixelColor(posX(iX), posY(iY)));
        }
    }
	#undef pixelColor
}

// First piece code out of many to be a near 1:1 to
// Lemon's code, I'm not reimagining the wheel :)
void emuPrint(const char* str, int x, int y, int color) {
	for (char c = *str; c; c = *(++str)) {
		// This turns the character into it's ASCII number
		// which is it's sprite number in the font sheet
		c &= 0x7F;

		// Just render the char whih the given color override
		emuSprtRender(c, x, y, false, false, fontSprtSheet, color);
		x += 4;
	}
}

#define drawColor(color) palette[color%16]
void emuRectFill(int x, int y, int width, int height, int color) {
	int sX = x, sY = y;
	int eX = (sX + width), eY = (sY + height);

	// Firts we check to see if the whole thing even is in the
	// Emulated screen, if not why bother ?
	if (eX <= 0 || eY <= 0 || sX >= pico8Size || sY >= pico8Size)
	{ return; }
	
	// Then we make sure the rect we try to render is cut to fit
	// inside the emulated screen using Lemon's CLAMP macro
	#define CLAMP(v,min,max) v = v < min ? min : v >= max ? max : v
	CLAMP(sX, 0, pico8Size); CLAMP(sY, 0, pico8Size);
	CLAMP(eX, 0, pico8Size); CLAMP(eY, 0, pico8Size);
	
	// Now that we know it's fully and only on the emulated Screen
	// we want to know if it's drawable on the physical screen
	sX = pico8XOrgin + (sX * renderScale), sY = pico8YOrgin + (sY * renderScale);
	eX = pico8XOrgin + (eX * renderScale), eY = pico8YOrgin + (eY * renderScale);
	if (eX <= 0 || eY <= 0 || sX >= screenW || sY >= screenH)
	{ return; }

	// And then we clamp again but this time to fit the actual screen
	// So if we have a renderScale > 1 we don't have drawing artefacts
	CLAMP(sX, 0, screenW); CLAMP(sY, 0, screenH);
	CLAMP(eX, 0, screenW); CLAMP(eY, 0, screenH);
	#undef CLAMP

	if ((eX > sX) && (eY > sY)) {
		Display::pushRectUniform(Rect(sX, sY, (eX - sX), (eY - sY)), drawColor(color));
	}
}

// A function directly pulled from Lemon's implementation
void emuLine(int startX, int startY, int finishX, int finishY, int color) {
  #define PLOT(x,y) do {                                                        \
	 emuRectFill(x, y, 1, 1, color); \
	} while (0)
	int sx, sy, dx, dy, err, e2;
	dx = abs(finishX - startX);
	dy = abs(finishY - startY);
	if (!dx && !dy) return;

	if (startX < finishX) sx = 1; else sx = -1;
	if (startY < finishY) sy = 1; else sy = -1;
	err = dx - dy;
	if (!dy && !dx) return;
	else if (!dx) { //vertical line
		for (int y = startY; y != finishY; y += sy) PLOT(startX,y);
	} else if (!dy) { //horizontal line
		for (int x = startX; x != finishX; x += sx) PLOT(x,startY);
	} while (startX != finishX || startY != finishY) {
		PLOT(startX, startY);
		e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			startX += sx;
		}
		if (e2 < dx) {
			err += dx;
			startY += sy;
		}
	}
	#undef PLOT
}

// Another function pulled directly from Lemon's Code
static int getTileFlag(int tile, int flag) {
	return tile < sizeof(tile_flags)/sizeof(*tile_flags) && (tile_flags[tile] & (1 << flag)) != 0;
}

// More code directly pulled from Lemon's Code
// On-screen display (for info, such as loading a state, toggling screenshake, etc)
static char osdText[200] = "";
static int osdTimer = 0;
static void OSDset(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	osdText[sizeof osdText - 1] = '\0'; //make sure to add NUL terminator in case of truncation
	osdTimer = 30;
	va_end(ap);
}
static void OSDdraw(void) {
	if (osdTimer > 0) {
		--osdTimer;
		const int x = 4;
		const int y = 120 + (osdTimer < 10 ? 10-osdTimer : 0); //disappear by going below the screen
		emuRectFill(x-2, y-2, x+4*strlen(osdText), y+6, 6); //outline
		emuRectFill(x-1, y-1, x+4*strlen(osdText)-1, y+5, 0);
		emuPrint(osdText, x, y, 7);
	}
}

// It's basicly the pico8emu() function from Lemon's code
// with the required adjustements and modifications
int emulator(CELESTE_P8_CALLBACK_TYPE call, ...) {
    static int cameraX = 0, cameraY = 0;
	if (!screenShake) {
		cameraX = 0, cameraY = 0;
	}

	va_list args;
	int ret = 0;
	va_start(args, call);
	
	#define INT_ARG() va_arg(args, int)
	#define BOOL_ARG() (Celeste_P8_bool_t)va_arg(args, int)
	#define RET_INT(_i)   do {ret = (_i); goto end;} while (0)
	#define RET_BOOL(_b) RET_INT(!!(_b))

	switch (call) {
		case CELESTE_P8_SPR: { //spr(sprite,x,y,cols,rows,flipx,flipy)
			int sprt = INT_ARG();
			int x = INT_ARG();
			int y = INT_ARG();
			int cols = INT_ARG();
			int rows = INT_ARG();
			int flipX = BOOL_ARG();
			int flipY = BOOL_ARG();

			(void)cols;
			(void)rows;

			if (!(rows == 1 && cols == 1)) { return 0; };

        	emuSprtRender(sprt, (x - cameraX), (y - cameraY), flipX, flipY, mainSprtSheet, -1);
		} break;

		case CELESTE_P8_BTN: { //btn(b)
			int b = INT_ARG();

			if (!(b >= 0 && b <= 5)) { return 0; };
			RET_BOOL(emuBtnState & (1 << b));
		} break;

		case CELESTE_P8_PAL: { //pal(a,b)
			int a = INT_ARG();
			int b = INT_ARG();
			if (a >= 0 && a < 16 && b >= 0 && b < 16) {
				//swap palette colors
				palette[a] = defltPalette[b];
			}
		} break;

		case CELESTE_P8_PAL_RESET: { //pal()
			memcpy(&palette, defltPalette, sizeof(defltPalette));
		} break;

		case CELESTE_P8_CIRCFILL: { //circfill(x,y,r,col)
			int cx = INT_ARG() - cameraX;
			int cy = INT_ARG() - cameraY;
			int r = INT_ARG();
			int color = INT_ARG();

			if (r <= 1) {
				emuRectFill((cx - 1), cy, 3, 1, color);
				emuRectFill(cx, (cy - 1), 1, 3, color);

			} else if (r <= 2) {
				emuRectFill((cx - 2), (cy - 1), 5, 3, color);
				emuRectFill((cx - 1), ((cy - 2)), 3, 5, color);

			} else if (r <= 3) {
				emuRectFill((cx - 3), (cy - 1), 7, 3, color);
				emuRectFill((cx - 1), (cy - 3), 3, 7, color);
				emuRectFill((cx - 2), (cy - 2), 5, 5, color);

			} else { //i dont think the game uses this
				int f = 1 - r; //used to track the progress of the drawn circle (since its semi-recursive)
				int ddFx = 1; //step x
				int ddFy = -2 * r; //step y
				int x = 0;
				int y = r;

				//this algorithm doesn't account for the diameters
				//so we have to set them manually
				emuLine(cx,cy-y, cx,cy+r, color);
				emuLine(cx+r,cy, cx-r,cy, color);

				while (x < y) {
					if (f >= 0) {
						y--;
						ddFy += 2;
						f += ddFy;
					}
					x++;
					ddFx += 2;
					f += ddFx;

					//build our current arc
					emuLine(cx+x,cy+y, cx-x,cy+y, color);
					emuLine(cx+x,cy-y, cx-x,cy-y, color);
					emuLine(cx+y,cy+x, cx-y,cy+x, color);
					emuLine(cx+y,cy-x, cx-y,cy-x, color);
				}
			}
		} break;

		case CELESTE_P8_PRINT: { //print(str,x,y,col)
			const char* str = va_arg(args, const char*);
			int x = INT_ARG() - cameraX;
			int y = INT_ARG() - cameraY;
			int col = INT_ARG() % 16;

			emuPrint(str,x,y,col);
		} break;

		case CELESTE_P8_RECTFILL: { //rectfill(x0,y0,x1,y1,col)
			int x0 = INT_ARG() - cameraX;
			int y0 = INT_ARG() - cameraY;
			int x1 = INT_ARG() - cameraX;
			int y1 = INT_ARG() - cameraY;
			int col = INT_ARG();

			emuRectFill(x0,y0,x1,y1,col);
		} break;

		case CELESTE_P8_LINE: { //line(x0,y0,x1,y1,col)
			int x0 = INT_ARG() - cameraX;
			int y0 = INT_ARG() - cameraY;
			int x1 = INT_ARG() - cameraX;
			int y1 = INT_ARG() - cameraY;
			int col = INT_ARG();

			emuLine(x0,y0,x1,y1,col);
		} break;

		case CELESTE_P8_MGET: { //mget(tx,ty)
			int tx = INT_ARG();
			int ty = INT_ARG();

			RET_INT(tilemap_data[tx+ty*128]);
		} break;

		case CELESTE_P8_CAMERA: { //camera(x,y)
			if (screenShake) {
				cameraX = INT_ARG();
				cameraY = INT_ARG();
			}
		} break;

		case CELESTE_P8_FGET: { //fget(tile,flag)
			int tile = INT_ARG();
			int flag = INT_ARG();

			RET_INT(getTileFlag(tile, flag));
		} break;

		case CELESTE_P8_MAP: { //map(mx,my,tx,ty,mw,mh,mask)
			int mx = INT_ARG(), my = INT_ARG();
			int tx = INT_ARG(), ty = INT_ARG();
			int mw = INT_ARG(), mh = INT_ARG();
			int mask = INT_ARG();
			
			for (int x = 0; x < mw; x++) {
				for (int y = 0; y < mh; y++) {
					int tile = tilemap_data[x + mx + (y + my)*128];
					//hack
					if (mask == 0 || (mask == 4 && tile_flags[tile] == 4) || getTileFlag(tile, mask != 4 ? mask-1 : mask)) {
						if (0) {
							emuSprtRender(tile, (x * 8 - cameraX), (y * 8 - cameraY), false, false, mainSprtSheet, -1);
							continue;
						}

						emuSprtRender(tile, (tx + x * 8 - cameraX), (ty + y * 8 - cameraY), false, false, mainSprtSheet, -1);
					}
				}
			}
		} break;
	}

	end:
	va_end(args);
	return ret;
}

// Simple function that inits some vars
// or resets them if that's what we're
// trying to do :)
void emuInit() {

	Celeste_P8_set_call_func(emulator);

    memcpy(&palette, defltPalette, sizeof(defltPalette));

	gameState = malloc(Celeste_P8_get_state_size());
	if (gameState) { Celeste_P8_save_state(gameState); }

	Celeste_P8_set_rndseed(EADK::random());

	Celeste_P8_init();

    return;
}

void gameMain() {
	lastEmuBtnState = emuBtnState;
	emuBtnState = 0;

	emuInput();

	if (pauseEmu) {
		const int x = pico8Size / 2 - 3 * 4, y = 8;

		emuRectFill(x - 1, y - 1, 6 * 4 + x + 1, 6 + y + 1, 6);
		emuRectFill(x, y, 6 * 4 + x, 6 + y, 0);
		emuPrint("paused", x + 1, y + 1, 7);
	} else {

		Celeste_P8_update();
		Celeste_P8_draw();
	}
	OSDdraw();
}
