/*
 * Additional procedures for vector theme support
 *
 * Slice, 2018
 *
 */


#include "nanosvg.h"
#include "FloatLib.h"

#include "lodepng.h"

#ifndef DEBUG_ALL
#define DEBUG_VEC 1
#else
#define DEBUG_VEC DEBUG_ALL
#endif

#if DEBUG_VEC == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_VEC, __VA_ARGS__)
#endif

#define TEST_MATH 0
#define TEST_SVG_IMAGE 1
#define TEST_SIZEOF 0


#define NSVG_RGB(r, g, b) (((unsigned int)b) | ((unsigned int)g << 8) | ((unsigned int)r << 16))
//#define NSVG_RGBA(r, g, b, a) (((unsigned int)b) | ((unsigned int)g << 8) | ((unsigned int)r << 16) | ((unsigned int)a << 24))

extern VOID
WaitForKeyPress(CHAR16 *Message);

extern void DumpFloat2 (char* s, float* t, int N);
extern EG_IMAGE *BackgroundImage;
extern EG_IMAGE *Banner;
extern EG_IMAGE *BigBack;

EFI_STATUS ParseSVGIcon(NSVGparser  *p, UINTN Id, float Scale)
{
  EFI_STATUS      Status = EFI_NOT_FOUND;
  NSVGimage       *SVGimage;
  NSVGrasterizer* rast = nsvgCreateRasterizer();
  SVGimage = p->image;
  CHAR8 *IconName = NULL;
  UINTN Size;
  if (Id == BUILTIN_ICON_BANNER) {
    IconName = AllocateCopyPool(sizeof("Banner")+1, "Banner");
  } else if (Id == BUILTIN_ICON_BACKGROUND) {
    IconName = AllocateCopyPool(sizeof("Background")+1, "Background");
  } else {
    CHAR16 *IconPath = BuiltinIconTable[Id].Path;
    CHAR16 *ptr = StrStr(IconPath, L"\\");
    if (!ptr) {
      ptr = IconPath;
    } else {
      ptr++;
    }
    Size = StrLen(ptr)+1;
    IconName = AllocateZeroPool(Size);
    UnicodeStrToAsciiStrS(ptr, IconName, Size);
  }

  NSVGshape   *shape;
  NSVGgroup   *group;
  NSVGimage *IconImage = (NSVGimage*)AllocateZeroPool(sizeof(NSVGimage));
  NSVGshape *shapeNext, *shapePrev = NULL;

  shape = SVGimage->shapes;
  while (shape) {
    group = shape->group;

    shapeNext = shape->next;
    while (group) {
      if (strcmp(group->id, IconName) == 0) {
        break;
      }
      group = group->parent;
    }

    if (group) {
      DBG("found shape %a", shape->id);
      DBG(" from group %a\n", group->id);
      if (strstr(shape->id, "BoundingRect") != NULL) {
        IconImage->width = shape->bounds[2] - shape->bounds[0];
        IconImage->height = shape->bounds[3] - shape->bounds[1];
        memcpy(IconImage->realBounds, shape->bounds, 4*sizeof(float));
        if (!IconImage->height) {
          IconImage->height = 200;
        }
        DBG("IconImage size [%d,%d]\n", (int)IconImage->width, (int)IconImage->height);
        DBG("IconImage shift dx=%s dy=%s\n", PoolPrintFloat(IconImage->realBounds[0]), PoolPrintFloat(IconImage->realBounds[1]));
        //     shape->flags = 0;  //invisible
        shape = shapeNext;
        Status = EFI_SUCCESS;
        continue;
      }
      shape->next = IconImage->shapes; //add to head
      IconImage->shapes = shape;
      if (shapePrev) {
        shapePrev->next = shapeNext;
      }
    } else {
      shapePrev = shape;
    }
    shape = shapeNext;
  } //while shape
  if (Id == BUILTIN_ICON_BACKGROUND) { // special case for background width
    IconImage->width = UGAWidth / Scale;
  }

  float Height = IconImage->height * Scale;
  float Width = IconImage->width * Scale;

  if (!EFI_ERROR(Status)) {
    EG_IMAGE  *NewImage = egCreateImage((int)Width, (int)Height, TRUE);
    nsvgRasterize(rast, IconImage, 0,0,Scale,Scale, (UINT8*)NewImage->PixelData, (int)Width, (int)Height, (int)Width*4, NULL, NULL);
    if (Id == BUILTIN_ICON_BACKGROUND) { // special case for background
      BigBack = NewImage;
      GlobalConfig.BackgroundScale = imScale;
    } else {
      BuiltinIconTable[Id].Image = NewImage;
      if (Id == BUILTIN_ICON_BANNER) {
        Banner = NewImage;
      }
    }
  } else {
    BuiltinIconTable[Id].Image = NULL;
  }

  nsvgDeleteRasterizer(rast);
  return Status;
}

