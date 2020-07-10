
#ifndef cnfalgoBufferedExampleReader
#define cnfalgoBufferedExampleReader


#include <deque>
#include <vector>
#include "ExampleReader.h"
#include <cassert>

namespace cnf
{

class BufferedExampleReader : public ExampleReader
{

private: 
	std::deque<std::vector<int> > buffer_;

	int buffer_start_;
	
	unsigned int current_pos_;
	
	bool buffering_;

public:
	BufferedExampleReader(std::string filename, int binfilter=CLASS_WIDTH, bool moveclass=true) : 
		ExampleReader(filename,binfilter,moveclass), buffer_start_(0), current_pos_(0), buffering_(false) {}

	~BufferedExampleReader() {}
	
	virtual bool readline(std::vector<int>& line) {
		if (current_pos_ < buffer_.size()) {
			line = buffer_[current_pos_];
			current_pos_++;
			return true;
		} else {
			bool result = ExampleReader::readline(line);
			if (buffering_) {
				buffer_.push_back(line);
				current_pos_++;
			}
			return result;
		}
	}
	
	inline int getFirstAvailable() { return buffer_start_; }
	
	inline int getLastAvailable() { return static_cast<int>(buffer_start_) + static_cast<int>(buffer_.size()); }
	
	inline void seek(int exid) {
		// Move position in stream such that the next example will be example exid.
	
		if (exid < getFirstAvailable()) {
			assert(false);
		} else if (exid < getLastAvailable()) { 
			current_pos_ = exid - getFirstAvailable();
		} else {
			current_pos_ = getLastAvailable();
			while (getLastAvailable() < exid) {
				std::vector<int> line;
				readline(line);
			}
		}
	}
	
	inline void purgeBuffer(int exid) {
		// Drop all elements BEFORE exid
	
		while (buffer_start_ < exid) {
			buffer_.pop_back();
			buffer_start_++;
			current_pos_--;
		}
	
	}
	
	inline void startBuffering() {
		buffering_ = true;
		buffer_.clear();
		current_pos_ = 0;
	}
	
	inline void stopBuffering() {
		buffering_ = false;
	}

};

}  // end namespace cnfalgo

#endif // cnfalgoBufferedExampleReader
