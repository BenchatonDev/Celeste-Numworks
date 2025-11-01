#include "translation.hpp"

using namespace EADK;

// Rendering variables
eadk_color_t palette[16] = {0};
eadk_color_t frameBuffer[pico8Size][pico8Size] = {0};
eadk_color_t rowBuffer[(renderScale * pico8Size <= 256 ? 256 : renderScale * pico8Size)] = {0};

// Emulator related variables
bool screenShake = true;
bool pauseEmu = false;
uint16_t emuBtnState = 0;
uint16_t lastEmuBtnState = 0;

// Input related variables :
EADK::Keyboard::State state = 0;
EADK::Keyboard::State lastState = 0;

// Copies a sprite from the given sprite sheet
// at the given coordinates on the framebuffer
template <size_t sprites, size_t rows, size_t columns>
void emuSprtRender(uint8_t sprt, int16_t x, int16_t y, bool flipX, bool flipY, const uint8_t (&sheet)[sprites][rows][columns], int8_t colorOverride) {
	#define pixelColor(x, y) ((colorOverride) != -1 ? palette[colorOverride%16] : palette[sheet[sprt][y][x]%16])

	// Don't render empty sprites or sprites outside the sheet
	static const int noSprt[8][8] = {0};
    if (!memcmp(sheet[sprt], noSprt, sizeof(noSprt)) || 
		sprt >= sprites || sprt < 0) { return; };

	int cornerX = x + sprtSize;
	int cornerY = y + sprtSize;

	if (cornerX < 0 || x >= pico8Size || 
		cornerY < 0 || y >= pico8Size) { return; }
	
	// Calculating the sprite's clip if needed on the X axis
	uint8_t startCopyX = sprtSize - cornerX <= 0 ? 0 : sprtSize - cornerX >= 128 ? 127 : sprtSize - cornerX;
	uint8_t endCopyX = pico8Size - x >= sprtSize ? sprtSize : pico8Size - x > 128 ? 128 : pico8Size - x;

	// Copy pasted for the Y axis
	uint8_t startCopyY = sprtSize - cornerY <= 0 ? 0 : sprtSize - cornerY >= 128 ? 127 : sprtSize - cornerY;
	uint8_t endCopyY = pico8Size - y >= sprtSize ? sprtSize : pico8Size - y > 128 ? 128 : pico8Size - y;

	// Stuff for reversal
	#define posX(iX) flipX ? (sprtSize - 1) - iX : iX
	#define posY(iY) flipY ? (sprtSize - 1) - iY : iY

	// We, at last, write the sprite to our buffer
	for (uint8_t iY = startCopyY; iY < endCopyY; iY++) {
		// Don't process black raws (because index 0 will always mean black / transparent)
		if (!memcmp(sheet[sprt][posY(iY)], noSprt[0], sizeof(noSprt[0]))) { continue; }

		for (uint8_t iX = startCopyX; iX < endCopyX; iX++) {
			// Skip black pixels (because index 0 will always mean black / transparent)
			if (sheet[sprt][posY(iY)][posX(iX)] == 0) { continue; }

			// Place the color on the framebuffer
			frameBuffer[y + iY][x + iX] = pixelColor(posX(iX), posY(iY));
		}
	}

	// These macros shouldn't be used anywhere else
	#undef poxX
	#undef posY
	#undef pixelColor
}

// First piece code out of many to be a near 1:1 to
// Lemon's code, I'm not reimagining the wheel :)
// This functions prints the given str to the framebuffer
void emuPrint(const char* str, int16_t x, int16_t y, uint8_t color) {
	for (char c = *str; c; c = *(++str)) {
		// This turns the character into it's ASCII number
		// which is it's sprite number in the font sheet
		c &= 0x7F;

		// Just render the char whih the given color override
		emuSprtRender(c, x, y, false, false, fontSprtSheet, color);
		x += 4;
	}
}

void emuRectFill(int16_t x, int16_t y, int16_t cornerX, int16_t cornerY, uint8_t color) {
	// Same bounding code as in emuSprtRender 
	// (absolutely a copy paste)

	int width = (cornerX - x) + 1;
	int height = (cornerY - y) + 1;

	if (cornerX < 0 || x >= pico8Size || 
		cornerY < 0 || y >= pico8Size ||
		width <= 0  || height <= 0) { return; }
	
	// Calculating the sprite's clip if needed on the X axis
	uint8_t startCopyX = width - cornerX <= 0 ? 0 : width - (cornerX + 1) >= 128 ? 127 : width - (cornerX + 1);
	uint8_t endCopyX = pico8Size - x >= width ? width - startCopyX : pico8Size - x > 128 ? 128 : pico8Size - x;

	// Same but for the Y Axis
	uint8_t startCopyY = height - cornerY <= 0 ? 0 : height - (cornerY + 1) >= 128 ? 127 : height - (cornerY + 1);
	uint8_t endCopyY = pico8Size - y >= height ? height : pico8Size - y > 128 ? 128 : pico8Size - y;

	eadk_color_t drawColor = palette[color%16];
	for (uint8_t iX = 0; iX < endCopyX; iX++) {rowBuffer[iX] = drawColor; }
	for (uint8_t iY = startCopyY; iY < endCopyY; iY++) {
		memcpy(&frameBuffer[y + iY][x + startCopyX], rowBuffer, sizeof(eadk_color_t) * endCopyX);
	}
	#undef drawColor
}

