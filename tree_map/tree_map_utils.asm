; @file tree_map_utils.asm
;
; This file defines utility functions for the treemap like getting key/value copys,
; checking if a key or value exists inside the treemap, replacement of values,
; and deriving pairs from keys or getting the min/max pair of the treemap.
;
; @author Collector
; @date 03/09/2023

	include tree_map.inc

	.code


	public getValue

; Gets the given value associated by the given key if the pair exists.
;
; @RCX qword[in] - Pointer to the treemap the value is looked up in.
; @RDX qword[in] - Pointer to the key of the value thats searched for.
; @R8 qword[out] - Pointer to a buffer tree node value where the found value is copied into.
;
; @return Success, treeMapNullptr, valueBufferNullptr, doesNotContain if the value does not exist
;		  in the map or a errCopyValueFunc when the copy function of the value fails.
getValue proc

	; Allocating storage for findAddress
	sub rsp, shadowStorage

	; Check if the treeMap is a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Check if the provided value buffer is a nullptr.
	cmp r8, nullptr
	je valueBufferInvalid

	; Save the buffer for the tree node value in the extra space.
	mov [rsp + valueBuffer], r8

	; Set parameters for findAddress
	mov r8, rcx
	mov rcx, [r8].TreeMap.root

	call findAddressOfKey
	cmp rax, nullptr
	je getValueContainsFailure

	; The treemap will be preserved by findAddress.
	; Only the buffer needs to be restored and the return address
	; must point at the value. Afterwards copy will be called.
	; The remaining status will be set by copy.
	mov r10, r8
	mov rcx, [rsp + valueBuffer]
	add rax, [r10].TreeMap.keySize
	mov rdx, rax
	mov R8B, false
	call [r10].TreeMap.copyValueFunc

	jmp functionReturn

getValueContainsFailure:
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

	jmp functionReturn

valueBufferInvalid:
	mov eax, valueBufferNullptr

functionReturn:
	add rsp, shadowStorage
	ret

getValue endp


	public getKey

; Retrieves the key that is paired together with the provided value if such
; pair exists in the treemap.
;
; @RCX qword[in] - Pointer to the treemap the given key is searched in.
; @RDX qword[in] - Pointer to the value that is paired with the searched key.
; @R8 qword[out] - Pointer to a buffer where the key will be copied into if it has been found.
;
; @return Success, treeMapNullptr, keyBufferNullptr, doesNotContain if the value does not exist
;		  in the map or a errCopyKeyFunc when the copy function of the key fails.
getKey proc

	; Allocate storage for the recursive findAddress function.
	push rbx
	push rsi
	push rdi
	sub rsp, qwordSize

	; Check if the treemap is a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Check if the key buffer is a nullptr.
	cmp r8, nullptr
	je keyBufferInvalid

	; Store the treemap, the searched value and the key buffer in
	; the non volatile registers.
	mov rsi, rcx
	mov rdi, rdx
	mov rbx, r8

	; Set parameters accordingly and save the current root in our space.
	mov rcx, [rsi].TreeMap.root
	call findAddressOfValue

	; Restore the current root and check if the addresses match.
	cmp rax, [rsi].TreeMap.root
	je getKeyContainsFailure

	; If rax is not the current root. We restore the treemap, the buffer
	; and move rax to point at the key.
	mov rcx, rbx
	mov rdx, rax
	sub rdx, [rsi].TreeMap.keySize

	; Call copyKeyFunc. The status value will be set
	; by the copy function.
	sub rsp, 3 * qwordSize
	call [rsi].TreeMap.copyKeyFunc
	add rsp, 3 * qwordSize

	jmp functionReturn

getKeyContainsFailure:
	; Return status that the tree does not contain the key.
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

	jmp functionReturn

keyBufferInvalid:
	mov eax, keyBufferNullptr

functionReturn:
	add rsp, qwordSize
	pop rdi
	pop rsi
	pop rbx
	ret

getKey endp


	public containsValue

; Finds out if the given value exists inside the treemap.
;
; @RCX qword[in] - Pointer to the treemap where the value is searched in.
; @RDX qword[in] - Pointer to the value that is searched inside the treemap.
;
; @return Success, treeMapNullptr or doesNotContain status value.
containsValue proc
	
	; We store the root in rbx.
	; The treemap in rsi.
	; The searched value in rdi.
	push rsi
	push rdi

	; Check if the treemap is not a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Store the treemap and root in the non volatile registers.
	mov rsi, rcx
	mov rdi, rdx

	; Call find address and test if the returned value
	; matches the root or not.
	mov rcx, [rsi].TreeMap.root
	call findAddressOfValue

	; Compare the result with the saved root.
	cmp rax, [rsi].TreeMap.root
	je containsFailure

	; If it does not match we found the value.
	mov eax, success
	jmp functionReturn

