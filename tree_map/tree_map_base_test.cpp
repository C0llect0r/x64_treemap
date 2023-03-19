/*
* @file tree_map_base_test.h
*
* Defines unit tests for the base functionality of the tree map
* implementation.
*
* @author Collector
* @data 03/18/2023
*/

#include "utils.h"

TEST(TreeMap, createTreeMapShouldFailForStatusNullptr) {
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, nullptr) };

	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldFailForZeroKeySize) {
	Status s;
	TreeMap* tm{ createTreeMap(0, sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::KEY_SIZE_ZERO, s);
	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldFailForZeroValueSize) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), 0, compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::VALUE_SIZE_ZERO, s);
	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldFailForNullptrKeyCompFunc) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), nullptr,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::KEY_COMP_FUNC_NULLPTR, s);
	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldFailForNullptrValueEqualFunc) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	nullptr, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::VALUE_EQUAL_FUNC_NULLPTR, s);
	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldFailForNullptrKeyCopyFunc) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, nullptr, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::KEY_COPY_FUNC_NULLPTR, s);
	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldFailForNullptrValueCopyFunc) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, nullptr, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::VALUE_COPY_FUNC_NULLPTR, s);
	ASSERT_EQ(nullptr, tm);
}

TEST(TreeMap, createTreeMapShouldBeSuccessful) {
	TreeMap* tm{ nullptr };
	size_t expectedKeySize{ sizeof(TreeNodeKey) }, expectedValueSize{ sizeof(TreeNodeValue) };
	Status s;

	tm = createTreeMap(expectedKeySize, expectedValueSize, compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s);

	ASSERT_EQ(Status::SUCCESS, s);

	ASSERT_EQ(tm->root, nullptr);
	ASSERT_EQ(tm->nodeAmount, 0);
	ASSERT_EQ(tm->keySize, expectedKeySize);
	ASSERT_EQ(tm->valueSize, expectedValueSize);

	ASSERT_EQ(reinterpret_cast<uintptr_t>(tm->compareKeyFunc), reinterpret_cast<uintptr_t>(compareTreeNodeKey));
	ASSERT_EQ(reinterpret_cast<uintptr_t>(tm->keyCopyFunc), reinterpret_cast<uintptr_t>(copyTreeNodeKey));
	ASSERT_EQ(reinterpret_cast<uintptr_t>(tm->valueCopyFunc), reinterpret_cast<uintptr_t>(copyTreeNodeValue));

	free(tm);
}

TEST(TreeMap, clearTreeMapShouldFailForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };

	s = clearTreeMap(tm);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, clearTreeMapShouldDoNothingForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(nullptr, tm->root);
	ASSERT_EQ(0, tm->nodeAmount);

	s = clearTreeMap(tm);

	ASSERT_EQ(nullptr, tm->root);
	ASSERT_EQ(0, tm->nodeAmount);
	ASSERT_EQ(Status::SUCCESS, s);

	free(tm);
}

TEST(TreeMap, clearTreeMapShouldClearMapWithSingleEntry) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNode* t{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };

	s = putPair(tm, &t->pair);
	ASSERT_EQ(Status::SUCCESS, s);
	ASSERT_EQ(1, tm->nodeAmount);

	s = clearTreeMap(tm);

	ASSERT_EQ(nullptr, tm->root);
	ASSERT_EQ(0, tm->nodeAmount);
	ASSERT_EQ(Status::SUCCESS, s);

	freeTreeNodes({ t });
	free(tm);
}

TEST(TreeMap, clearTreeMapShouldClearTestTree) {
	Status s;
	TreeMap* tm{ createTestTree() };

	s = clearTreeMap(tm);

	ASSERT_EQ(nullptr, tm->root);
	ASSERT_EQ(0, tm->nodeAmount);
	ASSERT_EQ(Status::SUCCESS, s);

	free(tm);
}

TEST(TreeMap, deleteTreeMapShouldFailForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };

	s = deleteTreeMap(tm);
	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, deleteTreeMapShouldFreeTestTreeMap) {
	Status s;
	TreeMap* tm{ createTestTree() };

	s = deleteTreeMap(tm);
	ASSERT_EQ(Status::SUCCESS, s);
}

TEST(TreeMap, putPairShouldFailForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };

	TreeNode* node{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, false) };

	s = putPair(tm, &node->pair);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);

	freeTreeNodes({ node });
}