// A function directly pulled from Lemon's implementation
void emuLine(int16_t startX, int16_t startY, int16_t finishX, int16_t finishY, uint8_t color) {
  #define PLOT(x,y) do {                                                        \
	 emuRectFill(x, y, x, y, color); \
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

// This function as the name implies "presents" the framebuffer
// which just means it renders it to the screen ! Hope this fixes
// (even though is 100% should) the flickering issues we had before
void emuFbPresent() {
	#define trueY(iY) pico8YOrigin + (iY * renderScale)

	for (uint8_t iY = 0; iY < pico8Size; iY++) {
		// Proventing artefacts by not using pushRectUniforms on coordinates
		// Outside the screen :)
		if (trueY(iY) < 0 || trueY(iY) >= screenH) { continue; }

		eadk_rect_t dstRect = {pico8XOrigin, trueY(iY), pico8Size * renderScale, 1};
		#if renderScale > 1
		// Scaling code
		#pragma unroll
		for (int iX = 0; iX < pico8Size * renderScale; iX++) {
			rowBuffer[iX] = frameBuffer[iY][iX/renderScale];
		}
		#pragma unroll
		for (uint8_t i = 0; i < renderScale; i++) {
			dstRect.y += i;
			eadk_display_push_rect(dstRect, rowBuffer);
		}
		#else
		// No extra operations if we don't scale :)
		eadk_display_push_rect(dstRect, frameBuffer[iY]);
		#endif
	}
	#undef trueY
};

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
	vsnprintf(osdText, sizeof osdText, fmt, ap);
	osdText[sizeof osdText - 1] = '\0'; //make sure to add NUL terminator in case of truncation
	osdTimer = 30;
	va_end(ap);
}
static void OSDdraw(void) {
	if (osdTimer > 0) {
		--osdTimer;
		const int x = 4;
		const int y = 116 + (osdTimer < 10 ? 10-osdTimer : 0); //disappear by going below the screen
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
			memcpy(palette, defltPalette, sizeof(defltPalette));
		} break;

		case CELESTE_P8_CIRCFILL: { //circfill(x,y,r,col)
			int cx = INT_ARG() - cameraX;
			int cy = INT_ARG() - cameraY;
			int r = INT_ARG();
			int color = INT_ARG();

			if (r <= 1) {
				emuRectFill((cx - 1), cy, (cx - 1) + 2, cy, color);
				emuRectFill(cx, (cy - 1), cx, (cy - 1) + 2, color);

			} else if (r <= 2) {
				emuRectFill((cx - 2), (cy - 1), (cx - 2) + 4, (cy - 1) + 2, color);
				emuRectFill((cx - 1), ((cy - 2)), (cx - 1) + 2, (cy - 2) + 4, color);

			} else if (r <= 3) {
				emuRectFill((cx - 3), (cy - 1), (cx - 3) + 6, (cy - 1) + 2, color);
				emuRectFill((cx - 1), (cy - 3), (cx - 1) + 2, (cy - 3) + 6, color);
				emuRectFill((cx - 2), (cy - 2), (cx - 2) + 4, (cy - 2) + 4, color);

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
void gameInit() {
    memcpy(palette, defltPalette, sizeof(defltPalette));

	Celeste_P8_set_rndseed(EADK::random());
	Celeste_P8_init();
}

// Public function meant to be ran by the our main()
// program fucntion
void emuInit() {
	Celeste_P8_set_call_func(emulator);

	gameInit();

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

	if (state.keyDown(Keyboard::Key::Toolbox)
        && !lastState.keyDown(Keyboard::Key::Toolbox))
		{ screenShake = !screenShake; OSDset("Screenshake: %s", screenShake ? "on" : "off"); }
	
	static uint8_t resetTimer = 0;
	if (state.keyDown(Keyboard::Key::XNT)) {
		resetTimer++;
		if (resetTimer >= 30) {
			resetTimer = 0;
			
			OSDset("Reset");
			pauseEmu = false;
			
			gameInit();
		}
	} else resetTimer = 0;

	// TODO : Maybe add Saving and loading
	/*
	if (state.keyDown(Keyboard::Key::Shift)
        && !lastState.keyDown(Keyboard::Key::Shift)) {
		if (gameState) {
			OSDset("Saved game state");
			Celeste_P8_save_state(gameState);
		}
	}

	if (state.keyDown(Keyboard::Key::Alpha)
        && !lastState.keyDown(Keyboard::Key::Alpha)) {
		if (gameState) {
			OSDset("Loaded game state");
			if (pauseEmu) { pauseEmu = false; }
			Celeste_P8_load_state(gameState);
		}
	}*/

    // Actual game input
    if (state.keyDown(Keyboard::Key::Left))  emuBtnState |= (1<<0);
	if (state.keyDown(Keyboard::Key::Right)) emuBtnState |= (1<<1);
	if (state.keyDown(Keyboard::Key::Up))    emuBtnState |= (1<<2);
	if (state.keyDown(Keyboard::Key::Down))  emuBtnState |= (1<<3);
	if (state.keyDown(Keyboard::Key::Back)) emuBtnState |= (1<<4);
	if (state.keyDown(Keyboard::Key::OK)) emuBtnState |= (1<<5);
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

	emuFbPresent();
}
