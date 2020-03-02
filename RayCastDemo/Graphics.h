#pragma once
#include "src/Renderer.h"
enum class RectStyle {
    OUTLINE,
    FILL
};
//declare SDL_Color values for each color in the original LUT
static constexpr SDL_Color Black = { 0, 0, 0 };
static constexpr SDL_Color DarkBlue = { 0, 0, 139 };
static constexpr SDL_Color DarkGreen = { 0, 139, 0 };
static constexpr SDL_Color DarkCyan = { 0, 139, 139 };
static constexpr SDL_Color DarkRed = { 139, 0, 0 };
static constexpr SDL_Color DarkMagenta = { 139, 0, 139 };
static constexpr SDL_Color Brown = { 139, 69, 19 };
static constexpr SDL_Color Gray = { 128, 128, 128 };
static constexpr SDL_Color DarkGray = { 105, 105, 105 };
static constexpr SDL_Color LightBlue = { 65, 105, 225 };
static constexpr SDL_Color LightGreen = { 144, 238, 144 };
static constexpr SDL_Color LightCyan = { 63, 255, 255 };
static constexpr SDL_Color LightRed = { 255, 128, 128 };
static constexpr SDL_Color LightMagenta = { 255, 128, 255 };
static constexpr SDL_Color Yellow = { 255, 255, 0 };
static constexpr SDL_Color White = { 255, 255, 255 };
//indexed palette to translate between original LUT value and SDL_Color
static constexpr int PALETTE_SIZE = 16;
static constexpr SDL_Color Palette[PALETTE_SIZE]{
    Black, DarkBlue, DarkGreen, DarkCyan,       //0, 1, 2, 3
    DarkRed, DarkMagenta, Brown, Gray,          //4, 5, 6, 7
    DarkGray, LightBlue, LightGreen, LightCyan, //8, 9, 10, 11
    LightRed, LightMagenta, Yellow, White       //12, 13, 14, 15 
};
struct Graphics{
    const Renderer& _r;
    Graphics(const Renderer& r) : _r(r) {};

    void clearScreen() const noexcept {
        _r.setColor(Black);
        _r.clear();
    }
    void present() const noexcept {
        _r.present();
    }
    void setColor(int lutIndex) const noexcept {
        SDL_assert(lutIndex > -1 && lutIndex < PALETTE_SIZE && "_setColor (int): invalid LUT color index specified, must be 0-15");
        const auto color = Palette[lutIndex];
        _r.setColor(color);
    }
    void setColor(const SDL_Color& color) const noexcept {
        _r.setColor(color);
    }
    void drawLine(int x1, int y1, int x2, int y2) const noexcept {
        _r.drawLine(x1, y1, x2, y2);
    } 
    void setPixel(int x, int y) const noexcept {
        _r.drawPoint(x, y);
    }
    void drawRectangle(RectStyle style, int left, int top, int right, int bottom) const noexcept {
        SDL_Rect rect{ left, top, right - left, bottom - top };
        if (style == RectStyle::FILL) {
            _r.drawFilledRect(rect);
        }
        else {
            _r.drawRect(rect);
        }
    }
};