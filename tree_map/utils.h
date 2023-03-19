/*
* @file utils.h
*
* Declares typedefs, structs and functions that are used to test
* the treemap implementation.
*
* @author Collector
* @data 03/18/2023
*/

// TODO padding of key and value in trivial cases e.g. char/int
// TODO tests for getKey compare only values not keys which is wrong.

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <stdexcept>

#include <gtest/gtest.h>

#include "tree_map.h"

/*
* Typedef for the getKey and getValue functions.
* 
* @param[in] tm - Treemap that is used to get the key or value.
* @param[in] data - Is used to get the key or value inside the map.
* @param[out] dataBuffer - Stores the other searched for half of the pair.
* 
* @return A status value of success, does not contains or an error if the
*		  copy functions fail or the treemap/valueBuffer is a nullptr.
*/
using GetTreeMapPartialData = Status(__fastcall*)(const TreeMap* tm, const void* data, void* dataBuffer);

/*
* Typedef for a function that compares two treenode keys or values.
* Both x and y have the same type meaning both have to be keys or values.
* Used for testing getKey/getValue.
* 
* @param[in] x - First key or value to compare with y.
* @param[in] y - Second key or value to compare with x.
* 
* @return Indicator if the keys/values are equal.
*/
using EqualTreeMapPartialData = bool(__fastcall*)(const void* x, const void* y);

/*
* Typedef for a function that frees treenode keys/values.
* Used for testing getKey/getValue.
* 
* @param[out] nodeData - Vector of keys/values that shall be freed.
*/
template <typename T>
using FreeTreeNodePartialData = void(__fastcall*)(const std::vector<T>& nodeData);

/*
* Typedef for a function that is used to hide away minPair/maxPair.
* Used for testing the minPair/maxPair functions.
* 
* @param[in] tm - Treemap where minPair/maxPair is searched for.
* @param[out] pairBuffer - Buffer thats used to copy the min/max pair into.
* 
* @return A status value of success, does not contain or an error if the copy
*		  functions fail or the treemap/pairBuffer is a nullptr.
*/
using GetMinMaxPair = Status(__fastcall*)(const TreeMap* tm, void* pairBuffer);

/*
* Typedef for a function that is used to hide away ceilingPair/floorPair/higherPair/lowerPair.
* 
* @param[in] tm - Treemap that is used to execute the derived pair functions.
* @param[in] key - Key that is used to derive a pair from it.
* @param[out] pairBuffer - Buffer used to store the derived pair.
* 
* @return A status value for success, doesNotContain if no derived pair for the given
*		  key was found or an error if the copy functions failed or the treemap or the pairBuffer
*		  is a nullptr.
*/
using GetDerivedPair = Status(__fastcall*)(const TreeMap* tm, const void* key, void* pairBuffer);

/*
* Typedef for a function that is used to hide away pollFirstWrapper/pollLastWrapper/deletePair.
* 
* @param[in] tm - Treemap that is used to delete a pair with the given function.
* @param[in] key - Key that is used for deletePair.
* @param[out] pairBuffer - Buffer that stores the deleted pair if specified.
* 
* @return A status value for success, does not contain or an error if the tree map is a nullptr.
*/
using PollFunc = Status(__fastcall*)(TreeMap* tm, const void* key, void* pairBuffer);

/*
* The user is responsible that the treenode key struct is aligned
* on a quadword boundary.
*/

/*
* TreeNodeKey structure that is used for testing.
* The treemap stores keys that hold the states of america with
* the length of the string.
* 
* @var stateName - Name of a state of america.
* @var nameLength - Length of the states name.
*/
struct TreeNodeKey {
	char* stateName;
	size_t nameLength;
};

/*
* The user is responsible that the treenode value struct is aligned
* on a quadword boundary.
*/

/*
* TreeNodeValue structure that is used for testing.
* The treemap stores values that hold the capital city of the state
* with its population and year of existance.
* 
* @var capitalCity - Capital city of the state.
* @var existsSince - Year of existance as a short.
* @var population - Population of the capital city.
*/
struct TreeNodeValue {
	char* capitalCity;
	unsigned short existsSince;
	unsigned int population;
};

