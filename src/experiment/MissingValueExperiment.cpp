#include "MissingValueExperiment.h"

#include "../filereader/ExampleReader.h"
#include "../filereader/FileReader.h"
#include <fstream>

using namespace cnf;

score_result MissingValueExperiment::test() {
    
    const std::string& testFile = getTestfile();
    std::cout << "Test File\t:\t" << str(testFile) << std::endl;
    ExampleReader& data = *createReader(testFile);
	
    const std::string& delFile = getDelfile();
    std::cout << "Delete File\t:\t" << str(delFile) << std::endl;
    FileReader& del = *(new FileReader(delFile));
	
    const std::string& outFile = getOutfile();
    std::cout << "Output File\t:\t" << str(outFile) << std::endl;
    std::ofstream out;
    out.open(outFile);
    // Output the number of clauses in the learned theory in the first line of the outFile
    out << getTheory().getRules().size() << std::endl;
    int totalLiterals = 0;
    RuleList rules;
    getTheory().getRules().listRules(&rules);
    for (RuleList::const_iterator i = rules.begin(); i != rules.end(); i++) {
        totalLiterals += (*i)->length();
    }
    
//    for (unsigned int i=0; i<rules.size() ; ++i) {
//        std::vector<int> rr;
//        rules[i]->getRule(rr);
//        totalLiterals += rr.size();
//    }
    // Output the total number of literals in the learned theory in the second line of the outFile
    out << totalLiterals << std::endl;
    
	data.open();
	
	std::vector<int> example;
	std::vector<int> deletion;
	
	score_result result = 0;
    std::string prediction = "";
    
	del.open();
	int s=0;
	while (data.readline(example)) {
		if (deletion.empty() || deletion[0] < s) {
			deletion.clear();
			del.readline(deletion);
		}
		
		if (s % 10 == 0) std::cout << "\nEvaluating example: "<< s << std::endl;
		
        prediction = "";
		result += test_example(example, s, deletion, prediction);
        if (prediction != ""){
            std::cout << "Prediction[" << s << "]\t= " << prediction << std::endl;
            // Output Format: line number (s) <space> prediction of completed example (comma separated)
            out << s << " " << prediction << std::endl;
            
        }
		example.clear();
		s++;
	};
	
	data.close();
	delete &data;
	
	del.close();
	delete &del;
	
    out.close();
    
	result /= s; 
	return result;	
}



// returns true if binary
bool MissingValueExperiment::findAttributeValues(int att,std::vector<int>& values) {
	std::vector<int> language;
	getTheory().getLanguage().getElements(language);
	
	for (std::vector<int>::iterator i = language.begin(); i != language.end(); ++i) {
		if (*i > BINARY_START && *i == binattvalue(att)) return true;
		else if (*i / CLASS_WIDTH == att) values.push_back(*i);	
	}
	return (values.empty());
}

