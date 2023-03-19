/*
* @file utils.h
*
*
* @author Collector
* @data 03/18/2023
*/

#include "utils.h"

namespace {
	/*
	* Compares two tree nodes to see if they're equal.
	* Used to not be dependent on the equal function of the treemap.
	* Also used in contains below.
	* 
	* @param[in] v1 - Tree node value that is compared to v2.
	* @param[in] v2 - Tree node value that is compared to v1.
	* 
	* @return Indicator if the values are equal.
	*/
	bool areTreeNodeValuesEqual(const TreeNodeValue* v1, const TreeNodeValue* v2) {
		return std::strcmp(v1->capitalCity, v2->capitalCity) == 0 && v1->existsSince == v2->existsSince && v1->population == v2->population;
	}

	/*
	* Tests if the given value is inside of the treenode.
	* It goes trough all branches recursively until the value is found.
	* 
	* @param[in] t - Treenode that is checked for containing the value v.
	* @param[in] v - The value that is searched in the treenode.
	* 
	* @return Indicator that the value is inside the treenode.
	*/
	bool containsTreeNodeValue(TreeNode* t, TreeNodeValue* v) {
		bool hasValue{ false };
		
		if (t != nullptr) {
			hasValue = ::areTreeNodeValuesEqual(&t->pair.value, v);

			if (!hasValue) {

				hasValue = ::containsTreeNodeValue(t->left, v);

				if (!hasValue) {
					hasValue = ::containsTreeNodeValue(t->right, v);
				}
			}
		}

		return hasValue;
	}

	/*
	* Initialises a tree node key with the given state name.
	* 
	* @param[out] k - Tree node key that will be initialised.
	* @param[in] stateName - Name of the state of america.
	* 
	* @return Indicator if the initialisation was successful.
	*		  Can only fail because of the nested heap memory.
	*/
	bool initialiseTreeNodeKey(TreeNodeKey* k, const char* stateName) {
		bool initSuccess{ false };

		k->stateName = _strdup(stateName);

		if (k->stateName != nullptr) {
			k->nameLength = std::strlen(k->stateName);

			initSuccess = true;
		}

		return initSuccess;
	}

	/*
	* Initialises the given treenode value.
	* 
	* @param[out] v - Tree node value that will be initialised.
	* @param[in] capitalCity - Capital city of the state of america.
	* @param[in] existsSince - Year of its existance.
	* @param[in] population - Population of the capital.
	* 
	* @return Indicator that the initialisation was successful. Can only fail
	*		  because of the nested heap memory.
	*/
	bool initialiseTreeNodeValue(TreeNodeValue* v, const char* capitalCity, unsigned short existsSince, unsigned int population) {
		bool initSuccess{ false };

		v->capitalCity = _strdup(capitalCity);
		if (v->capitalCity != nullptr) {
			v->existsSince = existsSince;
			v->population = population;

			initSuccess = true;
		}

		return initSuccess;
	}

	/*
	* Initialises the given tree node pair.
	* 
	* @param[out] p - Tree node pair that will be initialised.
	* @param[in] stateName - Name of the state of america.
	* @param[in] capitalCity - Capital city of the state of america.
	* @param[in] existsSince - Year of its existance.
	* @param[in] population - Population of the capital.
	*
	* @return Indicator that the initialisation was successful. Can only fail
	*		  because of the nested heap memory.
	*/
	bool initialiseTreeNodePair(TreeNodePair* p, const char* stateName, const char* capitalCity, unsigned short existsSince, unsigned int population) {
		if (!::initialiseTreeNodeKey(&p->key, stateName)) {
			return false;
		}

		if (!::initialiseTreeNodeValue(&p->value, capitalCity, existsSince, population)) {
			free(p->key.stateName);

			return false;
		}

		return true;
	}
}

long compareTreeNodeKey(const void* tKey, const void* insertedKey) {
	const TreeNodeKey* x{ reinterpret_cast<const TreeNodeKey*>(tKey) }, * y{ reinterpret_cast<const TreeNodeKey*>(insertedKey) };

	return std::strcmp(y->stateName, x->stateName);
}

bool equalsTreeNodeKey(const void* expectedKey, const void* resultKey) {
	return compareTreeNodeKey(expectedKey, resultKey) == 0;
}

bool equalsTreeNodeValue(const void* tValue, const void* tValueSearched) {
	const TreeNodeValue* x{ reinterpret_cast<const TreeNodeValue*>(tValue) }, * y{ reinterpret_cast<const TreeNodeValue*>(tValueSearched) };

	return std::strcmp(y->capitalCity, x->capitalCity) == 0 && y->existsSince == x->existsSince && y->population == x->population;
}

