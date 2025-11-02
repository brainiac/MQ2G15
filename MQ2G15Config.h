#pragma once

#include "TinyXml.h"

#include <memory>
#include <string>
#include <vector>

enum XmlType
{
	Type_StaticText,
	Type_ProgressBar
};

class XmlPosition
{
	int x = 0;
	int y = 0;

public:
	XmlPosition(TiXmlElement* node);
	XmlPosition();

	void fromXml(TiXmlElement* node);

	int getX() const { return x; }
	int getY() const { return y; }

};

class XmlSize
{
	int width = 0;
	int height = 0;

public:
	XmlSize(TiXmlElement* node);
	XmlSize();

	void fromXml(TiXmlElement* node);

	int getWidth() const { return width; }
	int getHeight() const { return height; }
};

class XmlBase
{
protected:
	XmlType type = Type_StaticText;
	XmlPosition pos;

public:
	XmlBase(TiXmlElement* node);
	virtual ~XmlBase() {}

	const XmlPosition& getPos() const { return pos; }
	const XmlType& getType() const { return type; }
};

class XmlText : public XmlBase
{
protected:
	std::string text;
	int align = 0;
	int fontsize = 0;
	int length = 0;
	bool dynamic = false;
	bool scrolling = false;

public:
	XmlText(TiXmlElement* node);

	const std::string& getText() const { return text; }
	int getAlign() const { return align; }
	int getFontSize() const { return fontsize; }
	int getLength() const { return length; }
	bool isDynamic() const { return dynamic; }
	bool isScrolling() const { return scrolling; }
};

class XmlProgressBar : public XmlBase
{
	XmlSize     size;
	std::string value;
	int         style = 0;

public:
	XmlProgressBar(TiXmlElement* node);

	const XmlSize& getSize() const { return size; }
	const std::string& getValue() const { return value; }
	int getStyle() const { return style; }
};

class XmlPageLayout
{
	std::vector<XmlBase*> m_elements;
	std::string m_name;

	XmlBase* createElementFromXml(TiXmlElement* node);

public:
	XmlPageLayout();
	XmlPageLayout(TiXmlElement* node);
	~XmlPageLayout();

	void fromXml(TiXmlElement* node);

	uint32_t numElements() const { return static_cast<uint32_t>(m_elements.size()); }
	XmlBase* getElement(uint32_t n) const;
	const std::string& getName() const { return m_name; }
};

class XmlConfig
{
	std::vector<std::unique_ptr<XmlPageLayout>> m_pages;
	std::unique_ptr<TiXmlDocument> m_document;

	void cleanup();

public:
	XmlConfig();
	~XmlConfig();

	bool loadConfig(const std::string& filename);

	uint32_t numPages() const { return static_cast<uint32_t>(m_pages.size()); }
	XmlPageLayout* getPage(uint32_t n) const;
};
