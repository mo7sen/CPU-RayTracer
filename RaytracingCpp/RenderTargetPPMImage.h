#pragma once

#include "RenderTarget.h"
#include "common.h"
#include <vector>
#include <fstream>

class RenderTargetPPMImage : public RenderTarget {
private:
	std::string out_file;

	std::vector<Color> m_pixels;

	void frameStream(std::ostream & os, std::ostream & progstream)
	{
		os << "P3\n" << m_width << ' ' << m_height << "\n255\n";

		for (int j = m_height - 1; j >= 0; --j)
		{
			progstream << "\rScanlines remaining: " << j << ' ' << std::flush;
			for (int i = 0; i < m_width; ++i)
			{
				int32_t idx = j * m_width + i;
				Color pixelColor = m_pixels[idx];
				os << pixelColor << ' ';
			}
			os << std::endl;
		}
		progstream << "\nDone.\n";
	}
	
	void renderFrameToFile(std::string filename)
	{
		std::ofstream imageFile;
		imageFile.open (filename);
		frameStream(imageFile, std::cerr);
		imageFile.close();
	}

public:
	RenderTargetPPMImage(int32_t width, int32_t height)
	{
		m_width = width;
		m_height = height;
		std::vector<Color> pixels(size_t(m_width) * size_t(m_height));
		m_pixels = pixels;
	}

	void renderFrame()
	{
		renderFrameToFile(out_file);
	}

	void setPixel(int32_t x, int32_t y, Color const& color) override
	{
		int32_t idx = y * m_width + x;
		m_pixels[idx] = color;
	}

	Color getPixel(int32_t x, int32_t y) const override
	{
		int32_t idx = y * m_width + x;
		return m_pixels[idx];
	}

	std::string getOutFile() const {
		return out_file;
	}

	void setOutFile(std::string outFileName) {
		out_file = outFileName;
	}
	
	~RenderTargetPPMImage() = default;
};