Status copyTreeNodeKey(void* dstKey, const void* srcKey) {
	TreeNodeKey* dst{ reinterpret_cast<TreeNodeKey*>(dstKey) };
	const TreeNodeKey* src{ reinterpret_cast<const TreeNodeKey*>(srcKey) };

	dst->nameLength = src->nameLength;
	dst->stateName = _strdup(src->stateName);

	return dst->stateName == nullptr ? Status::ERR_COPY_KEY : Status::SUCCESS;
}

Status copyTreeNodeValue(void* dstValue, const void* srcValue, bool replaceValue) {
	Status copyStat{ Status::ERR_COPY_VALUE };
	TreeNodeValue* dst{ reinterpret_cast<TreeNodeValue*>(dstValue) };
	const TreeNodeValue* src{ reinterpret_cast<const TreeNodeValue*>(srcValue) };

	char* capitalCityCpy{ _strdup(src->capitalCity) };

	if (capitalCityCpy != nullptr) {
		copyStat = Status::SUCCESS;

		dst->existsSince = src->existsSince;
		dst->population = src->population;

		if (replaceValue) {
			free(dst->capitalCity);
		}

		dst->capitalCity = capitalCityCpy;
	}

	return copyStat;
}

TreeNodeKey* createTreeNodeKey(const char* stateName) {
	TreeNodeKey* k{ new TreeNodeKey };

	if (!::initialiseTreeNodeKey(k, stateName)) {
		delete k;

		throw std::runtime_error("Failed to allocate tree node key.");
	}

	return k;
}

TreeNodeValue* createTreeNodeValue(const char* capitalCity, unsigned short existsSince, unsigned int population) {
	TreeNodeValue* v{ new TreeNodeValue };

	if (!::initialiseTreeNodeValue(v, capitalCity, existsSince, population)) {
		delete v;

		throw std::runtime_error("Failed to allocate tree node value.");
	}

	return v;
}

TreeNodePair* createTreeNodePair(const char* stateName, const char* capitalCity, unsigned short existsSince, unsigned int population) {
	TreeNodePair* p{ new TreeNodePair };

	if (!::initialiseTreeNodePair(p, stateName, capitalCity, existsSince, population)) {
		delete p;

		throw std::runtime_error("Failed to allocate tree node pair.");
	}

	return p;
}

TreeNode* createTreeNode(const char* stateName, const char* capitalCity, unsigned short existsSince, unsigned int population, bool isRed) {
	TreeNode* t{ new TreeNode };
	TreeNodePair* p{ &t->pair };

	if (!::initialiseTreeNodePair(p, stateName, capitalCity, existsSince, population)) {
		delete t;

		throw std::runtime_error("Failed to allocate tree node.");
	}
	else {
		t->isRed = isRed;
		t->left = nullptr;
		t->right = nullptr;
	}

	return t;
}

void freeTreeNodes(const std::vector<TreeNode*>& nodes) {
	for (const TreeNode* node : nodes) {
		free(node->pair.key.stateName);
		free(node->pair.value.capitalCity);

		delete node;
	}
}

void freeTreeNodeKeys(const std::vector<TreeNodeKey*>& keys) {
	for (TreeNodeKey* k : keys) {
		if (k != nullptr) {

			free(k->stateName);

			delete k;
		}
	}
}

void freeTreeNodeValues(const std::vector<TreeNodeValue*>& values) {
	for (TreeNodeValue* v : values) {
		if (v != nullptr) {
			free(v->capitalCity);

			delete v;
		}
	}
}

void freeTreeNodePair(void* treeNodePair) {
	TreeNodePair* v{ reinterpret_cast<TreeNodePair*>(treeNodePair) };

	free(v->key.stateName);
	free(v->value.capitalCity);
}

void freeTreeNodePairs(const std::vector<TreeNodePair*>& pairs) {
	for (TreeNodePair* p : pairs) {
		if (p != nullptr) {
			free(p->key.stateName);
			free(p->value.capitalCity);

			delete p;
		}
	}
}

TreeMap* createTestTree() {
	Status s;

	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	std::vector<TreeNode*> nodes{
		createTreeNode("Washington", "Olympia", 1889, 7705281, false),
		createTreeNode("Oregon", "Salem", 1859, 4237256, false),
		createTreeNode("New York", "Albany", 1788, 20201249, false),
		createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false),
		createTreeNode("Kansas", "Topeka", 1861, 2937880, false),
	};

	for (TreeNode* node : nodes) {
		putPair(tm, &node->pair);
	}

	freeTreeNodes(nodes);

	return tm;
}