/*
* Test struct for pair operations. Combines the key and value.
* 
* @var key - TreeNodeKey.
* @var value - TreeNodeValue.
*/
struct TreeNodePair {
	TreeNodeKey key;
	TreeNodeValue value;
};

/*
* Test structure that combines the treenode pair 
* with children and the red flag to form a valid
* red black tree node.
* 
* @var pair - The structure that holds the key & value pair.
* @var left - Left child of the tree node.
* @var right - Right child of the tree node.
* @var isRed - Flag that indicates if the tree node is red or black.
*/
struct TreeNode {
	TreeNodePair pair;

	TreeNode* left;
	TreeNode* right;

	bool isRed;
};

/*
* Helper function for the treemap to compare two keys with each other.
* The implementation is as the KeyComparison typedef specifies and serves as an example
* how the treemap is used.
* 
* @param[in] tKey - Key that is compared to the inserted one.
* @param[in] insertedKey - Key that is compared to the tree nodes key.
* 
* @return Value of -1, 0 or 1 to specify if the insertedKey is bigger, smaller or equal
*		  to the tree nodes key.
*/
long compareTreeNodeKey(const void* tKey, const void* insertedKey);

/*
* Helper function that tests two keys for equality. Uses the compareTreeNodeKey method
* and converts it into a boolean. Just a wrapper function for the getKey tests.
* 
* @param[in] expectedKey - Expected tree node key that is compared to the resultKey.
* @param[in] resultKey - Result key of the getKey method that is compared to the
*						 expected one.
* 
* @return Indicator if the two keys are equal.
*/
bool equalsTreeNodeKey(const void* expectedKey, const void* resultKey);

/*
* Helper function for the treemap to compare two values for equality.
* The implementation is as the ValueEquality typedef specifies and serves
* as an example on how to use the treemap.
* 
* @param[in] tValue - Value of the tree node that is compared with the searched one.
* @param[in] tValueSearched - Value that is searched inside the treemap and compared
*							  with the nodes value.
* 
* @return Indicator that the two are equal. True for equal, false for not.
*/
bool equalsTreeNodeValue(const void* tValue, const void* tValueSearched);

/*
* Helper function for the treemap to copy keys from a src to a dest buffer.
* The implementation is as the KeyCopy typedef specified and serves as an example
* on how to use the treemap.
* 
* @param[out] destKey - The destination key buffer that stores the source key.
* @param[in] srcKey - The source key that will be copied into destKey.
* 
* @return A status value that indicates if the copying was successful or not.
*/
Status copyTreeNodeKey(void* dstKey, const void* srcKey);


/*
* Helper function for the treemap to copy values from a src to a dest buffer.
* The implementation is as the ValueCopy typedef specified and serves as an example
* on how to use the treemap.
* 
* @param[out] dstValue - The destination value buffer that stores the source value.
* @param[in] srcValue - The source value that will be copied into dstValue.
* @param[in] replaceValue - Flag that decides wether the old value in dstValue
*							should be replaced.
* 
* @return A status value that indicates if the copying was successful or not.
*/
Status copyTreeNodeValue(void* dstValue, const void* srcValue, bool replaceValue);

/*
* Utility function that creates a tree node key on the heap.
* 
* @param[in] stateName - The name of a state of america.
* 
* @throws If the allocation fails a runtime error is produced.
* 
* @return The tree node key allocated on the heap.
*/
TreeNodeKey* createTreeNodeKey(const char* stateName);

/*
* Utility function that creates a tree node value on the heap.
* 
* @param[in] capitalCity - The capital city of the state.
* @param[in] existsSince - The year of existance of the state as a short.
* @param[in] population - The population of the city.
* 
* @throws If the allocation fails a runtime error is produced.
* 
* @return The tree node value allocated on the heap.
*/
TreeNodeValue* createTreeNodeValue(const char* capitalCity, unsigned short existsSince, unsigned int population);

/*
* Utility function that creates a tree node pair on the heap.
* 
* @param[in] stateName - The name of a state of america.
* @param[in] capitalCity - The capital city of the state.
* @param[in] existsSince - The year of existance of the state as a short.
* @param[in] population - The population of the city.
* 
* @throws If the allocation fails a runtime error is produced.
* 
* @return The tree node pair allocated on the heap.
*/
TreeNodePair* createTreeNodePair(const char* stateName, const char* capitalCity, unsigned short existsSince, unsigned int population);

