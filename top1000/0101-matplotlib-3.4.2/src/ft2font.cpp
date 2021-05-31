/* -*- mode: c++; c-basic-offset: 4 -*- */

#define NO_IMPORT_ARRAY

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ft2font.h"
#include "mplutils.h"
#include "numpy_cpp.h"
#include "py_exceptions.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338328
#endif

/**
 To improve the hinting of the fonts, this code uses a hack
 presented here:

 http://antigrain.com/research/font_rasterization/index.html

 The idea is to limit the effect of hinting in the x-direction, while
 preserving hinting in the y-direction.  Since freetype does not
 support this directly, the dpi in the x-direction is set higher than
 in the y-direction, which affects the hinting grid.  Then, a global
 transform is placed on the font to shrink it back to the desired
 size.  While it is a bit surprising that the dpi setting affects
 hinting, whereas the global transform does not, this is documented
 behavior of FreeType, and therefore hopefully unlikely to change.
 The FreeType 2 tutorial says:

      NOTE: The transformation is applied to every glyph that is
      loaded through FT_Load_Glyph and is completely independent of
      any hinting process. This means that you won't get the same
      results if you load a glyph at the size of 24 pixels, or a glyph
      at the size at 12 pixels scaled by 2 through a transform,
      because the hints will have been computed differently (except
      you have disabled hints).
 */

FT_Library _ft2Library;

void throw_ft_error(std::string message, FT_Error error) {
    std::ostringstream os("");
    os << message << " (error code 0x" << std::hex << error << ")";
    throw std::runtime_error(os.str());
}

FT2Image::FT2Image() : m_dirty(true), m_buffer(NULL), m_width(0), m_height(0)
{
}

FT2Image::FT2Image(unsigned long width, unsigned long height)
    : m_dirty(true), m_buffer(NULL), m_width(0), m_height(0)
{
    resize(width, height);
}

FT2Image::~FT2Image()
{
    delete[] m_buffer;
}

void FT2Image::resize(long width, long height)
{
    if (width <= 0) {
        width = 1;
    }
    if (height <= 0) {
        height = 1;
    }
    size_t numBytes = width * height;

    if ((unsigned long)width != m_width || (unsigned long)height != m_height) {
        if (numBytes > m_width * m_height) {
            delete[] m_buffer;
            m_buffer = NULL;
            m_buffer = new unsigned char[numBytes];
        }

        m_width = (unsigned long)width;
        m_height = (unsigned long)height;
    }

    if (numBytes && m_buffer) {
        memset(m_buffer, 0, numBytes);
    }

    m_dirty = true;
}

void FT2Image::draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y)
{
    FT_Int image_width = (FT_Int)m_width;
    FT_Int image_height = (FT_Int)m_height;
    FT_Int char_width = bitmap->width;
    FT_Int char_height = bitmap->rows;

    FT_Int x1 = CLAMP(x, 0, image_width);
    FT_Int y1 = CLAMP(y, 0, image_height);
    FT_Int x2 = CLAMP(x + char_width, 0, image_width);
    FT_Int y2 = CLAMP(y + char_height, 0, image_height);

    FT_Int x_start = MAX(0, -x);
    FT_Int y_offset = y1 - MAX(0, -y);

    if (bitmap->pixel_mode == FT_PIXEL_MODE_GRAY) {
        for (FT_Int i = y1; i < y2; ++i) {
            unsigned char *dst = m_buffer + (i * image_width + x1);
            unsigned char *src = bitmap->buffer + (((i - y_offset) * bitmap->pitch) + x_start);
            for (FT_Int j = x1; j < x2; ++j, ++dst, ++src)
                *dst |= *src;
        }
    } else if (bitmap->pixel_mode == FT_PIXEL_MODE_MONO) {
        for (FT_Int i = y1; i < y2; ++i) {
            unsigned char *dst = m_buffer + (i * image_width + x1);
            unsigned char *src = bitmap->buffer + ((i - y_offset) * bitmap->pitch);
            for (FT_Int j = x1; j < x2; ++j, ++dst) {
                int x = (j - x1 + x_start);
                int val = *(src + (x >> 3)) & (1 << (7 - (x & 0x7)));
                *dst = val ? 255 : *dst;
            }
        }
    } else {
        throw std::runtime_error("Unknown pixel mode");
    }

    m_dirty = true;
}

