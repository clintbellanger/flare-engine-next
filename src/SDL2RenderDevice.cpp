/*
Copyright © 2013 Igor Paliychuk

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "SharedResources.h"
#include "Settings.h"

#include "SDL2RenderDevice.h"

using namespace std;

Sprite::Sprite(const Sprite& other)
{
	local_frame = other.local_frame;
	keep_graphics = other.keep_graphics;
	src = other.src;
	offset = other.offset;
	dest = other.dest;

	if (other.sprite.surface != NULL) {
		//sprite.surface = SDL_DisplayFormatAlpha(other.sprite.surface);
	} else {
		sprite.surface = NULL;
	}
}

Sprite& Sprite::operator=(const Sprite& other) {
	if (other.sprite.surface != NULL) {
		//sprite.surface = SDL_DisplayFormatAlpha(other.sprite.surface);
	} else {
		sprite.surface = NULL;
	}
	local_frame = other.local_frame;
	keep_graphics = other.keep_graphics;
	src = other.src;
	offset = other.offset;
	dest = other.dest;

	return *this;
}

Sprite::~Sprite() {
	if (sprite.surface != NULL && !keep_graphics) {
		SDL_DestroyTexture(sprite.surface);
		sprite.surface = NULL;
	}
}

/**
 * Set the graphics context of a Sprite.
 * Initialize graphics resources. That is the SLD_surface buffer
 *
 * It is important that, if the client owns the graphics resources,
 * clearGraphics() method is called first in case this Sprite holds the
 * last references to avoid resource leaks.
 */
void Sprite::setGraphics(Image s, bool setClipToFull) {

	sprite = s;

	if (setClipToFull && sprite.surface != NULL) {
		src.x = 0;
		src.y = 0;
		src.w = sprite.getWidth();
		src.h = sprite.getHeight();
	}

}

Image* Sprite::getGraphics() {

	return &sprite;
}

bool Sprite::graphicsIsNull() {

	return (sprite.surface == NULL);
}

/**
 * Clear the graphics context of a Sprite.
 * Release graphics resources. That is the SLD_surface buffer
 *
 * It is important that this method is only called by clients who own the
 * graphics resources.
 */
void Sprite::clearGraphics() {

	if (sprite.surface != NULL) {
		SDL_DestroyTexture(sprite.surface);
		sprite.surface = NULL;
	}
}

void Sprite::setOffset(const Point& _offset) {
	this->offset = _offset;
}

void Sprite::setOffset(const int x, const int y) {
	this->offset.x = x;
	this->offset.y = y;
}

Point Sprite::getOffset() {

	return offset;
}
/**
 * Set the clipping rectangle for the sprite
 */
void Sprite::setClip(const SDL_Rect& clip) {
	src = clip;
}

/**
 * Set the clipping rectangle for the sprite
 */
void Sprite::setClip(const int x, const int y, const int w, const int h) {
	src.x = x;
	src.y = y;
	src.w = w;
	src.h = h;
}

void Sprite::setClipX(const int x) {
	src.x = x;
}

void Sprite::setClipY(const int y) {
	src.y = y;
}

void Sprite::setClipW(const int w) {
	src.w = w;
}

void Sprite::setClipH(const int h) {
	src.h = h;
}


SDL_Rect Sprite::getClip() {
	return src;
}
void Sprite::setDest(const SDL_Rect& _dest) {
	dest.x = (float)_dest.x;
	dest.y = (float)_dest.y;
}

void Sprite::setDest(const Point& _dest) {
	dest.x = (float)_dest.x;
	dest.y = (float)_dest.y;
}

void Sprite::setDest(int x, int y) {
	dest.x = (float)x;
	dest.y = (float)y;
}

void Sprite::setDestX(int x) {
	dest.x = (float)x;
}

void Sprite::setDestY(int y) {
	dest.y = (float)y;
}

FPoint Sprite::getDest() {
	return dest;
}

int Sprite::getGraphicsWidth() {
	return (sprite.surface ? sprite.getWidth() : 0);
}

int Sprite::getGraphicsHeight() {
	return (sprite.surface ? sprite.getHeight() : 0);
}

