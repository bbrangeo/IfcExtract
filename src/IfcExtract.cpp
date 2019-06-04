/********************************************************************************
 *                                                                              *
 * IfcExtract: Tool to convert an IFC file into an XML file containing          *
 * 2D geometry and the properties of objects in the IFC file. The output can    *
 * be processed further in other applications.                                  *
 *                                                                              *
 * This program is based on ifcextract which is a part of IfcOpenShell.         *
 *                                                                              *
 * IfcExtract is free software: you can redistribute it and/or modify           *
 * it under the terms of the Lesser GNU General Public License as published by  *
 * the Free Software Foundation, either version 3.0 of the License, or          *
 * (at your option) any later version.                                          *
 *                                                                              *
 * IfcExtract is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * Lesser GNU General Public License for more details.                          *
 *                                                                              *
 * You should have received a copy of the Lesser GNU General Public License     *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                              *
 ********************************************************************************/


#include "SvgSerializer.h"
#include "XmlSerializer.h"

#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>


std::string version = "1.1";
std::string version_date = "3 June 2019";


void die_after()
{
    boost::this_thread::sleep(boost::posix_time::seconds(10));
    exit(EXIT_FAILURE);
}


int error_message(std::string msg) {
    std::cerr << msg <<
        "\nThis program has finished and will exit after 10 seconds. Press ENTER to exit immediately.\n";
    boost::thread thrd(&die_after);
    std::cin.ignore(1);
    return EXIT_FAILURE;
}


int help_message(void) {
    std::cerr << 
        "Usage: IfcExtract [-e | -w] [-q] <IFC_file> [<IFC_file> ...]\n"
        "       IfcExtract -h | --help\n"
        "       IfcExtract -v | --version\n"
        "\nTool to convert an IFC file into an XML file containing 2D geometry and the properties of objects in the IFC file.\n"
        "The output can be processed further in other applications. The 3D shapes in the IFC input are sectioned half way between\n"
        "the lowest and heighest points with a maximum height of the section plane of 1 meter above the lowest point of a shape.\n"
        "The optional flags -q (quiet), -e (error) and -w (warning) set the verbosity level. Default is to log fatal errors and progress.\n"
        "\nVersion "<< version << ", " << version_date << "\n";
    return EXIT_SUCCESS;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        return error_message("Usage: IfcExtract <IFC_file> [<IFC_file> ...]\n"
            "\tRead IFC input and save extracted geometries and properties to XML output.\n"
			"\nVersion " + version + ". "
            "Enter 'IfcExtract -h' or 'IfcExtract --help' for more information and options.\n");
    }

    // Process command line parameters
    std::list<std::string> filenames;
    int loglevel = 0;

    for (int idx = 1; idx < argc; idx++) {
        std::string argument = argv[idx];
        if ((argument == "-h") || (argument == "--help")) {
            return help_message();
        } else if ((argument == "-v") || (argument == "--version")) {
            std::cout << version << std::endl;
            return EXIT_SUCCESS;
        } else if (argument == "-q") {
            loglevel = -1;
        } else if (argument == "-e") {
            loglevel = 1;
        } else if (argument == "-w") {
            loglevel = 2;
        } else if (boost::algorithm::to_lower_copy(argument).find(".ifc") == std::string::npos) {
            return error_message("The extension of " + argument + " must be .ifc\n");
        } else {
            filenames.push_back(argument);
        }
    }

    // Main loop
    for (std::list<std::string>::iterator input_filename = filenames.begin(); input_filename != filenames.end(); ++input_filename) {
        std::string output_xml_filename = *input_filename;
        output_xml_filename.replace(output_xml_filename.length() - 3, 3, "xml");

        std::stringstream error_stream;
        if (loglevel == 2) {
            Logger::Verbosity(Logger::Severity::LOG_WARNING);
        } else {
            Logger::Verbosity(Logger::Severity::LOG_ERROR);
        }
        Logger::SetOutput(NULL, &error_stream);

        // Update settings
        SerializerSettings settings;
        settings.set(IfcGeom::IteratorSettings::DISABLE_TRIANGULATION, true);
        settings.set(SerializerSettings::USE_ELEMENT_GUIDS, true);
        settings.set_deflection_tolerance(1e-3);
        settings.precision = SerializerSettings::DEFAULT_PRECISION;

        if (loglevel >= 0) {
            std::cout << "Processing " << *input_filename << "\n";
        }

        // Read IFC file
        IfcParse::IfcFile ifc_file;
        if (!ifc_file.Init(*input_filename)) {
            return error_message(error_stream.str() + "\nCannot read the IFC file\n");
        }

        // Create SVG for the shapes
        SvgSerializer svg_serializer(output_xml_filename, settings);
        svg_serializer.setFile(&ifc_file);
        svg_serializer.setUnitNameAndMagnitude("METER", 1.0f);
        svg_serializer.writeHeader();

        // Iterate over the objects
        IfcGeom::Iterator<real_t> context_iterator(settings, &ifc_file);
        if (!context_iterator.initialize()) {
            return error_message(error_stream.str() + "\nThe IFC file " + *input_filename + " contains unsupported data\n");
        }
        int counter = 0;
        do {
            IfcGeom::Element<real_t> *geom_object = context_iterator.get();
            svg_serializer.write(static_cast<const IfcGeom::BRepElement<real_t>*>(geom_object));
            if (loglevel >= 0) {
                counter++;
                if (counter % 5 == 0) {
                    std::cout << "." << std::flush;
                }
                if (counter % 500 == 0) {
                    std::cout << " " << counter << "\n" << std::flush;
                }
            }
        } while (context_iterator.next());

        // Write the output
        svg_serializer.finalize();

        // Create XML for the attribute values
        if (loglevel >= 0) {
            std::cout << "\nCreating XML\n";
        }
        XmlSerializer xml_serializer(output_xml_filename);
        xml_serializer.setFile(&ifc_file);
        xml_serializer.finalize();

        if (loglevel > 0) {
            std::cout << error_stream.str();
        }
    }

    return EXIT_SUCCESS;
}