/*
* Utility function that creates a tree node on the heap.
*
* @param[in] stateName - The name of a state of america.
* @param[in] capitalCity - The capital city of the state.
* @param[in] existsSince - The year of existance of the state as a short.
* @param[in] population - The population of the city.
* @param[in] isRed - Flag that indicates if the tree node is red or black.
* 
* @throws If the allocation fails a runtime error is produces.
* 
* @return The tree node allocated on the heap.
*/
TreeNode* createTreeNode(const char* stateName, const char* capitalCity, unsigned short existsSince,
	unsigned int population, bool isRed);

/*
* Creates a treemap with the following node structure:
* 
*									   "Oregon"
*										  B
*				  "Minnesota"						   "Washington"
*					  R										B
*		 "Kansas"		  "New York"
*			B				  B
*
* used for testing.
* 
* @throws A runtime error if the tree node creation fails.
* 
* Underlying putPair failures are not tracked because the tests would fail anyway.
* 
* @return The treemap allocated on the heap with the above tree node structure.
*/
TreeMap* createTestTree();

/*
* Creates a treemap on the heap with a tree node structure that is build up
* by inserting nodes into the map from the given vector.
* 
* Failures of putPair will not be tracked because the test would fail anyways.
* 
* @return The treemap with a tree node structure made up by the inserted vector.
*/
TreeMap* createTestTreeByNodes(const std::vector<const TreeNode*>& nodes);

/*
* Frees the given tree nodes.
* All nested heap memory will be freed.
* 
* @param[out] nodes - The tree nodes that will be freed.
*/
void freeTreeNodes(const std::vector<TreeNode*>& nodes);

/*
* Frees the given tree node keys.
* All nested heap memory will be freed.
* The vector is allowed to hold nullptrs, they will be ignored.
* 
* @param[out] keys - The keys that will be freed.
*/
void freeTreeNodeKeys(const std::vector<TreeNodeKey*>& keys);

/*
* Frees the given tree node values.
* All nested heap memory will be freed.
* The vector is allowed to hold nullptrs, they will be ignored.
* 
* @param[out] values - The values that will be freed.
*/
void freeTreeNodeValues(const std::vector<TreeNodeValue*>& values);

/*
* Frees a tree single tree node pair. Is a helper function that is used inside the treemap
* to clear a tree node from its nested heap memory as specified in the typedef FreePair.
* 
* @param[out] treeNodePair - Pair that gets its nested heap memory freed.
*/
void freeTreeNodePair(void* treeNodePair);

/*
* Frees all tree node pairs completely. Nested heap memory will be freed.
* The vector is allowed to hold nullptrs, they will be ignored.
* 
* @param[out] pairs - Pairs that will be freed.
*/
void freeTreeNodePairs(const std::vector<TreeNodePair*>& pairs);

/*
* Asserts that two tree node keys are equal.
* 
* @param[in] expected - The expected tree node key.
* @param[in] result - The result tree node key of the operation thats tested.
*/
void assertTreeNodeKeyEquals(const TreeNodeKey* expected, const TreeNodeKey* result);

/*
* Asserts that two tree node values are equal.
* 
* @param[in] expected - The expected tree node value.
* @param[in] result - The result tree node value of the operation thats tested.
*/
void assertTreeNodeValueEquals(const TreeNodeValue* expected, const TreeNodeValue* result);

/*
* Asserts wether two tree node structures are completely equal, meaning
* the whole tree they build up will be evaluated.
* 
* @param[in] expected - The expected tree node structure.
* @param[in] result - The result structure of the operation thats being tested.
*/
void assertTreeNodeEquals(const TreeNode* expected, const TreeNode* result);

/*
* Asserts that the treemap specified has the given nodeAmount and that the root is not
* a nullptr.
* 
* @param[in] result - Treemap result of an operation thats tested.
* @param[in] nodeAmount - The expected node amount that the treemap should hold.
*/
void assertTreeMapMemberEqual(const TreeMap* result, size_t nodeAmount);