EFI_STATUS ParseSVGTheme(CONST CHAR8* buffer, TagPtr * dict, UINT32 bufSize)
{
  EFI_STATUS      Status;
  NSVGparser      *p = NULL, *p1 = NULL;
  NSVGfont        *fontSVG;
  NSVGimage       *SVGimage;
  NSVGtext        *text;
  NSVGrasterizer  *rast = nsvgCreateRasterizer();
  UINT8           *FileData = NULL;
  UINTN           FileDataLength = 0;
  INTN    i;

  float Scale;

  p = nsvgParse((CHAR8*)buffer, "px", 72);
  SVGimage = p->image;
  DBG("Image width=%d heigth=%d\n", (int)(SVGimage->width), (int)(SVGimage->height));
  float vbx = SVGimage->realBounds[2] - SVGimage->realBounds[0];
  float vby = SVGimage->realBounds[3] - SVGimage->realBounds[1];
  DBG("Image real-bounds: w=%d h=%d units=%a\n", (int)vbx, (int)vby, "px");
  if (SVGimage->height == 0) {
    if (vby > 1.0f) {
      SVGimage->height = vby;
    } else {
      SVGimage->height = 768.f;  //default height
    }
  }
  Scale = UGAHeight / SVGimage->height;
  DBG("using scale %s\n", PoolPrintFloat(Scale));
  fontSVG = p->font;
  if (fontSVG) {
    DBG("found embedded font-name=%a\n", fontSVG->fontFamily);
  }
  //TODO search attributes for MessageRow: font, size, color
  text = p->text;
  if (text && text->font) {
    DBG("text uses font-name=%a\n", text->font->fontFamily);
    //    DBG("text uses font size=%d\n", (int)text->font->fontSize);
    if (!fontSVG || strcmp(fontSVG->fontFamily, text->font->fontFamily) != 0) {
      Status = egLoadFile(ThemeDir, PoolPrint(L"%a.svg", text->font->fontFamily), &FileData, &FileDataLength);
      DBG("font loaded status=%r\n", Status);
      if (!EFI_ERROR(Status)) {
        p1 = nsvgParse((CHAR8*)FileData, "px", 72);
        if (!p1) {
          DBG("font not parsed\n");
        } else {
          fontSVG = p1->font;
          //         DBG("font %a parsed\n", fontSVG->fontFamily);
        }
        FreePool(FileData);
        FileData = NULL;
      }
    }
  }
  if (fontSVG) {
    if (text->font) {
      FontHeight = (int)(text->font->fontSize * Scale);
      DBG("font height=%d color=%x\n", FontHeight, text->fontColor);
    }
    if (!FontHeight) FontHeight = 16;
    LoadSVGfont(fontSVG, text->fontColor | 0xFF000000);
    DBG("font %a parsed\n", fontSVG->fontFamily);
  }

  // make background
  BackgroundImage = egCreateFilledImage(UGAWidth, UGAHeight, TRUE, &MenuBackgroundPixel);
  Status = ParseSVGIcon(p, BUILTIN_ICON_BACKGROUND, Scale);
  //make other icons
  for (i = BUILTIN_ICON_FUNC_ABOUT; i < BUILTIN_ICON_COUNT; i++) {
    Status = ParseSVGIcon(p, i, Scale);
    if (EFI_ERROR(Status)) {
      DBG("icon %d not parsed, status %r\n", Status);
    }
  }

#if 0 //test banner
  NSVGshape   *shape;
  NSVGgroup   *group;
  NSVGimage *Banner = (NSVGimage*)AllocateZeroPool(sizeof(NSVGimage));
  NSVGshape *shapeNext, *shapePrev = NULL;

  shape = SVGimage->shapes;
  while (shape) {
    group = shape->group;

    shapeNext = shape->next;
    while (group) {
      if (strcmp(group->id, "Banner") == 0) {
        break;
      }
      group = group->parent;
    }

    if (group) {
      DBG("found shape %a", shape->id);
      DBG(" from group %a\n", group->id);
      if (strstr(shape->id, "BoundingRect") != NULL) {
        Banner->width = shape->bounds[2] - shape->bounds[0];
        Banner->height = shape->bounds[3] - shape->bounds[1];
        memcpy(Banner->realBounds, shape->bounds, 4*sizeof(float));
        if (!Banner->height) {
          Banner->height = 200;
        }
        DBG("Banner size [%d,%d]\n", (int)Banner->width, (int)Banner->height);
        DBG("Banner shift dx=%s dy=%s\n", PoolPrintFloat(Banner->realBounds[0]), PoolPrintFloat(Banner->realBounds[1]));
        //     shape->flags = 0;  //invisible
        shape = shapeNext;
        continue;
      }
      shape->next = Banner->shapes; //add to head
      Banner->shapes = shape;
      if (shapePrev) {
        shapePrev->next = shapeNext;
      }
    } else {
      shapePrev = shape;
    }
    shape = shapeNext;
  }

  //suppose Banner will be 2-30% of Height
  float Height = UGAHeight * 0.28f;

  float Width = Banner->width * Scale;
  EG_IMAGE        *NewImage;
  // FreePool(FileData);
  // FileData = NULL;
  if (Width > 1.0f) {
    NewImage = egCreateImage((int)Width, (int)Height, TRUE);
    DBG("new banner size=[%d,%d]\n", (int)Width, (int)Height);

    float tx = 0; //-Banner->realBounds[0] * Scale;
    float ty = 0; //-Banner->realBounds[1] * Scale;
    //  DBG("Banner shift by %d, %d\n", (int)tx, (int)ty);

    nsvgRasterize(rast, Banner, tx,ty,Scale,Scale, (UINT8*)NewImage->PixelData, (int)Width, (int)Height, (int)Width*4, NULL, NULL);
    //now show it!

    BltImageAlpha(NewImage,
                  (int)(UGAWidth - Width) / 4,
                  (int)(UGAHeight * 0.05f),
                  &MenuBackgroundPixel,
                  16);
    WaitForKeyPress(L"waiting for key press...\n");
    //save banner as png yyyyy

    EFI_UGA_PIXEL *ImagePNG = (EFI_UGA_PIXEL *)NewImage->PixelData;
    unsigned lode_return = eglodepng_encode(&FileData, &FileDataLength, (CONST UINT8*)ImagePNG, (UINTN)NewImage->Width, (UINTN)NewImage->Height);
    if (!lode_return) {
      Status = egSaveFile(NULL, L"\\Banner.png", FileData, FileDataLength);
      FreePool(FileData);
      FileData = NULL;
    }
    egFreeImage(NewImage);

  }
  //  nsvg__deleteParser(p);

#endif
  //Test text

#if 1  // test font
  if (fontSVG) {
    INTN iHeight = 260;
    INTN iWidth = UGAWidth-200;
    DBG("create textbuffer\n");
    EG_IMAGE* TextBufferXY = egCreateFilledImage(iWidth, iHeight, TRUE, &DarkSelectionPixel);
    drawSVGtext(TextBufferXY, fontSVG, L"Clover ready", NSVG_RGBA(150, 150, 150, 255));
    //---------
    //save picture as png yyyyy
    UINT8           *FileData = NULL;
    UINTN           FileDataLength = 0U;
    EFI_UGA_PIXEL *ImagePNG = (EFI_UGA_PIXEL *)TextBufferXY->PixelData;

    unsigned lode_return =
    eglodepng_encode(&FileData, &FileDataLength, (CONST UINT8*)ImagePNG, iWidth, iHeight);
    DBG("  encode %d filelen=%d\n", lode_return, FileDataLength);
    if (!lode_return) {
      Status = egSaveFile(SelfRootDir, L"\\TestTextSVG.png", FileData, FileDataLength);
      DBG("save file status=%r\n", Status);
    } else {
      DBG("wrong encode %d\n", lode_return);
    }
    //----------


    DBG("text ready to blit\n");
    BltImageAlpha(TextBufferXY,
                  (UGAWidth - iWidth) / 2,
                  (UGAHeight - iHeight) / 2,
                  &MenuBackgroundPixel,
                  16);
    egFreeImage(TextBufferXY);
    //    DBG("draw finished\n");
    WaitForKeyPress(L"waiting for key press...\n");
  }
#endif
  if (p) {
    nsvg__deleteParser(p);
    p = NULL;
  }
  if (p1) {
    nsvg__deleteParser(p1);
    p1 = NULL;
  }

  nsvgDeleteRasterizer(rast);

  *dict = AllocatePool(sizeof(TagStruct));
  (*dict)->type = kTagTypeNone;
  GlobalConfig.TypeSVG = TRUE;
  return EFI_SUCCESS;
//  return EFI_NOT_READY;
}

