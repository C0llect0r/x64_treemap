/*
* @file tree_map_utils_test.h
*
* Defines unit tests for the utility functionality of the tree map
* implementation.
*
* @author Collector
* @data 03/18/2023
*/


#include "utils.h"


TEST(TreeMap, containsValueFailsForTreeMapNullptr) {
	TreeMap* tm{ nullptr };

	assertContainsValueEquals({ nullptr }, tm, Status::TREE_MAP_NULLPTR);
}

TEST(TreeMap, containsValueFailsForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertContainsValueEquals({ nullptr }, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsValueSucceedsForFoundValue) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Saint Paul", 1858, 5706494)
	};

	assertContainsValueEquals(values, tm, Status::SUCCESS);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsValueFailsForMissingValue) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Bla", 1234, 1337)
	};

	assertContainsValueEquals(values, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsValueSucceedsForAllValues) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Albany", 1788, 20201249),
		createTreeNodeValue("Topeka", 1861, 2937880),
		createTreeNodeValue("Olympia", 1889, 7705281)
	};

	assertContainsValueEquals(values, tm, Status::SUCCESS);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsValueFailsForAllValues) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Topkek", 0000, 26652),
		createTreeNodeValue("Wow", 42, 2547880),
		createTreeNodeValue("wuff", 69, 7321181)
	};

	assertContainsValueEquals(values, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsKeyFailsForTreeMapNullptr) {
	TreeMap* tm{ nullptr };

	assertContainsKeyEquals({ nullptr }, tm, Status::TREE_MAP_NULLPTR);
}

TEST(TreeMap, containsKeyFailsForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertContainsKeyEquals({ nullptr }, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsKeySucceedsForFoundKey) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Washington")
	};

	assertContainsKeyEquals(keys, tm, Status::SUCCESS);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsKeyFailsForMissingKey) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Nice")
	};

	assertContainsKeyEquals(keys, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsKeySucceedsForAllKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Washington"),
		createTreeNodeKey("Kansas"),
		createTreeNodeKey("Minnesota")
	};

	assertContainsKeyEquals(keys, tm, Status::SUCCESS);

	deleteTreeMap(tm);
}

TEST(TreeMap, containsKeyFailsForAllKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("wuff"),
		createTreeNodeKey("xd"),
		createTreeNodeKey("dota")
	};

	assertContainsKeyEquals(keys, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, getValueFailsForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };
	TreeNodeValue valueBuffer;

	s = getValue(tm, nullptr, &valueBuffer);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, getValueFailsForValueBufferNullptr) {
	Status s;
	TreeMap* tm{ createTestTree() };

	s = getValue(tm, nullptr, nullptr);

	ASSERT_EQ(Status::VALUE_BUFFER_NULLPTR, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, getValueFailsForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNodeValue valueBuffer;

	s = getValue(tm, nullptr, &valueBuffer);

	ASSERT_EQ(Status::DOES_NOT_CONTAIN, s);

	deleteTreeMap(tm);
}


TEST(TreeMap, getValueReturnsCorrectValueForExistingKey) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Washington")
	};

	std::vector<TreeNodeValue*> expectedValues{
		createTreeNodeValue("Olympia", 1889, 7705281),
	};

	assertTreePartialDataEquals<TreeNodeKey, TreeNodeValue>(keys, expectedValues, tm, Status::SUCCESS,
		getValue, equalsTreeNodeValue, freeTreeNodeKeys, freeTreeNodeValues);

	deleteTreeMap(tm);
}

TEST(TreeMap, getValueFailsForNonExistingKey) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Not found")
	};

	assertTreePartialDataEquals<TreeNodeKey, TreeNodeValue>(keys, {}, tm, Status::DOES_NOT_CONTAIN,
		getValue, equalsTreeNodeValue, freeTreeNodeKeys, freeTreeNodeValues);

	deleteTreeMap(tm);
}