void FT2Image::draw_rect(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1)
{
    if (x0 > m_width || x1 > m_width || y0 > m_height || y1 > m_height) {
        throw std::runtime_error("Rect coords outside image bounds");
    }

    size_t top = y0 * m_width;
    size_t bottom = y1 * m_width;
    for (size_t i = x0; i < x1 + 1; ++i) {
        m_buffer[i + top] = 255;
        m_buffer[i + bottom] = 255;
    }

    for (size_t j = y0 + 1; j < y1; ++j) {
        m_buffer[x0 + j * m_width] = 255;
        m_buffer[x1 + j * m_width] = 255;
    }

    m_dirty = true;
}

void
FT2Image::draw_rect_filled(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1)
{
    x0 = std::min(x0, m_width);
    y0 = std::min(y0, m_height);
    x1 = std::min(x1 + 1, m_width);
    y1 = std::min(y1 + 1, m_height);

    for (size_t j = y0; j < y1; j++) {
        for (size_t i = x0; i < x1; i++) {
            m_buffer[i + j * m_width] = 255;
        }
    }

    m_dirty = true;
}

static FT_UInt ft_get_char_index_or_warn(FT_Face face, FT_ULong charcode)
{
    FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
    if (!glyph_index) {
        PyErr_WarnFormat(NULL, 1, "Glyph %lu missing from current font.", charcode);
        // Apparently PyErr_WarnFormat returns 0 even if the exception propagates
        // due to running with -Werror, so check the error flag directly instead.
        if (PyErr_Occurred()) {
            throw py::exception();
        }
    }
    return glyph_index;
}


// ft_outline_decomposer should be passed to FT_Outline_Decompose.  On the
// first pass, vertices and codes are set to NULL, and index is simply
// incremented for each vertex that should be inserted, so that it is set, at
// the end, to the total number of vertices.  On a second pass, vertices and
// codes should point to correctly sized arrays, and index set again to zero,
// to get fill vertices and codes with the outline decomposition.
struct ft_outline_decomposer
{
    int index;
    double* vertices;
    unsigned char* codes;
};

static int
ft_outline_move_to(FT_Vector const* to, void* user)
{
    ft_outline_decomposer* d = reinterpret_cast<ft_outline_decomposer*>(user);
    if (d->codes) {
        if (d->index) {
            // Appending ENDPOLY is important to make patheffects work.
            *(d->vertices++) = 0;
            *(d->vertices++) = 0;
            *(d->codes++) = ENDPOLY;
        }
        *(d->vertices++) = to->x / 64.;
        *(d->vertices++) = to->y / 64.;
        *(d->codes++) = MOVETO;
    }
    d->index += d->index ? 2 : 1;
    return 0;
}

static int
ft_outline_line_to(FT_Vector const* to, void* user)
{
    ft_outline_decomposer* d = reinterpret_cast<ft_outline_decomposer*>(user);
    if (d->codes) {
        *(d->vertices++) = to->x / 64.;
        *(d->vertices++) = to->y / 64.;
        *(d->codes++) = LINETO;
    }
    d->index++;
    return 0;
}

static int
ft_outline_conic_to(FT_Vector const* control, FT_Vector const* to, void* user)
{
    ft_outline_decomposer* d = reinterpret_cast<ft_outline_decomposer*>(user);
    if (d->codes) {
        *(d->vertices++) = control->x / 64.;
        *(d->vertices++) = control->y / 64.;
        *(d->vertices++) = to->x / 64.;
        *(d->vertices++) = to->y / 64.;
        *(d->codes++) = CURVE3;
        *(d->codes++) = CURVE3;
    }
    d->index += 2;
    return 0;
}

static int
ft_outline_cubic_to(
  FT_Vector const* c1, FT_Vector const* c2, FT_Vector const* to, void* user)
{
    ft_outline_decomposer* d = reinterpret_cast<ft_outline_decomposer*>(user);
    if (d->codes) {
        *(d->vertices++) = c1->x / 64.;
        *(d->vertices++) = c1->y / 64.;
        *(d->vertices++) = c2->x / 64.;
        *(d->vertices++) = c2->y / 64.;
        *(d->vertices++) = to->x / 64.;
        *(d->vertices++) = to->y / 64.;
        *(d->codes++) = CURVE4;
        *(d->codes++) = CURVE4;
        *(d->codes++) = CURVE4;
    }
    d->index += 3;
    return 0;
}