containsFailure:
	; If the returned address is the root,
	; we dont have the value.
	mov eax, doesNotContain
	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

functionReturn:
	pop rdi
	pop rsi
	ret

containsValue endp


	public containsKey

; Finds out if the specified key exists in the treemap.
; 
; @RCX qword[in] - Pointer to the treemap where the key is searched in.
; @RDX qword[in] - Pointer to the key that is searched inside the treemap.
;
; @return Success, treeMapNullptr or doesNotContain status value.
containsKey proc

	; Allocate shadow storage for the findAddress function to preserve
	; volatile registers used.
	sub rsp, shadowStorage

	; Check if the treemap is not a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Set r8 to the treemap and rcx to the current root.
	mov r8, rcx
	mov rcx, [rcx].TreeMap.root
	call findAddressOfKey

	; Check if we get an address for the key or nullptr.
	cmp rax, nullptr
	je containsFailure

	; Return success if we have an address.
	mov eax, success
	jmp functionReturn

containsFailure:
	; Return doesNotContain if we don't have an address.
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

functionReturn:
	add rsp, shadowStorage
	ret	

containsKey endp


; Retrieves the address of the given value inside the treemap.
;
; @RCX qword[in] - Pointer to the current tree node that has a value.
; @RSI qword[in] - Pointer to the currently used treemap.
; @RDI qword[in] - Pointer to the address of the value that is searched inside the treemap.
;
; @return Address of the specified value inside the treemap or the address passed into rcx.
findAddressOfValue proc

	; The stack is misaligned when calling this function
	; and needs to stay like that to be consistant with the
	; alignment.
	sub rsp, 2 * qwordSize

	; Test if this node is a nullptr.
	cmp rcx, nullptr
	mov rax, nullptr
	je functionReturn

	; Add keySize to point at the value.
	add rcx, [rsi].TreeMap.keySize

	; Store the address to the value.
	mov [rsp], rcx

	; Add shadow storage for potential abi call.
	sub rsp, shadowStorage
	mov rdx, rdi
    call [rsi].TreeMap.equalsValueFunc
	add rsp, shadowStorage

	; check if the values match and restore the value pointer.
	cmp al, true
	mov rax, [rsp]

	je functionReturn
	jne checkLeftBranch

checkLeftBranch:
	; Move pointer to the left child node and call this function again.
	add rax, [rsi].TreeMap.valueSize
	mov [rsp], rax
	mov rcx, [rax]
	call findAddressOfValue
	
	; Restore rcx and compare with the find return address.
	mov rcx, [rsp]
	cmp rax, [rcx]
	je checkRightBranch
	jne functionReturn

checkRightBranch:
	; Restore the pointer to the left child.
	mov rcx, [rsp]
	
	; Point to the right child, save it and call find for it.
	add rcx, qwordSize
	mov [rsp], rcx
	mov rcx, [rcx]
	call findAddressOfValue

	; Restore old right child pointer and compare it with the result.
	mov rcx, [rsp]
	cmp rax, [rcx]
	jne functionReturn

	; If no match was found revert
	; the adding to point at the start address of the
	; evaluated node.
	mov rax, rcx
	sub rax, qwordSize
	sub rax, [rsi].TreeMap.valueSize
	sub rax, [rsi].TreeMap.keySize

	jmp functionReturn

functionReturn:
	add rsp, 2 * qwordSize
	ret

findAddressOfValue endp

; Retrieves the address of the specified key inside the treemap if it exists.
;
; @RCX qword[in] - Pointer to the current tree node that has a key.
; @RDX qword[in] - Pointer to the address of the key that is searched for inside the treemap.
; @R8 qword[in] - Pointer to the currently used treemap.
;
; @return Address of the specified value inside the treemap or nullptr if it does not exist.
findAddressOfKey proc

