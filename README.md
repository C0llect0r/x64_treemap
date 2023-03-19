# x64 Treemap implementation

This small repository implements a treemap with basic functionality.  
It is completely written in assembly and adheres to the microsoft abi.
Due to the usage of 64 bit registers a usage in plain x86 is not possible.

The internal treemap implementation is based upon the left leaning red black tree robert sedegewick showed
in his book [Algorithms 4th Edition](https://algs4.cs.princeton.edu/home/) with a few tweaks in the deletion functions.

Additionaly, the implementation comes with unit tests that were made to see if everything works correctly and to give an example on how to use the treemap.  
The library to test the assembly code is [google test](https://github.com/google/googletest).

## Usage

The basic layout of the treemap structure is as follows:

```
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
```

The map holds the treenodes starting at root and the amount of nodes that are populated inside the map.
To achieve a generic map, the structure stores the size of the key and value and just copies the amount of bytes
per pair, solo key or value. On top of that the implementation expects that the user uses structs for keys, values and pairs.
These structure definitions will then be combined as a treenode like this:

```
struct TreeNodeKey {
    // Your key data.
};

struct TreeNodeValue {
    // Your value data.
};

struct TreeNodePair {
	TreeNodeKey key;
	TreeNodeValue value;
};

struct TreeNode {
	TreeNodePair pair;

	TreeNode* left;
	TreeNode* right;

	bool isRed;
};
```

Because of this the key and value sizes need to include any padding that comes along in the structs.
For decent information about structure padding/packing you can refer to this [guide](http://www.catb.org/esr/structure-packing/).

Furthermore to generically compare keys/values and also copy them, the structure needs specific functions
that implement this behaviour for your given use case. The functions must be the same as the typedefs in `tree_map.h` explain.

Last but not least for the case of nested heap inside of a structure the tree map can be given a
freeing function that clears its nested heap before deleting tree nodes from the map. The same constraints are listed
inside the `tree_map.h` file as for the others.