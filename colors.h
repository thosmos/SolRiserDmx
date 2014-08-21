/**
 * A color represented by the four components Hue, Saturation, Value, and Alpha.
 * H must be between 0 and 360 (degrees).
 * S must be between 0 and 1.
 * V and A are normally between 0 and 1, but may exceed that range to give bloom.
 */
//typedef struct HSVColor
//{
//      float H, S, V;
//} HsvColor;
//
//typedef struct RGBColor
//{
//      float R, G, B;
//} RgbColor;

//
//typedef struct RgbColor
//{
//    unsigned char r;
//    unsigned char g;
//    unsigned char b;
//} RgbColor;
//
//typedef struct HsvColor
//{
//    unsigned char h;
//    unsigned char s;
//    unsigned char v;
//} HsvColor;
//
//RgbColor HsvToRgb(HsvColor hsv)
//{
//    RgbColor rgb;
//    unsigned char region, p, q, t;
//    unsigned int h, s, v, remainder;
//
//    if (hsv.s == 0)
//    {
//        rgb.r = hsv.v;
//        rgb.g = hsv.v;
//        rgb.b = hsv.v;
//        return rgb;
//    }
//
//    // converting to 16 bit to prevent overflow
//    h = hsv.h;
//    s = hsv.s;
//    v = hsv.v;
//
//    region = h / 43;
//    remainder = (h - (region * 43)) * 6; 
//
//    p = (v * (255 - s)) >> 8;
//    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
//    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
//
//    switch (region)
//    {
//        case 0:
//            rgb.r = v;
//            rgb.g = t;
//            rgb.b = p;
//            break;
//        case 1:
//            rgb.r = q;
//            rgb.g = v;
//            rgb.b = p;
//            break;
//        case 2:
//            rgb.r = p;
//            rgb.g = v;
//            rgb.b = t;
//            break;
//        case 3:
//            rgb.r = p;
//            rgb.g = q;
//            rgb.b = v;
//            break;
//        case 4:
//            rgb.r = t;
//            rgb.g = p;
//            rgb.b = v;
//            break;
//        default:
//            rgb.r = v;
//            rgb.g = p;
//            rgb.b = q;
//            break;
//    }
//
//    return rgb;
//}
//
//HsvColor RgbToHsv(RgbColor rgb)
//{
//    HsvColor hsv;
//    unsigned char rgbMin, rgbMax;
//
//    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
//    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);
//
//    hsv.v = rgbMax;
//    if (hsv.v == 0)
//    {
//        hsv.h = 0;
//        hsv.s = 0;
//        return hsv;
//    }
//
//    hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
//    if (hsv.s == 0)
//    {
//        hsv.h = 0;
//        return hsv;
//    }
//
//    if (rgbMax == rgb.r)
//        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
//    else if (rgbMax == rgb.g)
//        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
//    else
//        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
//
//    return hsv;
//}


typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} RgbColor;

    typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} HsvColor;

static HsvColor      RgbToHsv(RgbColor in);
static RgbColor      HsvToRgb(HsvColor in);
//double nan (const char* tagp);

HsvColor RgbToHsv(RgbColor in)
{
    HsvColor         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = 0.0;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


RgbColor HsvToRgb(HsvColor in)
{
    double      hh, p, q, t, ff;
    long        i;
    RgbColor         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}