compareKeyLoop:
	; Test if the current node is a nullptr.
	cmp rcx, nullptr
	je doesNotContainKey

	; Safe the parameters in the shadow storage given by the caller.
	mov [rsp + currentTreeNode3], rcx
	mov [rsp + searchedKey], rdx
	mov [rsp + treemap2], r8

	; Add another shadow storage in case the user needs abi compliance.
	sub rsp, shadowStorage
	call [r8].TreeMap.compareKeyFunc
	add rsp, shadowStorage

	; Check if we found the key.
	cmp rax, 0
	mov rcx, [rsp + currentTreeNode3]
	je doesContainKey

	; If keys don't match restore params and
	; move rcx to the left child.
	mov rdx, [rsp + searchedKey]
	mov r8, [rsp + treemap2]
	add rcx, [r8].TreeMap.keySize
	add rcx, [r8].TreeMap.valueSize

	; Compare again because add 
	cmp rax, 0
	jg loadRightBranch

loadLeftBranch:
	; Repeat the process with the left child.
	mov rcx, [rcx]

	jmp compareKeyLoop

loadRightBranch:
	; Repeat the process with the right child.
	add rcx, qwordSize
	mov rcx, [rcx]

	jmp compareKeyLoop

doesNotContainKey:
	; If nothing is found return nullptr.
	xor rax, rax
	jmp functionReturn

doesContainKey:
	; If something was found return the address.
	mov rax, rcx

functionReturn:
	ret

findAddressOfKey endp

	public replaceValue

; Replaces the value of a key value pair specified by the given key inside the treemap
; if the key value pair exists.
;
; @RCX qword[in,out] - Pointer to the treemap where the replacement will take place.
; @RDX qword[in] - Pointer to the key that identifies the key value pair where the value will be replaced.
; @R8 qword[in] - Pointer to the value that will replace the current value inside the key value pair.
;
; @return A Status value of either success, doesNotContain, treeMapNullptr or errCopyValueFunc if the replacement fails.
replaceValue proc

	sub rsp, shadowStorage

	; Check if the treemap is a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	mov [rsp + replacementValue], r8

	; Set r8 to the treemap and rcx to the current root.
	mov r10, rcx
	mov rcx, [r10].TreeMap.root
	mov r8, r10
	call findAddressOfKey

	; Check if we get an address for the key or nullptr.
	cmp rax, nullptr
	je replaceContainsFailure

	; Restore the treemap and the new value source pointer.
	; The new value is stored as the source.
	mov r10, [rsp + treemap3]
	mov rdx, [rsp + replacementValue]
	mov rcx, rax
	add rcx, [r10].TreeMap.keySize
	mov r8B, true
	sub rsp, shadowStorage
	call [r10].TreeMap.copyValueFunc
	add rsp, shadowStorage

	jmp functionReturn

replaceContainsFailure:
	; Return false if we got a nullptr or memcpy failed.
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

functionReturn:
	add rsp, shadowStorage
	ret

replaceValue endp

; Creates a deep copy of a treenodes pair. The buffer has to be provided
; and is not created.
;
; @RCX qword[out] - Pointer to the buffer that is filled with the given treenode pair.
; @RDX qword[in] - Pointer to the treenode whose key value pair is deep copied.
; @R8 qword[in] - Pointer to the treemap that is used to access the copy functions.
;
; @returns A status value of success or failure in case the copy functions fail.
copyPair proc

	sub rsp, shadowStorage * 2

	; Save the treemap, the found min/max treenode and
	; the buffer to deep copy it.
	mov [rsp + copyBuffer], rcx
	mov [rsp + copyTreeNodePair], rdx
	mov [rsp + treemap4], r8

	; Copy the key.
	call [r8].TreeMap.copyKeyFunc

	; Return if any error happened.
	cmp eax, success
	jne functionReturn

	; Restore the treemap, the found min/max treenode
	; and the buffer to deep copy it.
	mov rcx, [rsp + copyBuffer]
	mov rdx, [rsp + copyTreeNodePair]
	mov r8, [rsp + treemap4]

	; Copy the value.
	add rcx, [r8].TreeMap.keySize
	add rdx, [r8].TreeMap.keySize
	mov r10, r8
	mov r8B, false
	call [r10].TreeMap.copyValueFunc

functionReturn:
	add rsp, shadowStorage * 2
	ret

copyPair endp

	public ceilingPair

; Retrieves the next higher or the same key value pair for a given key if it exists.
;
; @RCX qword[in] - Pointer to the current treemap that will be used.
; @RDX qword[in] - Pointer to the key that is used to get the ceiling key value pair.
; @R8 qword[out] - Pointer to a buffer that is used to copy the result key value pair into it.
;
; @return A status value for success, doesNotContain if no ceiling pair for the given
;		  key was found or an error if the copy functions failed or the treemap or the pairBuffer
;		  is a nullptr.
ceilingPair proc

	push rbp
	mov rbp, rsp

	mov r9, searchAsCeiling
	call getFloorCeilingPair

	mov rsp, rbp
	pop rbp
	ret

