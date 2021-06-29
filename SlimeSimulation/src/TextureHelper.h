#pragma once
#include <glad/glad.h>

#include <iostream>

#include "vendor/stb_image/stb_image.h"

class TextureHelper
{
public:
    static unsigned int loadEmptyTexture(unsigned width, unsigned height, unsigned numberChannels)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        GLenum format;
        switch (numberChannels)
        {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                format = GL_RGB;
        }
    
        // binding texture, loading data
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        // generating mipmaps automaticly not needed
        // glGenerateMipmap(GL_TEXTURE_2D);
        // setting the parameters of the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        return textureID;
    }

    static unsigned int loadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        // loading the width, height, number of channels and the data of the texture
        int width, height, numberChannels;
        unsigned char* data = stbi_load(path, &width, &height, &numberChannels, 0);

        // if texture loaded successfully
        if (data)
        {
            GLenum format;
            switch (numberChannels)
            {
                case 1:
                    format = GL_RED;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    format = GL_RGB;
            }

            // binding texture, loading data
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            // generating mipmaps automaticly
            glGenerateMipmap(GL_TEXTURE_2D);

            // setting the parameters of the texture
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // GL_REPEAT
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // GL_REPEAT
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
        }

        // at last free the memory on the CPU side
        stbi_image_free(data);

        return textureID;
    }
};