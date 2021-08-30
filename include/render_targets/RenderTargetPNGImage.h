#pragma once

#include <RenderTarget.h>
#include <common.h>
#include <vector>
#include <fstream>

class RenderTargetPNGImage : public RenderTarget {
private:
	std::string out_file;

	const static int flipVert = 1;

	std::vector<Color> m_pixels;

	void renderFrameToFile(std::string filename);

public:
	RenderTargetPNGImage(int32_t width, int32_t height)
	{
		m_width = width;
		m_height = height;
		std::vector<Color> pixels(size_t(m_width) * size_t(m_height));
		m_pixels = pixels;
	}

	void renderFrame();

	void setPixel(int32_t x, int32_t y, Color const& color)
	{
		int32_t idx = y * m_width + x;
		m_pixels[idx] = color;
	}

	Color getPixel(int32_t x, int32_t y) const
	{
		int32_t idx = y * m_width + x;
		return m_pixels[idx];
	}

	std::string getOutFile() const 
	{
		return out_file;
	}

	void setOutFile(std::string outFileName) 
	{
		out_file = outFileName;
	}
	
	~RenderTargetPNGImage() = default;
};