TEST(TreeMap, getValueReturnsValuesForAllKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("New York"),
		createTreeNodeKey("Minnesota"),
		createTreeNodeKey("Kansas")
	};

	std::vector<TreeNodeValue*> expectedValues{
		createTreeNodeValue("Albany", 1788, 20201249),
		createTreeNodeValue("Saint Paul", 1858, 5706494),
		createTreeNodeValue("Topeka", 1861, 2937880)
	};

	assertTreePartialDataEquals<TreeNodeKey, TreeNodeValue>(keys, expectedValues, tm, Status::SUCCESS,
		getValue, equalsTreeNodeValue, freeTreeNodeKeys, freeTreeNodeValues);

	deleteTreeMap(tm);
}

TEST(TreeMap, getValueFailsForAllKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Non existant"),
		createTreeNodeKey("Not in it"),
		createTreeNodeKey("Wow"),
	};

	assertTreePartialDataEquals<TreeNodeKey, TreeNodeValue>(keys, {}, tm, Status::DOES_NOT_CONTAIN,
		getValue, equalsTreeNodeValue, freeTreeNodeKeys, freeTreeNodeValues);

	deleteTreeMap(tm);
}

TEST(TreeMap, getKeyFailsForTreeMapNullptr) {
	Status s;
	TreeMap* tm{ nullptr };
	TreeNodeKey keyBuffer;

	s = getKey(tm, nullptr, &keyBuffer);

	ASSERT_EQ(Status::TREE_MAP_NULLPTR, s);
}

TEST(TreeMap, getKeyFailsForKeyBufferNullptr) {
	Status s;
	TreeMap* tm{ createTestTree() };

	s = getKey(tm, nullptr, nullptr);

	ASSERT_EQ(Status::KEY_BUFFER_NULLPTR, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, getKeyFailsForEmptyMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNodeKey keyBuffer;

	s = getKey(tm, nullptr, &keyBuffer);

	ASSERT_EQ(Status::DOES_NOT_CONTAIN, s);

	deleteTreeMap(tm);
}

TEST(TreeMap, getKeySucceedsForMatchingValue) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Topeka", 1861, 2937880)
	};

	std::vector<TreeNodeKey*> expectedKeys{
		createTreeNodeKey("Kansas")
	};

	assertTreePartialDataEquals<TreeNodeValue, TreeNodeKey>(values, expectedKeys, tm, Status::SUCCESS,
		getKey, equalsTreeNodeKey, freeTreeNodeValues, freeTreeNodeKeys);

	deleteTreeMap(tm);
}

TEST(TreeMap, getKeyFailsForNonExistingValue) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("wow", 18161, 880)
	};

	assertTreePartialDataEquals<TreeNodeValue, TreeNodeKey>(values, {}, tm, Status::DOES_NOT_CONTAIN,
		getKey, equalsTreeNodeKey, freeTreeNodeValues, freeTreeNodeKeys);

	deleteTreeMap(tm);
}

TEST(TreeMap, getKeySucceedsForAllValues) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Saint Paul", 1858, 5706494),
		createTreeNodeValue("Salem", 1859, 4237256),
		createTreeNodeValue("Olympia", 1889, 7705281),
	};

	std::vector<TreeNodeKey*> expectedKeys{
		createTreeNodeKey("Minnesota"),
		createTreeNodeKey("Oregon"),
		createTreeNodeKey("Washington")
	};

	assertTreePartialDataEquals<TreeNodeValue, TreeNodeKey>(values, expectedKeys, tm, Status::SUCCESS,
		getKey, equalsTreeNodeKey, freeTreeNodeValues, freeTreeNodeKeys);

	deleteTreeMap(tm);
}

TEST(TreeMap, getKeyFailsForAllValues) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("nice", 161, 880),
		createTreeNodeValue("fatality", 431, 43141),
		createTreeNodeValue("fire", 3212, 81240)
	};

	assertTreePartialDataEquals<TreeNodeValue, TreeNodeKey>(values, {}, tm, Status::DOES_NOT_CONTAIN,
		getKey, equalsTreeNodeKey, freeTreeNodeValues, freeTreeNodeKeys);

	deleteTreeMap(tm);
}

TEST(TreeMap, replaceValueShouldFailForTreeMapNullptr) {
	assertReplacedValueEquals({ nullptr }, { nullptr }, nullptr, Status::TREE_MAP_NULLPTR);
}

