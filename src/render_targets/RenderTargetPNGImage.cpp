#include <render_targets/RenderTargetPNGImage.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <extern/stb/stb_image_write.h>

void RenderTargetPNGImage::renderFrameToFile(std::string filename)
{
	std::vector<uint8_t> data;
	data.resize(m_pixels.size() * Color::COMP_PER_PIXEL);
	printf("m_pixels.size() = %u\n", m_pixels.size());
	printf("data.size() = %u\n", data.size());

	for(uint32_t i = 0; i < m_pixels.size(); i++)
	{
		data[i * Color::COMP_PER_PIXEL + 0] = static_cast<uint8_t>(m_pixels[i].r_u8());
		data[i * Color::COMP_PER_PIXEL + 1] = static_cast<uint8_t>(m_pixels[i].g_u8());
		data[i * Color::COMP_PER_PIXEL + 2] = static_cast<uint8_t>(m_pixels[i].b() * 255.999);
		data[i * Color::COMP_PER_PIXEL + 3] = 255.0;
	}

	if(!stbi_write_png(filename.c_str(), m_width, m_height, 4, data.data(), m_width * 4))
	{
		std::cerr << "Couldn't write PNG Image to " << filename << std::endl;
	}
	else
	{
		std::cout << "Successfully wrote PNG image to " << filename << std::endl;
	}
}

void RenderTargetPNGImage::renderFrame()
{
	int old_flip = stbi__flip_vertically_on_write;
	stbi_flip_vertically_on_write(flipVert);
	renderFrameToFile(out_file);
	stbi_flip_vertically_on_write(old_flip);
}