/*
* Asserts that the treemap contains (or not) the given values.
* The containsValue function is tested here.
* 
* Frees all provided values in the vector.
* 
* @param[in, out] values - Values that are expected to be in (or not) the treemap.
* @param[in] tm - Treemap that is expected to hold the values.
* @param[in] expectedStat - Expected result status for all values. Decides if a value is expected or not.
*/
void assertContainsValueEquals(const std::vector<TreeNodeValue*>& values, const TreeMap* tm, Status expectedStat);

/*
* Asserts that the treemap contains (or not) the given keys.
* The containsValue function is tested here. 
*
* Frees all provided keys in the vector.
* 
* @param[in, out] keys - Keys that are expected to be in (or not) the treemap.
* @param[in] tm - Treemap that is expected to hold the keys.
* @param[in] expectedStat - Expected result status for all keys. Decides if a key is expected or not.
*/
void assertContainsKeyEquals(const std::vector<TreeNodeKey*>& keys, const TreeMap* tm, Status expectedStat);

/*
* Asserts that the treemaps getValue function retrieves the correct values for the given key.
* 
* Frees all provided keys in the vector.
* 
* @param[in, out] keys - Keys that are used to query getValue.
* @param[in] expectedValues - Expected values of the getValue function.
* @param[in] tm - Treemap that is used for the testing of getValue.
* @param[in] expectedStat - The expected result status of getValue, decides if the test wants to match valid gets or not.
*/
void assertValuesEquals(const std::vector<TreeNodeKey*>& keys, const std::vector<TreeNodeValue>& expectedValues, TreeMap* tm, Status expectedStat);

/*
* Asserts that the function getKey/getValue work as expected.
* 
* Frees the provided and expected vectors accordingly.
* 
* @param[in, out] provided - Keys or values that are served to the getKey/getValue functions.
* @param[in, out] expected - Expected Keys or values of the getKey/getValue functions.
* @param[in] tm - Treemap that is used for testing these functions.
* @param[in] expectedStatus - The expected status. Decides if a value/key should be found or not.
* @param[in] getTreeMapData - Function pointer for the getKey/getValue functions.
* @param[in] getTreeMapDataEquals - Function pointer for the equals function used to compare the result
*									with the expected.
* @param[in] freeProvided - Function pointer that frees the provided key/value vector.
* @param[in] freeExpected - Function pointer that frees the expected key/value vector.
*/
template<typename T, typename E>
void assertTreePartialDataEquals(const std::vector<T*>& provided, const std::vector<E*>& expected, TreeMap* tm, Status expectedStat,
	GetTreeMapPartialData getTreeMapData, EqualTreeMapPartialData getTreeMapDataEquals, FreeTreeNodePartialData<T*> freeProvided,
	FreeTreeNodePartialData<E*> freeExpected) {
	E buffer;
	Status getDataStat;

	for (size_t i = 0; i < provided.size(); ++i) {
		getDataStat = getTreeMapData(tm, provided[i], &buffer);

		ASSERT_EQ(expectedStat, getDataStat);

		if (expectedStat == Status::SUCCESS) ASSERT_EQ(true, getTreeMapDataEquals(expected[i], &buffer));
	}

	freeProvided(provided);
	freeExpected(expected);
}

/*
* Asserts that two tree node pairs are equal.
* 
* @param[in] expectedPair - Expected pair.
* @param[in] resultPair - The result pair of the tested operation.
*/
void assertTreeNodePairEquals(TreeNodePair* expectedPair, TreeNodePair* resultPair);

/*
* Asserts the min/max pair function work as expected for the given treemap.
* 
* If the expected status is success, the expectedPair will be freed.
* 
* @param[in, out] expectedPair - The expected pair of the min/max function.
* @param[in] tm - The treemap thats used for testing min/max pair.
* @param[in] getMinMaxPairFunc - Function pointer to the min/max pair functions.
* @param[in] expectedStat - Status that decides if the function tests for success or errors.
* @param[in] dismissPair - Flag that makes the min/max pair buffer to a nullptr, meaning
*						   the function will return an error status.
*/
void assertMinMaxPairEquals(TreeNodePair* expectedPair, const TreeMap* tm, GetMinMaxPair getMinMaxPairFunc,
	Status expectedStat, bool dismissPair);

