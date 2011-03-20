#pragma once

#include "TinyXml.h"

using namespace std;

enum XmlType
{
	Type_StaticText,
	Type_ProgressBar
};

class XmlPosition
{
	int x, y;

public:
	XmlPosition(TiXmlElement* node);
	XmlPosition();

	void fromXml(TiXmlElement* node);

	int		getX()			{ return x; }
	int		getY()			{ return y; }

};

class XmlSize
{
	int width, height;

public:
	XmlSize(TiXmlElement* node);
	XmlSize();

	void fromXml(TiXmlElement* node);

	int		getWidth()		{ return width; }
	int		getHeight()		{ return height; }
};

class XmlBase
{
protected:
	XmlType type;
	XmlPosition pos;

public:
	XmlBase(TiXmlElement* node);

	XmlPosition	getPos()	{ return pos; }
	XmlType		getType()	{ return type; }
};

class XmlText : public XmlBase
{
protected:
	string		text;
	int			align;
	int			fontsize;
	int			length;
	bool		dynamic;
	bool		scrolling;

public:
	XmlText(TiXmlElement* node);

	string	getText()		{ return text; }
	int		getAlign()		{ return align; }
	int		getFontSize()	{ return fontsize; }
	int		getLength()		{ return length; }
	bool	isDynamic()		{ return dynamic; }
	bool	isScrolling()	{ return scrolling; }
};

class XmlProgressBar : public XmlBase
{
	XmlSize		size;
	string		value;
	int			style;
public:

	XmlProgressBar(TiXmlElement* node);

	XmlSize	getSize()		{ return size; }
	string	getValue()		{ return value; }
	int		getStyle()		{ return style; }
};

class XmlPageLayout
{
	vector<XmlBase*> elements;
	string name;

	XmlBase* createElementFromXml(TiXmlElement* node);

public:
	XmlPageLayout();
	XmlPageLayout(TiXmlElement* node);
	~XmlPageLayout();

	void fromXml(TiXmlElement* node);

	int numElements() { return elements.size(); }
	XmlBase* getElement(int n);
	string getName() { return name; }
};

class XmlConfig
{
	vector<XmlPageLayout*> pages;
	TiXmlDocument* document;

	void cleanup();
	
public:
	XmlConfig();
	~XmlConfig();

	bool loadConfig(std::string filename);

	int numPages() { return pages.size(); }
	XmlPageLayout* getPage(int n);
};