SDL2RenderDevice::SDL2RenderDevice() {
	cout << "Using Render Device: SDL2RenderDevice" << endl;
}

int SDL2RenderDevice::createContext(int width, int height) {
	if (is_initialized) {
		if (FULLSCREEN) {
			SDL_SetWindowSize(screen, width, height);
			SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN);
		}
		else {
			SDL_SetWindowFullscreen(screen, SDL_WINDOW_SHOWN);
			SDL_SetWindowSize(screen, width, height);
		}
		return 0;
	}

	Uint32 flags = 0;

	if (FULLSCREEN) flags = SDL_WINDOW_FULLSCREEN;
	else flags = SDL_WINDOW_SHOWN;

	screen = SDL_CreateWindow(msg->get(WINDOW_TITLE).c_str(),
								SDL_WINDOWPOS_CENTERED,
								SDL_WINDOWPOS_CENTERED,
								width, height,
								flags);

	if (HWSURFACE) flags = SDL_RENDERER_ACCELERATED;
	else flags = SDL_RENDERER_SOFTWARE;

	renderer = SDL_CreateRenderer(screen, -1, flags);

	if (screen == NULL && renderer == NULL && !is_initialized) {
			// If this is the first attempt and it failed we are not
			// getting anywhere.
			SDL_Quit();
			exit(1);
	}
	else {
		is_initialized = true;
	}

	// Add Window Titlebar Icon
	titlebar_icon = IMG_Load(mods->locate("images/logo/icon.png").c_str());
	SDL_SetWindowIcon(screen, titlebar_icon);

	return (screen != NULL ? 0 : -1);
}

SDL_Rect SDL2RenderDevice::getContextSize() {
	SDL_Rect size;
	size.x = size.y = 0;
	SDL_GetWindowSize(screen, &size.w, &size.h);

	return size;
}

int SDL2RenderDevice::render(Renderable& r, SDL_Rect dest) {
	dest.w = r.src.w;
	dest.h = r.src.h;
	return SDL_RenderCopy(renderer, r.sprite.surface, &r.src, &dest);
}

int SDL2RenderDevice::render(ISprite& r) {
	if (r.graphicsIsNull()) {
		return -1;
	}
	if ( !local_to_global(r) ) {
		return -1;
	}
	m_dest.w = m_clip.w;
	m_dest.h = m_clip.h;

	return SDL_RenderCopy(renderer, r.getGraphics()->surface, &m_clip, &m_dest);
}

int SDL2RenderDevice::renderImage(Image* image, SDL_Rect& src) {
	if (!image) return -1;
	return SDL_RenderCopy(renderer, image->surface, &src, 0);
}

int SDL2RenderDevice::renderToImage(Image* src_image, SDL_Rect& src, Image* dest_image, SDL_Rect& dest, bool dest_is_transparent) {
	if (!src_image || !dest_image) return -1;
	if (SDL_SetRenderTarget(renderer, dest_image->surface) != 0) return -1;
	SDL_SetTextureBlendMode(dest_image->surface, SDL_BLENDMODE_BLEND);
	dest.w = src.w;
	dest.h = src.h;
	SDL_RenderCopy(renderer, src_image->surface, &src, &dest);
	SDL_SetRenderTarget(renderer, NULL);
	return 0;
}

int SDL2RenderDevice::renderText(
	TTF_Font *ttf_font,
	const std::string& text,
	SDL_Color color,
	SDL_Rect& dest
) {
	int ret = 0;
	Image ttf;
	ttf.surface = SDL_CreateTextureFromSurface(renderer,TTF_RenderUTF8_Blended(ttf_font, text.c_str(), color));
	m_ttf_renderable.setGraphics(ttf);
	if (!m_ttf_renderable.graphicsIsNull()) {
		SDL_Rect clip = m_ttf_renderable.getClip();
		dest.w = clip.w;
		dest.h = clip.h;
		ret = SDL_RenderCopy(
				  renderer,
				  m_ttf_renderable.getGraphics()->surface,
				  &clip,
				  &dest
			  );
		SDL_DestroyTexture(m_ttf_renderable.getGraphics()->surface);
	}
	else {
		ret = -1;
	}

	return ret;
}

