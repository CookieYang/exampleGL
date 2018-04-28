#include "KTXLoader.h"
#include "GL/glew.h"
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif /* _MSC_VER */

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ktx {
	static const unsigned char identifier[] = {
		0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
	};

	static const unsigned int swap32(const unsigned int u32) {
		union 
		{
			unsigned int u32;
			unsigned char u8[4];
		} a, b;
		a.u32 = u32;
		b.u8[0] = a.u8[3];
		b.u8[1] = a.u8[2];
		b.u8[2] = a.u8[1];
		b.u8[3] = a.u8[0];
		return b.u32;
	}

	static const unsigned short swap16(const unsigned short u16) {
		union 
		{
			unsigned short u16;
			unsigned char u8[2];
		}a, b;
		a.u16 = u16;
		b.u8[0] = a.u8[1];
		b.u8[1] = a.u8[0];
		return b.u16;
	}

	static unsigned int calculateStride(const ktxHeader& header, unsigned int width, unsigned int pad = 4) {
		unsigned int channels = 0;
		switch (header.glbaseinternalformat)
		{
			case GL_RED:    channels = 1;
				break;
			case GL_RG:     channels = 2;
				break;
			case GL_BGR:
			case GL_RGB:    channels = 3;
				break;
			case GL_BGRA:
			case GL_RGBA:   channels = 4;
				break;
		}

		unsigned int stride = channels * header.gltypesize * width;
		stride = (stride + (pad - 1)) & ~(pad - 1);
		return stride;
	}

	static unsigned int calculateFacesize(const ktxHeader& header) {
		unsigned int stride = calculateStride(header, header.pixelwidth);
		return stride * header.pixelheight;
	}

	extern unsigned int load(const char* filename, unsigned int tex) {
		FILE* fp;
		GLuint temp = 0;
		GLuint retval = 0;
		ktxHeader head;
		size_t data_start, data_end;
		unsigned char* data;
		GLenum target = GL_NONE;

		fp = fopen(filename, "rb");
		if (!fp)
		{
			return 0;
		}

		if (fread(&head,sizeof(head),1,fp) != 1)
		{
			goto fail_read;
		}

		if (memcmp(head.identifier,identifier,sizeof(identifier)) != 0)
		{
			goto fail_head;
		}

		if (head.endianness == 0x01020304)
		{
			head.endianness = swap32(head.endianness);
			head.gltype = swap32(head.gltype);
			head.gltypesize = swap32(head.gltypesize);
			head.glformat = swap32(head.glformat);
			head.glinternalformat = swap32(head.glinternalformat);
			head.glbaseinternalformat = swap32(head.glbaseinternalformat);
			head.pixelwidth = swap32(head.pixelwidth);
			head.pixelheight = swap32(head.pixelheight);
			head.pixeldepth = swap32(head.pixeldepth);
			head.arrayelements = swap32(head.arrayelements);
			head.faces = swap32(head.faces);
			head.miplevels = swap32(head.miplevels);
			head.keypairbytes = swap32(head.keypairbytes);
		}
		else if (head.endianness == 0x04030201)
		{

		}
		else
		{
			goto fail_head;
		}

		if (head.pixelheight == 0)
		{
			if (head.arrayelements == 0)
			{
				target = GL_TEXTURE_1D;
			}
			else
			{
				target = GL_TEXTURE_1D_ARRAY;
			}
		}
		else if (head.pixeldepth == 0)
		{
			if (head.arrayelements == 0)
			{
				if (head.faces == 0)
				{
					target = GL_TEXTURE_2D;
				}
				else
				{
					target = GL_TEXTURE_CUBE_MAP;
				}
			}
			else
			{
				if (head.faces == 0)
				{
					target = GL_TEXTURE_2D_ARRAY;
				}
				else
				{
					target = GL_TEXTURE_CUBE_MAP_ARRAY;
				}
			}
		}
		else
		{
			target = GL_TEXTURE_3D;
		}

		// Check for insanity...
		if (target == GL_NONE ||                                    // Couldn't figure out target
			(head.pixelwidth == 0) ||                                  // Texture has no width???
			(head.pixelheight == 0 && head.pixeldepth != 0))              // Texture has depth but no height???
		{
			goto fail_head;
		}

		temp = tex;
		if (tex == 0)
		{
			glGenTextures(1, &tex);
		}

		glBindTexture(target, tex);

		data_start = ftell(fp) + head.keypairbytes;
		fseek(fp, 0, SEEK_END);
		data_end = ftell(fp);
		fseek(fp, data_start, SEEK_SET);

		data = new unsigned char[data_end - data_start];
		memset(data, 0, data_end - data_start);

		fread(data, 1, data_end - data_start, fp);

		if (head.miplevels == 0)
		{
			head.miplevels = 1;
		}

		switch (target)
		{
		case GL_TEXTURE_1D:
			glTexStorage1D(GL_TEXTURE_1D, head.miplevels, head.glinternalformat, head.pixelwidth);
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, head.pixelwidth, head.glformat, head.glinternalformat, data);
			break;
		case GL_TEXTURE_2D:
			// glTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, h.glformat, h.gltype, data);
			if (head.gltype == GL_NONE)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, head.glinternalformat, head.pixelwidth, head.pixelheight, 0, 420 * 380 / 2, data);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, head.miplevels, head.glinternalformat, head.pixelwidth, head.pixelheight);
				{
					unsigned char * ptr = data;
					unsigned int height = head.pixelheight;
					unsigned int width = head.pixelwidth;
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					for (unsigned int i = 0; i < head.miplevels; i++)
					{
						glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, height, head.glformat, head.gltype, ptr);
						ptr += height * calculateStride(head, width, 1);
						height >>= 1;
						width >>= 1;
						if (!height)
							height = 1;
						if (!width)
							width = 1;
					}
				}
			}
			break;
		case GL_TEXTURE_3D:
			glTexStorage3D(GL_TEXTURE_3D, head.miplevels, head.glinternalformat, head.pixelwidth, head.pixelheight, head.pixeldepth);
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, head.pixelwidth, head.pixelheight, head.pixeldepth, head.glformat, head.gltype, data);
			break;
		case GL_TEXTURE_1D_ARRAY:
			glTexStorage2D(GL_TEXTURE_1D_ARRAY, head.miplevels, head.glinternalformat, head.pixelwidth, head.arrayelements);
			glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, head.pixelwidth, head.arrayelements, head.glformat, head.gltype, data);
			break;
		case GL_TEXTURE_2D_ARRAY:
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, head.miplevels, head.glinternalformat, head.pixelwidth, head.pixelheight, head.arrayelements);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, head.pixelwidth, head.pixelheight, head.arrayelements, head.glformat, head.gltype, data);
			break;
		case GL_TEXTURE_CUBE_MAP:
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, head.miplevels, head.glinternalformat, head.pixelwidth, head.pixelheight);
			// glTexSubImage3D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces, h.glformat, h.gltype, data);
			{
				unsigned int face_size = calculateFacesize(head);
				for (unsigned int i = 0; i < head.faces; i++)
				{
					glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, head.pixelwidth, head.pixelheight, head.glformat, head.gltype, data + face_size * i);
				}
			}
			break;
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, head.miplevels, head.glinternalformat, head.pixelwidth, head.pixelheight, head.arrayelements);
			glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, head.pixelwidth, head.pixelheight, head.faces * head.arrayelements, head.glformat, head.gltype, data);
			break;
		default:                                               // Should never happen
			goto fail_target;
		}

		if (head.miplevels == 1)
		{
			glGenerateMipmap(target);
		}

		retval = tex;

	fail_target:
		delete[] data;

	fail_head:;
	fail_read:;
		fclose(fp);

		return retval;
	}
}