TEST(TreeMap, putPairShouldFailForTreeNodePairNullptr) {
	Status s;
	TreeMap* tm{ createTestTree() };

	s = putPair(tm, nullptr);

	ASSERT_EQ(Status::TREE_NODE_PAIR_NULLPTR, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, putPairOfSingleEntryShouldBeSuccessful) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	ASSERT_EQ(Status::SUCCESS, s);

	TreeNode* expectedRoot{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, false) };

	s = putPair(tm, &expectedRoot->pair);

	ASSERT_EQ(Status::SUCCESS, s);

	assertTreeMapMemberEqual(tm, 1);

	assertTreeNodeEquals(expectedRoot, reinterpret_cast<TreeNode*>(tm->root));
	freeTreeNodes({expectedRoot});
	deleteTreeMap(tm);
}

TEST(TreeMap, putPairForcingLeftRotationShouldBeSuccessful) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* expectedLeftChild{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, true) },
		* expectedRoot{ createTreeNode("Kentucky", "Frankfort", 1792, 4505836, false) };

	s = putPair(tm, &expectedLeftChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedRoot->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	expectedRoot->left = expectedLeftChild; 

	assertTreeNodeEquals(expectedRoot, reinterpret_cast<TreeNode*>(tm->root));
	freeTreeNodes({expectedLeftChild, expectedRoot});
	deleteTreeMap(tm);
}

TEST(TreeMap, putPairForcingRightRotationAndFlipShouldBeSuccessful) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* expectedRightChild{createTreeNode("Connecticut", "Hartford", 1788, 3605944, false)},
		* expectedRoot{ createTreeNode("California", "Sacramento", 1836, 39538223, false) },
		* expectedLeftChild{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, false) };

	s = putPair(tm, &expectedRightChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedRoot->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedLeftChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	expectedRoot->left = expectedLeftChild;
	expectedRoot->right = expectedRightChild;

	assertTreeNodeEquals(expectedRoot, reinterpret_cast<TreeNode*>(tm->root));

	freeTreeNodes({ expectedRoot, expectedLeftChild, expectedRightChild });
	deleteTreeMap(tm);
}

TEST(TreeMap, putPairForcingFlipShouldBeSuccessful) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* expectedRightChild{ createTreeNode("Connecticut", "Hartford", 1788, 3605944, false) },
		* expectedRoot{ createTreeNode("California", "Sacramento", 1836, 39538223, false) },
		* expectedLeftChild{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, false) };

	s = putPair(tm, &expectedRoot->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedRightChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedLeftChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	expectedRoot->left = expectedLeftChild;
	expectedRoot->right = expectedRightChild;

	assertTreeNodeEquals(expectedRoot, reinterpret_cast<TreeNode*>(tm->root));

	freeTreeNodes({ expectedRoot, expectedLeftChild, expectedRightChild });
	deleteTreeMap(tm);
}

TEST(TreeMap, putPairForcingEveryTreeFixingRoutineShouldBeSuccessful) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* expectedRightChild{ createTreeNode("Connecticut", "Hartford", 1788, 3605944, false) },
		* expectedRoot{ createTreeNode("California", "Sacramento", 1836, 39538223, false) },
		* expectedLeftChild{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, false) };

	s = putPair(tm, &expectedRightChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedLeftChild->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = putPair(tm, &expectedRoot->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	expectedRoot->left = expectedLeftChild;
	expectedRoot->right = expectedRightChild;

	assertTreeNodeEquals(expectedRoot, reinterpret_cast<TreeNode*>(tm->root));

	freeTreeNodes({ expectedRoot, expectedLeftChild, expectedRightChild });
	deleteTreeMap(tm);
}

TEST(TreeMap, putPairOfManyEntrysShouldHoldTreeInvariant) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* a{ createTreeNode("Connecticut", "Hartford", 1788, 3605944, false) },
			* b{ createTreeNode("California", "Sacramento", 1836, 39538223, false) },
			* c{ createTreeNode("Alabama", "Montgomery", 1819, 5039877, false) },
			* d{ createTreeNode("Georgia", "Atlanta", 1788, 10711908, false) },
			* f{ createTreeNode("Maryland", "Annapolis", 1788, 6177224, false) },
			* g{ createTreeNode("Ohio", "Columbus", 1803, 11799448, false) },
			* h{ createTreeNode("Wyoming", "Cheyenne", 1890, 576851, false) };

	std::vector<TreeNode*> nodes = {
		a, b, c, d, f, g, h
	};

	for (TreeNode* node : nodes) {
		s = putPair(tm, &node->pair);
		ASSERT_EQ(Status::SUCCESS, s);
	}

	/*
	* Test Tree Visualisation:
	*									  "Georgia" (d)
	*										   B
	*				"California" (b)						"Ohio" (g)
	*					  B										B
	*		"Alabama" (c)	"Connecticut" (a)	"Maryland" (f)		"Wyoming" (h)
	*			B				  B					 B					B
	*			
	*/

	d->left = b;
	d->right = g;

	b->left = c;
	b->right = a;

	g->left = f;
	g->right = h;

	assertTreeNodeEquals(d, reinterpret_cast<TreeNode*>(tm->root));

	freeTreeNodes(nodes);
	deleteTreeMap(tm);
}