void SDL2RenderDevice::renderTextToImage(Image* image, TTF_Font* ttf_font, const std::string& text, SDL_Color color, bool blended) {
	if (!image) return;
	if (blended)
		image->surface = SDL_CreateTextureFromSurface(renderer, TTF_RenderUTF8_Blended(ttf_font, text.c_str(), color));
	else
		image->surface = SDL_CreateTextureFromSurface(renderer, TTF_RenderUTF8_Solid(ttf_font, text.c_str(), color));
}

void SDL2RenderDevice::drawPixel(
	int x,
	int y,
	Uint32 color
) {
	//Unimplemented
	return;
}

/*
 * Set the pixel at (x, y) to the given value
 */
void SDL2RenderDevice::drawPixel(Image *image, int x, int y, Uint32 pixel) {
	//Unimplemented
}

void SDL2RenderDevice::drawLine(
	int x0,
	int y0,
	int x1,
	int y1,
	Uint32 color
) {
	const int dx = abs(x1-x0);
	const int dy = abs(y1-y0);
	const int sx = x0 < x1 ? 1 : -1;
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx-dy;

	do {
		//skip draw if outside screen
		if (x0 > 0 && y0 > 0 && x0 < VIEW_W && y0 < VIEW_H) {
			this->drawPixel(x0,y0,color);
		}

		int e2 = 2*err;
		if (e2 > -dy) {
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 <  dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	}
	while(x0 != x1 || y0 != y1);
}

void SDL2RenderDevice::drawLine(
	const Point& p0,
	const Point& p1,
	Uint32 color
) {
	//Unimplemented
}

/**
 * draw line to the screen
 */
void SDL2RenderDevice::drawLine(Image *image, int x0, int y0, int x1, int y1, Uint32 color) {
	if (!image || !image->surface) return;

	const int dx = abs(x1-x0);
	const int dy = abs(y1-y0);
	const int sx = x0 < x1 ? 1 : -1;
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx-dy;

	do {
		//skip draw if outside screen
		if (x0 > 0 && y0 > 0 && x0 < VIEW_W && y0 < VIEW_H)
			render_device->drawPixel(image,x0,y0,color);

		int e2 = 2*err;
		if (e2 > -dy) {
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 <  dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	}
	while(x0 != x1 || y0 != y1);
}

void SDL2RenderDevice::drawLine(Image *image, Point pos0, Point pos1, Uint32 color) {
	if (!image || !image->surface) return;
	//Unimplemented
}

void SDL2RenderDevice::drawRectangle(Image *image, Point pos0, Point pos1, Uint32 color) {
	//Unimplemented
}

void SDL2RenderDevice::drawRectangle(
	const Point& p0,
	const Point& p1,
	Uint32 color
) {
	//Unimplemented
}

void SDL2RenderDevice::blankScreen() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	return;
}

void SDL2RenderDevice::commitFrame() {
	SDL_RenderPresent(renderer);
	return;
}

void SDL2RenderDevice::destroyContext() {
	SDL_FreeSurface(titlebar_icon);
	SDL_DestroyWindow(screen);
	SDL_DestroyRenderer(renderer);

	return;
}

void SDL2RenderDevice::fillImageWithColor(Image *dst, SDL_Rect *dstrect, Uint32 color) {
	if (!dst) return;

	Uint32 u_format = SDL_GetWindowPixelFormat(screen);
	SDL_PixelFormat* format = SDL_AllocFormat(u_format);

	if (!format) return;

	SDL_Color rgb;
	SDL_GetRGB(color, format, &rgb.r, &rgb.g, &rgb.b);
	SDL_FreeFormat(format);

	SDL_SetRenderTarget(renderer, dst->surface);
	SDL_SetTextureBlendMode(dst->surface, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g , rgb.b, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);
}

Uint32 SDL2RenderDevice::MapRGB(Image *src, Uint8 r, Uint8 g, Uint8 b) {
	if (!src || !src->surface) return 0;

	Uint32 u_format;
	SDL_QueryTexture(src->surface, &u_format, NULL, NULL, NULL);
	SDL_PixelFormat* format = SDL_AllocFormat(u_format);

	if (format) {
		Uint32 ret = SDL_MapRGB(format, r, g, b);
		SDL_FreeFormat(format);
		return ret;
	}
	else {
		return 0;
	}
}

Uint32 SDL2RenderDevice::MapRGB(Uint8 r, Uint8 g, Uint8 b) {
	Uint32 u_format = SDL_GetWindowPixelFormat(screen);
	SDL_PixelFormat* format = SDL_AllocFormat(u_format);

	if (format) {
		Uint32 ret = SDL_MapRGB(format, r, g, b);
		SDL_FreeFormat(format);
		return ret;
	}
	else {
		return 0;
	}
}

Uint32 SDL2RenderDevice::MapRGBA(Image *src, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	if (!src || !src->surface) return 0;

	Uint32 u_format;
	SDL_QueryTexture(src->surface, &u_format, NULL, NULL, NULL);
	SDL_PixelFormat* format = SDL_AllocFormat(u_format);

	if (format) {
		Uint32 ret = SDL_MapRGBA(format, r, g, b, a);
		SDL_FreeFormat(format);
		return ret;
	}
	else {
		return 0;
	}
}

Uint32 SDL2RenderDevice::MapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	Uint32 u_format = SDL_GetWindowPixelFormat(screen);
	SDL_PixelFormat* format = SDL_AllocFormat(u_format);

	if (format) {
		Uint32 ret = SDL_MapRGBA(format, r, g, b, a);
		SDL_FreeFormat(format);
		return ret;
	}
	else {
		return 0;
	}
}