static FT_Outline_Funcs ft_outline_funcs = {
    ft_outline_move_to,
    ft_outline_line_to,
    ft_outline_conic_to,
    ft_outline_cubic_to};

PyObject*
FT2Font::get_path()
{
    if (!face->glyph) {
        PyErr_SetString(PyExc_RuntimeError, "No glyph loaded");
        return NULL;
    }
    ft_outline_decomposer decomposer = {};
    if (FT_Error error =
        FT_Outline_Decompose(
          &face->glyph->outline, &ft_outline_funcs, &decomposer)) {
        PyErr_Format(PyExc_RuntimeError,
                     "FT_Outline_Decompose failed with error 0x%x", error);
        return NULL;
    }
    if (!decomposer.index) {  // Don't append ENDPOLY to null glyphs.
      npy_intp vertices_dims[2] = { 0, 2 };
      numpy::array_view<double, 2> vertices(vertices_dims);
      npy_intp codes_dims[1] = { 0 };
      numpy::array_view<unsigned char, 1> codes(codes_dims);
      return Py_BuildValue("NN", vertices.pyobj(), codes.pyobj());
    }
    npy_intp vertices_dims[2] = { decomposer.index + 1, 2 };
    numpy::array_view<double, 2> vertices(vertices_dims);
    npy_intp codes_dims[1] = { decomposer.index + 1 };
    numpy::array_view<unsigned char, 1> codes(codes_dims);
    decomposer.index = 0;
    decomposer.vertices = vertices.data();
    decomposer.codes = codes.data();
    if (FT_Error error =
        FT_Outline_Decompose(
          &face->glyph->outline, &ft_outline_funcs, &decomposer)) {
        PyErr_Format(PyExc_RuntimeError,
                     "FT_Outline_Decompose failed with error 0x%x", error);
        return NULL;
    }
    *(decomposer.vertices++) = 0;
    *(decomposer.vertices++) = 0;
    *(decomposer.codes++) = ENDPOLY;
    return Py_BuildValue("NN", vertices.pyobj(), codes.pyobj());
}

FT2Font::FT2Font(FT_Open_Args &open_args, long hinting_factor_) : image(), face(NULL)
{
    clear();

    FT_Error error = FT_Open_Face(_ft2Library, &open_args, 0, &face);

    if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error("Can not load face.  Unknown file format.");
    } else if (error == FT_Err_Cannot_Open_Resource) {
        throw std::runtime_error("Can not load face.  Can not open resource.");
    } else if (error == FT_Err_Invalid_File_Format) {
        throw std::runtime_error("Can not load face.  Invalid file format.");
    } else if (error) {
        throw_ft_error("Can not load face", error);
    }

    // set default kerning factor to 0, i.e., no kerning manipulation
    kerning_factor = 0;

    // set a default fontsize 12 pt at 72dpi
    hinting_factor = hinting_factor_;

    error = FT_Set_Char_Size(face, 12 * 64, 0, 72 * (unsigned int)hinting_factor, 72);
    if (error) {
        FT_Done_Face(face);
        throw_ft_error("Could not set the fontsize", error);
    }

    if (open_args.stream != NULL) {
        face->face_flags |= FT_FACE_FLAG_EXTERNAL_STREAM;
    }

    FT_Matrix transform = { 65536 / hinting_factor, 0, 0, 65536 };
    FT_Set_Transform(face, &transform, 0);
}

FT2Font::~FT2Font()
{
    for (size_t i = 0; i < glyphs.size(); i++) {
        FT_Done_Glyph(glyphs[i]);
    }

    if (face) {
        FT_Done_Face(face);
    }
}

void FT2Font::clear()
{
    pen.x = 0;
    pen.y = 0;

    for (size_t i = 0; i < glyphs.size(); i++) {
        FT_Done_Glyph(glyphs[i]);
    }

    glyphs.clear();
}

