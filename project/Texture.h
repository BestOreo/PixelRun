#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GLEW/glew.h>
#endif
#include <SOIL/SOIL.h>
#include <iostream>

// GL_TEXTURE_MAG_FILTER = GL_LINEAR
// GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR
#define TEXTURE_FILTER_TYPE_MAL_MILM 0

// GL_TEXTURE_MAG_FILTER = GL_NEAREST
// GL_TEXTURE_MIN_FILTER = GL_NEAREST
#define TEXTURE_FILTER_TYPE_MAN_MIN 1

class TextureHelper
{
public:
    /*
    /* 成功加载2D纹理则返回纹理对象Id 否则返回0
    */
    static  GLint load2DTexture(const char* filename, int filterType = TEXTURE_FILTER_TYPE_MAN_MIN, 
        GLint internalFormat = GL_RGBA, GLenum picFormat = GL_RGBA, int loadChannels = SOIL_LOAD_RGBA)
    {
        // Step1 创建并绑定纹理对象
        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Step2 设定wrap参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Step3 设定filter参数
        switch (filterType)
        {
            case TEXTURE_FILTER_TYPE_MAL_MILM: {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
                    break;
                }
            case TEXTURE_FILTER_TYPE_MAN_MIN: {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    break;
                }
            default: {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
                    break;
                }
        }

        // Step4 加载纹理
        GLubyte *imageData = NULL;
        int picWidth, picHeight;
        imageData = SOIL_load_image(filename, &picWidth, &picHeight, 0, loadChannels);
        if (imageData == NULL)
        {
            std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
            return 0;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, picWidth, picHeight,
            0, picFormat, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Step5 释放纹理图片资源
        SOIL_free_image_data(imageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureId;
    }

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

	static GLuint loadDDS(const char * imagepath) {

		unsigned char header[124];

		FILE *fp;

		/* try to open the file */
		fp = fopen(imagepath, "rb");
		if (fp == NULL) {
			printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
			return 0;
		}

		/* verify the type of file */
		char filecode[4];
		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0) {
			fclose(fp);
			return 0;
		}

		/* get the surface desc */
		fread(&header, 124, 1, fp);

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);


		unsigned char * buffer;
		unsigned int bufsize;
		/* how big is it going to be including all mipmaps? */
		bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
		fread(buffer, 1, bufsize, fp);
		/* close the file pointer */
		fclose(fp);

		unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
		unsigned int format;
		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
		}

		// Create one OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;

		/* load the mipmaps */
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;

			// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
			if (width < 1) width = 1;
			if (height < 1) height = 1;

		}

		free(buffer);

		return textureID;


	}
};
#endif // !TEXTURE_H_
