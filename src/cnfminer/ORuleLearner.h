//#ifndef RULELEARNER_H_
//#define RULELEARNER_H_
//
//#include "../ruletrie/RuleTrie.h"
//#include "../exampletrie/ExampleTrie.h"
//#include "../filereader/ExampleReader.h"
//#include "../lib/functions.h"
//#include "../language/LanguageListener.h"
//#include "../language/SimpleLanguage.h"
//#include "../lib/SmartPointer.h"
//#include "../filter/Filter.h"
//#include <memory>
//#include "ExampleFilter.h"
//#include "DistBasedDriftFilter.h"
//
//
//
//namespace cnf
//{
//
///*
// * Note on destruction of member variables.
// * This class makes use of the custom smart_ptr to store its pointers.
// * In addition to the pointer itself, it also contains a boolean indicating if the
// * pointer should be deleted when the smart_ptr itself is deleted.
// * 
// * When the object itself creates a new member variable it will set this flag to true.
// * When the member variable is passed into the constructor the flag will be false.
// * When the current object is removed then automatically the smart_ptr objects will be 
// * destroyed. They will also destroy their pointer is their flag is true, meaning that 
// * only pointers that were created by the current object itself will be deleted.
// * 
// * The use of these smart pointers is totally transparent from the outside (except off course
// *  for the behaviour).
// * It should only affect the variable declaration and the initialization list of the 
// *  constructors.
// * 
// * 
// */
//	class ExampleFilter;
//
//	class RuleLearner : public LanguageListener {
//		
//		private:
//		
//			smart_ptr<RuleTrie> rules;
//			
//			smart_ptr<Language> examples;
//			
//			smart_ptr<Filter> filter;
//			
//			ExampleFilter* example_filter;
//			
//			int k;
//			
//			int examplecount;
//				
//			void init(int kvalue)  {
//				
//			//	addInitial(1000000);
//			//	addInitial(1000001);
//				
//				//example_filter = new ExampleFilter(*(new ExampleTrie()));
//				createExampleFilter();
//				examplecount = 0;
//				k = kvalue;
//				rules->addRule(NULL,0,1);		// add the empty rule
//				
//				examples->addListener(this);
//			}
//						
//			// double measure(rulelist& rules) { return (double)rules.size() / getRules().getNumberOfRules(); }
//	
//			void createExampleFilter();
//			
//		public:
//
//			RuleLearner(int k) :
//				rules(new RuleTrie(),true),
//				examples(new SimpleLanguage(),true),
//				filter(new Filter(), true)
//				{
//					init(k);	
//				}
//	
//			RuleLearner(int k, smart_ptr<Language> lang) :
//				rules(new RuleTrie(),true),
//				examples(lang),
//				filter(new Filter(), true)
//				{
//					init(k);
//				}
//				
//			RuleLearner(int k, smart_ptr<Filter> f) :
//				rules(new RuleTrie(),true),
//				examples(new SimpleLanguage(),true),
//				filter(f)
//				{
//					init(k);	
//				}
//	
//			RuleLearner(int k, smart_ptr<Language> lang, smart_ptr<Filter> f) :
//				rules(new RuleTrie(),true),
//				examples(lang),
//				filter(f)
//				{
//					init(k);
//				}
//			
//			virtual ~RuleLearner() {
//				examples->removeListener(this);
//				// destruction of the pointers is automatic because the
//				//  smart_ptr structs go out of scope causing deletion of the pointers
//				//	if they are marked for deletion
//			}
//			
//			virtual double process_example(const std::vector<int>& example, int ex_id);
//			
//			// virtual double rate_example(int* rate, int ratel);
//			
//		virtual void languageElementAdded(int element, int ex_id) {
//			if (getRules().findRule(NULL,0) == NULL) { // empty rule not there
//				int r = -element;
//				getRules().addRule(&r,1,ex_id);
//				//	std::cout << "element added: new L = " << getLanguage().getSize() << std::endl;
//			}
//		}
//		
//		virtual void languageElementRemoved(int element, int ex_id) {
//			int r = -element;
//			getRules().removeRule(&r, 1);
//		}
//	
//			inline RuleTrie& getRules() const { return *rules; }
//			
//			inline Filter& getFilter() const { return *filter; }
//			
//			inline ExampleFilter& getExampleFilter() const { return *example_filter; }
//	
//			virtual inline Language& getLanguage() const { return *examples; }
//	
//			inline int getK() { return k; }
//			
//		/*	inline void addInitial(int r) {
//				initialtheory.push_back(r);
//			} */
//			
//			inline int getExampleCount() { return examplecount; }
//
//			
//	};
//}
//		
//#endif /*RULELEARNER_H_*/