TEST(TreeMap, deletePairShouldFailForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };

	s = deletePair(tm, nullptr, nullptr);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, deletePairShouldFailForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	s = deletePair(tm, nullptr, nullptr);

	ASSERT_EQ(Status::DOES_NOT_CONTAIN, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, deletePairShouldSucceedForSinglePair) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* expected{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };
	TreeNodePair result{};
	TreeNodeKey* providedKey{ createTreeNodeKey("Kansas") };

	s = putPair(tm, &expected->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = deletePair(tm, providedKey, &result);
	ASSERT_EQ(Status::SUCCESS, s);

	assertDeletionEquals(&expected->pair, &result, tm, nullptr, { expected }, providedKey, false);
}

TEST(TreeMap, deletePairShouldSucceedDeletingRootThreeNodeMin) {
	testMinDeletionWithRootThreeNode(deletePair, true, false);
}

TEST(TreeMap, deletePairShouldSucceedDeletingRootThreeNodeMax) {
	testMaxDeletionWithRootThreeNode(deletePair, true, false);
}

TEST(TreeMap, deletePairShouldSucceedDeletingRootThreeNodeMinAndDismissedBuffer) {
	testMinDeletionWithRootThreeNode(deletePair, true, true);
}

TEST(TreeMap, deletePairShouldSucceedDeletingRootThreeNodeMaxAndDismissedBuffer) {
	testMaxDeletionWithRootThreeNode(deletePair, true, true);
}