VOID LoadSVGfont(NSVGfont  *fontSVG, UINT32 color)
{
//  EFI_STATUS      Status;
  float           FontScale;
  NSVGparser      *p;
  NSVGrasterizer  *rast;
  INTN i;
  if (!fontSVG) {
    return;
  }
  //free old font
  if (FontImage != NULL) {
    egFreeImage (FontImage);
    FontImage = NULL;
  }
  INTN Height = FontHeight + 2;
  INTN Width = Height * (0xC0 + GlobalConfig.CodepageSize);
  FontImage = egCreateImage(Width, Height, TRUE);
//  DBG("load font %a\n", fontSVG->fontFamily);
  if (!fontSVG->unitsPerEm) {
    fontSVG->unitsPerEm = 1000;
  }
  float fH = fontSVG->bbox[3] - fontSVG->bbox[1];
  if (fH == 0.f) {
    fH = (float)fontSVG->unitsPerEm;
  }
  FontScale = (float)FontHeight / fH;
  DBG("font scale %s\n", PoolPrintFloat(FontScale));

  p = nsvg__createParser();
  if (!p) {
    return;
  }
//  p->font = fontSVG;
  p->image->height = Height;
  p->image->width = Width;

  NSVGtext* text = (NSVGtext*)AllocateZeroPool(sizeof(NSVGtext));
  if (!text) {
    return;
  }
  text->fontSize = FontHeight;
  text->font = fontSVG;
  text->fontColor = color;

  DBG("LoadSVGfont: fontID=%a\n", text->font->id);
  DBG("LoadSVGfont:  family=%a\n", text->font->fontFamily);
  //add to head
  text->next = p->text;
  p->text = text;
  //for each letter rasterize glyph into FontImage
  //0..0xC0 == AsciiPageSize
  // cyrillic 0x410..0x450 at 0xC0
  INTN x = 0;
  INTN y = 0;
  p->isText = TRUE;
  for (i = 0; i < AsciiPageSize; i++) {
    if (i > 0x20) {
//      DBG("addLetter %x\n", i);
    }
    addLetter(p, i, x, y, FontScale, color);
    x += Height;
  }
  x = AsciiPageSize * Height;
  for (i = GlobalConfig.Codepage; i < GlobalConfig.Codepage+GlobalConfig.CodepageSize; i++) {
    addLetter(p, i, x, y, FontScale, color);
    x += Height;
  }
  p->image->realBounds[0] = fontSVG->bbox[0] * FontScale;
  p->image->realBounds[1] = fontSVG->bbox[1] * FontScale;
  p->image->realBounds[2] = fontSVG->bbox[2] * FontScale + x; //last bound
  p->image->realBounds[3] = fontSVG->bbox[3] * FontScale;

  //We made an image, then rasterize it
  rast = nsvgCreateRasterizer();
  DBG("begin raster text\n");
  nsvgRasterize(rast, p->image, 0, 0, 1.0f, 1.0f, (UINT8*)FontImage->PixelData, (int)Width, (int)Height, (int)(Width*4), NULL, NULL);
  DBG("end raster text\n");
#if DEBUG_FONT
  //save font as png yyyyy
  UINT8           *FileData = NULL;
  UINTN           FileDataLength = 0U;

  EFI_UGA_PIXEL *ImagePNG = (EFI_UGA_PIXEL *)FontImage->PixelData;

  unsigned lode_return =
    eglodepng_encode(&FileData, &FileDataLength, (CONST UINT8*)ImagePNG, (UINTN)FontImage->Width, (UINTN)FontImage->Height);

  if (!lode_return) {
    egSaveFile(NULL, L"\\FontSVG.png", FileData, FileDataLength);
  }
#endif
  nsvgDeleteRasterizer(rast);
  nsvg__deleteParser(p);
  return;
}
// it is for test purpose