TreeMap* createTestTreeByNodes(const std::vector<const TreeNode*>& nodes) {
	Status s;

	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	for (const TreeNode* node : nodes) {
		putPair(tm, &node->pair);
	}

	return tm;
}

void assertTreeNodeKeyEquals(const TreeNodeKey* expected, const TreeNodeKey* result) {
	ASSERT_EQ(0, std::strcmp(expected->stateName, result->stateName));
	ASSERT_EQ(expected->nameLength, result->nameLength);
}

void assertTreeNodeValueEquals(const TreeNodeValue* expected, const TreeNodeValue* result) {
	ASSERT_EQ(true, ::areTreeNodeValuesEqual(expected, result));
}

void assertTreeNodeEquals(const TreeNode* expected, const TreeNode* result) {
	if (expected == nullptr) {
		ASSERT_EQ(nullptr, result);
	}
	else {
		assertTreeNodeKeyEquals(&expected->pair.key, &result->pair.key);
		assertTreeNodeValueEquals(&expected->pair.value, &result->pair.value);

		assertTreeNodeEquals(expected->left, result->left);
		assertTreeNodeEquals(expected->right, result->right);

		ASSERT_EQ(expected->isRed, result->isRed);
	}
}

void assertTreeMapMemberEqual(const TreeMap* result, size_t expectedNodeAmount) {
	ASSERT_EQ(expectedNodeAmount, result->nodeAmount);
	ASSERT_NE(nullptr, result->root);
}

void assertContainsValueEquals(const std::vector<TreeNodeValue*>& values, const TreeMap* tm, Status expectedStat) {
	Status containsStat;

	for (const TreeNodeValue* value : values) {
		containsStat = containsValue(tm, value);

		ASSERT_EQ(expectedStat, containsStat);
	}

	freeTreeNodeValues(values);
}

void assertContainsKeyEquals(const std::vector<TreeNodeKey*>& keys, const TreeMap* tm, Status expectedStat) {
	Status containsStat;

	for (TreeNodeKey* key : keys) {
		containsStat = containsKey(tm, key);

		ASSERT_EQ(expectedStat, containsStat);
	}

	freeTreeNodeKeys(keys);
}

void assertValuesEquals(const std::vector<TreeNodeKey*>& keys, const std::vector<TreeNodeValue>& expectedValues, TreeMap* tm, Status expectedStat) {
	TreeNodeValue valueBuffer;
	Status getValueStat;

	for (size_t i = 0; i < keys.size(); ++i) {
		getValueStat = getValue(tm, keys[i], &valueBuffer);

		ASSERT_EQ(expectedStat, getValueStat);

		// quick and dirty, better than writing another function.
		if (expectedStat == Status::SUCCESS) ASSERT_EQ(true, equalsTreeNodeValue(&expectedValues[i], &valueBuffer));
	}

	freeTreeNodeKeys(keys);
}

void assertTreeNodePairEquals(TreeNodePair* expectedPair, TreeNodePair* resultPair) {
	assertTreeNodeKeyEquals(&expectedPair->key, &resultPair->key);
	assertTreeNodeValueEquals(&expectedPair->value, &resultPair->value);
}

void assertMinMaxPairEquals(TreeNodePair* expectedPair, const TreeMap* tm, GetMinMaxPair getMinMaxPairFunc,
	Status expectedStat, bool dismissPair) {
	Status resultStat;
	TreeNodePair resultPair, * resultPairPtr{ dismissPair ? nullptr : &resultPair };

	resultStat = getMinMaxPairFunc(tm, resultPairPtr);

	ASSERT_EQ(expectedStat, resultStat);

	if (expectedStat == Status::SUCCESS) {
		assertTreeNodePairEquals(expectedPair, resultPairPtr);

		freeTreeNodePairs({ expectedPair });
	}
}

void assertDerivedKeyPairsEqual(const std::vector<TreeNodeKey*> providedKeys, const std::vector<TreeNodePair*> expectedPairs,
	TreeMap* tm, GetDerivedPair getDerivedKeyPairFunc, Status expectedStat, bool dismissBuffer) {
	Status resultStat;
	// Little hack if you only run one loop and want to force a nullptr buffer error flag.
	TreeNodePair resultPair, * resultPairPtr{ dismissBuffer ? nullptr : &resultPair };

	for (unsigned long i = 0; i < providedKeys.size(); i++) {
		resultStat = getDerivedKeyPairFunc(tm, providedKeys[i], resultPairPtr);

		ASSERT_EQ(expectedStat, resultStat);

		if (expectedStat == Status::SUCCESS) {
			assertTreeNodePairEquals(expectedPairs[i], resultPairPtr);
		}
	}

	freeTreeNodePairs(expectedPairs);
	freeTreeNodeKeys(providedKeys);
}