void SDL2RenderDevice::loadIcons() {
	icons.clearGraphics();
	icons.setGraphics(loadGraphicSurface("images/icons/icons.png", "Couldn't load icons", false), false);
}

bool SDL2RenderDevice::local_to_global(ISprite& r) {
	m_clip = r.getClip();

	int left = r.getDest().x - r.getOffset().x;
	int right = left + r.getClip().w;
	int up = r.getDest().y - r.getOffset().y;
	int down = up + r.getClip().h;

	// Check whether we need to render.
	// If so, compute the correct clipping.
	if (r.local_frame.w) {
		if (left > r.local_frame.w) {
			return false;
		}
		if (right < 0) {
			return false;
		}
		if (left < 0) {
			m_clip.x = r.getClip().x - left;
			left = 0;
		};
		right = (right < r.local_frame.w ? right : r.local_frame.w);
		m_clip.w = right - left;
	}
	if (r.local_frame.h) {
		if (up > r.local_frame.h) {
			return false;
		}
		if (down < 0) {
			return false;
		}
		if (up < 0) {
			m_clip.y = r.getClip().y - up;
			up = 0;
		};
		down = (down < r.local_frame.h ? down : r.local_frame.h);
		m_clip.h = down - up;
	}

	m_dest.x = left + r.local_frame.x;
	m_dest.y = up + r.local_frame.y;

	return true;
}

/**
 * create blank surface
 */