VOID drawSVGtext(EG_IMAGE* TextBufferXY, VOID* font, CONST CHAR16* text, UINT32 color)
{
  INTN Width, Height;
  int i;
  UINTN len;
  NSVGparser* p;
  NSVGshape *shape; //, *cur, *prev;
  NSVGrasterizer* rast;
  NSVGfont* fontSVG = (NSVGfont*)font;
  float Scale, sx, sy;
  INTN x, y;
  if (!fontSVG) {
    DBG("no font\n");
    return;
  }
  if (!TextBufferXY) {
    DBG("no buffer\n");
    return;
  }
  p = nsvg__createParser();
  if (!p) {
    return;
  }
  len = StrLen(text);
  Width = TextBufferXY->Width;
  Height = TextBufferXY->Height;
//  Height = 180; //for test
  DBG("textBuffer: [%d,%d], fontUnits=%d\n", Width, TextBufferXY->Height, fontSVG->unitsPerEm);
  if (!fontSVG->unitsPerEm) {
    fontSVG->unitsPerEm = 1000;
  }
  float fH = fontSVG->bbox[3] - fontSVG->bbox[1]; //1250
  if (fH == 0.f) {
    fH = (float)fontSVG->unitsPerEm;  //1000
  }
  sy = (float)Height / fH; //(float)fontSVG->unitsPerEm; // 260./1250.
  //in font units
  float fW = fontSVG->bbox[2] - fontSVG->bbox[0];
  sx = (float)Width / (fW * len);
  Scale = (sx > sy)?sy:sx;
  x = 0;
  y = 0;
  for (i=0; i < len; i++) {
    CHAR16 letter = text[i];
    NSVGglyph* g;
    if (!letter) {
      break;
    }

    shape = (NSVGshape*)AllocateZeroPool(sizeof(NSVGshape));
    if (shape == NULL) return;
    shape->strokeWidth = 1.2f;

    g = fontSVG->glyphs;
    while (g) {
      if (g->unicode == letter) {
        shape->paths = g->path;
        if (shape->paths) {
          DBG("Found letter %x, point[0]=(%d,%d)\n", letter,
              (int)shape->paths->pts[0], (int)shape->paths->pts[1]);

        } else {
          DBG("Found letter %x, no path\n", letter);
        }
        break;
      }
      g = g->next;
    }
    if (!g) {
      //missing glyph
      NSVGglyph* g = fontSVG->missingGlyph;
      shape->paths = g->path;
      DBG("Missing letter %x\n", letter);
    }
    if (!shape->paths) {
      if (g) {
        x += g->horizAdvX;
      }
      if (shape) {
        FreePool(shape);
      }
      continue;
    }
    //fill shape
    shape->id[0] = (char)(letter & 0xff);
    shape->id[1] = (char)((letter >> 8) & 0xff);
    shape->fill.type = NSVG_PAINT_COLOR;
    shape->fill.color = color; //NSVG_RGBA(150, 150, 150, 255); //dark grey 20%
    shape->stroke.type = NSVG_PAINT_NONE;
    shape->stroke.color = NSVG_RGBA(0,0,0, 255); //black?
    shape->strokeWidth = 2.0f;
    shape->flags = NSVG_FLAGS_VISIBLE;
    nsvg__xformIdentity(shape->xform);
    shape->xform[0] = 1.f;
    shape->xform[3] = -1.f; //glyphs are mirrored by Y
    shape->xform[4] = (float)x - fontSVG->bbox[0];
    shape->xform[5] = (float)y - fontSVG->bbox[3]; // Y2 is a floor for a letter
//    DumpFloat2(shape->xform, 6);
    //in glyph units
    shape->bounds[0] = fontSVG->bbox[0];
    shape->bounds[1] = fontSVG->bbox[1];
    shape->bounds[2] = fontSVG->bbox[2];
    shape->bounds[3] = fontSVG->bbox[3];
    DumpFloat2("letter bounds", shape->bounds, 4);

    x += g->horizAdvX; //position for next letter
    shape->strokeLineJoin = NSVG_JOIN_MITER;
    shape->strokeLineCap = NSVG_CAP_BUTT;
    shape->miterLimit = 4;
    shape->fillRule = NSVG_FILLRULE_NONZERO;
    shape->opacity = 1;

    // Add to tail
    if (p->image->shapes == NULL)
      p->image->shapes = shape;
    else
      p->shapesTail->next = shape;
    p->shapesTail = shape;
  } //end of text

  p->image->realBounds[0] = fontSVG->bbox[0];
  p->image->realBounds[1] = fontSVG->bbox[1];
  p->image->realBounds[2] = fontSVG->bbox[2] + x; //last bound
  p->image->realBounds[3] = fontSVG->bbox[3];

  //We made an image, then rasterize it
  rast = nsvgCreateRasterizer();
  DBG("begin raster text, scale=%s\n", PoolPrintFloat(Scale));
  nsvgRasterize(rast, p->image, 0, 0, Scale, Scale, (UINT8*)TextBufferXY->PixelData, (int)Width, (int)Height, (int)(Width*4), NULL, NULL);

  DBG("end raster text\n");
  nsvgDeleteRasterizer(rast);
}

