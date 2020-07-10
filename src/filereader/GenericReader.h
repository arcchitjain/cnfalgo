
#ifndef cnfalgoGenericReader
#define cnfalgoGenericReader

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

namespace cnf
{

class GenericReader
{

  private:
  
	std::istream& data;
	
	std::map<std::string, int> dict1;
	
	std::map<int, std::string> dict2;
	
	int split(const std::string& tosplit, const std::string& delimiter, std::vector<std::string>& result) const {
		unsigned int pos_start = 0;
		unsigned int pos_end = static_cast<int>(tosplit.find(delimiter,pos_start));
		
		while (pos_end != static_cast<int>(std::string::npos)) {
			
			std::string sstr = tosplit.substr(pos_start,pos_end-pos_start);
			if (sstr.size() > 0) result.push_back(sstr);
			pos_start = pos_end + (static_cast<int>(delimiter.size()) == 0 ? 1 : static_cast<int>(delimiter.size()));
			pos_end = static_cast<int>(tosplit.find(delimiter,pos_start));
		}
		if (pos_start < tosplit.size()) {
			std::string sstr = tosplit.substr(pos_start);
			if (sstr.size() > 0) result.push_back(sstr);
		}
		return static_cast<int>(result.size());
	}

  public:

	GenericReader(std::istream& data) : data(data) {}

	~GenericReader() {}
	
	
	std::string getDelimiter() const {
		return " ";
	}
	
	int readline(std::vector<std::string>& line) {
		// read a line from the file
		std::string str;
		getline(data,str);
		split(str," ",line);
		return 0;
	}
	
	

	
	

};

}  // end namespace cnfalgo

#endif // cnfalgoGenericReader
