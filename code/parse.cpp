#include "tinyxml.h"
#include "tinystr.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
	TiXmlDocument doc(argc[1]);
	
	bool loadOk = doc.LoadFile();
	if(!loadOk)
	{
		cout << "coulld load:" << doc.ErrorDes() <<endl;
		
	}
	TiXmlPrinter printer;
	doc.Accept(&printer);
	cout << printer.CStr() <<endl;
	
	TiXmlElement *node = doc.FirsrtChildElement();
	cout << node->Value() <<endl;
	string t;
	node->QueryValueAttribute("type", &t);
	cout<< t<< endl;

	doc.FirstChild()->NextSibling()->ToElement()->QueryStringAttribute("type", &t);
	cout<<"2:" << t<< endl;
	TiXmlNode* child =NULL;
	TiXmlElement* element =NULL;
	TiXmlAtrribute* attr =NULL;
	int ct;
	while(childe = doc.FirstChild()->IterateChildren(child))
	{
		cout<<child->ValueStr()<< "\t";
		ct= child->Type();
		cout << ct << "\t";
		switch(ct)
		{
			case TiXmlNode::TINYXML_TEXT;
				break;
			case TiXmlNode::TINYXML_ELEMENT:
				element = child->ToElement();
				attr = element->FirstAtrribute();
				while(attr)
				{
					cout<< attr->NameTstr()<<"="<<attr->ValueStr();
					attr=attr->Next();
				}
				break;
		}
	}
	

	return 0;
}