VOID testSVG()
{
  do {

    EFI_STATUS      Status;
    UINT8           *FileData = NULL;
    UINTN           FileDataLength = 0;

    INTN Width = 400, Height = 400;

#if TEST_MATH
    //Test mathematique
#define fabsf(x) ((x >= 0.0f)?x:(-x))
#define pr(x) (int)fabsf(x), (int)fabsf((x - (int)x) * 1000000.0f)
    int i;
    float x, y1, y2;
    //          CHAR8 Str[128];
    DBG("Test float: -%d.%06d\n", pr(-0.7612f));
    for (i=0; i<15; i++) {
      x=(PI)/30.0f*i;
      y1=SinF(x);
      y2=CosF(x);

      DBG("x=%d: %d.%06d ", i*6, pr(x));
      DBG("  sinx=%c%d.%06d", (y1<0)?'-':' ', pr(y1));
      DBG("  cosx=%c%d.%06d\n", (y2<0)?'-':' ',pr(y2));
      y1 = Atan2F(y1, y2);
      DBG("  atan2x=%c%d.%06d", (y1<0)?'-':' ',pr(y1));
      y1 = AcosF(y2);
      DBG("  acos=%c%d.%06d", (y1<0)?'-':' ',pr(y1));
      y1 = SqrtF(x);
      DBG("  sqrt=%d.%06d", pr(y1));
      y1 = CeilF(x);
      DBG("  ceil=%c%d.%06d\n", (y1<0)?'-':' ',pr(y1));
    }
#undef pr
#endif
    NSVGparser* p;
#if TEST_SVG_IMAGE
    NSVGrasterizer* rast = nsvgCreateRasterizer();
    EG_IMAGE        *NewImage;
    NSVGimage       *SVGimage;
    float Scale, ScaleX, ScaleY;
    // load file
    Status = egLoadFile(SelfRootDir, L"Sample.svg", &FileData, &FileDataLength);
    if (!EFI_ERROR(Status)) {
      //Parse XML to vector data

      p = nsvgParse((CHAR8*)FileData, "px", 72);
      SVGimage = p->image;
      DBG("Image width=%d heigth=%d\n", (int)(SVGimage->width), (int)(SVGimage->height));
      FreePool(FileData);

      // Rasterize
      NewImage = egCreateImage(Width, Height, TRUE);
      if (SVGimage->width <= 0) SVGimage->width = Width;
      if (SVGimage->height <= 0) SVGimage->height = Height;

      ScaleX = Width / SVGimage->width;
      ScaleY = Height / SVGimage->height;
      Scale = (ScaleX > ScaleY)?ScaleY:ScaleX;
      float tx = 0; //-SVGimage->realBounds[0] * Scale;
      float ty = 0; //-SVGimage->realBounds[1] * Scale;
      DBG("timing rasterize start tx=%s ty=%s\n", PoolPrintFloat(tx), PoolPrintFloat(ty));
      nsvgRasterize(rast, SVGimage, tx,ty,Scale,Scale, (UINT8*)NewImage->PixelData, (int)Width, (int)Height, (int)Width*4, NULL, NULL);
      DBG("timing rasterize end\n");
      //now show it!
      BltImageAlpha(NewImage,
                    (UGAWidth - Width) / 2,
                    (UGAHeight - Height) / 2,
                    &MenuBackgroundPixel,
                    16);
      egFreeImage(NewImage);
      nsvg__deleteParser(p);
      nsvgDeleteRasterizer(rast);
    }
#endif
    //Test text
    Height = 260;
    Width = UGAWidth-200;
    DBG("create textbuffer\n");
    EG_IMAGE* TextBufferXY = egCreateFilledImage(Width, Height, TRUE, &MenuBackgroundPixel);
    Status = egLoadFile(SelfRootDir, L"Font.svg", &FileData, &FileDataLength);
    DBG("font loaded status=%r\n", Status);
    if (!EFI_ERROR(Status)) {
      p = nsvgParse((CHAR8*)FileData, "px", 72);
      if (!p) {
        DBG("font not parsed\n");
        break;
      }
      NSVGfont* fontSVG = p->font;
      DBG("font parsed\n");
      FreePool(FileData);
      //   Scale = Height / fontSVG->unitsPerEm;
      drawSVGtext(TextBufferXY, fontSVG, L"Clover", NSVG_RGBA(0, 0, 0, 255));
      DBG("text ready to blit\n");
      BltImageAlpha(TextBufferXY,
                    (UGAWidth - Width) / 2,
                    (UGAHeight - Height) / 2,
                    &MenuBackgroundPixel,
                    16);
      egFreeImage(TextBufferXY);
      nsvg__deleteParser(p);
      DBG("draw finished\n");
    }
  } while (0);

}
