
#include "pch.h"

#include "MQ2G15.h"
#include "MQ2G15Config.h"
#include "MQ2G15PagesMono.h"

#include <string_view>

static bool XmlToBoolean(TiXmlElement* node, bool def)
{
	if (node == nullptr)
		return def;

	std::string boolStr = node->GetText();
	if (boolStr == "True")
		return true;

	if (boolStr == "False")
		return false;
	return def;
}

XmlPosition::XmlPosition()
{
	x = 0;
	y = 0;
}

XmlPosition::XmlPosition(TiXmlElement* node)
{
	fromXml(node);
}

void XmlPosition::fromXml(TiXmlElement* node)
{
	ASSERT(node != nullptr);

	// convert X coordinate
	if (TiXmlElement* coordX = node->FirstChildElement("X"))
	{
		x = atoi(coordX->GetText());
	}

	// convert Y coordinate
	if (TiXmlElement* coordY = node->FirstChildElement("Y"))
	{
		y = atoi(coordY->GetText());
	}
}

XmlSize::XmlSize()
{
	width = 0;
	height = 0;
}

XmlSize::XmlSize(TiXmlElement* node)
{
	fromXml(node);
}

void XmlSize::fromXml(TiXmlElement* node)
{
	ASSERT(node != nullptr);
	
	// convert Width tag
	if (TiXmlElement* pWidth = node->FirstChildElement("Width"))
	{
		width = atoi(pWidth->GetText());
	}

	if (TiXmlElement* pHeight = node->FirstChildElement("Height"))
	{
		height = atoi(pHeight->GetText());
	}
}

XmlBase::XmlBase(TiXmlElement* node)
{
	ASSERT(node != nullptr);
	
	if (TiXmlElement* position = node->FirstChildElement("Position"))
	{
		pos.fromXml(position);
	}
}

XmlText::XmlText(TiXmlElement* pElement) 
	: XmlBase(pElement)
{
	ASSERT(pElement != nullptr);
	
	// defaults
	align		= DT_LEFT;
	fontsize	= LG_SMALL;
	length		= 0;
	dynamic		= false;
	scrolling	= false;

	type		= Type_StaticText;
	TiXmlElement* pNode = nullptr;

	// read text tag	<Text>blah</Text>
	pNode = pElement->FirstChildElement("Text");
	if (pNode != nullptr)
	{
		text = pNode->GetText();
	}

	// read alignment tag				<Align>Left</Align>
	pNode = pElement->FirstChildElement("Align");
	if (pNode != nullptr)
	{
		std::string_view alignStr = pNode->GetText();
		if (alignStr == "Right")
		{
			align = DT_RIGHT;
		}
		else if (alignStr == "Center")
		{
			align = DT_CENTER;
		}
	}

	// read font size tag				<FontSize>Small</FontSize>
	pNode = pElement->FirstChildElement("FontSize");
	if (pNode != nullptr)
	{
		std::string_view sizeStr = pNode->GetText();
		if (sizeStr == "Medium")
		{
			fontsize = LG_MEDIUM;
		}
		else if (sizeStr == "Large")
		{
			fontsize = LG_BIG;
		}
	}

	// read max length tag				<Length>80</Length>
	pNode = pElement->FirstChildElement("Length");
	if (pNode != nullptr)
	{
		length = atoi(pNode->GetText());
	}
	if (length == 0)
	{
		length = 160 - pos.getX();
	}

	// read dynamic tag					<Dynamic>True</Dynamic>
	pNode = pElement->FirstChildElement("Dynamic");
	dynamic = XmlToBoolean(pNode, dynamic);

	// read scrolling tag				<Scrolling>False</Scrolling>
	pNode = pElement->FirstChildElement("Scrolling");
	scrolling = XmlToBoolean(pNode, scrolling);
}