void FT2Font::set_size(double ptsize, double dpi)
{
    FT_Error error = FT_Set_Char_Size(
        face, (FT_F26Dot6)(ptsize * 64), 0, (FT_UInt)(dpi * hinting_factor), (FT_UInt)dpi);
    if (error) {
        throw_ft_error("Could not set the fontsize", error);
    }
    FT_Matrix transform = { 65536 / hinting_factor, 0, 0, 65536 };
    FT_Set_Transform(face, &transform, 0);
}

void FT2Font::set_charmap(int i)
{
    if (i >= face->num_charmaps) {
        throw std::runtime_error("i exceeds the available number of char maps");
    }
    FT_CharMap charmap = face->charmaps[i];
    if (FT_Error error = FT_Set_Charmap(face, charmap)) {
        throw_ft_error("Could not set the charmap", error);
    }
}

void FT2Font::select_charmap(unsigned long i)
{
    if (FT_Error error = FT_Select_Charmap(face, (FT_Encoding)i)) {
        throw_ft_error("Could not set the charmap", error);
    }
}

int FT2Font::get_kerning(FT_UInt left, FT_UInt right, FT_UInt mode)
{
    if (!FT_HAS_KERNING(face)) {
        return 0;
    }
    FT_Vector delta;

    if (!FT_Get_Kerning(face, left, right, mode, &delta)) {
        return (int)(delta.x) / (hinting_factor << kerning_factor);
    } else {
        return 0;
    }
}

void FT2Font::set_kerning_factor(int factor)
{
    kerning_factor = factor;
}

void FT2Font::set_text(
    size_t N, uint32_t *codepoints, double angle, FT_Int32 flags, std::vector<double> &xys)
{
    FT_Matrix matrix; /* transformation matrix */

    angle = angle / 360.0 * 2 * M_PI;

    // this computes width and height in subpixels so we have to divide by 64
    matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

    FT_Bool use_kerning = FT_HAS_KERNING(face);
    FT_UInt previous = 0;

    clear();

    bbox.xMin = bbox.yMin = 32000;
    bbox.xMax = bbox.yMax = -32000;

    for (unsigned int n = 0; n < N; n++) {
        FT_UInt glyph_index;
        FT_BBox glyph_bbox;
        FT_Pos last_advance;

        glyph_index = ft_get_char_index_or_warn(face, codepoints[n]);

        // retrieve kerning distance and move pen position
        if (use_kerning && previous && glyph_index) {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
            pen.x += delta.x / (hinting_factor << kerning_factor);
        }
        if (FT_Error error = FT_Load_Glyph(face, glyph_index, flags)) {
            throw_ft_error("Could not load glyph", error);
        }
        // ignore errors, jump to next glyph

        // extract glyph image and store it in our table

        FT_Glyph thisGlyph;
        if (FT_Error error = FT_Get_Glyph(face->glyph, &thisGlyph)) {
            throw_ft_error("Could not get glyph", error);
        }
        // ignore errors, jump to next glyph

        last_advance = face->glyph->advance.x;
        FT_Glyph_Transform(thisGlyph, 0, &pen);
        FT_Glyph_Transform(thisGlyph, &matrix, 0);
        xys.push_back(pen.x);
        xys.push_back(pen.y);

        FT_Glyph_Get_CBox(thisGlyph, FT_GLYPH_BBOX_SUBPIXELS, &glyph_bbox);

        bbox.xMin = std::min(bbox.xMin, glyph_bbox.xMin);
        bbox.xMax = std::max(bbox.xMax, glyph_bbox.xMax);
        bbox.yMin = std::min(bbox.yMin, glyph_bbox.yMin);
        bbox.yMax = std::max(bbox.yMax, glyph_bbox.yMax);

        pen.x += last_advance;

        previous = glyph_index;
        glyphs.push_back(thisGlyph);
    }

    FT_Vector_Transform(&pen, &matrix);
    advance = pen.x;

    if (bbox.xMin > bbox.xMax) {
        bbox.xMin = bbox.yMin = bbox.xMax = bbox.yMax = 0;
    }
}

void FT2Font::load_char(long charcode, FT_Int32 flags)
{
    FT_UInt glyph_index = ft_get_char_index_or_warn(face, (FT_ULong)charcode);
    if (FT_Error error = FT_Load_Glyph(face, glyph_index, flags)) {
        throw_ft_error("Could not load charcode", error);
    }
    FT_Glyph thisGlyph;
    if (FT_Error error = FT_Get_Glyph(face->glyph, &thisGlyph)) {
        throw_ft_error("Could not get glyph", error);
    }
    glyphs.push_back(thisGlyph);
}