/*
* Asserts that the function ceiling/floor/higher/lower are working correctly.
* 
* This function frees the providedKeys and expectedPairs vectors.
* 
* @param[in, out] providedKeys - Keys that are fed into the deriving key pair function.
* @param[in, out] expectedPairs - Expected pairs that should be returned for the specified keys.
* @param[in] tm - Treemap thats used for testing these functions.
* @param[in] getDerivedKeyPairFunc - Function pointer for ceiling/floor/higher/lower.
* @param[in] expectedStat - Expected status that decides if the function tests for success or failure.
* @param[in] dismissBuffer - Flag that indicates if the buffer should be a nullptr leading to an error.
*/
void assertDerivedKeyPairsEqual(const std::vector<TreeNodeKey*> providedKeys, const std::vector<TreeNodePair*> expectedPairs,
	TreeMap* tm, GetDerivedPair getDerivedKeyPairFunc, Status expectedStat, bool dismissBuffer);

/*
* Asserts that the replaceValue function works as expected.
* 
* The providedKeys and expectedValues will be freed accordingly.
* 
* @param[in, out] providedKeys - Provided keys that are used to query the value
*								 inside the treemap.
* @param[in, out] expectedValues - Expected values that the tree must hold after the replacement.
* @param[in] tm - The treemap that is used for testing the replaceValue function.
* @param[in] expectedStat - The expected status of the function. Decides if we test for success or failure.
*/
void assertReplacedValueEquals(const std::vector<TreeNodeKey*>& providedKeys, const std::vector<TreeNodeValue*>& expectedValues,
	TreeMap* tm, Status expectedStat);

/*
* Asserts that after a deletion function the deleted pair saved is as expected and the given
* treenode structure is equal to the expected one.
* 
* The result deleted pair and the provided key will be freed accordingly.
* 
* @param[in] expectedDeletedPair - The expected deleted pair.
* @param[in, out] resultDeletedPair - The result deleted pair of the delete pair functions.
* @param[in] tm - The treemap thats used to test the deletion functions.
* @param[in] expectedRoot - The expected tree node structure after the deletion.
* @param[in, out] providedKey - The key that is used to delete the node or a nullptr if function like pollFirst/Last
*						   are used. Its just freed here.
* @param[in] dismissBuffer - Flag that indicates if the buffer should be dismissed.
*							 If it is dismissed the pairs won't be compared.
*/
void assertTreeNodesEqual(TreeNodePair* expectedDeletedPair, TreeNodePair* resultDeletedPair, TreeMap* tm,
	TreeNode* expectedRoot, TreeNodeKey* providedKey, bool dismissBuffer);

/*
* Asserts that the deletion functions work as expected.
* 
* Uses assertTreeNodesEqual and just clears the given tree map and the nodes too.
* 
* @param[in] expectedDeletedPair - The pair that is expected to be returned by the
*								   deletion function.
* @param[in, out] resultDeletedPair - The result pair of the deletion function.
* @param[in, out] tm - The treemap that is used to test the deletion functions.
* @param[in] expectedRoot - The tree node structure that is expected after the deletion.
* @param[in, out] nodes - Nodes that are used to build up the tree
* @param[in, out] providedKey - The key that is used in the deletion function, its just freed here.
* @param[in] dismissBuffer - Flag that indicates if the pair buffer should be dismissed.
*							 It will not be compares with the expected or being freed if it is dismissed.
*/
void assertDeletionEquals(TreeNodePair* expectedDeletedPair, TreeNodePair* resultDeletedPair, TreeMap* tm, TreeNode* expectedRoot,
	const std::vector<TreeNode*> nodes, TreeNodeKey* providedKey, bool dismissBuffer);

/*
* Wrapper for the pollFirstPair function that excepts a key that is not used.
* Its just for compatibility with the Pollfunc typedef.
* 
* @param[in] tm - Treemap that is used with pollFirstPair.
* @param[in] key - Unused variable thats here for compatibility.
* @param[in] pairBuffer - Buffer that stores the deleted pair.
*/
Status pollFirstWrapper(TreeMap* tm, const void* key, void* pairBuffer);

/*
* Wrapper for the pollLastPair function that excepts a key that is not used.
* Its just for compatibility with the Pollfunc typedef.
*
* @param[in] tm - Treemap that is used with pollLastPair.
* @param[in] key - Unused variable thats here for compatibility.
* @param[in] pairBuffer - Buffer that stores the deleted pair.
*/
Status pollLastWrapper(TreeMap* tm, const void* key, void* pairBuffer);