XmlProgressBar::XmlProgressBar(TiXmlElement* pElement) 
	: XmlBase(pElement)
{
	ASSERT(pElement != nullptr);
	
	style		= LG_FILLED;
	type		= Type_ProgressBar;

	// read size tag
	TiXmlElement* pNode = pElement->FirstChildElement("Size");
	if (pNode != nullptr)
	{
		size.fromXml(pNode);
	}

	// read value tag
	pNode = pElement->FirstChildElement("Value");
	if (pNode != nullptr)
	{
		value = pNode->GetText();
	}

	pNode = pElement->FirstChildElement("Style");
	if (pNode != nullptr)
	{
		std::string_view styleStr = pNode->GetText();
		if (styleStr == "Filled")
		{
			style = LG_FILLED;
		}
		else if (styleStr == "Cursor")
		{
			style = LG_CURSOR;
		}
	}
}

XmlBase* XmlPageLayout::createElementFromXml(TiXmlElement* pElement)
{
	ASSERT(pElement != nullptr);

	std::string_view type = pElement->Attribute("Type");
	if (type == "Text")
	{
		return new XmlText(pElement);
	}

	if (type == "Gauge")
	{
		return new XmlProgressBar(pElement);
	}
	
	// unrecognized type
	return nullptr;
}

XmlPageLayout::XmlPageLayout()
{
	m_name = "Unnamed Page";
}

XmlPageLayout::XmlPageLayout(TiXmlElement* node)
{
	m_name = "Unnamed Page";
	fromXml(node);
}

XmlPageLayout::~XmlPageLayout()
{
	DebugSpew("~XmlPageLayout %p", (void*)this);
}

void XmlPageLayout::fromXml(TiXmlElement* pLayout)
{
	ASSERT(pLayout != nullptr);
	ASSERT(m_elements.empty());

	const char* pageName = pLayout->Attribute("Name");		// <PageLayout Name="Default">
	if (pageName != nullptr)
	{
		m_name = pageName;
	}

	TiXmlElement* pElement = nullptr;
	while ((pElement = static_cast<TiXmlElement*>(pLayout->IterateChildren("Element", pElement))))
	{
		if (XmlBase* element = createElementFromXml(pElement))
		{
			m_elements.push_back(element);
		}
	}
}

XmlBase* XmlPageLayout::getElement(uint32_t n) const
{
	ASSERT(n < static_cast<uint32_t>(m_elements.size()));
	return m_elements[n];
}

//-----------------------------------------------------------------------------

XmlConfig::XmlConfig()
{
}

XmlConfig::~XmlConfig()
{
	cleanup();
}

bool XmlConfig::loadConfig(const std::string& filename)
{
	// cleanup first
	cleanup();

	DebugSpew("Configuration File: %s", filename.c_str());

	m_document = std::make_unique<TiXmlDocument>(filename.c_str());
	if (m_document->LoadFile())
	{
		DebugSpew("%s: Loaded XML File", __FUNCTION__);
		TiXmlElement* pConfig = m_document->FirstChildElement("Config");
		if (pConfig != nullptr)
		{
			TiXmlElement* pLayout = nullptr;
			while ((pLayout = static_cast<TiXmlElement*>(pConfig->IterateChildren("PageLayout", pLayout))))
			{
				m_pages.push_back(std::make_unique<XmlPageLayout>(pLayout));
			}
			return true;
		}
	}
	else
	{
		if (m_document->Error())
		{
			const char* error = m_document->ErrorDesc();
			if (!error)
				error = "Unknown Error or Missing File";
			WriteChatf("\arFailed to load XML: %s", m_document->ErrorDesc());
		}
	}
	return false;
}

XmlPageLayout* XmlConfig::getPage(uint32_t n) const
{
	if (n < static_cast<uint32_t>(m_pages.size()))
	{
		return m_pages[n].get();
	}

	return nullptr;
}

void XmlConfig::cleanup()
{
	m_pages.clear();
	m_document.reset();
}
