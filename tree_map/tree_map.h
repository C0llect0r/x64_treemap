/*
* @file tree_map.h
* 
* Declares the treemaps base and utility functions with typedefs that are used
* for the functions the map needs to operate properly.
* 
* @author Collector
* @data 03/18/2023
*/

#ifndef TREEMAP_H
#define TREEMAP_H

/*
* Status values that functions return
* to indicate if something went wrong.
* 
* Recheck this enum to see what the functions return.
* It should be obvious by the names and comments.
*/
enum class Status {
	SUCCESS = 0, // Everything went without errors.
	ERR_HEAP_ALLOCATION, // Malloc failed.
	ERR_COPY_KEY, // The key copy function failed.
	ERR_COPY_VALUE, // The value copy function failed.
	DOES_NOT_CONTAIN, // The tree map does not contains the key/value.
	ALREADY_CONTAINS, // The tree map already has the specified the given pair.
	KEY_SIZE_ZERO, // The key size for createTreeMap is 0.
	VALUE_SIZE_ZERO, // The value size for createTreeMap is 0.
	KEY_COMP_FUNC_NULLPTR, // The key comparison function in createTreeMap is a nullptr.
	VALUE_EQUAL_FUNC_NULLPTR, // The value equality function in createTreeMap is a nullptr.
	KEY_COPY_FUNC_NULLPTR, // The key copy function in createTreeMap is a nullptr.
	VALUE_COPY_FUNC_NULLPTR, // The value copy function in createTreeMap is a nullptr.
	TREE_MAP_NULLPTR, // The specified treemap is a nullptr.
	TREE_NODE_PAIR_NULLPTR, // The given treenode pair is a nullptr.
	KEY_BUFFER_NULLPTR, // The given key buffer is a nullptr.
	VALUE_BUFFER_NULLPTR, // The given value buffer is a nullptr.
	PAIR_BUFFER_NULLPTR // The given pair buffer is a nullptr.
};

/*
* Typedef for a comparison function that compares two keys for equality.
* If k1 is smaller than k2 -1 shall be returned.
* If k1 and k2 are equal 0 shall be returned.
* If k1 is bigger than k2 1 shall be returned.
* 
* Is used in most functions that have a key as the parameter to find 
* if a key value pair exists.
* 
* @param[in] k1 - Key that is compared with k2.
* @param[in] k2 - Key that is compared with k1.
* 
* @return Indicator that tells the ordering relation of k1 and k2.
*/
using KeyComparison = long (*)(const void* k1, const void* k2);

/*
* Typedef for an equality function that tests wether two values are equal.
* 
* Is used to find a specified value inside a treemap.
* 
* @param[in] v1 - Value that is compared with v2.
* @param[in] v2 - Value that is compared with v1.
* 
* @return Indicator if v1 and v2 are equal.
*/
using ValueEquality = bool (*)(const void* v1, const void* v2);

/*
* Typedef for a copy function that copies a source key into the destination key buffer.
* 
* @param[out] dstTreeNodeKey - TreeNode key that will receive srcTreeNodeKey's data.
* @param[in] srcTreeNodeKey - TreeNode key that gives it's data to dstTreeNodeKey.
* 
* @return A status value that indicates if the copying was successful or not.
*/
using KeyCopy = Status (*)(void* dstTreeNodeKey, const void* srcTreeNodeKey);

/*
* Typedef for a copy function that copies a source value into the given destination value.
* Also here the value can be completely replaced meaning dstTreeNodeValue can hold data
* on the heap and it will be freed accordingly.
* 
* @param[out] destTreeNodeValue - Value buffer that stores the contents of srcTreeNodeValue.
* @param[in] srcTreeNodeValue - Value buffer that provides data for dstTreeNodeValue.
* @param[in] replaceValue - Flag to indicate if the old data of dstTreeNodeValue should be completely replaced.
* 
* @return A status value that indicates if the copying was successful or not.
*/
using ValueCopy = Status (*)(void* dstTreeNodeValue, const void* srcTreeNodeValue, bool replaceValue);

/*
* Typedef for a function that is used to free all nested heap memory
* of a tree nodes pair.
* The function does not need to be defined in the treemap if the nodes
* dont have nested heap memory.
* 
* @param[out] treeNodePair - Treenode pair that gets its heap memory freed.
*/
using FreePair = void (*)(void* treeNodePair);

/*
* Treemap structure that builds the core of this application.
* 
* @var root - Root node of the treemap building the red black tree at runtime.
* @var nodeAmount - Count of nodes that the treemap has allocated.
* @var keySize - Size of the key of a node. Used to know exactly how much bytes need
*				 to be copied.
* @var valueSize - Size of the value of a node. Used to know exactly how much bytes need
*				   to be copied. The byte copying makes this treemap generic.
* @var compareKeyFunc - Function that is used to compare two tree nodes keys.
* @var equalsValueFunc - Function that is used to compare two tree node values.
* @var keyCopyFunc - Function that is used to copy tree node keys.
* @var valueCopyFunc - Function that is used to copy tree node values.
* @var freePairFunc - Function that frees heap memory of a tree nodes pair.
*/
struct TreeMap {
	void* root;
	size_t nodeAmount;
	size_t keySize;
	size_t valueSize;
	KeyComparison compareKeyFunc;
	ValueEquality equalsValueFunc;
	KeyCopy keyCopyFunc;
	ValueCopy valueCopyFunc;
	FreePair freePairFunc;
};

