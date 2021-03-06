//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

// WARNING: WIP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <errno.h>

#ifdef _WIN32
#include <conio.h>
char const *blacklist = "\"";
#else
char const *blacklist = "'";
#endif

std::string getStart_sector(std::string fileName) {
	unsigned start = fileName.find('_');
	unsigned end = fileName.find('.');
	if (start == std::string::npos) {
		fprintf(stderr, "ERROR: '%s' does not specify the start sector, xml will be incomplete!", const_cast<char*>(fileName.c_str()));
	}
	return fileName.substr(start+1, end-start-1);
}

std::string getStart_byte_hex(std::string fileName) {
	int sector = (atoi(getStart_sector(fileName).c_str()))*512;
	std::stringstream ss;
	ss << "0x" << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << sector;
	return ss.str();
}

int getSize_in_KB(std::string fileName) {
	std::ifstream file;
	file.open(fileName.c_str(), std::ios_base::binary);
	file.seekg(0, std::ios_base::end);
	int size = file.tellg()/1024;
	file.close();
	return size;
}

int getNum_partition_sectors(std::string fileName) {
	return getSize_in_KB(fileName)*2;
}

std::string getLabel(std::string fileName) {
	unsigned end = fileName.find('_');
	if (end == std::string::npos) {
		fprintf(stderr, "ERROR: '%s' does not specify the label, xml will be incomplete!", const_cast<char*>(fileName.c_str()));
	}
	return fileName.substr(0, end);
}


int main() {
	std::cout << "NOTE: This tool uses information contained in the file names after DZ extraction " << std::endl
		<< "hence it's only suitable for LG devices. Make sure you don't rename the files from the DZ." << std::endl
		<< "Name of the file should look like \"aboot_152576.bin\" where aboot is the label" << std::endl
		<< "and the number specifies the start sector. This program reads those from a text file." << std::endl
		<< "Please look at partitions_example.txt."<< std::endl;
	std::cout << "Enter file path here > " << std::flush;
	std::string fileName;
	std::getline(std::cin, fileName);
	fileName.erase(std::remove(fileName.begin(), fileName.end(), blacklist[0]), fileName.end());


	std::ifstream file(fileName.c_str());
	if (file.is_open()) {
		std::ofstream xml("rawprogram0.xml");
		xml << "<xml version = \"1.0\" >" << std::endl
			<< "<data>" << std::endl
			<< "<!-- NOTE: This file is autogenerated by make_rawprogram0.cpp -->" << std::endl
			<< "<!-- Created by Adam Myczkowski (mycax) -->" << std::endl;

		std::string fileLine;
		while (std::getline(file, fileLine)) {
			xml << "  <program SECTOR_SIZE_IN_BYTES=\"512\" file_sector_offset=\"0\" filename=\"" << fileLine
				<< "\" label=\"" << getLabel(fileLine)
				<< "\" num_partition_sectors=\""  << getNum_partition_sectors(fileLine)
				<< "\" physical_partition_number=\"0\" size_in_KB=\"" << getSize_in_KB(fileLine)
				<< "\" sparse=\"false\" start_byte_hex=\"" << getStart_byte_hex(fileLine)
				<< "\" start_sector=\"" << getStart_sector(fileLine)
				<< "\" />" << std::endl;
		}

		xml << "</data>";
		xml.close();
		file.close();
	}

#ifdef _WIN32
	std::cout << "Press any key to exit . . ." << std::endl;
	while (true) {
		if (_kbhit()) {
			return EXIT_SUCCESS;
		}
	}
#else
	return 0;
#endif

}