TEST(TreeMap, deletePairShouldSucceedDeletingRootThreeNodeRoot) {
	Status s;

	/*
	* Test Tree Visualisation:
	*									   "Oregon"
	*										  B
	*				  "Minnesota"						   "Washington"
	*					  B										B
	*
	*/
	TreeNode* expectedDeletedNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
		* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
		* expectedRoot{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	TreeNodePair result;
	TreeNodeKey* providedKey{ createTreeNodeKey("Oregon") };
	TreeMap* tm{ createTestTreeByNodes({expectedDeletedNode, expectedLeftNode, expectedRoot}) };

	/*
	* Test Tree Visualisation:
	*									   "Washington"
	*										    B
	*				  "Minnesota"
	*					  R
	*
	*/
	s = deletePair(tm, providedKey, &result);

	expectedRoot->left = expectedLeftNode;
	expectedLeftNode->isRed = true;

	assertDeletionEquals(&expectedDeletedNode->pair, &result, tm, expectedRoot,
		{ expectedDeletedNode, expectedRoot, expectedLeftNode }, providedKey, false);
}

TEST(TreeMap, deleteShouldSucceedWithLeftLeaningRightNodeDeletingMax) {
	testMaxDeletionWithLeftLeaningRightNode(deletePair, true, false);
}

TEST(TreeMap, deleteShouldSucceedWithLeftLeaningRightNodeDeletingMin) {
	testMinDeletionWithLeftLeaningRightNode(deletePair, true, false);
}

TEST(TreeMap, deleteShouldSucceedWithLeftLeaningRightNodeDeletingMaxAndDismissedBuffer) {
	testMaxDeletionWithLeftLeaningRightNode(deletePair, true, true);
}

TEST(TreeMap, deleteShouldSucceedWithLeftLeaningRightNodeDeletingMinDismissedBuffer) {
	testMinDeletionWithLeftLeaningRightNode(deletePair, true, true);
}

TEST(TreeMap, deleteShouldSucceedWithLeftLeaningRightNodeDeletingRedNode) {
	Status s;

	/*
	* Test Tree Visualisation:
	*									   "Minnesota"
	*										    B
	*				  "Kansas"								 "Oregon"
	*					 B										B
	*												"New York"
	*												    R
	*
	*/
	TreeNode* expectedRoot{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
		* expectedRightNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
		* expectedDeletedNode{ createTreeNode("New York", "Albany", 1788, 20201249, false), },
		* expectedLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };

	TreeNodePair result;
	TreeMap* tm{ createTestTreeByNodes({expectedRoot, expectedLeftNode, expectedRightNode, expectedDeletedNode}) };
	TreeNodeKey* providedKey{ createTreeNodeKey("New York") };

	/*
	* Test Tree Visualisation:
	*									   "Minnesota"
	*										    B
	*				  "Kansas"								 "Oregon"
	*					 B										B
	*
	*/
	s = deletePair(tm, providedKey, &result);

	expectedRoot->left = expectedLeftNode;
	expectedRoot->right = expectedRightNode;

	assertDeletionEquals(&expectedDeletedNode->pair, &result, tm, expectedRoot,
		{ expectedRoot, expectedDeletedNode, expectedRightNode, expectedLeftNode }, providedKey, false);
}

TEST(TreeMap, deleteShouldSucceedWithLeftLeaningRightNodeDeletingRoot) {
	Status s;

	/*
	* Test Tree Visualisation:
	*									   "Minnesota"
	*										    B
	*				  "Kansas"								 "Oregon"
	*					 B										B
	*												"New York"
	*												    R
	*
	*/
	TreeNode* expectedDeletedNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
		* expectedRightNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
		* expectedRoot{ createTreeNode("New York", "Albany", 1788, 20201249, false), },
		* expectedLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };

	TreeNodePair result;
	TreeMap* tm{ createTestTreeByNodes({expectedRoot, expectedLeftNode, expectedRightNode, expectedDeletedNode}) };
	TreeNodeKey* providedKey{ createTreeNodeKey("Minnesota") };

	/*
	* Test Tree Visualisation:
	*									    "New York"
	*										     B
	*				  "Kansas"								 "Oregon"
	*					 B										B
	*
	*/
	s = deletePair(tm, providedKey, &result);

	expectedRoot->left = expectedLeftNode;
	expectedRoot->right = expectedRightNode;

	assertDeletionEquals(&expectedDeletedNode->pair, &result, tm, expectedRoot,
		{ expectedRoot, expectedDeletedNode, expectedRightNode, expectedLeftNode }, providedKey, false);
}

TEST(TreeMap, deletePairShouldSucceedDeletingLeftLeaningThreeNodeMax) {
	testMaxDeletionWithLeftLeaningThreeNode(deletePair, true, false);
}

TEST(TreeMap, deletePairShouldSucceedDeletingLeftLeaningThreeNodeMin) {
	testMinDeletionWithLeftLeaningThreeNode(deletePair, true, false);
}

TEST(TreeMap, deletePairShouldSucceedDeletingLeftLeaningThreeNodeMaxAndDismissedBuffer) {
	testMaxDeletionWithLeftLeaningThreeNode(deletePair, true, true);
}

TEST(TreeMap, deletePairShouldSucceedDeletingLeftLeaningThreeNodeMinAndDismissedBuffer) {
	testMinDeletionWithLeftLeaningThreeNode(deletePair, true, true);
}