extern "C" {
	// ----------------------------------------------------------- Everything below is part of the base implementation. -----------------------------------------------------------

	/*
	* Creates a treemap.
	* 
	* @runtime O(1).
	* 
	* @param[in] keySize - Size of the key inside a treenode in bytes.
	* @param[in] valueSize - Size of the value inside of a treenode in bytes.
	* @param[in] kComp - Function that compares two treenode keys with another.
	* @param[in] vEquals - Function that tests two treenode values for equality.
	* @param[in] kCopy - Function that copies a treenodes key into another treenodes
	*					 key destination buffer.
	* @param[in] vCopy - Function that copies a treenodes value into another treenodes
	*					 value destination buffer.
	* @param[in] fPair - Frees heap memory of a treenodes pair.
	* @param[out] s - Status flag that indicates if the treemaps creation was successful.
	*				  Also throws errors if the key/valueSize is zero, the KeyCompare/ValueEquality/
	*				  KeyCopy/ValueCopy/Status is a nullptr.
	* 
	* @return A pointer to a treemap thats allocated on the heap.
	*/
	TreeMap* createTreeMap(size_t keySize, size_t valueSize, KeyComparison kComp,
		ValueEquality vEqual, KeyCopy kCopy, ValueCopy vCopy, FreePair fPair, Status* s);

	/*
	* Clears the complete tree of the treemap, freeing all its treenode memory.
	* 
	* @runtime O(N).
	* 
	* @param[in, out] tm - Treemap that gets its tree cleared.
	* 
	* @return A status value of success or an error if the treemap is a nullptr.
	*/
	Status clearTreeMap(TreeMap* tm);

	/*
	* Deletes the specified treemap freeing all nodes allocated inside of it
	* and freeing the treemap structure too.
	* 
	* @runtime O(N).
	* 
	* @param[in, out] tm - TreeMap that shall be deleted.
	* 
	* @return Status value that is set to success or a treemap nullptr error.
	*/
	Status deleteTreeMap(TreeMap* tm);

	/*
	* Inserts a treenode into a treemap if the specified key does not already exist.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in, out] tm - Treemap that gets a new pair inserted.
	* @param[in] pair - Treenode pair that gets inserted into the treemap.
	* 
	* @return Status value for success, if the key is already inside the map, an allocation
	* error happened, the copy functions failed or the treemap is a nullptr.
	*/
	Status putPair(TreeMap* tm, const void* pair);
	
	/*
	* Deletes a key value pair from the treemap by the given key.
	* The deleted pair can be copied into a buffer if it is provided.
	* If a nullptr is given the pair will be deleted without a copy return.
	* 
	* The returned pairBuffer is a shallow copy meaning any nested heap
	* memory still has to be freed manually.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in, out] tm - The treemap that gets a key value pair deleted.
	* @param[in, out] key - The key to identify the key value pair.
	* @param[out] pairBuffer - A buffer to copy the deleted pair.
	* 
	* @return A status value for success, does not contain or an error if the tree map is a nullptr.
	*/
	Status deletePair(TreeMap* tm, const void* key, void* pairBuffer);
	
	/*
	* Deletes the minimum from the specified treemap and returns it inside the buffer.
	* If the buffer is a nullptr no copy will be returned.
	* 
	* The returned pairBuffer is a shallow copy meaning any nested heap
	* memory still has to be freed manually.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in, out] tm - Treemap that gets its min node deleted.
	* @param[out] pairBuffer - Buffer that holds a copy of the deleted node.
	* 
	* @return A status value for success, does not contain a min or an error if the treemap
	*		   is a nullptr.
	*/
	Status pollFirstPair(TreeMap* tm, const void* pairBuffer);

	/*
	* Deletes the maximum key value pair from the given treemap.
	* If a buffer is specified the deleted pair will be copied into it.
	* 
	* The returned pairBuffer is a shallow copy meaning any nested heap
	* memory still has to be freed manually.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in, out] tm - Treemap that gets its maximum pair deleted.
	* @param[out] pairBuffer - Buffer that stores the deleted max pair if
	*						   it is not a nullptr.
	* 
	* @return A Status value of success, does not contain or an error if
	*		  the specified treemap is a nullptr.
	*/
	Status pollLastPair(TreeMap* tm, const void* pairBuffer);

	// ----------------------------------------------------------- Everything below is part of the utility implementation. -----------------------------------------------------------

	/*
	* Retrieves the given value identified by the specified key if such a key value pair
	* exists. The returned value is a deep copy.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - The treemap that is searched for the specified key value pair. 
	* @param[in] key - Key that identifies the key value pair.
	* @param[out] valueBuffer - Buffer that stores the value of the key value pair.
	* 
	* @return A status value of success, does not contains or an error if the
	*		  copy functions fail or the treemap/valueBuffer is a nullptr.
	*/
	Status getValue(const TreeMap* tm, const void* key, void* valueBuffer);

	/*
	* Retrieves the given key for the specified value if such a key value pair exists.
	* The returned key is a deep copy.
	* 
	* @runtime O(N).
	* 
	* @param[in] tm - Treemap thats searched for a key that holds the given value.
	* @param[in] value - Value that is used to find the given key.
	* @param[out] keyBuffer - Buffer that stores the found key.
	* 
	* @return A status value for success, does not contain or an error if the
	*		  the copy functions fail or the treemap/keybuffer is a nullptr.
	*/
	Status getKey(const TreeMap* tm, const void* value, void* keyBuffer);
	
	/*
	* Tests if the given value is inside of the treemap.
	* 
	* @runtime O(N).
	* 
	* @param[in] tm - Treemap that is searched for the given value.
	* @param[in] value - Value that is searched inside the treemap.
	* 
	* @return A status value for success, meaning the value is inside of
	*		  the treemap, does not contain meaning the value is not in the treemap
	*		  or an error for the treemap being a nullptr.
	*/
	Status containsValue(const TreeMap* tm, const void* value);

	/*
	* Test if the given key is inside the treemap.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap that is searched for the given key.
	* @param[in] key - The key that is searched inside the treemap.
	* 
	* @return A status flag of success meaning the key is inside the map,
	*		  does not contain meaning it is not or an error for the treemap
	*		  being a nullptr.
	*/
	Status containsKey(const TreeMap* tm, const void* key);

	/*
	* Replaces a value identified by the given key with the specifed replacement value
	* if it exists. The replaced value is automatically freed if nested heap was acquired.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in, out] tm - Treemap that gets its value replaced.
	* @param[in] - Key to identify the key value pair for the replacement.
	* @param[in] replacementValue - Value that replaces the old one.
	* 
	* @return A status value of success, does not contain or an error if the
	*		  treemap is a nullptr.
	*/
	Status replaceValue(TreeMap* tm, const void* key, const void* replacementValue);

	/*
	* Fetches the next higher or equal key value pair in relation to the specified key
	* if it exists.
	* 
	* The received pair is a deep copy.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap thats searched for the ceiling pair.
	* @param[in] key - Key that is used to find the next ceiling pair.
	* @param[out] pairBuffer - Buffer that is used to store the ceiling pair.
	* 
	* @return A status value of success, does not contain or an error if the copy functions
	*		  fail or the treemap/pairBuffer is a nullptr.
	*/
	Status ceilingPair(const TreeMap* tm, const void* key, void* pairBuffer);
	
	/*
	* Fetches the next lower or equal key value pair in relation to the given key
	* if it exists.
	* The returned pair is a deep copy.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap that is searched for a floor pair.
	* @param[in] key - Key that is used to find the floor pair for the key.
	* @param[out] pairBuffer - Buffer that stores the floor pair.
	* 
	* @return A status value of success, does not contain or an error the copy functions fail
	*		  or the treemap/pairBuffer is a nullptr.
	*/
	Status floorPair(const TreeMap* tm, const void* key, void* pairBuffer);

	/*
	* Fetches the next lower key value pair in relation to the given key.
	* The returned pair is a deep copy.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap that is searched for a lower pair.
	* @param[in] key - Key that is used to find the lower pair.
	* @param[out] pairBuffer - Buffer that holds the lower pair.
	* 
	* @return A status value of success, does not contain or an error
	*		  if the copy functions fail or the treemap/pairBuffer is a nullptr.
	*/
	Status lowerPair(const TreeMap* tm, const void* key, void* pairBuffer);
	
	/*
	* Fetches the next higher key value pair in relation to the given key.
	* The returned pair is a deep copy. 
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap that is searched for a higher pair.
	* @param[in] key - Key that is used to find the higher pair.
	* @param[out] pairBuffer - Buffer that holds the higher pair.
	* 
	* @return A status value of success, does not contain or an error
	*		  if the copy functions fail or the treemap/pairBuffer is a nullptr.
	*/
	Status higherPair(const TreeMap* tm, const void* key, void* pairBuffer);

	/*
	* Retrieves the most left key value pair inside the treemap.
	* 
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap that is used to retrieve the minimum key value pair.
	* @param[out] pairBuffer - Buffer that is used to store a copy of the minimum
	*						   key value pair.
	* 
	* @return A status value of success, does not contain or an error if the copy
	*		  functions fail or the treemap/pairBuffer is a nullptr.
	*/
	Status minPair(const TreeMap* map, void* pairBuffer);
	
	/*
	* Retrieves the most right key value pair inside the treemap.
	*
	* @runtime O(Log(N)).
	* 
	* @param[in] tm - Treemap that is used to retrieve the minimum key value pair.
	* @param[out] pairBuffer - Buffer that is used to store a copy of the minimum
	*						   key value pair.
	*
	* @return A status value of success, does not contain or an error if the copy
	*		  functions fail or the treemap/pairBuffer is a nullptr.
	*/
	Status maxPair(const TreeMap* map, void* pairBuffer);
}


#endif