#ifndef CHARACTER_H
#define CHARACTER_H
#include<vector>
#include "ft2build.h"
#include "freetype/ftglyph.h"
#include FT_FREETYPE_H
using namespace std;
typedef struct  vertexData
{
    float x;
    float y;
    float z;
    float u;
    float v;
} VertexData;
class   Character
{
public:
    Character()
    {
        x0          =   0;
        y0          =   0;
        x1          =   0;
        y1          =   0;
        offsetX     =   0;
        offsetY     =   0;
    }
    //存储当前字符在纹理上的坐标位置
    unsigned short   x0 = 10;
    unsigned short   y0 = 10;
    unsigned short   x1 = 10;
    unsigned short   y1 = 10;
    unsigned int   offsetX : 10;
    unsigned int   offsetY : 10;
};


class TextureFont
{
public:
    
    TextureFont(const char* ptrFontFile, int fontSize)
    {
        if (!ptrFontFile)
        {
            return;
        }
        m_fontSize = fontSize;
        m_xStart = 0;
        m_yStart = 0;
        memset(m_character, 0, sizeof(m_character));
        FT_Init_FreeType(&m_library);
        assert(m_library != 0);

        FT_New_Face(m_library, ptrFontFile, 0, &m_face);
        FT_Set_Char_Size(m_face, fontSize << 6, fontSize << 6, 72, 72);
        assert(m_face != 0);
        glGenTextures(1, &m_textureId);
        /**
        *   使用这个纹理id,或者叫绑定(关联)
        */
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        /**
        *   指定纹理的放大,缩小滤波，使用线性方式，即当图片放大的时候插值方式
        */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



        glTexImage2D(
            GL_TEXTURE_2D,      //! 指定是二维图片
            0,                  //! 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
            GL_ALPHA,           //! 纹理的使用的存储格式
            1024,
            1024,
            0,                  //! 是否的边
            GL_ALPHA,           //! 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
            GL_UNSIGNED_BYTE,   //! 数据是8bit数据
            0
        );
    }
    
    Character* GetCharacter(wchar_t ch)
    {
        if (m_character[ch].x0 == 0 &&
            m_character[ch].x0 == 0 &&
            m_character[ch].x1 == 0 &&
            m_character[ch].y1 == 0
            )
        {
            /**
            *   说明字符还没有绘制到纹理上，则进行绘制
            */
            if (m_xStart + m_fontSize > 1024)
            {
                /**
                *   写满一行,从新开始
                */
                m_xStart = 0;
                /**
                *   y开始位置要增加
                */
                m_yStart += m_fontSize;
            }

            FT_Load_Glyph(m_face, FT_Get_Char_Index(m_face, ch), FT_LOAD_DEFAULT);
            FT_Glyph glyph;
            FT_Get_Glyph(m_face->glyph, &glyph);

            FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
            FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
            FT_Bitmap& bitmap = bitmap_glyph->bitmap;
            /**
            *   如果没有数据，则不写，直接过去
            */
            if (bitmap.width == 0 || bitmap.rows == 0)
            {
                m_xStart += m_fontSize / 2;

                m_character[ch].x0 = m_xStart;
                m_character[ch].y0 = m_yStart;
                m_character[ch].x1 = m_xStart + bitmap.width;
                m_character[ch].y1 = m_yStart + bitmap.rows;
                m_character[ch].offsetY = 0;
                m_character[ch].offsetX = 0;

            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_textureId);

                m_character[ch].x0 = m_xStart;
                m_character[ch].y0 = m_yStart;
                m_character[ch].x1 = m_xStart + bitmap.width;
                m_character[ch].y1 = m_yStart + bitmap.rows;

                m_character[ch].offsetY = bitmap_glyph->top;
                m_character[ch].offsetX = bitmap_glyph->left;

                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    m_xStart,
                    m_yStart,
                    bitmap.width,
                    bitmap.rows,
                    GL_ALPHA,
                    GL_UNSIGNED_BYTE,
                    bitmap.buffer
                );
                m_xStart += bitmap.width + 1;
            }
        }
        return  &m_character[ch];
    }

    bool  GetVertexData(float x, float y, const wchar_t* ptrText,vector<VertexData> & vecVertexData)
    {
        bool bret = false;
        if (!ptrText)
        {
            return bret;
        }
        float       texWidth = 1024;
        float       texHeight = 1024;
        float       xStart = x;
        float       yStart = y + 16;
        float       zStart = -1;
        unsigned    index = 0;
        unsigned    nSize = wcslen(ptrText);
        float       fHeight = 0;


        for (unsigned i = 0; i < nSize; ++i)
        {
            Character* ch = GetCharacter(ptrText[i]);

            int         h = ch->y1 - ch->y0;
            int         w = ch->x1 - ch->x0;
            float       offset = (float(h) - float(ch->offsetY));
            float       offsetX = float(ch->offsetX);

            VertexData vd;
            /**
            *   第一个点
            */
            vd.x = xStart;
            vd.y = yStart - h + offset;
            vd.z = zStart;
            vd.u = ch->x0 / texWidth;
            vd.v = ch->y0 / texHeight;
            vecVertexData.emplace_back(vd);
            /**
            *   第二个点
            */
        
            vd.x = xStart + w;
            vd.y = yStart - h + offset;
            vd.z = zStart;
            vd.u = ch->x1 / texWidth;
            vd.v = ch->y0 / texHeight;
            vecVertexData.emplace_back(vd);
            /**
            *   第三个点
            */
         
            vd.x = xStart + w;
            vd.y = yStart + offset;
            vd.z = zStart;
            vd.u = ch->x1 / texWidth;
            vd.v = ch->y1 / texHeight;
            vecVertexData.emplace_back(vd);

            /**
            *   第四个点
            */
           
            vd.x = xStart;
            vd.y = yStart + offset;
            vd.z = zStart;
            vd.u = ch->x0 / texWidth;
            vd.v = ch->y1 / texHeight;
            vecVertexData.emplace_back(vd);
            index += 4;
            xStart += w + (ch->offsetX);
        }
        bret = vecVertexData.size() > 0;
    }
    unsigned GetTexturedId()
    {
        return m_textureId;
    }
protected:
    Character m_character[1 << 16];
    FT_Library  m_library;
    FT_Face     m_face;
    unsigned    m_textureId;
    int         m_yStart;
    int         m_xStart;
    int         m_fontSize;
};
#endif