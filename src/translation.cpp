#include "translation.hpp"

using namespace EADK;

// Simple function that inits some vars
// or resets them if that's what we're
// trying to do :)
void emuInit() {

    Celeste_P8_set_rndseed(random());

    memcpy(&palette, defltPalette, sizeof(defltPalette));

    return;
}

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

static void emuRectFill(int x, int y, int width, int height, int color);
static void emuLine(int startX, int startY, int finishX, int finishY, unsigned char color);
static void emuPrint(const char* str, int x, int y, int color);

// This one is pulled directly from Lemon's Code
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
    static int cameraX, cameraY = 0;
	if (!screenShake) {
		cameraX, cameraY = 0;
	}

	va_list args;
	int ret = 0;
	va_start(args, call);
	
	#define INT_ARG() va_arg(args, int)
	#define BOOL_ARG() (Celeste_P8_bool_t)va_arg(args, int)
	#define RET_INT(_i)   do {ret = (_i); goto end;} while (0)
	#define RET_BOOL(_b) RET_INT(!!(_b))

    #define drawColor(col)(x) palette[x%16]

	switch (call) {
		case CELESTE_P8_SPR: { //spr(sprite,x,y,cols,rows,flipx,flipy)
			int sprite = INT_ARG();
			int x = INT_ARG();
			int y = INT_ARG();
			int cols = INT_ARG();
			int rows = INT_ARG();
			int flipx = BOOL_ARG();
			int flipy = BOOL_ARG();

			(void)cols;
			(void)rows;

			assert(rows == 1 && cols == 1);

			if (sprite >= 0) {
				SDL_Rect srcrc = {
					8*(sprite % 16),
					8*(sprite / 16)
				};
				srcrc.x *= renderScale;
				srcrc.y *= renderScale;
				srcrc.w = srcrc.h = renderScale*8;
				SDL_Rect dstrc = {
					(x - cameraX)*renderScale, (y - cameraY)*renderScale,
					renderScale, renderScale
				};
				Xblit(gfx, &srcrc, screen, &dstrc, 0,flipx,flipy); // TODO : replace with direct render of the sprite
			}
		} break;
		case CELESTE_P8_BTN: { //btn(b)
			int b = INT_ARG();
			assert(b >= 0 && b <= 5); 
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
			int col = INT_ARG();

			if (r <= 1) { // TODO : Replace all SDL stuff
				SDL_FillRect(screen, &(SDL_Rect){renderScale*(cx-1), renderScale*cy, renderScale*3, renderScale}, drawColor(col));
				SDL_FillRect(screen, &(SDL_Rect){renderScale*cx, renderScale*(cy-1), renderScale, renderScale*3}, drawColor(col));
			} else if (r <= 2) {
				SDL_FillRect(screen, &(SDL_Rect){renderScale*(cx-2), renderScale*(cy-1), renderScale*5, renderScale*3}, drawColor(col));
				SDL_FillRect(screen, &(SDL_Rect){renderScale*(cx-1), renderScale*(cy-2), renderScale*3, renderScale*5}, drawColor(col));
			} else if (r <= 3) {
				SDL_FillRect(screen, &(SDL_Rect){renderScale*(cx-3), renderScale*(cy-1), renderScale*7, renderScale*3}, drawColor(col));
				SDL_FillRect(screen, &(SDL_Rect){renderScale*(cx-1), renderScale*(cy-3), renderScale*3, renderScale*7}, drawColor(col));
				SDL_FillRect(screen, &(SDL_Rect){renderScale*(cx-2), renderScale*(cy-2), renderScale*5, renderScale*5}, drawColor(col));
			} else { //i dont think the game uses this
				int f = 1 - r; //used to track the progress of the drawn circle (since its semi-recursive)
				int ddFx = 1; //step x
				int ddFy = -2 * r; //step y
				int x = 0;
				int y = r;

				//this algorithm doesn't account for the diameters
				//so we have to set them manually
				emuLine(cx,cy-y, cx,cy+r, col);
				emuLine(cx+r,cy, cx-r,cy, col);

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
					emuLine(cx+x,cy+y, cx-x,cy+y, col);
					emuLine(cx+x,cy-y, cx-x,cy-y, col);
					emuLine(cx+y,cy+x, cx-y,cy+x, col);
					emuLine(cx+y,cy-x, cx-y,cy-x, col);
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
						SDL_Rect srcrc = {
							8*(tile % 16),
							8*(tile / 16)
						};
						srcrc.x *= renderScale;
						srcrc.y *= renderScale;
						srcrc.w = srcrc.h = renderScale*8;
						SDL_Rect dstrc = {
							(tx+x*8 - cameraX)*renderScale, (ty+y*8 - cameraY)*renderScale,
							renderScale*8, renderScale*8
						};

						if (0) {
							srcrc.x = srcrc.y = 0;
							srcrc.w = srcrc.h = 8;
							dstrc.x = x*8, dstrc.y = y*8;
							dstrc.w = dstrc.h = 8;
						}

						Xblit(gfx, &srcrc, screen, &dstrc, 0, 0, 0); // TODO : replace with direct render of the tile
					}
				}
			}
		} break;
	}

	end:
	va_end(args);
	return ret;
}