; @file tree_map.inc 
;	
; Treemap include file thats used for defining external c/c++ functions,
; structures that are used to implement the treemap and constants that are
; are used inside the treemaps functions.
;
; @author Collector
; @date 03/09/2023

ifndef tree_map_inc

	tree_map_inc = 0

	option casemap:none

	.const

; Utility constants that are used where needed.
nullptr = 0
linebreak = 10
shadowStorage = 32
qwordSize = sizeof qword

; For the createTreeMap function.
keySize = 16
valueSize = 24
keyCompFunc = 32
valueEqualFunc = 40
keyCopyFunc = 48
valueCopyFunc = 56
treeNodeBaseSize = 2 * qwordSize + 1
parameterStackStart = 16
parameterStackLimit = 64
statusFlagPtr = 72

; Used in containsValue.
searchedValueOffsetRSP = 8

; Used for putPair and insertPair.
currentTreeNode = 16
toInsertValuePair = 24
leftTreeNode = 24
rightTreeNode = 32
treemap = 40

; Used inside findAddressOfKey.
currentTreeNode3 = 8
searchedKey = 16

; Used inside getValue.
valueBuffer = 24

; Used inside findAddressOfValue/getKey.
currentRoot = 8
treeNodeValueOffset = 8
searchedValueOffsetRBP = 16
treemapOffset = 24

; Used by replaceValue.
replacementValue = 24
treemap3 = 16

; Used inside the copyPair function.
copyBuffer = 32
copyTreeNodePair = 40
treemap4 = 48

; Used for the higher/lower/ceiling/floor functions.
searchKey = 16
treemap2 = 24
pairBuffer = 32
higherLowerFlag = 40
foundPair = 32
currentTreeNode2 = 40
ceilingFloorFlag = 40

; Used for the higher and lower search flag.
searchAsLower = 0
searchAsHigher = 1

; Used for the ceiling and floor search flag.
searchAsFloor = 0
searchAsCeiling = 1

; Adding booleans for readability.
true = 1
false = 0

; Status flags that are returned by various functions
; to indicate if they were successful.
success = 0
errHeapAllocation = 1
errCopyKeyFunc = 2
errCopyValueFunc = 3
doesNotContain = 4
alreadyContains = 5
keySizeZero = 6
valueSizeZero = 7
keyCompFuncNullptr = 8
valueEqualFuncNullptr = 9
keyCopyFuncNullptr = 10
valueCopyFuncNullptr = 11
treeMapNullptr = 12
treeNodePairNullptr = 13
keyBufferNullptr = 14
valueBufferNullptr = 15
pairBufferNullptr = 16


	.data

; Defines the treemap structure that is the core of this implementation.
; To achieve a generic treemap the single nodes just copy the bytes of the given pair
; through the specified copy function.
; Comparison is also achieved through the specified functions. In case of deleting a pair
; in case of additional heap memory referenced from the deleted pair the freePairFunc is used.
TreeMap struct qwordSize
root qword ?
nodeAmount qword ?
keySize qword ?
valueSize qword ?
compareKeyFunc qword ?
equalsValueFunc qword ?
copyKeyFunc qword ?
copyValueFunc qword ?
freePairFunc qword ?
TreeMap ends

; Only as a reference for sizeof. To enable generic tree nodes we just copy x amount
; of bytes that the user provides and reserve enough storage so that the left and
; right references exist with the color at the bottom of the allocated memory.
TreeNode struct qwordSize
left qword ?
right qword ?
isRed byte ?
TreeNode ends

	.code

; c standard function used inside the assembly code.
externdef malloc:proc
externdef free:proc
externdef printf:proc
externdef memcpy:proc

endif