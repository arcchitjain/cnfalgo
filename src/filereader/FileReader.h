#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <fstream>
#include <vector>
#include "../lib/functions.h"
#include <cassert>

namespace cnf {

/*
 * Class for reading files.
 * 
 */

class FileReader {
						
	private :
		
		std::string filename;
		
		int lines;
		
		std::istream* file;
			
	protected :	
	
		virtual inline bool isEndOfFile(int nc) {
			return (nc == -1);
		}
	
		virtual inline bool isEndOfLine(int nc) {
			return (nc == 10 || isEndOfFile(nc));	
		}
		
		virtual inline bool isComment(int nc) {
			return (nc == '#');
		}
		
		virtual inline bool isNegative(int nc) {
			return false;
		}
		
		virtual inline bool isSeparator(int nc) {
			return (nc == ' ');
		}
		
		virtual inline bool isNumber(int nc) {
			return (nc >= 48 && nc <= 57);
		}
		
		virtual inline int skipToEndOfLine() {
			int nc = file->get();
			
			while (!isEndOfLine(nc)) {
				nc = file->get();
			}
			return nc;	
		}
		
		virtual inline int isIllegal(int nc) {
			// !(isEndOfLine || isSeparator) should be enough
			return !(isEndOfLine(nc) || isSeparator(nc)); // ||isComment(nc) || isNegative(nc) || isNumber(nc));
		}	 
	
	public :
	
		virtual bool readline(std::vector<int>& line) {
			assert(file!=NULL);
			lines++;
				
			int nc = file->get();		// read next character
				
			while (!isEndOfLine(nc)) {
				
				while (isSeparator(nc)) { 	
					nc = file->get();		// skip whitespace
				}
				
				bool negative = false;
				if (isNegative(nc)) {
					negative = true;
					nc = file->get();
				}
					
				if (isNumber(nc)) {
					int value = 0;
					while (isNumber(nc)) {
						value = 10*value + (nc-48);
						nc = file->get();	
					}
					if (negative) value = -value;
					line.push_back(value);
				}
					
				if (isComment(nc)) nc = skipToEndOfLine();
				
				if (isIllegal(nc)) {
					std::cerr << "Illegal character found (" << (char)nc << ") ... skipped line " << lines << " of file " << filename << std::endl;
					line.clear();
					nc = skipToEndOfLine();
				} 
			}
			if (!isEndOfFile(nc) && line.empty())
				return readline(line);
			return !isEndOfFile(nc);		// if end of file reached: return false
		}

						
		FileReader(const std::string& filename) :  
			filename(filename), lines(0), file(NULL) {}		
		
		inline void open() { 
			if (filename == "_") {
				file = &std::cin;
			} else {
				file = new std::ifstream(filename.c_str(),std::ifstream::in);
            if (!(*file)) {
                fprintf(stderr, "Could not open file %s\n", filename.c_str());
                exit(1);
            }
			//	assure(*((std::ifstream*)file),filename);
			}
			
		 }
			
		inline void close() {
			if (file != &std::cin) {
				delete file;
			}
			file = NULL;
		}
		
		virtual ~FileReader() {  close(); }
	
	
};

}



#endif /* FILEREADER_H */
