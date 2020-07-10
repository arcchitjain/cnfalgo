/*
 *  RuleTrie.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */


#ifndef RULETRIE_H_
#define RULETRIE_H_
#define FTRULETRIE
#ifdef FTRULETRIE

#include "ft/FTRuleTrie.h"
#include "ft/RuleNode.h"

namespace cnf {
	typedef class FTRuleTrie RuleTrie;
	
	typedef class RuleNode TreeNode;
	 
}
#else

#include "nft/NFTRuleTrie.h"
#include "nft/NFTTreeNode.h"

namespace cnf {
	typedef class NFTRuleTrie RuleTrie;
	
	typedef class NFTTreeNode TreeNode;
	
}
#endif


#endif