ceilingPair endp

	public floorPair

; Retrieves the next lower or the same key value pair for a given key if it exists.
;
; @RCX qword[in] - Pointer to the current treemap that will be used.
; @RDX qword[in] - Pointer to the key that is used to get the floor key value pair.
; @R8 qword[out] - Pointer to a buffer that is used to copy the result key value pair into it.
;
; @return A status value for success, doesNotContain if no floor pair for the given
;		  key was found or an error if the copy functions failed or the treemap or the pairBuffer
;		  is a nullptr.
floorPair proc

	push rbp
	mov rbp, rsp

	mov r9, searchAsFloor
	call getFloorCeilingPair

	mov rsp, rbp
	pop rbp
	ret

floorPair endp

; Retrieves the ceiling or floor key value pair for a given key if it exists.
;
; @RCX qword[in] - Pointer to the current treemap that will be used.
; @RDX qword[in] - Pointer to the key that is used to get the floor or ceiling key value pair.
; @R8 qword[out] - Pointer to a buffer that is used to copy the result key value pair into it.
; @R9 byte[in] - Flag that indicates if we search as floor or ceiling. False for floor,
;				 true for ceiling.
;
; @return A status value for success, doesNotContain if no floor or ceiling pair for the given
;		  key was found or an error if the copy functions failed or the treemap or the pairBuffer
;		  is a nullptr.
getFloorCeilingPair proc

	; Shadowstorage for the comparison function is always allocated.
	; Additionally storage needs to exist for the current tree node
	; and the last found pair which takes 16 bytes and 8 extra for alignment
	; on a 16 byte boundary.
	sub rsp, shadowStorage + 24
	
	; Check if the treemap is a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Check if the provided pair buffer is a nullptr.
	cmp r8, nullptr
	je pairBufferInvalid

	; Save the comparison key, the treemap and the buffer.
	mov [rbp + searchKey], rdx
	mov [rbp + treemap2], rcx
	mov [rbp + pairBuffer], r8
	
	; Mov the treemap to an unused register
	; and load the root.
	; Store the higher or lower flag
	; and the result => at the start its a nullptr.
	mov r10, rcx
	mov r11, [r10].TreeMap.root
	mov [rbp + ceilingFloorFlag], r9B
	mov qword ptr [rsp + foundPair], nullptr

ceilingFloorLoop:

	cmp r11, nullptr
	je checkIfNodeWasFound

	; Save the current treenode and call the compare function.
	mov [rsp + currentTreeNode2], r11
	mov rcx, r11
	call [r10].TreeMap.compareKeyFunc

	; Restore the flag, the current tree node, the treemap and the comparison key.
	mov r9B, byte ptr [rbp + ceilingFloorFlag]
	mov r11, [rsp + currentTreeNode2]
	mov r10, [rbp + treemap2]
	mov rdx, [rbp + searchKey]

	; Move treenode to point at the left child.
	add r11, [r10].TreeMap.keySize
	add r11, [r10].TreeMap.valueSize

	; Check if we want to search as ceiling or floor.
	cmp r9B, searchAsFloor
	jne fetchCeiling

	; Check if we go left or right.
	cmp al, 0
	jge foundPotentialPair
	jl getNextChild
	
fetchCeiling:
	; Check if we go left or right.
	cmp al, 0
	jle foundPotentialPair
	jg getNextChild

getNextChild:
	; For floor we take the left branch and
	; do another iteration.
	; For ceiling its the same but with the right branch.
	xor rcx, rcx
	mov cl, r9B
	imul cx, qwordSize
	add r11, rcx
	mov r11, [r11]

	jmp ceilingFloorLoop

foundPotentialPair:
	; If we have a potential ceiling or floor
	; candidate save it.
	mov rcx, [rsp + currentTreeNode2]
	mov [rsp + foundPair], rcx

	; Check again if we got the exact key value pair
	; inside the treemap or if it was a potential pair.
	cmp al, 0
	jne continueWithOppositeBranch

	jmp executeCpy