TEST(TreeMap, replaceValueShouldFailForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertReplacedValueEquals({ nullptr }, { nullptr }, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, replaceValueShouldFailForMissingKey) {
	TreeMap* tm{ createTestTree() };
	TreeNodeKey* k{ createTreeNodeKey("Not existant") };

	assertReplacedValueEquals({ k }, { nullptr }, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, replaceValueShouldReplaceValueForMatchingKey) {
	TreeMap* tm{ createTestTree() };
	TreeNodeKey* k{ createTreeNodeKey("Washington") };
	TreeNodeValue* v{ createTreeNodeValue("Random", 123, 456) };

	assertReplacedValueEquals({ k }, { v }, tm, Status::SUCCESS);

	deleteTreeMap(tm);
}

TEST(TreeMap, replaceValueShouldReplaceValuesForMatchingKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("Washington"),
		createTreeNodeKey("New York"),
		createTreeNodeKey("Kansas")
	};

	std::vector<TreeNodeValue*> values{
		createTreeNodeValue("Wuff", 1, 2),
		createTreeNodeValue("Hello", 4, 5),
		createTreeNodeValue("Wow", 6, 7)
	};

	assertReplacedValueEquals(keys, values, tm, Status::SUCCESS);

	deleteTreeMap(tm);
}

TEST(TreeMap, replaceValueShouldReturnFailForAllKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> keys{
		createTreeNodeKey("first"),
		createTreeNodeKey("second"),
		createTreeNodeKey("third")
	};

	std::vector<TreeNodeValue*> values{
		nullptr,
		nullptr,
		nullptr
	};

	assertReplacedValueEquals(keys, values, tm, Status::DOES_NOT_CONTAIN);

	deleteTreeMap(tm);
}

TEST(TreeMap, ceilingPairShouldFailForTreeMapNullptr) {
	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, nullptr, ceilingPair,
		Status::TREE_MAP_NULLPTR, false);
}

TEST(TreeMap, ceilingPairShouldFailForPairBufferNullptr) {
	TreeMap* tm{ createTestTree() };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, ceilingPair,
		Status::PAIR_BUFFER_NULLPTR, true);

	deleteTreeMap(tm);
}

TEST(TreeMap, ceilingPairShouldFailForEmptyTree) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, ceilingPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, ceilingPairShouldReturnSamePairForSameKey) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNodePair* p{ createTreeNodePair("Kansas", "Topeka", 1861, 2937880) };
	TreeNodeKey* k{ createTreeNodeKey("Kansas") };

	s = putPair(tm, p);

	ASSERT_EQ(Status::SUCCESS, s);

	assertDerivedKeyPairsEqual({ k }, { p }, tm, ceilingPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, ceilingPairShouldFailForMissingForeignKeyMatch) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("Z") };

	assertDerivedKeyPairsEqual({ k }, { nullptr }, tm, ceilingPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, ceilingPairShouldSucceedForMatchingForeignKey) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("No") };
	TreeNodePair* p{ createTreeNodePair("Oregon", "Salem", 1859, 4237256) };

	assertDerivedKeyPairsEqual({ k }, { p }, tm, ceilingPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, ceilingPairShouldReturnCorrectPairForMultipleKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> providedKeys{
		createTreeNodeKey("Minnesota"),
		createTreeNodeKey("A"),
		createTreeNodeKey("Na")
	};

	std::vector<TreeNodePair*> expectedPairs{
		createTreeNodePair("Minnesota", "Saint Paul", 1858, 5706494),
		createTreeNodePair("Kansas", "Topeka", 1861, 2937880),
		createTreeNodePair("New York", "Albany", 1788, 20201249),
	};

	assertDerivedKeyPairsEqual(providedKeys, expectedPairs, tm, ceilingPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, floorPairShouldFailForTreeMapNullptr) {
	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, nullptr, floorPair,
		Status::TREE_MAP_NULLPTR, false);
}

TEST(TreeMap, floorPairShouldFailForPairBufferNullptr) {
	TreeMap* tm{ createTestTree() };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, floorPair,
		Status::PAIR_BUFFER_NULLPTR, true);

	deleteTreeMap(tm);
}

