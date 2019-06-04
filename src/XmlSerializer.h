/********************************************************************************
 *                                                                              *
 * This file is part of IfcOpenShell.                                           *
 *                                                                              *
 * IfcOpenShell is free software: you can redistribute it and/or modify         *
 * it under the terms of the Lesser GNU General Public License as published by  *
 * the Free Software Foundation, either version 3.0 of the License, or          *
 * (at your option) any later version.                                          *
 *                                                                              *
 * IfcOpenShell is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * Lesser GNU General Public License for more details.                          *
 *                                                                              *
 * You should have received a copy of the Lesser GNU General Public License     *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                              *
 ********************************************************************************/

#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include "Serializer.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class XmlSerializer : public Serializer {
private:
	IfcParse::IfcFile* file;
	std::string xml_filename;
	boost::property_tree::ptree svg_data;
public:
	XmlSerializer(const std::string& xml_filename)
		: Serializer()
		, xml_filename(xml_filename)
	{       
        // Parse the SVG XML file for later use.
        boost::property_tree::read_xml(xml_filename, svg_data, 
                boost::property_tree::xml_parser::trim_whitespace);
    }

	bool ready() { return true; }
	void writeHeader() {}
	void finalize();
	void setFile(IfcParse::IfcFile* f) { file = f; }
};

#endif