TEST(TreeMap, deletePairShouldSucceedDeletingLeftLeaningThreeNodeRoot) {
	Status s;
	TreeMap* tm{ createTestTree() };

	/*
	* Test Tree Visualisation:
	*									   "Oregon"
	*										  B
	*				  "Minnesota"						   "Washington"
	*					  R										B
	*		 "Kansas"		  "New York"
	*			B				  B
	*					"Montana"
	*						R
	*
	*/
	TreeNode* additionalLeftRightNode{ createTreeNode("Montana", "Helena", 1889, 1084225, false) },
		* expectedRoot{ createTreeNode("New York", "Albany", 1788, 20201249, false) },
		* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
		* expectedLeftLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) },
		* expectedDeletedNode{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
		* expectedRightNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	TreeNodePair result;
	TreeNodeKey* providedKey{ createTreeNodeKey("Oregon") };

	s = putPair(tm, &additionalLeftRightNode->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	/*
	* Test Tree Visualisation:
	*									   "New York"
	*										   B
	*				  "Minnesota"					       "Washington"
	*					  R										B
	*		  "Kansas"		  "Montana"
	*			 B				  B
	*
	*/
	s = deletePair(tm, providedKey, &result);

	expectedRoot->left = expectedLeftNode;
	expectedRoot->right = expectedRightNode;

	expectedLeftNode->left = expectedLeftLeftNode;
	expectedLeftNode->right = additionalLeftRightNode;
	expectedLeftNode->isRed = true;

	assertDeletionEquals(&expectedDeletedNode->pair, &result, tm, expectedRoot,
		{ expectedRoot, additionalLeftRightNode, expectedRightNode,
		expectedDeletedNode, expectedLeftLeftNode, expectedLeftNode }, providedKey, false);
}

TEST(TreeMap, deletePairShouldSucceedToClearTestTree) {
	Status s;
	TreeMap* tm{ createTestTree() };

	TreeNode* expectedRoot{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
		* expectedRightNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) },
		* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
		* expectedLeftLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) },
		* expectedLeftRightNode{ createTreeNode("New York", "Albany", 1788, 20201249, false) };

	TreeNodePair result;
	TreeNodeKey* providedKey{ createTreeNodeKey("Minnesota") };

	/*
	* Test Tree Visualisation after deletePair:
	*									   "Oregon"
	*										  B
	*				  "New York"						   "Washington"
	*					  B										B
	*		 "Kansas"
	*			R
	*
	*/
	s = deletePair(tm, providedKey, &result);

	expectedRoot->left = expectedLeftRightNode;
	expectedRoot->right = expectedRightNode;

	expectedLeftRightNode->left = expectedLeftLeftNode;
	expectedLeftLeftNode->isRed = true;

	assertTreeNodesEqual(&expectedLeftNode->pair, &result, tm, expectedRoot,
		providedKey, false);

	/*
	* Test Tree Visualisation after deletePair:
	*									     "Oregon"
	*										    B
	*				  "Kansas"								   "Washington"
	*					 B										    B
	*
	*/
	providedKey = createTreeNodeKey("New York");
	s = deletePair(tm, providedKey, &result);

	expectedRoot->left = expectedLeftLeftNode;
	expectedLeftLeftNode->isRed = false;

	assertTreeNodesEqual(&expectedLeftRightNode->pair, &result, tm, expectedRoot,
		providedKey, false);

	/*
	* Test Tree Visualisation after deletePair:
	*									   "Washington"
	*										    B
	*				  "Kansas"
	*					 R
	*
	*/
	providedKey = createTreeNodeKey("Oregon");
	s = deletePair(tm, providedKey, &result);

	expectedRightNode->left = expectedLeftLeftNode;
	expectedLeftLeftNode->isRed = true;

	assertTreeNodesEqual(&expectedRoot->pair, &result, tm, expectedRightNode,
		providedKey, false);

	/*
	* Test Tree Visualisation after deletePair:
	*									     "Kansas"
	*										    B
	*
	*/
	providedKey = createTreeNodeKey("Washington");
	s = deletePair(tm, providedKey, &result);

	expectedLeftLeftNode->isRed = false;

	assertTreeNodesEqual(&expectedRightNode->pair, &result, tm, expectedLeftLeftNode,
		providedKey, false);

	/*
	* Test Tree Visualisation after deletePair => empty.
	*/
	providedKey = createTreeNodeKey("Kansas");
	s = deletePair(tm, providedKey, &result);

	assertDeletionEquals(&expectedLeftLeftNode->pair, &result, tm, nullptr,
		{ expectedRoot, expectedLeftNode, expectedRightNode, expectedLeftRightNode, expectedLeftLeftNode },
		providedKey, false);
}

TEST(TreeMap, pollFirstPairShouldFailForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };

	s = pollFirstPair(tm, nullptr);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, pollFirstPairShouldFailForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	s = pollFirstPair(tm, nullptr);
	ASSERT_EQ(Status::DOES_NOT_CONTAIN, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, pollFirstPairShouldSucceedForSinglePair) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	
	TreeNode* expected{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) };
	TreeNodePair result{};

	s = putPair(tm, &expected->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = pollFirstPair(tm, &result);
	assertDeletionEquals(&expected->pair, &result, tm, nullptr, { expected }, nullptr, false);
}