Image SDL2RenderDevice::createAlphaSurface(int width, int height) {

	Image image;

	if (width > 0 && height > 0) {
		image.surface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
		if(image.surface == NULL) {
			fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		}
		else {
				SDL_SetRenderTarget(renderer, image.surface);
				SDL_SetTextureBlendMode(image.surface, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(renderer, 0,0,0,0);
				SDL_RenderClear(renderer);
				SDL_SetRenderTarget(renderer, NULL);
		}
	}

	return image;
}

Image SDL2RenderDevice::createSurface(int width, int height) {

	Image image;

	if (width > 0 && height > 0) {
		image.surface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
		if(image.surface == NULL) {
			fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		}
		else {
			SDL_SetTextureColorMod(image.surface, 255,0,255);
		}
	}

	return image;
}

void SDL2RenderDevice::setGamma(float g) {
	//Unimplemented
}

void SDL2RenderDevice::listModes(std::vector<SDL_Rect> &modes) {
	SDL_Rect** detect_modes;
	std::vector<SDL_Rect> vec_detect_modes;
	SDL_Rect detect_mode;
	SDL_DisplayMode mode;
	/* SDL_compat.c */
	for (int i = 0; i < SDL_GetNumDisplayModes(SDL_GetWindowDisplayIndex(screen)); ++i) {
		SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(screen), i, &mode);

		if (!vec_detect_modes.empty())
		{
			if (vec_detect_modes.back().w == mode.w && vec_detect_modes.back().h == mode.h) {
			continue;
			}
		}

		detect_mode.x = 0;
		detect_mode.y = 0;
		detect_mode.w = mode.w;
		detect_mode.h = mode.h;
		vec_detect_modes.push_back(detect_mode);

	}
	detect_modes = (SDL_Rect**)calloc(vec_detect_modes.size(),sizeof(SDL_Rect));
	for (unsigned i = 0; i < vec_detect_modes.size(); ++i) {
		detect_modes[i] = &vec_detect_modes[i];
	}
	vec_detect_modes.clear();
	// Check if there are any modes available
	if (detect_modes == (SDL_Rect**)0) {
		fprintf(stderr, "No modes available!\n");
		return;
	}
 
	// Check if our resolution is restricted
	if (detect_modes == (SDL_Rect**)-1) {
		fprintf(stderr, "All resolutions available.\n");
	}
 
	for (unsigned i=0; detect_modes[i]; ++i) {
		modes.push_back(*detect_modes[i]);
		if (detect_modes[i]->w < MIN_VIEW_W || detect_modes[i]->h < MIN_VIEW_H) {
			// make sure the resolution fits in the constraints of MIN_VIEW_W and MIN_VIEW_H
			modes.pop_back();
		}
		else {
		// check previous resolutions for duplicates. If one is found, drop the one we just added
		for (unsigned j=0; j<modes.size()-1; ++j) {
			if (modes[j].w == detect_modes[i]->w && modes[j].h == detect_modes[i]->h) {
				modes.pop_back();
				break;
			}
		}
		}
	}
}

void SDL2RenderDevice::setColorKey(Image* image, Uint32 flag, Uint32 key) {
	if (!image) return;
	//Unimplemented
}

void SDL2RenderDevice::setAlpha(Image* image, Uint32 flag, Uint8 alpha) {
	if (!image) return;
	SDL_SetTextureAlphaMod(image->surface, alpha);
}

Image SDL2RenderDevice::loadGraphicSurface(std::string filename, std::string errormessage, bool IfNotFoundExit, bool HavePinkColorKey) {
	Image image;

	if (HavePinkColorKey) {
		// SDL_Textures don't support colorkeying
		// so we instead create an SDL_Surface, key it, and convert to a texture
		SDL_Surface* cleanup = IMG_Load(mods->locate(filename).c_str());
		if (cleanup) {
			SDL_SetColorKey(cleanup, true, SDL_MapRGB(cleanup->format, 255, 0, 255));
			image.surface = SDL_CreateTextureFromSurface(renderer, cleanup);
			SDL_FreeSurface(cleanup);
		}
	}
	else {
		image.surface = IMG_LoadTexture(renderer, mods->locate(filename).c_str());
	}

	if(image.graphicIsNull()) {
		if (!errormessage.empty())
			fprintf(stderr, "%s: %s\n", errormessage.c_str(), IMG_GetError());
		if (IfNotFoundExit) {
			SDL_Quit();
			exit(1);
		}
	}

	return image;
}

void SDL2RenderDevice::scaleSurface(Image *source, int width, int height) {
	//Unimplemented
}

Uint32 SDL2RenderDevice::readPixel(Image *image, int x, int y) {
	//Unimplemented
	return 0;
}

/*
 * Returns false if a pixel at Point px is transparent
 */
bool SDL2RenderDevice::checkPixel(Point px, Image *image) {
	//Unimplemented
	return true;
}

void SDL2RenderDevice::freeImage(Image *image) {
	if (image && image->surface)
		SDL_DestroyTexture(image->surface);
}

void setSDL_RGBA(Uint32 *rmask, Uint32 *gmask, Uint32 *bmask, Uint32 *amask) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	*rmask = 0xff000000;
	*gmask = 0x00ff0000;
	*bmask = 0x0000ff00;
	*amask = 0x000000ff;
#else
	*rmask = 0x000000ff;
	*gmask = 0x0000ff00;
	*bmask = 0x00ff0000;
	*amask = 0xff000000;
#endif
}

