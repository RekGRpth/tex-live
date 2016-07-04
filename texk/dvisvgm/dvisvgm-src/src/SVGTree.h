/*************************************************************************
** SVGTree.h                                                            **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#ifndef DVISVGM_SVGTREE_H
#define DVISVGM_SVGTREE_H

#include <map>
#include <set>
#include <stack>
#include "Color.h"
#include "GFGlyphTracer.h"
#include "Matrix.h"
#include "SVGCharHandler.h"
#include "XMLDocument.h"
#include "XMLNode.h"

class  BoundingBox;
class  Color;
class Font;
class  Matrix;
class  PhysicalFont;

class SVGTree {
	public:
		SVGTree ();
		~SVGTree ();
		void reset ();
		void write (std::ostream &os) const    {_doc.write(os);}
		void newPage (int pageno);
		void appendToDefs (XMLNode *node);
		void appendToPage (XMLNode *node);
		void prependToPage (XMLNode *node);
		void appendToDoc (XMLNode *node)  {_doc.append(node);}
		void appendToRoot (XMLNode *node) {_root->append(node);}
		void appendChar (int c, double x, double y) {_charHandler->appendChar(c, x, y);}
		void appendFontStyles (const std::set<const Font*> &fonts);
		void append (const PhysicalFont &font, const std::set<int> &chars, GFGlyphTracer::Callback *cb=0);
		void pushContextElement (XMLElementNode *node);
		void popContextElement ();
		void removeRedundantElements ();
		void setBBox (const BoundingBox &bbox);
		void setFont (int id, const Font &font);
		void setX (double x)              {_charHandler->notifyXAdjusted();}
		void setY (double y)              {_charHandler->notifyYAdjusted();}
		void setMatrix (const Matrix &m)  {_charHandler->setMatrix(m);}
		void setColor (const Color &c);
		void setVertical (bool state)     {_charHandler->setVertical(state);}
		void transformPage (const Matrix &m);
		Color getColor () const           {return _charHandler->getColor();}
		const Matrix& getMatrix () const  {return _charHandler->getMatrix();}
		XMLElementNode* rootNode () const {return _root;}

	public:
		static bool USE_FONTS;           ///< if true, create font references and don't draw paths directly
		static bool CREATE_CSS;          ///< define and use CSS classes to reference fonts?
		static bool CREATE_USE_ELEMENTS; ///< allow generation of <use/> elements?
		static bool RELATIVE_PATH_CMDS;  ///< relative path commands rather than absolute ones?
		static bool MERGE_CHARS;         ///< whether to merge chars with common properties into the same <text> tag
		static bool ADD_COMMENTS;        ///< add comments with additional information
		static double ZOOM_FACTOR;       ///< factor applied to width/height attribute

	private:
		XMLDocument _doc;
		XMLElementNode *_root, *_page, *_defs;
		SVGCharHandler *_charHandler;
		std::stack<XMLElementNode*> _contextElementStack;
};

#endif