TEST(TreeMap, pollFirstPairShouldSucceedWithRootThreeNode) {
	testMinDeletionWithRootThreeNode(pollFirstWrapper, false, false);
}

TEST(TreeMap, pollFirstPairShouldSucceedWithLeftLeaningRightNode) {
	testMinDeletionWithLeftLeaningRightNode(pollFirstWrapper, false, false);
}

TEST(TreeMap, pollFirstPairShouldSucceedLeftLeaningThreeNode) {
	testMinDeletionWithLeftLeaningRightNode(pollFirstWrapper, false, false);
}

TEST(TreeMap, pollFirstPairShouldSucceedWithRootThreeNodeAndDismissedBuffer) {
	testMinDeletionWithRootThreeNode(pollFirstWrapper, false, true);
}

TEST(TreeMap, pollFirstPairShouldSucceedWithLeftLeaningRightNodeAndDismissedBuffer) {
	testMinDeletionWithLeftLeaningRightNode(pollFirstWrapper, false, true);
}

TEST(TreeMap, pollFirstPairShouldSucceedLeftLeaningThreeNodeAndDismissedBuffer) {
	testMinDeletionWithLeftLeaningRightNode(pollFirstWrapper, false, true);
}

TEST(TreeMap, pollFirstPairShouldSucceedToClearTestTree) {
	Status s;
	TreeMap* tm{ createTestTree() };

	TreeNode* expectedRoot{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedRightNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) },
			* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedLeftLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) },
			* expectedLeftRightNode{ createTreeNode("New York", "Albany", 1788, 20201249, false) };

	TreeNodePair result;

	/*
	* Test Tree Visualisation after pollFirst:
	*									    "Oregon"
	*										   B
	*				  "New York"								  "Washington"
	*					  B											   B
	*		"Minnesota"
	*			 R
	* 
	*/
	s = pollFirstPair(tm, &result);

	expectedRoot->right = expectedRightNode;
	expectedRoot->left = expectedLeftRightNode;

	expectedLeftRightNode->left = expectedLeftNode;
	expectedLeftNode->isRed = true;

	assertTreeNodesEqual(&expectedLeftLeftNode->pair, &result, tm, expectedRoot, nullptr, false);

	/*
	* Test Tree Visualisation after pollFirst:
	*									    "Oregon"
	*										   B
	*				  "New York"								  "Washington"
	*					  B											   B
	*
	*/
	s = pollFirstPair(tm, &result);

	expectedLeftRightNode->left = nullptr;

	assertTreeNodesEqual(&expectedLeftNode->pair, &result, tm, expectedRoot, nullptr, false);

	/*
	* Test Tree Visualisation after pollFirst:
	*									    "Washington"
	*										     B
	*				  "Oregon"								  
	*					 B											   
	*
	*/
	s = pollFirstPair(tm, &result);

	expectedRightNode->left = expectedRoot;
	
	expectedRoot->left = nullptr;
	expectedRoot->right = nullptr;
	expectedRoot->isRed = true;

	assertTreeNodesEqual(&expectedLeftRightNode->pair, &result, tm, expectedRightNode, nullptr, false);

	/*
	* Test Tree Visualisation after pollFirst:
	*									    "Washington"
	*										     B
	*
	*/
	s = pollFirstPair(tm, &result);

	expectedRightNode->left = nullptr;

	assertTreeNodesEqual(&expectedRoot->pair, &result, tm, expectedRightNode, nullptr, false);

	/*
	* Test Tree Visualisation after pollFirst => empty.
	*/
	s = pollFirstPair(tm, &result);

	assertDeletionEquals(&expectedRightNode->pair, &result, tm, nullptr,
		{ expectedRoot, expectedLeftNode, expectedRightNode, expectedLeftRightNode, expectedLeftLeftNode },
		nullptr, false);
}

TEST(TreeMap, pollLastPairShouldFailForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };

	s = pollLastPair(tm, nullptr);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, pollLastPairShouldFailForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	s = pollLastPair(tm, nullptr);
	ASSERT_EQ(Status::DOES_NOT_CONTAIN, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, pollLastPairShouldSucceedForSinglePair) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
	equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	TreeNode* expected{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) };
	TreeNodePair result{};

	s = putPair(tm, &expected->pair);
	ASSERT_EQ(Status::SUCCESS, s);

	s = pollLastPair(tm, &result);

	assertDeletionEquals(&expected->pair, &result, tm, nullptr, { expected }, nullptr, false);
}

