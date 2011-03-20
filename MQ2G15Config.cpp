
#include "StdAfx.h"

#include "MQ2G15.h"
#include "MQ2G15Config.h"
#include "MQ2G15PagesMono.h"

static bool XmlToBoolean(TiXmlElement* node, bool def)
{
	if (node == NULL)
		return def;
	string boolStr = node->GetText();
	if (boolStr == "True")
		return true;
	else if (boolStr == "False")
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
	ASSERT(node != NULL);

	// convert X coordinate
	TiXmlElement* coordX = node->FirstChildElement("X");
	if (coordX)
	{
		x = atoi(coordX->GetText());
	}

	// convert Y coordinate
	TiXmlElement* coordY = node->FirstChildElement("Y");
	if (coordY)
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
	ASSERT(node != NULL);
	
	// convert Width tag
	TiXmlElement* pWidth = node->FirstChildElement("Width");
	if (pWidth != NULL)
	{
		width = atoi(pWidth->GetText());
	}

	TiXmlElement* pHeight = node->FirstChildElement("Height");
	if (pHeight != NULL)
	{
		height = atoi(pHeight->GetText());
	}
}

XmlBase::XmlBase(TiXmlElement* node)
{
	ASSERT(node != NULL);
	
	TiXmlElement* position = node->FirstChildElement("Position");
	if (position != NULL)
	{
		pos.fromXml(position);
	}
}

XmlText::XmlText(TiXmlElement* pElement) 
	: XmlBase(pElement)
{
	ASSERT(pElement != NULL);
	
	// defaults
	align		= DT_LEFT;
	fontsize	= LG_SMALL;
	length		= 0;
	dynamic		= false;
	scrolling	= false;

	type		= Type_StaticText;
	TiXmlElement* pNode = NULL;

	// read text tag	<Text>blah</Text>
	pNode = pElement->FirstChildElement("Text");
	if (pNode != NULL)
	{
		text = pNode->GetText();
	}

	// read alignment tag				<Align>Left</Align>
	pNode = pElement->FirstChildElement("Align");
	if (pNode != NULL)
	{
		string alignStr = pNode->GetText();
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
	if (pNode != NULL)
	{
		string sizeStr = pNode->GetText();
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
	if (pNode != NULL)
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
	ASSERT(pElement != NULL);
	
	style		= LG_FILLED;
	type		= Type_ProgressBar;
	TiXmlElement* pNode = NULL;

	// read size tag
	pNode = pElement->FirstChildElement("Size");
	if (pNode != NULL)
	{
		size.fromXml(pNode);
	}

	// read value tag
	pNode = pElement->FirstChildElement("Value");
	if (pNode != NULL)
	{
		value = pNode->GetText();
	}

	pNode = pElement->FirstChildElement("Style");
	if (pNode != NULL)
	{
		string styleStr = pNode->GetText();
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
	ASSERT(pElement != NULL);

	string type = pElement->Attribute("Type");
	if (type == "Text")
	{
		return new XmlText(pElement);
	}
	else if (type == "Gauge")
	{
		return new XmlProgressBar(pElement);
	}
	else
	{
		// unrecognized type
		return NULL;
	}
}

XmlPageLayout::XmlPageLayout()
{
	name = "Unnamed Page";
}

XmlPageLayout::XmlPageLayout(TiXmlElement* node)
{
	name = "Unnamed Page";
	fromXml(node);
}

XmlPageLayout::~XmlPageLayout()
{
	vector<XmlBase*>::iterator it;
	for (it = elements.begin(); it != elements.end(); ++it)
	{
		delete *it;
	}
}

void XmlPageLayout::fromXml(TiXmlElement* pLayout)
{
	ASSERT(pLayout != NULL);
	ASSERT(elements.size() == 0);

	const char* pageName = pLayout->Attribute("Name");		// <PageLayout Name="Default">
	if (pageName != NULL)
	{
		name = pageName;
	}

	TiXmlElement* pElement = NULL;
	while (pElement = (TiXmlElement*)pLayout->IterateChildren("Element", pElement))
	{
		XmlBase* element = createElementFromXml(pElement);
		if (element != NULL)
		{
			elements.push_back(element);
		}
	}
}

XmlBase* XmlPageLayout::getElement(int n)
{
	ASSERT(n < (int)elements.size());
	return elements[n];
}

XmlConfig::XmlConfig()
{
	document = NULL;
}

XmlConfig::~XmlConfig()
{
	cleanup();
}

bool XmlConfig::loadConfig(string filename)
{
	// cleanup first
	cleanup();

	DebugSpew("Configuration File: %s", filename.c_str());

	document = new TiXmlDocument(filename.c_str());
	if (document->LoadFile())
	{
		DebugSpew("%s: Loaded XML File", __FUNCTION__);
		TiXmlElement* pConfig = document->FirstChildElement("Config");
		if (pConfig != NULL)
		{
			TiXmlElement* pLayout = NULL;
			while (pLayout = (TiXmlElement*)pConfig->IterateChildren("PageLayout", pLayout))
			{
				XmlPageLayout* layout = new XmlPageLayout(pLayout);
				{
					pages.push_back(layout);
				}
			}
			return true;
		}
	}
	else
	{
		if (document->Error())
		{
			const char* error = document->ErrorDesc();
			if (!error)
				error = "Unknown Error or Missing File";
			WriteChatf("\arFailed to load XML: %s", document->ErrorDesc());
		}
	}
	return false;
}

XmlPageLayout* XmlConfig::getPage(int n)
{
	if (n < (int)pages.size())
	{
		return pages[n];
	}
	return NULL;
}

void XmlConfig::cleanup()
{
	if (pages.size() > 0)
	{
		vector<XmlPageLayout*>::iterator it;
		for (it = pages.begin(); it != pages.end(); ++it)
		{
			XmlPageLayout* page = *it;
			delete page;
		}
		pages.erase(pages.begin(), pages.end());
	}

	if (document != NULL)
	{
		delete document;
		document = NULL;
	}
}
