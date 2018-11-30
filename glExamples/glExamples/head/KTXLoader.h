#pragma once
namespace ktx {
	struct ktxHeader
	{
		unsigned char identifier[12];
		unsigned int endianness;
		unsigned int gltype;
		unsigned int gltypesize;
		unsigned int glformat;
		unsigned int glinternalformat;
		unsigned int glbaseinternalformat;
		unsigned int pixelwidth;
		unsigned int pixelheight;
		unsigned int pixeldepth;
		unsigned int arrayelements;
		unsigned int faces;
		unsigned int miplevels;
		unsigned int keypairbytes;
	};

	union  keyvaluepair
	{
		unsigned int size;
		unsigned char rawbytes[4];
	};

	unsigned int load(const char* filename, unsigned int tex = 0);
}