continueWithOppositeBranch:
	; Go left for higher and right for lower.
	; Negate the flag or R9B and add it to the
	; tree node pointer.
	xor rcx, rcx
	mov cl, r9B
	xor cl, 1
	imul cx, qwordSize
	add r11, rcx
	mov r11, [r11]
	
	jmp ceilingFloorLoop

checkIfNodeWasFound:
	; Test if a floor or ceiling tree node was found.
	cmp qword ptr [rsp + foundPair], nullptr
	je floorCeilingPairFailure

executeCpy:
	; Copy the pair into the destination buffer.
	mov rcx, [rbp + pairBuffer]
	mov rdx, [rsp + foundPair]
	mov r8, r10
	call copyPair

	jmp functionReturn

floorCeilingPairFailure:
	; Set the return value to a failure.
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

	jmp functionReturn

pairBufferInvalid:
	mov eax, pairBufferNullptr

functionReturn:
	add rsp, shadowStorage + 24
	ret

getFloorCeilingPair endp

	public higherPair

; Retrieves the next key value pair of the treemap that has a key greater than
; the provided one.
;
; @RCX qword[in] - Pointer to the current treemap that will be used.
; @RDX qword[in] - Pointer to the key that is used to get the next higher key value pair if it exists.
; @R8 qword[out] - Pointer to a buffer to copy the key value pair into.
;
; @return A status value for success, doesNotContain if no lower pair was found, an error
;		  in case the copy functions fail or the treemap or pair buffer is a nullptr.
higherPair proc

	push rbp
	mov rbp, rsp

	mov r9B, searchAsHigher
	call getLowerHigherPair

	mov rsp, rbp
	pop rbp
	ret

higherPair endp

	public lowerPair

; Retrieves the next key value pair of the treemap that has a key less than
; the provided one.
;
; @RCX qword[in] - Pointer to the current treemap that will be used.
; @RDX qword[in] - Pointer to the key that is used to get the next lower key value pair if it exists.
; @R8 qword[out] - Pointer to a buffer to copy the key value pair into.
;
; @return A status value for success, doesNotContain if no lower pair was found, an error
;		  in case the copy functions fail or the treemap or pair buffer is a nullptr.
lowerPair proc

	push rbp
	mov rbp, rsp

	mov r9B, searchAsLower
	call getLowerHigherPair

	mov rsp, rbp
	pop rbp
	ret

lowerPair endp

; Retrieves the higher or lower key value pair by the specified key if it exists.
; The potential lower or higher key value pair is saved on the stack. The same
; applies to the currently evaluated tree node.
;
; @RCX qword[in] - Pointer to the current treemap that will be used.
; @RDX qword[in] - Pointer to the key that is used to get the higher or lower pair if it exists.
; @R8 qword[out] - Pointer to a buffer to copy the key value pair into.
; @R9 byte[in] - Flag that indicates if we want to get a higher or lower pair. False for lower,
;				 true for higher.
;
; @return A status value for success, doesNotContain if no lower or higher pair was found or an error
;		  in case the copy functions fail.
getLowerHigherPair proc

	; Shadowstorage for the comparison function is always allocated.
	; Additionally storage needs to exist for the current tree node
	; and the last found pair which takes 16 bytes and 8 extra for alignment
	; on a 16 byte boundary.
	sub rsp, shadowStorage + 24

	; Check if the treemap is a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Check if the provided pair buffer is a nullptr.
	cmp r8, nullptr
	je pairBufferInvalid

	; Save the comparison key, the treemap and the buffer.
	mov [rbp + searchKey], rdx
	mov [rbp + treemap2], rcx
	mov [rbp + pairBuffer], r8

	; Mov the treemap to an unused register
	; and load the root.
	; Store the higher or lower flag
	; and the result => at the start its a nullptr.
	mov r10, rcx
	mov r11, [r10].TreeMap.root
	mov [rbp + higherLowerFlag], r9B
	mov qword ptr [rsp + foundPair], nullptr

lowerHigherLoop:
	; check if our current treenode is a nullptr.
	cmp r11, nullptr
	je checkIfNodeWasFound

	; Save the current treenode and call the compare function.
	mov [rsp + currentTreeNode2], r11
	mov rcx, r11
	call [r10].TreeMap.compareKeyFunc

	; Restore the flag, the current tree node, the treemap and the comparison key.
	mov r9B, byte ptr [rbp + higherLowerFlag]
	mov r11, [rsp + currentTreeNode2]
	mov r10, [rbp + treemap2]
	mov rdx, [rbp + searchKey]

	; Move treenode to point at the left child.
	add r11, [r10].TreeMap.keySize
	add r11, [r10].TreeMap.valueSize

	; Check if we want to search as higher or lower.
	cmp r9B, searchAsLower
	jne fetchHigher

	; Check if we go left or right.
	cmp al, 0
	jg foundPotentialHigherLowerBranch
	jle getNextChild

