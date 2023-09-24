#ifndef CHARACTER_H
#define CHARACTER_H
class   Character
{
public:
    Character()
    {
        x0          =   0;
        y0          =   0;
        x1          =   0;
        y1          =   0;
        texId       =   0;
    }
private:
    unsigned short   x0;
    unsigned short   y0;
    unsigned short   x1;
    unsigned short   y1;
    //! 索引纹理，即当前字符所在的纹理
    unsigned         texId;
};
#endif