TEST(TreeMap, pollLastPairShouldSucceedWithRootThreeNode) {
	testMaxDeletionWithRootThreeNode(pollLastWrapper, false, false);
}

TEST(TreeMap, pollLastPairShouldSucceedWithLeftLeaningRightNode) {
	testMaxDeletionWithLeftLeaningRightNode(pollLastWrapper, false, false);
}

TEST(TreeMap, pollLastPairShouldSucceedLeftLeaningThreeNode) {
	testMaxDeletionWithLeftLeaningThreeNode(pollLastWrapper, false, false);
}

TEST(TreeMap, pollLastPairShouldSucceedWithRootThreeNodeAndDismissedBuffer) {
	testMaxDeletionWithRootThreeNode(pollLastWrapper, false, true);
}

TEST(TreeMap, pollLastPairShouldSucceedWithLeftLeaningRightNodeAndDismissedBuffer) {
	testMaxDeletionWithLeftLeaningRightNode(pollLastWrapper, false, true);
}

TEST(TreeMap, pollLastPairShouldSucceedLeftLeaningThreeNodeAndDismissedBuffer) {
	testMaxDeletionWithLeftLeaningThreeNode(pollLastWrapper, false, true);
}

TEST(TreeMap, pollLastPairShouldSucceedToClearTestTree) {
	Status s;
	TreeMap* tm{ createTestTree() };

	TreeNode* expectedRoot{ createTreeNode("Oregon", "Salem", 1859, 4237256, false) },
			* expectedRightNode{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) },
			* expectedLeftNode{ createTreeNode("Minnesota", "Saint Paul", 1858, 5706494, false) },
			* expectedLeftLeftNode{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) },
			* expectedLeftRightNode{ createTreeNode("New York", "Albany", 1788, 20201249, false) };

	TreeNodePair result;

	/*
	* Test Tree Visualisation after pollLast:
	*									   "Minnesota"
	*										    B
	*				  "Kansas"									"Oregon"
	*					 B										   B
	*											"New York"
	*												R
	*
	*/
	s = pollLastPair(tm, &result);

	expectedLeftNode->right = expectedRoot;
	expectedLeftNode->left = expectedLeftLeftNode;

	expectedRoot->left = expectedLeftRightNode;
	expectedLeftRightNode->isRed = true;

	assertTreeNodesEqual(&expectedRightNode->pair, &result, tm, expectedLeftNode,
		nullptr, false);

	/*
	* Test Tree Visualisation after pollLast:
	*									   "Minnesota"
	*										    B
	*				  "Kansas"									"New York"
	*					 B										    B
	*
	*/
	s = pollLastPair(tm, &result);

	expectedLeftNode->right = expectedLeftRightNode;
	expectedLeftRightNode->isRed = false;

	assertTreeNodesEqual(&expectedRoot->pair, &result, tm, expectedLeftNode, nullptr, false);
	
	/*
	* Test Tree Visualisation after pollLast:
	*									   "Minnesota"
	*										    B
	*				  "Kansas"									
	*					 R										    
	*
	*/
	s = pollLastPair(tm, &result);

	expectedLeftNode->right = nullptr;
	expectedLeftLeftNode->isRed = true;

	assertTreeNodesEqual(&expectedLeftRightNode->pair, &result, tm, expectedLeftNode, nullptr, false);

	/*
	* Test Tree Visualisation after pollLast:
	*									     "Kansas"
	*										    B
	* 
	*/
	s = pollLastPair(tm, &result);

	expectedLeftLeftNode->isRed = false;

	assertTreeNodesEqual(&expectedLeftNode->pair, &result, tm, expectedLeftLeftNode, nullptr, false);

	/*
	* Test Tree Visualisation after pollLast => empty.
	*/
	s = pollLastPair(tm, &result);

	assertDeletionEquals(&expectedLeftLeftNode->pair, &result, tm, nullptr,
		{ expectedRoot, expectedLeftNode, expectedRightNode, expectedLeftRightNode, expectedLeftLeftNode },
		nullptr, false);
}