fetchHigher:
	; Check if we go left or right.
	cmp al, 0
	jl foundPotentialHigherLowerBranch
	jge getNextChild

getNextChild:
	; For lower we take the left branch and
	; do another iteration.
	; For higher its the same but with the right branch.
	xor rcx, rcx
	mov cl, r9B
	imul cx, qwordSize
	add r11, rcx
	mov r11, [r11]
	jmp lowerHigherLoop

foundPotentialHigherLowerBranch:
	; If we have a potential higher or lower
	; candidate save it.
	mov rcx, [rsp + currentTreeNode2]
	mov [rsp + foundPair], rcx

	; Go left for higher and right for lower.
	; Negate the flag or R9B and add it to the
	; tree node pointer.
	xor rcx, rcx
	mov cl, r9B
	xor cl, 1
	imul cx, qwordSize
	add r11, rcx
	mov r11, [r11]
	
	jmp lowerHigherLoop

checkIfNodeWasFound:
	; Test if a lower or higher tree node was found.
	cmp qword ptr [rsp + foundPair], nullptr
	je lowerHigherPairFailure

	; Copy the pair into the destination buffer.
	mov rcx, [rbp + pairBuffer]
	mov rdx, [rsp + foundPair]
	mov r8, r10
	call copyPair

	jmp functionReturn

lowerHigherPairFailure:
	; Set the return value to a failure.
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

	jmp functionReturn

pairBufferInvalid:
	mov eax, pairBufferNullptr

functionReturn:
	add rsp, shadowStorage + 24
	ret

getLowerHigherPair endp

	public minPair

; Retrieves the smallest pair the tree holds or nothing if the tree is empty.
;
; @RCX qword[in] - Pointer to the treemap the smallest pair shall be extracted.
; @RDX qword[out] - Pointer to a buffer where a deep copy of the min pair is stored.
;
; @return Status value of success, failure of the copy functions or if the treemap or
;		  or pair buffer is a nullptr.
minPair proc

	xor r11, r11
	call lastPair

	ret

minPair endp

	public maxPair

; Retrieves the biggest pair of the tree or nothing if the tree is empty.
;
; @RCX qword[in] - Pointer to the treemap the biggest pair shall be extracted.
; @RDX qword[out] - Pointer to a buffer where a deep copy of the max pair is stored.
;
; @return Status value of success, failure of the copy functions or if the treemap or
;		  or pair buffer is a nullptr.
maxPair proc

	mov r11, qwordSize
	call lastPair

	ret

maxPair endp

; Retrieves the biggest or smallest pair of the tree or nothing if the tree is empty.
;
; @RCX qword[in] - Pointer to the treemap the biggest or smallest pair shall be extracted.
; @RDX qword[out] - Pointer to a buffer where a deep copy of the pair is stored.
; @R11 byte[in] - Offset that decides wether we take the left or right branches.
;
; @return A status that indicates if the function was successful, doesNotContain in case
; a min/max pair does not exist, an error if the copy functions failed or
; the treemap or pair buffer is a nullptr.
lastPair proc

	; Check if the treemap is a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Check if the provided pair buffer is a nullptr.
	cmp rdx, nullptr
	je pairBufferInvalid

	; Save the treemap in an unused register.
	; Get the rootnode and check if it not a nullptr.
	mov r10, rcx
	mov r8, [r10].TreeMap.root
	cmp r8, nullptr
	je hasNoPair

iterateBranches:
	; Iterate over the branches and go left or right
	; depending on the value of r11 until we found
	; a min or max.
	mov r9, r8
	add r8, [r10].TreeMap.keySize
	add r8, [r10].TreeMap.valueSize
	add r8, r11
	mov r8, [r8]
	cmp r8, nullptr
	jne iterateBranches

	mov r8, r10
	mov r11, rdx
	mov rdx, r9
	mov rcx, r11
	call copyPair

	jmp functionReturn

hasNoPair:
	; If no min pair is found return failure.
	mov eax, doesNotContain

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

	jmp functionReturn

pairBufferInvalid:
	mov eax, pairBufferNullptr

functionReturn:
	ret

lastPair endp

end