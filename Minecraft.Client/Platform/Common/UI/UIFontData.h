#pragma once

#include <unordered_map>

#define _DEBUG_BLOCK_CHARS 0

// For hardcoded font data.
struct SFontData {
public:
    static const unsigned short FONTCOLS = 23;
    static const unsigned short FONTROWS = 20;

    static const unsigned short FONTSIZE = FONTCOLS * FONTROWS;

public:
    // Font name.
    std::string m_strFontNam       // Filename of the glyph archive.
        std::wstring m_wstrFi      // Number of glyphs in the archive.
        unsigned int m_uiGlyp      // Unicode values of each glyph.
        unsigned short* m_arrCode  // X resolution of glyph archive.
        unsigned int m_uiGly       // Y resolution of glyph archive.
        unsigned int m_uiGly       // Number of columns in the glyph archive.
        unsigned int m_uiGlyphM    // Number of rows in the glyph archive.
        unsigned int m_uiGlyphM    // Width of each glyph.
        unsigned int m_uiGlyp      // Height of each glyph.
        unsigned int m_uiGlyph     // Ascent of each glyph above the baseline
                                   // (units?).
        float m_f       // Descent of each glyph below the baseline (units?).
        float m_fD      // How much to advance for each pixel wide the glyph
                        // is.
        float m_fAdvPe  // How many pixels wide any whitespace characters
                        // are.
        unsigned int m_uiWhitespaceWidth;

public:
    static unsigned short Codepoints[FONTSIZE];
    static SFontData Mojangles_7;
    static SFontData Mojangle  // Provides a common interface for dealing with
                               // font data.
        class CFontData {
    public:
        CFont// pbRawImage consumed by constructor.
	CFontData(SFontData &sFontData, int *pbRaw// Release memory.
	void				release();

pro// Hardcoded font data.
	SFontData			*m_sFo// Map Unicodepoints to glyph ids.
	std::unordered_map<unsigned int, unsigned short> m_unic// Kerning value for each glyph.
	unsigned short		*m_kernin// Binary blob of the archive image.
	unsigned char		*m_pbRa// Total advance of each character.
	float				*m_pfAdvanceTable;

// Accessor for the font name in the internal SFontData.
	const std::string		ge// Accessor for the hardcoded internal font data.
	SFontData			*ge// Get the glyph id corresponding to a unicode point.
	unsigned short		getGlyphId(unsigned int un// Get the unicodepoint corresponding to a glyph id.
	unsigned int		getUnicode(unsigned sho// Get a pointer to the top left pixel of a row/column in the raw image.
	unsigned char		*topLeftPixel(int ro// Get the row and column where a glyph appears in the archive.
	void				getPos(unsigned short gyphId, int &row// Get the advance of this character (units?).
	float				getAdvance(unsigned sho// Get the width (in pixels) of a given character.
	int					getWidth(unsigned sho// Returns true if this glyph is whitespace.
	bool				glyphIsWhitespace(unsigned shor// Returns true if this unicodepoint is whitespace
	bool				unicodeIsWhitespace(unsigned int unicodepoint// Move a pointer in an image dx pixels right and dy pixels down, wrap around in either dimension leads to unknown behaviour.
	void				moveCursor(unsigned char *&cursor, unsigned int dx, unsigned int dy);
    };