TEST(TreeMap, floorPairShouldFailForEmptyTree) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, floorPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, floorPairShouldReturnSamePairForSameKey) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNodePair* p{ createTreeNodePair("Washington", "Olympia", 1889, 7705281) };
	TreeNodeKey* k{ createTreeNodeKey("Washington") };

	s = putPair(tm, p);

	ASSERT_EQ(Status::SUCCESS, s);

	assertDerivedKeyPairsEqual({ k }, { p }, tm, floorPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, floorPairShouldFailForMissingForeignKeyMatch) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("A") };

	assertDerivedKeyPairsEqual({ k }, { nullptr }, tm, floorPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, floorPairShouldSucceedForMatchingForeignKey) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("No") };
	TreeNodePair* p{ createTreeNodePair("New York", "Albany", 1788, 20201249) };

	assertDerivedKeyPairsEqual({ k }, { p }, tm, floorPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, floorPairShouldReturnCorrectPairForMultipleKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> providedKeys{
		createTreeNodeKey("Oregon"),
		createTreeNodeKey("Z"),
		createTreeNodeKey("Na")
	};

	std::vector<TreeNodePair*> expectedPairs{
		createTreeNodePair("Oregon", "Salem", 1859, 4237256),
		createTreeNodePair("Washington", "Olympia", 1889, 7705281),
		createTreeNodePair("Minnesota", "Saint Paul", 1858, 5706494)
	};

	assertDerivedKeyPairsEqual(providedKeys, expectedPairs, tm, floorPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldFailForTreeMapNullptr) {
	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, nullptr, lowerPair,
		Status::TREE_MAP_NULLPTR, false);
}

TEST(TreeMap, lowerPairShouldFailForPairBufferNullptr) {
	TreeMap* tm{ createTestTree() };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, lowerPair,
		Status::PAIR_BUFFER_NULLPTR, true);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldFailForEmptyTree) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, lowerPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldFailEqualSameKey) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNodePair* p{ createTreeNodePair("New Jersey", "Trenton", 1787, 9288994) };
	TreeNodeKey* k{ createTreeNodeKey("New Jersey") };

	s = putPair(tm, p);

	ASSERT_EQ(Status::SUCCESS, s);

	assertDerivedKeyPairsEqual({ k }, { p }, tm, lowerPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldFailForMissingLowerPair) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("Kansas") };

	assertDerivedKeyPairsEqual({ k }, { nullptr }, tm, lowerPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldFailForMissingForeignKeyMatch) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("A") };

	assertDerivedKeyPairsEqual({ k }, { nullptr }, tm, lowerPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldSucceedForMatchingForeignKey) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("No") };
	TreeNodePair* p{ createTreeNodePair("New York", "Albany", 1788, 20201249) };

	assertDerivedKeyPairsEqual({ k }, { p }, tm, lowerPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, lowerPairShouldReturnCorrectPairForMultipleKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> providedKeys{
		createTreeNodeKey("Washington"),
		createTreeNodeKey("New York"),
		createTreeNodeKey("Minnesota")
	};

	std::vector<TreeNodePair*> expectedPairs{
		createTreeNodePair("Oregon", "Salem", 1859, 4237256),
		createTreeNodePair("Minnesota", "Saint Paul", 1858, 5706494),
		createTreeNodePair("Kansas", "Topeka", 1861, 2937880)
	};

	assertDerivedKeyPairsEqual(providedKeys, expectedPairs, tm, lowerPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldFailForTreeMapEmpty) {
	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, nullptr, higherPair,
		Status::TREE_MAP_NULLPTR, false);
}