/*
* Tests the deletion of the minimum with the following tree structure:
* 
*									   "Oregon"
*										  B
*				  "Minnesota"						   "Washington"
*					  B										B
*
* Which will end up like this:
* 
*									 "Washington"
*										  B
*				   "Oregon"
*					  R
* 
* @param[in] p - deletePair or pollFirst function.
* @param[in] withKey - Indicator if the function used needs a key.
* @param[in] dismissBuffer - Indicator that the buffer should be a nullptr.
*/
void testMinDeletionWithRootThreeNode(PollFunc p, bool withKey, bool dismissBuffer);

/*
* Tests the deletion of the maximum with the following tree structure:
*
*									   "Oregon"
*										  B
*				  "Minnesota"						   "Washington"
*					  B										B
*
* Which will end up like this:
* 
*									   "Oregon"
*										  B
*				  "Minnesota"
*					  R
* 
* @param[in] p - deletePair or pollLast function.
* @param[in] withKey - Indicator if the function used needs a key.
* @param[in] dismissBuffer - Indicator that the buffer should be a nullptr.
*/
void testMaxDeletionWithRootThreeNode(PollFunc p, bool withKey, bool dismissBuffer);

/*
* Tests the deletion of the maximum with the following tree structure:
* 
*									   "Minnesota"
*										    B
*				  "Kansas"								 "Oregon"
*					 B										B
*												"New York"
*												    R
*
* Which will end up like this:
* 
*									   "Minnesota"
*										    B
*				  "Kansas"								"New York"
*					 B										B
*
* 
* @param[in] p - deletePair or pollLast function.
* @param[in] withKey - Indicator if the function used needs a key.
* @param[in] dismissBuffer - Indicator that the buffer should be a nullptr.
*/
void testMaxDeletionWithLeftLeaningRightNode(PollFunc p, bool withKey, bool dismissBuffer);

/*
* Tests the deletion of the minimum with the following tree structure:
*
*									   "Minnesota"
*										    B
*				  "Kansas"								 "Oregon"
*					 B										B
*												"New York"
*												    R
*
* Which will end up like this:
* 
*									   "New York"
*										    B
*				  "Minnesota"								 "Oregon"
*					   B										B
* 
* @param[in] p - deletePair or pollFirst function.
* @param[in] withKey - Indicator if the function used needs a key.
* @param[in] dismissBuffer - Indicator that the buffer should be a nullptr.
*/
void testMinDeletionWithLeftLeaningRightNode(PollFunc p, bool withKey, bool dismissBuffer);

/*
* Tests the deletion of the minimum with the following tree structure:
* 
*									   "Oregon"
*										  B
*				  "Minnesota"						   "Washington"
*					  R										B
*		 "Kansas"		  "New York"
*			B				  B
*					"Montana"
*						R
*
* Which will end up like this:
* 
*									   "Oregon"
*										  B
*				  "Montana"						       "Washington"
*					  R										B
*		 "Minnesota"	  "New York"
*			 B				  B
* 
* @param[in] p - deletePair or pollFirst function.
* @param[in] withKey - Indicator if the function used needs a key.
* @param[in] dismissBuffer - Indicator that the buffer should be a nullptr.
*/
void testMinDeletionWithLeftLeaningThreeNode(PollFunc p, bool withKey, bool dismissBuffer);

/*
* Tests the deletion of the maximum with the following tree structure:
* 
*									   "Oregon"
*										  B
*				  "Minnesota"						   "Washington"
*					  R										B
*		 "Kansas"		  "New York"
*			B				  B
*					"Montana"
*						R
*
* Which will end up like this:
* 
*									  "New York"
*										  B
*				  "Minnesota"						     "Oregon"
*					  R										B
*		 "Kansas"		  "Montana"
*			B				  B
*
* 
* @param[in] p - deletePair or pollLast function.
* @param[in] withKey - Indicator if the function used needs a key.
* @param[in] dismissBuffer - Indicator that the buffer should be a nullptr.
*/
void testMaxDeletionWithLeftLeaningThreeNode(PollFunc p, bool withKey, bool dismissBuffer);

#endif