void FT2Font::load_glyph(FT_UInt glyph_index, FT_Int32 flags)
{
    if (FT_Error error = FT_Load_Glyph(face, glyph_index, flags)) {
        throw_ft_error("Could not load glyph", error);
    }
    FT_Glyph thisGlyph;
    if (FT_Error error = FT_Get_Glyph(face->glyph, &thisGlyph)) {
        throw_ft_error("Could not get glyph", error);
    }
    glyphs.push_back(thisGlyph);
}

void FT2Font::get_width_height(long *width, long *height)
{
    *width = advance;
    *height = bbox.yMax - bbox.yMin;
}

long FT2Font::get_descent()
{
    return -bbox.yMin;
}

void FT2Font::get_bitmap_offset(long *x, long *y)
{
    *x = bbox.xMin;
    *y = 0;
}

void FT2Font::draw_glyphs_to_bitmap(bool antialiased)
{
    size_t width = (bbox.xMax - bbox.xMin) / 64 + 2;
    size_t height = (bbox.yMax - bbox.yMin) / 64 + 2;

    image.resize(width, height);

    for (size_t n = 0; n < glyphs.size(); n++) {
        FT_Error error = FT_Glyph_To_Bitmap(
            &glyphs[n], antialiased ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO, 0, 1);
        if (error) {
            throw_ft_error("Could not convert glyph to bitmap", error);
        }

        FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyphs[n];
        // now, draw to our target surface (convert position)

        // bitmap left and top in pixel, string bbox in subpixel
        FT_Int x = (FT_Int)(bitmap->left - (bbox.xMin / 64.));
        FT_Int y = (FT_Int)((bbox.yMax / 64.) - bitmap->top + 1);

        image.draw_bitmap(&bitmap->bitmap, x, y);
    }
}

void FT2Font::get_xys(bool antialiased, std::vector<double> &xys)
{
    for (size_t n = 0; n < glyphs.size(); n++) {

        FT_Error error = FT_Glyph_To_Bitmap(
            &glyphs[n], antialiased ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO, 0, 1);
        if (error) {
            throw_ft_error("Could not convert glyph to bitmap", error);
        }

        FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyphs[n];

        // bitmap left and top in pixel, string bbox in subpixel
        FT_Int x = (FT_Int)(bitmap->left - bbox.xMin / 64.);
        FT_Int y = (FT_Int)(bbox.yMax / 64. - bitmap->top + 1);
        // make sure the index is non-neg
        x = x < 0 ? 0 : x;
        y = y < 0 ? 0 : y;
        xys.push_back(x);
        xys.push_back(y);
    }
}

void FT2Font::draw_glyph_to_bitmap(FT2Image &im, int x, int y, size_t glyphInd, bool antialiased)
{
    FT_Vector sub_offset;
    sub_offset.x = 0; // int((xd - (double)x) * 64.0);
    sub_offset.y = 0; // int((yd - (double)y) * 64.0);

    if (glyphInd >= glyphs.size()) {
        throw std::runtime_error("glyph num is out of range");
    }

    FT_Error error = FT_Glyph_To_Bitmap(
      &glyphs[glyphInd],
      antialiased ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO,
      &sub_offset, // additional translation
      1 // destroy image
      );
    if (error) {
        throw_ft_error("Could not convert glyph to bitmap", error);
    }

    FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyphs[glyphInd];

    im.draw_bitmap(&bitmap->bitmap, x + bitmap->left, y);
}

void FT2Font::get_glyph_name(unsigned int glyph_number, char *buffer)
{
    if (!FT_HAS_GLYPH_NAMES(face)) {
        /* Note that this generated name must match the name that
           is generated by ttconv in ttfont_CharStrings_getname. */
        PyOS_snprintf(buffer, 128, "uni%08x", glyph_number);
    } else {
        if (FT_Error error = FT_Get_Glyph_Name(face, glyph_number, buffer, 128)) {
            throw_ft_error("Could not get glyph names", error);
        }
    }
}

long FT2Font::get_name_index(char *name)
{
    return FT_Get_Name_Index(face, (FT_String *)name);
}
