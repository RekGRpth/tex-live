/*
 * Copyright © 2018  Ebrahim Byagowi
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#ifndef HB_AAT_LAYOUT_ANKR_TABLE_HH
#define HB_AAT_LAYOUT_ANKR_TABLE_HH

#include "hb-aat-layout-common.hh"

/*
 * ankr -- Anchor Point
 * https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6ankr.html
 */
#define HB_AAT_TAG_ankr HB_TAG('a','n','k','r')


namespace AAT {


struct Anchor
{
  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this));
  }

  public:
  FWORD		xCoordinate;
  FWORD		yCoordinate;
  public:
  DEFINE_SIZE_STATIC (4);
};

typedef LArrayOf<Anchor> GlyphAnchors;

struct ankr
{
  static const hb_tag_t tableTag = HB_AAT_TAG_ankr;

  inline const Anchor &get_anchor (hb_codepoint_t glyph_id,
				   unsigned int i,
				   unsigned int num_glyphs,
				   const char *end) const
  {
    unsigned int offset = (this+lookupTable).get_value_or_null (glyph_id, num_glyphs);
    const GlyphAnchors &anchors = StructAtOffset<GlyphAnchors> (&(this+anchorData), offset);
    /* TODO Use sanitizer; to avoid overflows and more. */
    if (unlikely ((const char *) &anchors + anchors.get_size () > end))
      return Null(Anchor);
    return anchors[i];
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this) &&
			  version == 0 &&
			  lookupTable.sanitize (c, this)));
  }

  protected:
  HBUINT16	version; 	/* Version number (set to zero) */
  HBUINT16	flags;		/* Flags (currently unused; set to zero) */
  LOffsetTo<Lookup<Offset<HBUINT16, false> >, false>
		lookupTable;	/* Offset to the table's lookup table */
  LOffsetTo<HBUINT8, false>
		anchorData;	/* Offset to the glyph data table */

  public:
  DEFINE_SIZE_STATIC (12);
};

} /* namespace AAT */


#endif /* HB_AAT_LAYOUT_ANKR_TABLE_HH */
