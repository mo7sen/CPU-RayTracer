#pragma once
#include "common.h"
#include "Color.h"

class RenderTarget {
protected:
	int32_t m_width;
	int32_t m_height;

public:
	void setHeight(int32_t height) { m_height = height; }
	void setWidth(int32_t width) { m_width = width; }	
	
	int32_t getHeight() const { return m_height; }
	int32_t getWidth() const { return m_width; }

	virtual Color getPixel(int32_t x, int32_t y) const = 0;
	virtual void setPixel(int32_t x, int32_t y, Color const& color) = 0;
};