void assertReplacedValueEquals(const std::vector<TreeNodeKey*>& providedKeys, const std::vector<TreeNodeValue*>& expectedValues,
	TreeMap* tm, Status expectedStat) {
	Status replaceStat;

	for (unsigned long i = 0; i < providedKeys.size(); i++) {
		replaceStat = replaceValue(tm, providedKeys[i], expectedValues[i]);

		ASSERT_EQ(expectedStat, replaceStat);

		if (expectedStat == Status::SUCCESS) ASSERT_EQ(true, containsTreeNodeValue(reinterpret_cast<TreeNode*>(tm->root), expectedValues[i]));
	}

	freeTreeNodeKeys(providedKeys);
	freeTreeNodeValues(expectedValues);
}

void assertTreeNodesEqual(TreeNodePair* expectedDeletedPair, TreeNodePair* resultDeletedPair, TreeMap* tm,
	TreeNode* expectedRoot, TreeNodeKey* providedKey, bool dismissBuffer) {
	if (!dismissBuffer) {
		assertTreeNodePairEquals(expectedDeletedPair, resultDeletedPair);
		freeTreeNodePair(resultDeletedPair);
	}
	
	assertTreeNodeEquals(expectedRoot, reinterpret_cast<TreeNode*>(tm->root));

	freeTreeNodeKeys({ providedKey });
}

void assertDeletionEquals(TreeNodePair* expectedDeletedPair, TreeNodePair* resultDeletedPair, TreeMap* tm, TreeNode* expectedRoot,
	const std::vector<TreeNode*> nodes, TreeNodeKey* providedKey, bool dismissBuffer) {
	assertTreeNodesEqual(expectedDeletedPair, resultDeletedPair, tm, expectedRoot, providedKey, dismissBuffer);

	freeTreeNodes(nodes);

	deleteTreeMap(tm);
}

Status pollFirstWrapper(TreeMap* tm, const void* key, void* pairBuffer) {
	return pollFirstPair(tm, pairBuffer);
}

Status pollLastWrapper(TreeMap* tm, const void* key, void* pairBuffer) {
	return pollLastPair(tm, pairBuffer);
}