TEST(TreeMap, higherPairShouldFailForPairBufferNullptr) {
	TreeMap* tm{ createTestTree() };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, higherPair,
		Status::PAIR_BUFFER_NULLPTR, true);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldFailForEmptyTree) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertDerivedKeyPairsEqual({ nullptr }, { nullptr }, tm, higherPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldFailForSameKey) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNodePair* p{ createTreeNodePair("Washington", "Olympia", 1889, 7705281) };
	TreeNodeKey* k{ createTreeNodeKey("Washington") };

	s = putPair(tm, p);

	ASSERT_EQ(Status::SUCCESS, s);

	assertDerivedKeyPairsEqual({ k }, { p }, tm, higherPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldFailForMissingHigherPair) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("Washington") };

	assertDerivedKeyPairsEqual({ k }, { nullptr }, tm, higherPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldFailForMissingForeignKeyMatch) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("Z") };

	assertDerivedKeyPairsEqual({ k }, { nullptr }, tm, higherPair,
		Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldSucceedForMatchingForeignKey) {
	TreeMap* tm{ createTestTree() };

	TreeNodeKey* k{ createTreeNodeKey("No") };
	TreeNodePair* p{ createTreeNodePair("Oregon", "Salem", 1859, 4237256) };

	assertDerivedKeyPairsEqual({ k }, { p }, tm, higherPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, higherPairShouldReturnCorrectPairForMultipleKeys) {
	TreeMap* tm{ createTestTree() };

	std::vector<TreeNodeKey*> providedKeys{
		createTreeNodeKey("Oregon"),
		createTreeNodeKey("New York"),
		createTreeNodeKey("Kansas")
	};

	std::vector<TreeNodePair*> expectedPairs{
		createTreeNodePair("Washington", "Olympia", 1889, 7705281),
		createTreeNodePair("Oregon", "Salem", 1859, 4237256),
		createTreeNodePair("Minnesota", "Saint Paul", 1858, 5706494)
	};

	assertDerivedKeyPairsEqual(providedKeys, expectedPairs, tm, higherPair,
		Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, minPairShouldFailForTreeMapNullptr) {
	assertMinMaxPairEquals(nullptr, nullptr, minPair, Status::TREE_MAP_NULLPTR, false);
}

TEST(TreeMap, minPairShouldFailForPairBufferNullptr) {
	TreeMap* tm{ createTestTree() };

	assertMinMaxPairEquals(nullptr, tm, minPair, Status::PAIR_BUFFER_NULLPTR, true);

	deleteTreeMap(tm);
}

TEST(TreeMap, minPairShouldFailForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertMinMaxPairEquals(nullptr, tm, minPair, Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, minPairShouldReturnRootForASinglePair) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNode* t{ createTreeNode("Montana", "Helena", 1889, 1084225, false) };

	s = putPair(tm, &t->pair);

	ASSERT_EQ(Status::SUCCESS, s);

	assertMinMaxPairEquals(&t->pair, tm, minPair, Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, minPairShouldReturnMinPairForTestTree) {
	TreeMap* tm{ createTestTree() };
	TreeNode* t{ createTreeNode("Kansas", "Topeka", 1861, 2937880, false) };

	assertMinMaxPairEquals(&t->pair, tm, minPair, Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, maxPairShouldFailForTreeMapNullptr) {
	assertMinMaxPairEquals(nullptr, nullptr, maxPair, Status::TREE_MAP_NULLPTR, false);
}

TEST(TreeMap, maxPairShouldFailForPairBufferNullptr) {
	TreeMap* tm{ createTestTree() };

	assertMinMaxPairEquals(nullptr, tm, maxPair, Status::PAIR_BUFFER_NULLPTR, true);

	deleteTreeMap(tm);
}

TEST(TreeMap, maxPairShouldFailForEmptyTreeMap) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };

	assertMinMaxPairEquals(nullptr, tm, maxPair, Status::DOES_NOT_CONTAIN, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, maxPairShouldReturnRootForASinglePair) {
	Status s;
	TreeMap* tm{ createTreeMap(sizeof(TreeNodeKey), sizeof(TreeNodeValue), compareTreeNodeKey,
		equalsTreeNodeValue, copyTreeNodeKey, copyTreeNodeValue, freeTreeNodePair, &s) };
	TreeNode* t{ createTreeNode("New Jersey", "Trenton", 1787, 9288994, false) };

	s = putPair(tm, &t->pair);

	ASSERT_EQ(Status::SUCCESS, s);

	assertMinMaxPairEquals(&t->pair, tm, maxPair, Status::SUCCESS, false);

	deleteTreeMap(tm);
}

TEST(TreeMap, maxPairShouldReturnMaxPairForTestTree) {
	TreeMap* tm{ createTestTree() };
	TreeNode* t{ createTreeNode("Washington", "Olympia", 1889, 7705281, false) };

	assertMinMaxPairEquals(&t->pair, tm, maxPair, Status::SUCCESS, false);

	deleteTreeMap(tm);
}