/* return true if a completion was found with a score < correct => misclassification */
bool MissingValueExperiment::scoreExample(std::vector<int>& example, const int* del, int dell, std::vector<double>& scores, int count, std::vector<int>& counts, double correct, double& best_score, std::string& prediction) {
	if (dell == 0) { 
		// example is complete
		// do the actual scoring here
		
        double score = getMetric().calculate(example,getTheory().getRules());
        
		std::cout << "Constructed example\t:\t" << str(example) << "\t Score: " << score << std::endl;
        
        if(score < best_score){
            prediction = str(example);
            best_score = score;
        }
        else if (score == best_score){
            // Update the prediction with half probability
            double r = rand() / (RAND_MAX + 1.);
            if (r < 0.5){
                prediction = str(example);
            }
        }
		//if (score <= correct) {
			scores.push_back(score);
			counts.push_back(count);
		//}
		return (score < correct);
	} else {
		std::vector<int> values;
		if (findAttributeValues(del[0],values)) {
			int valcount = getTheory().getLanguage().getOccurence(binattvalue(del[0]));
			
		//	cout << "BINARY ATTRIBUTE" << endl;
			int bval = binattvalue(del[0]);
			//cout << del[0] << " ==> " << bval << endl;
			int position = findValue(example,bval);
			if (position >= 0) {		// value present				
				if (scoreExample(example,del+1,dell-1,scores,count+valcount,counts,correct,best_score,prediction)) return true;
				example.erase(example.begin()+position); 	// modify
				if (scoreExample(example,del+1,dell-1,scores,count+(getTheory().getExampleCount()-valcount),counts, correct,best_score,prediction)) return true;
				example.insert(example.begin()+position,bval);	// undo
			} else {  // value not present
                if (scoreExample(example,del+1,dell-1,scores,count+(getTheory().getExampleCount()-valcount),counts,correct,best_score,prediction)) return true;
				example.insert(example.begin()-position-1,bval);	// modify
				if (scoreExample(example,del+1,dell-1,scores,count+valcount,counts,correct,best_score,prediction)) return true;
				example.erase(example.begin()-position-1); 	// undo
			}
		} else {
		//	cout << "NOMINAL ATTRIBUTE " << str(values) << endl;
			// finds the position of any of the values in value in the example 
			// puts the value that was found first in values
			// in the end, scores[0] will give the score of the original value
			int position = findValue(example,values);
			
			//cout << del[0] << " ==> nominal" << endl;
			
			if (position >= 0) {		
				for (unsigned int val = 0; val < values.size(); val++) {
					example[position] = values[val];	// modify the example
					int valcount = getTheory().getLanguage().getOccurence(values[val]);
					if (scoreExample(example,del+1,dell-1,scores,count+valcount,counts,correct,best_score,prediction)) return true;
				}
				example[position] = values[0]; // undo the change
			} else {
				print_warning("Trying to delete value that was already missing!");
			}
		}
	}
	return false;	
} 

int MissingValueExperiment::findValue(const std::vector<int>& example, std::vector<int>& values) {
	for (unsigned int i = 0; i < example.size(); i++) {
		if (example[i] >= values[0] && (example[i] <= values.back())) {
			// example[i] should be in values
			for (std::vector<int>::iterator val = values.begin(); val != values.end(); ++val) {
				if (*val == example[i]) {
					int value = *val;
					values.erase(val);
					values.insert(values.begin(),value);
					return i;	
				}
			}
		} else if (example[i] > values.back()) {
			return -i-1;
		}
	}
	return static_cast<int>(-example.size()-1);
}

int MissingValueExperiment::findValue(const std::vector<int>& example, int value) {
	for (unsigned int i = 0; i < example.size(); i++) {
		if (example[i] == value) {
			return i;
		} else if (example[i] > value) {
			return -i-1; 
		}	
	}
	return static_cast<int>(-example.size()-1);
}


score_result MissingValueExperiment::test_example(const std::vector<int>& example, int s, const std::vector<int>& deletion, std::string& prediction) {
	
    if (deletion.size() <= 1 || deletion[0] != s) return 1;
	std::cout << "\nOriginal Row:\t" << str(example) << std::endl;
	std::cout << "Deleted cols:\t" << str(deletion) << std::endl;
	
	std::vector<double> scores;
	std::vector<int> counts;
	double correct = getMetric().calculate(example,getTheory().getRules());
    double best_score = 1;
	if (scoreExample(const_cast<std::vector<int>& >(example),&deletion[1],static_cast<int>(deletion.size()-1),scores,0,counts, correct, best_score, prediction)) {
        // std::cout << deletion[1] << " --> ";
        // for (int i = 0; i< example.size(); i++) std::cout << example[i] << " "; std::cout << std::endl;
        // for (int i = 0; i< scores.size(); i++) std::cout << scores[i] << " "; std::cout << std::endl;
        return 0;
	} else {
        // std::cout << deletion[1] << " --> ";
        // for (int i = 0; i< example.size(); i++) std::cout << example[i] << " "; std::cout << std::endl;
        // for (int i = 0; i< scores.size(); i++) std::cout << scores[i] << " "; std::cout << std::endl;
		return getScorer().calculate_score(scores,counts);
	}
	
	
	
	
}