void testMinDeletionWithRootThreeNode(PollFunc p, bool withKey, bool dismissBuffer) {
	Status s;

	TreeNode* expectedLeftNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedDeletedNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedRoot{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	TreeNodePair result, * resultPtr{ dismissBuffer ? nullptr : &result };
	TreeNodeKey* providedKey{ withKey ? createTreeNodeKey("Minnesota") : nullptr };
	TreeMap* tm{ createTestTreeByNodes({expectedLeftNode, expectedDeletedNode, expectedRoot}) };

	s = p(tm, providedKey, resultPtr);

	expectedRoot->left = expectedLeftNode;
	expectedLeftNode->isRed = true;

	assertDeletionEquals(&expectedDeletedNode->pair, resultPtr, tm, expectedRoot,
		{ expectedRoot, expectedLeftNode, expectedDeletedNode }, providedKey, dismissBuffer);
}

void testMaxDeletionWithRootThreeNode(PollFunc p, bool withKey, bool dismissBuffer) {
	Status s;

	TreeNode* expectedRoot{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedDeletedNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	TreeNodePair result, * resultPtr{ dismissBuffer ? nullptr : &result };
	TreeNodeKey* providedKey{ withKey ? createTreeNodeKey("Washington") : nullptr };
	TreeMap* tm{ createTestTreeByNodes({expectedRoot, expectedLeftNode, expectedDeletedNode}) };

	s = p(tm, providedKey, resultPtr);

	expectedRoot->left = expectedLeftNode;
	expectedLeftNode->isRed = true;

	assertDeletionEquals(&expectedDeletedNode->pair, resultPtr, tm, expectedRoot,
		{ expectedRoot, expectedLeftNode, expectedDeletedNode }, providedKey, dismissBuffer);
}


void testMaxDeletionWithLeftLeaningRightNode(PollFunc p, bool withKey, bool dismissBuffer) {
	Status s;

	TreeNode* expectedRoot{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedDeletedNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedRightNode{ createTreeNode("New York", "Albany", 1788, 20201249, false), },
			* expectedLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };

	TreeNodePair result, * resultPtr{ dismissBuffer ? nullptr : &result };
	TreeNodeKey* providedKey{ withKey ? createTreeNodeKey("Oregon") : nullptr };
	TreeMap* tm{ createTestTreeByNodes({expectedRoot, expectedLeftNode, expectedDeletedNode, expectedRightNode}) };

	s = p(tm, providedKey, resultPtr);

	expectedRoot->left = expectedLeftNode;
	expectedRoot->right = expectedRightNode;

	assertDeletionEquals(&expectedDeletedNode->pair, resultPtr, tm, expectedRoot,
		{ expectedRoot, expectedDeletedNode, expectedRightNode, expectedLeftNode },
		providedKey, dismissBuffer);
}

void testMinDeletionWithLeftLeaningRightNode(PollFunc p, bool withKey, bool dismissBuffer) {
	Status s;

	TreeNode* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedRightNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedRoot{ createTreeNode("New York", "Albany", 1788, 20201249, false), },
			* expectedDeletedNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };

	TreeNodePair result, * resultPtr{ dismissBuffer ? nullptr : &result };
	TreeMap* tm{ createTestTreeByNodes({expectedLeftNode, expectedDeletedNode, expectedRightNode, expectedRoot}) };
	TreeNodeKey* providedKey{ withKey ? createTreeNodeKey("Kansas") : nullptr };

	s = p(tm, providedKey, resultPtr);

	expectedRoot->left = expectedLeftNode;
	expectedRoot->right = expectedRightNode;

	assertDeletionEquals(&expectedDeletedNode->pair, resultPtr, tm, expectedRoot,
		{ expectedRoot, expectedDeletedNode, expectedRightNode, expectedLeftNode },
		providedKey, dismissBuffer);
}

void testMinDeletionWithLeftLeaningThreeNode(PollFunc p, bool withKey, bool dismissBuffer) {
	Status s;
	TreeMap* tm{ createTestTree() };

	TreeNode* additionalLeftNode{ createTreeNode("Montana", "Helena", 1889, 1084225, false) },
			* expectedLeftRightNode{ createTreeNode("New York", "Albany", 1788, 20201249, false) },
			* expectedLeftLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedDeletedNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) },
			* expectedRoot{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedRightNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	TreeNodePair result, * resultPtr{ dismissBuffer ? nullptr : &result };
	TreeNodeKey* providedKey{ withKey ? createTreeNodeKey("Kansas") : nullptr };

	s = putPair(tm, &additionalLeftNode->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = p(tm, providedKey, resultPtr);

	expectedRoot->left = additionalLeftNode;
	additionalLeftNode->isRed = true;
	expectedRoot->right = expectedRightNode;

	additionalLeftNode->left = expectedLeftLeftNode;
	additionalLeftNode->right = expectedLeftRightNode;

	assertDeletionEquals(&expectedDeletedNode->pair, resultPtr, tm, expectedRoot,
		{ expectedRoot, additionalLeftNode, expectedRightNode, expectedDeletedNode,
		expectedLeftLeftNode, expectedLeftRightNode }, providedKey, dismissBuffer);
}

void testMaxDeletionWithLeftLeaningThreeNode(PollFunc p, bool withKey, bool dismissBuffer) {
	Status s;
	TreeMap* tm{ createTestTree() };

	TreeNode* additionalLeftRightNode{ createTreeNode("Montana", "Helena", 1889, 1084225, false) },
			* expectedRoot{ createTreeNode("New York", "Albany", 1788, 20201249, false) },
			* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedLeftLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) },
			* expectedRightNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedDeletedNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	TreeNodePair result, * resultPtr{ dismissBuffer ? nullptr : &result };
	TreeNodeKey* providedKey{ withKey ? createTreeNodeKey("Washington") : nullptr };

	s = putPair(tm, &additionalLeftRightNode->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = p(tm, providedKey, resultPtr);

	expectedRoot->left = expectedLeftNode;
	expectedRoot->right = expectedRightNode;

	expectedLeftNode->left = expectedLeftLeftNode;
	expectedLeftNode->right = additionalLeftRightNode;
	expectedLeftNode->isRed = true;

	assertDeletionEquals(&expectedDeletedNode->pair, resultPtr, tm, expectedRoot,
		{ expectedRightNode, additionalLeftRightNode, expectedLeftNode,
		expectedDeletedNode, expectedLeftLeftNode, expectedRoot }, providedKey, dismissBuffer);
}