; @file tree_map_base.asm
;	
; Defines the base functionality of a treemap that allows
; the creation/deletion of the treemap and insertion/deletion of key value pairs.
;
; @author Collector
; @date 03/09/2023

	include tree_map.inc

	.code


	public createTreeMap

; Allocates a treemap on the heap.
;
; @RCX qword[in] - Holds the size of the key.
; @RDX byte[in] - Holds the size of the value.
; @R8 qword[in] - Holds a pointer to the function that compares treenodes by their keys.
; @R9 qword[in] - Holds a pointer to the function that compares values for equality.
; @Stack qword[in] - Holds a pointer to the function that deep copies given treenode keys.
; @Stack qword[in] - Holds a pointer to the function that deep copies given treenode values.
; @Stack qword[in] - Holds a pointer to the function that is used to clear nested heap memory from pairs.
; @Stack qword[out] - Holds a pointer to a status code which is set to success if the treemap has been allocated
;					  and set to errTreeMapAllocation on failure.
;
; @return A treemap struct. Also a status flag is set inside the given stack memory where the pointer to
; it is located. The function throws several status errors if for example the key size is 0 or
; the key compare function is a nullptr. If no error is encountered the status will be set to success.
createTreeMap proc

	push rbp
	mov rbp, rsp
	sub rsp, shadowStorage

	mov rax, nullptr

	; Check if a status flag pointer was given, otherwise fail silently.
	cmp qword ptr [rbp + statusFlagPtr], nullptr
	je functionReturn

	; Check if the key size is not zero.
	cmp rcx, 0
	je keySizeInvalid

	; Check if the value size is not zero.
	cmp rdx, 0
	je valueSizeInvalid

	; Check if the key comparing function is a nullptr.
	cmp r8, nullptr
	je keyCompFuncInvalid

	; Check if the value equality function is a nullptr.
	cmp r9, nullptr
	je valueEqualFuncInvalid

	; Check if the key copy function is a nullptr.
	cmp qword ptr [rbp + keyCopyFunc], nullptr
	je keyCopyFuncInvalid

	; Check if the value copy function is a nullptr.
	cmp qword ptr [rbp + valueCopyFunc], nullptr
	je valueCopyFuncInvalid

	; Save the keySize, the valueSize, the keyCompareFunc and the valueEqualFunc.
	mov [rbp + keySize], rcx
	mov [rbp + valueSize], rdx
	mov [rbp + keyCompFunc], r8
	mov [rbp + valueEqualFunc], r9

	; Allocate the new treemap.
	mov rcx, sizeof TreeMap
	call malloc

	; Check if malloc was successful.
	cmp rax, nullptr
	je heapAllocationError

	; Set the root to nullptr and the nodeAmount to zero.
	mov qword ptr [rax], nullptr
	mov qword ptr [rax + qwordSize], 0

	; Start setting the stack parameters offset and iterate over them.
	mov r10, parameterStackStart
	jmp fetchAndStoreParams

fetchAndStoreParams:
	; Get the parameters in order and move them into
	; the treemap structure.
	mov rcx, [rbp + r10]
	mov [rax + r10], rcx

	; Add to the offset until we reached the stack limit.
	add r10, qwordSize
	cmp r10, parameterStackLimit
	jle fetchAndStoreParams

	mov edx, success
	jmp setStatus

heapAllocationError:
	mov edx, errHeapAllocation

	jmp setStatus

keySizeInvalid:
	mov edx, keySizeZero

	jmp setStatus

valueSizeInvalid:
	mov edx, valueSizeZero

	jmp setStatus

keyCompFuncInvalid:
	mov edx, keyCompFuncNullptr
	
	jmp setStatus

valueEqualFuncInvalid:
	mov edx, valueEqualFuncNullptr
	
	jmp setStatus

keyCopyFuncInvalid:
	mov edx, keyCopyFuncNullptr

	jmp setStatus

valueCopyFuncInvalid:
	mov edx, valueCopyFuncNullptr

setStatus:
	; Sets the returned status value. It's the last stack parameter meaning
	; it's the furthest away from rbp.
	mov rcx, [rbp + statusFlagPtr]
	mov dword ptr [rcx], edx

functionReturn:
	mov rsp, rbp
	pop rbp
	ret

createTreeMap endp


	public deleteTreeMap

; Deletes the specified treemap freeing all nodes allocated inside of it
; and finally freeing the treemap structure itself.
;
; @RCX qword[in,out] - Pointer to the treemap that should be deleted.
;
; @return Status flag of a success or that the specified treemap is a nullptr.
deleteTreeMap proc

	; sub 16 bytes so that the stack is still aligned after calling
	; clearTreeMap.
	sub rsp, 2 * qwordSize

	; Save the treemap and clear the nodes.
	mov [rsp], rcx
	call clearTreeMap

	; Check if everything was successful.
	cmp eax, success
	jne functionReturn

	; Free the treemap.
	mov rcx, [rsp]
	call free

functionReturn:
	add rsp, 2 * qwordSize
	ret

deleteTreeMap endp


	public clearTreeMap

; Resets a treemap so that it's root is back to a nullptr and
; the count will be at 0. All nodes will be freed separately.
;
; @RCX qword[in,out] - Pointer to the treemap that will be cleared.
;
; @return Status flag of a success or that the specified treemap is a nullptr.
clearTreeMap proc
	
	push rsi

	; Check if the treemap is a nullptr.
	mov eax, treeMapNullptr
	cmp rcx, nullptr
	je functionReturn

	; Free the tree nodes.
	mov rsi, rcx
	mov rcx, [rsi].TreeMap.root
	call freeTreeNodes

	; Set the root to a nullptr.
	mov [rsi].TreeMap.root, nullptr
	mov eax, success

functionReturn:
	pop rsi
	ret

clearTreeMap endp


; Frees the tree structure of the treemap completely.
; Decrements nodeAmount for every freed treenode structure.
;
; @RCX qword[in,out] - Pointer to the current tree node.
; @RSI qword[in] - Pointer to the current treemap used.
freeTreeNodes proc

	; sub 16 bytes from the stack to keep beeing missaligned all
	; the time and align on a 16 byte boundary before a free call.
	sub rsp, 2 * qwordSize

	cmp rcx, nullptr
	je functionReturn

	; Save the current treenode on the stack.
	mov [rsp], rcx
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize

	; Call freeTreeNodes for the left child.
	mov rcx, [rcx]
	call freeTreeNodes

	; Get the right child and set the left one
	; to a nullptr because it was freed earlier.
	mov rcx, [rsp]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize
	
	; Call freeTreeNodes for the right child.
	mov rcx, [rcx]
	call freeTreeNodes

	; Align the stack and take memory away for the shadow space.
	mov rcx, [rsp]
	sub rsp, shadowStorage + qwordSize

	; If we have a free pair function call it for the node.
	cmp [rsi].TreeMap.freePairFunc, nullptr
	je freeNode

	call [rsi].TreeMap.freePairFunc

freeNode:
	; Free the current tree nodes heap memory.
	mov rcx, [rsp + shadowStorage + qwordSize]
	call free

	add rsp, shadowStorage + qwordSize
	dec [rsi].TreeMap.nodeAmount

functionReturn:
	add rsp, 2 * qwordSize
	ret

freeTreeNodes endp


	public putPair

; Inserts a treenode into a treemap of the specified key not already exists inside the map.
;
; @RCX qword[in,out] - Pointer to the treemap the node shall be inserted in.
; @RDX qword[in] - The key value pair that shall be inserted.
;
; @return Status value for success, alreadyContains, memory allocation, copy function failure
; or nullptr errors e.g. when the treemap passed is a nullptr.
putPair proc

	push rsi
	push rdi
	sub rsp, shadowStorage + qwordSize

	; Check if the given treemap is not a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Check if the given pair is not a nullptr.
	cmp rdx, nullptr
	je treeNodePairInvalid

	; Save the treemap and set the success status value.
	mov rsi, rcx
	mov edi, success

	; Add the current treemap on the stack and call insertPair.
	; Also store the address to the status flag as a parameter.
	mov rcx, [rsi].TreeMap.root
	call insertPair

	; Update the root and turn is back to black.
	mov [rsi], rax

	add rax, [rsi].TreeMap.keySize
	add rax, [rsi].TreeMap.valueSize
	add rax, 2 * qwordSize
	mov byte ptr [rax], false

	mov eax, edi

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

	jmp functionReturn

treeNodePairInvalid:
	mov eax, treeNodePairNullptr

functionReturn:
	add rsp, shadowStorage + qwordSize
	pop rdi
	pop rsi
	ret

putPair endp


; Recursively inserts a key value pair into the redblack tree map.
;
; @RCX qword[in,out] - Treemap root or the current node of the treemap.
; @RDX qword[in] - Pointer to the key value pair thats inserted into the treenode.
; @RSI qword[in,out] - A pointer to the current treemap.
; @RDI dword[out] - The function gets the code with a success value and will
;				   on failure turn it into a alreadyContains, errHeapAllocation or copy function value.
;
; @return The currently modified treenode.
insertPair proc
	
	push rbp
	mov rbp, rsp
	sub rsp, shadowStorage

	; Saving fastcall registers in the shadow storage automatically provided
	; before calling any other function.
	mov [rbp + currentTreeNode], rcx
	mov [rbp + toInsertValuePair], rdx

	; Check if the current treenode is a nullptr.
	cmp rcx, nullptr
	je createTreeNode

	; Compare the given key with the treenode currently selected.
	; rcx holds the current treenode and rdx the pointer to the key to insert.
	call [rsi].TreeMap.compareKeyFunc

	; Preload the current treemap and the node before checking
	; the comparison result.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize

	; Compare the result of the compare key function
	; If both keys are equal return immediately.
	cmp al, 0
	je containsTreeNode

	mov rdx, [rbp + toInsertValuePair]

	; Save the address of the current tree node that points to the left
	; child node.
	mov [rbp + leftTreeNode], rcx
	
	jg continueRight

continueLeft:
	; Dereference to the left child node and call insert with the restored parameters.
	mov rcx, [rcx]
	call insertPair

	; Restore the old pointer to the left child
	; dereference it and store the recursive call result.
	mov rcx, [rbp + leftTreeNode]
	mov [rcx], rax

	jmp changeTreeChildAndFixTree

continueRight:
	; Save the right child pointer, dereference it and call insertPair.
	add rcx, qwordSize
	mov [rbp + rightTreeNode], rcx
	mov rcx, [rcx]
	call insertPair

	; Restore the old pointer to the right child and dereference it
	; to store the recursive call result.
	mov rcx, [rbp + rightTreeNode]
	mov [rcx], rax

changeTreeChildAndFixTree:
	call balance
	jmp returnRax

containsTreeNode:
	mov edi, dword ptr alreadyContains

returnRax:
	mov rax, [rbp + currentTreeNode]

	jmp functionReturn

createTreeNode:
	 ; Adding sizes together for malloc.
	 xor rcx, rcx
	 add rcx, [rsi].TreeMap.keySize
	 add rcx, [rsi].TreeMap.valueSize
	 add rcx, sizeof TreeNode

	 ; Reserve heap memory for a new TreeNode.
	 call malloc
	 cmp rax, nullptr
	 je handleAllocationError

	 ; Replace nullptr with new treenode.
	 mov [rbp + currentTreeNode], rax

	 ; Initialise the key of the node 
	 mov rcx, rax
	 mov rdx, [rbp + toInsertValuePair]
	 call [rsi].TreeMap.copyKeyFunc

	 ; Compare copy key function result for success.
	 cmp eax, success
	 jne handleCopyKeyError

	 ; Initialise the value of the node.
	 mov rcx, [rbp + currentTreeNode]
	 mov rdx, [rbp + toInsertValuePair]
	 add rcx, [rsi].TreeMap.keySize
	 add rdx, [rsi].TreeMap.keySize
	 mov r8B, false
	 call [rsi].TreeMap.copyValueFunc

	 ; Compare copy value function result for success.
	 cmp eax, success
	 jne handleCopyValueError

	 ; Mov the address of the treenode to the child pointers.
	 mov rcx, [rbp + currentTreeNode]
	 add rcx, [rsi].TreeMap.keySize
	 add rcx, [rsi].TreeMap.valueSize

	 ; Initialise left child pointer to 0.
	 mov qword ptr [rcx], nullptr
	 add rcx, qwordSize

	 ; Initialise right child pointer to 0
	 mov qword ptr [rcx], nullptr
	 add rcx, qwordSize

	 ; Set node color to red
	 mov byte ptr [rcx], true

	 ; Increase nodeAmount and return the created node.
	 inc [rsi].TreeMap.nodeAmount
	 mov rax, [rbp + currentTreeNode]

	 jmp functionReturn

handleAllocationError:
	mov edi, errHeapAllocation

	jmp functionReturn

handleCopyKeyError:
	mov edi, errCopyKeyFunc

	jmp freeTreeNode

handleCopyValueError:
	mov edi, errCopyValueFunc

	jmp freeTreeNode

freeTreeNode:
	mov rax, [rbp + currentTreeNode]
	call free
	mov rax, nullptr

functionReturn:
	 mov rsp, rbp
	 pop rbp
	 ret

insertPair endp


; Balances a redblack tree after an insertion/deletion has been done.
; The function does the following tests/fixes in order:
; 1. Left rotation.
; 2. Right rotation.
; 3. Flipping of the calling treenodes color and it's children with the flag set to true for insertion.
;
; The function assumes a previous shadowstorage from another function call
; and a rbp register that is used to index this storage with a base offset of 16 bytes.
; 
; @RSI qword[in] - Pointer to the treemap whose tree is balanced.
; @Stack qword[in,out] - Pointer to the currently evaluated treenode.
; @Stack qword[in,out] - Pointer to the left child of the treenode.
; @Stack qword[in,out] - Pointer to the right child of the treenode.
; 
; @returns Nothing, but holds the treenode in @RCX, the treemap in @RDX, the left child node in @R8
;		   and the right child node in @R9 when this function call is finished.
balance proc

	; In case we went left, restore rcx
	; and save the address of the left child node.
	mov rcx, [rbp + leftTreeNode]
	mov rax, [rcx]
	mov [rbp + leftTreeNode], rax

	; Do the same for the right child node.
	add rcx, qwordSize
	mov rcx, [rcx]
	mov [rbp + rightTreeNode], rcx

	; Fetch the right child node and
	; check if it is red.
	call isRed

	; Compare the functions result.
	cmp al, true
	jne testRightRotation

	; Fetch the left child node
	; and check if it is red.
	mov rcx, [rbp + leftTreeNode]
	call isRed

	; Test the functions result.
	cmp al, false
	jne testRightRotation

	; Call rotateleft.
	mov rcx, [rbp + rightTreeNode]
	mov r8, [rbp + currentTreeNode]
	call rotateLeft

	; Save new rotated tree node into the shadow storage.
	; Provide the address of the pointer to the left and right child.
	mov [rbp + currentTreeNode], rax
	add rax, [rsi].TreeMap.keySize
	add rax, [rsi].TreeMap.valueSize
	mov rcx, [rax]
	mov [rbp + leftTreeNode], rcx
	add rax, qwordSize
	mov rcx, [rax]
	mov [rbp + rightTreeNode], rcx

testRightRotation:
	; Get address of pointer to the left child.
	mov rcx, [rbp + leftTreeNode]
	call isRed

	; Test if the left child is red.
	cmp al, true
	jne testFlip

	; Reload left nodes address of the pointer.
	mov rcx, [rbp + leftTreeNode]
	
	; Get the the left node of the left child.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	call isRed

	; Check if the node is also red.
	cmp al, true
	jne testFlip

	; Load the left child node pointer address
	; and the current node to evaluate.
	mov rcx, [rbp + leftTreeNode]
	mov r8, [rbp + currentTreeNode]
	call rotateRight

	; Save the new tree node and restore
	; left and right children for the testflip phase.
	mov [rbp + currentTreeNode], rax
	add rax, [rsi].TreeMap.keySize
	add rax, [rsi].TreeMap.valueSize
	mov rcx, [rax]
	mov [rbp + leftTreeNode], rcx
	add rax, qwordSize
	mov rcx, [rax]
	mov [rbp + rightTreeNode], rcx

testFlip:
	; Restore the left child and test if it's red.
	mov rcx, [rbp + leftTreeNode]
	call isRed

	cmp al, true
	jne functionReturn
	
	; Do the same for the right child.
	mov rcx, [rbp + rightTreeNode]
	call isRed

	cmp al, true
	jne functionReturn

	; Flip the colors for insertion.
	mov rcx, [rbp + currentTreeNode]
	mov r8, [rbp + leftTreeNode]
	mov r9, [rbp + rightTreeNode]
	mov r10B, true
	call flip

functionReturn:
	ret

balance endp


; Rotates a tree structure to the left.
; This will happen when a new node is inserted at the right
; because the tree only permits left leaning red nodes.
;
; @RCX qword[in,out] - Pointer to the right child of the current node evaluated.
; @RSI qword[in] - Pointer to the current treemap structure.
; @R8 qword[in] - Pointer to the current treenode.
;
; @return The rotated tree node, meaning the right nodes address of rdx.
rotateLeft proc

	; Save the right child of the current tree node into rax as ret.
	; Save the current tree node evaluated into r10.
	mov rax, rcx
	mov r10, r8

	; Mov rcx to point at the left child pointer.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize

	; Mov r10 to point at the right child pointer.
	add r10, [rsi].TreeMap.keySize
	add r10, [rsi].TreeMap.valueSize
	add r10, qwordSize

	; Replace the current tree nodes right child with
	; the left one that was saved before in rax.
	mov rdx, [rcx]
	mov [r10], rdx

	; Replace the left node of the tree saved in rax with
	; the current tree node evaluated.
	mov [rcx], r8

	; Move rcx to the red flag.
	add rcx, 2 * qwordSize

	; Mov r10 to the red flag.
	add r10, qwordSize

	; Set the tree node of rax red flag to the currently evaluated
	; tree node.
	mov dl, [r10]
	mov [rcx], dl

	; Set the currently evaluated tree nodes color to red.
	mov byte ptr [r10], true

	ret

rotateLeft endp


; Rotates a tree structure to the right.
; Will be executed when the left node and the left node of the left node
; of the currently evaluated node are both red.
;
; @RCX qword[in,out] - Address of the pointer to the left child of the current treenode evaluated.
; @RSI qword[in] - Pointer to the current treemap structure.
; @R8 qword[in] - Pointer to the current treenode.
;
; @return The rotated tree node, meaning the left nodes address of rdx.
rotateRight proc

	; Save the left child of the currently evaluated tree node
	; as the return value + store the current tree node.
	mov rax, rcx
	mov r10, r8
	
	; Move rcx to point at the right child node.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize

	; Move r10 to point at the left child node.
	add r10, [rsi].TreeMap.keySize
	add r10, [rsi].TreeMap.valueSize

	; The left child of the currently evaluated node get
	; the right child of the rax tree node assigned.
	mov rdx, [rcx]
	mov [r10], rdx

	; The tree node in rax gets the evaluated tree node
	; as its right child.
	mov [rcx], r8

	; Move rcx to point at the red flag.
	add rcx, qwordSize

	; Move r10 to point at the red flag.
	add r10, 2 * qwordSize

	; Store the red flag of the current tree node into
	; the one returned in rax.
	mov dl, [r10]
	mov [rcx], dl

	; Set the color of the tree node evaluated to red.
	mov byte ptr [r10], true

	ret

rotateRight endp


; Flips the colors of the children and the calling node.
; For insertion the calling node is set to red and the children to black.
; For deletion the calling node is set to black and the children to red.
;
; @RCX qword[out] - Pointer to the currently evaluated node.
; @RSI qword[in] - Pointer to the corresponding treemap.
; @R8 qword[out] - Pointer to the left child.
; @R9 qword[out] - Pointer to the right child.
; @R10 byte[in] - Flag that decides how the flip function changes the colors.
;				  True changes the children to black and the calling node to red (insertion).
;				  False changes the children to red and the calling node to black (deletion).
flip proc
	
	; Offset the current tree node address to access its red flag.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, 2 * qwordSize
	mov byte ptr [rcx], r10B

	; Negate the color flag.
	xor r10B, true

	; Offset the left child address to access its red flag.
	add r8, [rsi].TreeMap.keySize
	add r8, [rsi].TreeMap.valueSize
	add r8, 2 * qwordSize
	mov byte ptr [r8], r10B

	; Offset the right child address to access its red flag.
	add r9, [rsi].TreeMap.keySize
	add r9, [rsi].TreeMap.valueSize
	add r9, 2 * qwordSize
	mov byte ptr [r9], r10B

	ret

flip endp


; Check whether the given treenode is marked as red.
;
; @RCX qword[in] - Pointer to a treenode.
; @RSI qword[in] - Pointer to the treemap.
;
; return Boolean that indicates if the current node is red.
isRed proc

	mov al, false

	; Check if the treenode is a nullptr.
	cmp rcx, nullptr
	je functionReturn
	
	; Move to the red flag and test if it is red.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, 2 * qwordSize

	mov al, byte ptr [rcx]
	jmp functionReturn

functionReturn:
	ret

isRed endp


	public deletePair

; Removes the key value pair inside the specified treemap that matches the given key.
;
; @RCX qword[in,out] - Pointer to the treemap that gets a specific pair deleted.
; @RDX qword[in] - Pointer to the key that is used to identify the pair to delete.
; @R8 qword[in] - Pointer to a buffer that is used to store the deleted pair.
;
; @returns A status flag of success, doesNotContain if the key value pair is not found or
;		   a treeMapNullptr error if the given treemap is a nullptr.
deletePair proc

	push rbp
	mov rbp, rsp
	
	; Check if the treemap is not a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	; Allocate stack memory as a temporary copy space
	; for the deletion later on.
	sub rsp, [rcx].TreeMap.keySize
	sub rsp, [rcx].TreeMap.valueSize
	and rsp, -16

	lea r9, delete
	call executeDelete

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

functionReturn:
	mov rsp, rbp
	pop rbp
	ret

deletePair endp


	public pollFirstPair

; Deletes the minimum key value pair of the given treemap.
;
; @RCX qword[in,out] - Pointer to the treemap whose min pair will be deleted.
; @RDX qword[out] - Pointer to a buffer of a treenode thats used to copy the deleted min pair.
;
; @returns A status flag of success, doesNotContain if no min key value pair exists or
; a treeMapNullptr error if the given treemap is a nullptr.
pollFirstPair proc

	push rbp
	mov rbp, rsp

	; Check if the treemap is not a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	mov r8, rdx
	lea r9, deleteMin
	call executeDelete

	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

functionReturn:
	pop rbp
	ret

pollFirstPair endp


	public pollLastPair

; Deletes the maximum key value pair of the given treemap.
;
; @RCX qword[in,out] - Pointer to the treemap whose max pair will be deleted.
; @RDX qword[out] - Pointer to a buffer of a treenode thats used to copy the deleted max pair.
;
; @returns A status flag of success, doesNotContain if no max key value pair exists or
; a treeMapNullptr error if the given treemap is a nullptr.
pollLastPair proc

	push rbp
	mov rbp, rsp

	; Check if the treemap is not a nullptr.
	cmp rcx, nullptr
	je treeMapInvalid

	mov r8, rdx
	lea r9, deleteMax
	call executeDelete
	
	jmp functionReturn

treeMapInvalid:
	mov eax, treeMapNullptr

functionReturn:
	pop rbp
	ret

pollLastPair endp


; Executes the given delete method and it's pre and endphase.
;
; @RCX qword[in,out] - Pointer to the treemap that is used for the deletion.
; @RDX qword[in] - Pointer to the key that is used for deletion if its used.
; @R8 qword[out] - Pointer to a buffer that is used to store a deleted min/max pair.
; @R9 qword[in] - Pointer to the deletion function that is used.
;
; @returns A status flag of success or doesNotContain if the given function in R9 couldn't find
; a matching key value pair.
executeDelete proc

	push r13

	; Store the allocated memory of the deletePair function into r13
	; or random stuff if it is not called by deletePair. Will only be used when deletePair
	; is used.
	mov r13, rsp
	add r13, 2 * qwordSize

	push rbx
	push rsi
	push rdi
	push r12

	; Store the buffer, the treemap and the error
	; inside non volatile registers.
	mov rbx, r8
	mov rsi, rcx
	mov edi, doesNotContain
	mov r12, rdx

	; Get the left child of the root node
	; and test if it is black.
	mov r11, [rsi].TreeMap.root
	cmp r11, nullptr
	je functionReturn

	add r11, [rsi].TreeMap.keySize
	add r11, [rsi].TreeMap.valueSize
	mov rcx, [r11]
	call isRed

	cmp al, false
	jne deletion

	; Test the right node for blackness.
	add r11, qwordSize
	mov rcx, [r11]
	call isRed

	cmp al, false
	jne deletion

	; Turn the root red.
	add r11, qwordSize
	mov byte ptr [r11], true

deletion:
	; Set rcx to the root.
	mov rcx, [rsi].TreeMap.root
	call r9

	; Change the root to the returned one.
	; Also test if we need to mark the root node black.
	mov [rsi], rax
	mov rdx, [rsi].TreeMap.nodeAmount

	cmp rdx, 0
	je functionReturn

	add rax, [rsi].TreeMap.keySize
	add rax, [rsi].TreeMap.valueSize
	add rax, 2 * qwordSize
	mov byte ptr [rax], false

functionReturn:
	mov eax, edi
	pop r12
	pop rdi
	pop rsi
	pop rbx
	pop r13
	ret

executeDelete endp


; Deletes the maximum node inside the given treemap recursively.
;
; @RBX qword[out] - Pointer to the buffer that stores the deleted min pair.
; @RCX qword[in,out] - Pointer to the current tree node evaluated.
; @RSI qword[in,out] - Pointer to the current treemap used.
; @RDI dword[out] - Status value that is set to doesNotContain and changes
;					when a max node has been deleted.
deleteMax proc

	push rbp
	mov rbp, rsp
	sub rsp, shadowStorage

	; Store the current tree node.
	mov [rbp + currentTreeNode], rcx

	; Move the current treenode to the left and
	; test if it is red.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	mov [rbp + leftTreeNode], rcx
	call isRed

	cmp al, true
	jne maxFoundCheck

	; Restore the current and the left treenode 
	; and do the right rotation.
	mov rcx, [rbp + leftTreeNode]
	mov r8, [rbp + currentTreeNode]
	call rotateRight

	; Save the new tree and go further down the right
	; branch. Other tests will fail because of the right rotation
	; a right child is defined and is also red.
	mov [rbp + currentTreeNode], rax
	jmp deleteRightBranch

maxFoundCheck:
	; Move the current treenode to the
	; right child and check if it is a nullptr.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize

	mov rdx, [rcx]
	cmp rdx, nullptr
	jne executeMoveRedRight

	; Check if a buffer to copy was given.
	cmp rbx, nullptr
	jne shallowCopy

	; If no buffer is given test if
	; a freePair function is defined to clear
	; nested heap memory.
	cmp [rsi].TreeMap.freePairFunc, nullptr
	je freeNode

	; Clear nested heap memory.
	mov rcx, [rbp + currentTreeNode]
	call [rsi].TreeMap.freePairFunc

	jmp freeNode

shallowCopy:
	; If the right child is a nullptr, shallow
	; copy the node that will be deleted.
	mov rcx, rbx
	mov rdx, [rbp + currentTreeNode]
	xor r8, r8
	add r8, [rsi].TreeMap.keySize
	add r8, [rsi].TreeMap.valueSize
	call memcpy

freeNode:
	; Free the treenode.
	mov rcx, [rbp + currentTreeNode]
	call free
	 
	; Return nullptr and a success.
	mov rax, nullptr
	dec [rsi].TreeMap.nodeAmount
	mov edi, success

	jmp functionReturn

executeMoveRedRight:
	; Call moveRedRight with the current treenode.
	mov rcx, [rbp + currentTreeNode]
	call moveRedRight

deleteRightBranch:
	; Go further down the right branch and
	; call deleteMax for it.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize

	mov [rbp + rightTreeNode], rcx
	mov rcx, [rcx]
	call deleteMax

	; Update the right branch.
	mov rcx, [rbp + rightTreeNode]
	mov [rcx], rax

	; Set the left child correctly in the stack.
	; Current and right are up to date so nothing is needed
	; to be done. Rebalance the tree.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov [rbp + leftTreeNode], rcx
	call balance

returnRax:
	mov rax, [rbp + currentTreeNode]

functionReturn:
	mov rsp, rbp
	pop rbp
	ret

deleteMax endp


; Flips and rotates the currently evaluated treenode to the right if
; the right side has no red nodes and the left side has no double red nodes
; to force a right rotation. The function essentially created 3 nodes on the right
; side so that a deletion is possible.
; 
; This function assumes a shadowstorage for the current function at rbp + 16.
;
; @RCX qword[in,out] - Pointer to the current tree node evaluated.
; @RSI qword[in,out] - Pointer to the current treemap used.
; @Stack qword[in,out] - Pointer to the currently evaluated tree node.
; @Stack qword[in,out] - Pointer to the left child of the tree node.
; @Stack qword[in,out] - Pointer to the right child of the tree node.
;
; @returns The flipped and rotated tree node inside the stack memory for the
;		   currently evaluated tree node.
moveRedRight proc

	; Get the left and the right child
	; and save them in their stack memory.
	; Test if the right child is black.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov [rbp + leftTreeNode], rcx
	add rcx, qwordSize
	mov rcx, [rcx]
	mov [rbp + rightTreeNode], rcx
	call isRed

	cmp al, false
	jne functionReturn

	; Get the left child of the already tested right treenode.
	; Test if it is also black.
	mov rcx, [rbp + rightTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	call isRed

	cmp al, false
	jne functionReturn

	; Set the parameters for the flip function
	; that will turn the current tree node to black
	; and its children red.
	mov rcx, [rbp + currentTreeNode]
	mov r8, [rbp + leftTreeNode]
	mov r8, [r8]
	mov [rbp + leftTreeNode], r8
	mov r9, [rbp + rightTreeNode]
	mov r10B, false
	call flip

	; Test if the left child of the left node is black.
	mov rcx, [rbp + leftTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	call isRed

	cmp al, false
	jne functionReturn

	; Do a right rotation.
	mov rcx, [rbp + leftTreeNode]
	mov r8, [rbp + currentTreeNode]
	call rotateRight

	mov [rbp + currentTreeNode], rax

functionReturn:
	ret

moveRedRight endp

; Deletes the minimum node inside the given treemap recursively.
;
; @RBX qword[out] - Pointer to the buffer that stores the deleted min pair.
; @RCX qword[in,out] - Pointer to the current tree node evaluated.
; @RSI qword[in,out] - Pointer to the current treemap used.
; @RDI dword[out] - Status value that is set to doesNotContain and changes to success
;					when a min was deleted.
deleteMin proc

	push rbp
	mov rbp, rsp
	sub rsp, shadowStorage

	; Store the currentTreeNode
	; and move the pointer of the treenode to the left child.
	mov [rbp + currentTreeNode], rcx
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize

	; Check if the left node is a nullptr.
	; If it is prepare for copy function.
	cmp qword ptr [rcx], nullptr
	jne executeMoveRedLeft

	; Check if a buffer was provided.
	cmp rbx, nullptr
	jne shallowCopy

	; If not check if we have nested heap memory.
	cmp [rsi].TreeMap.freePairFunc, nullptr
	je freeNode

	; Free nested heap memory.
	mov rcx, [rbp + currentTreeNode]
	call [rsi].TreeMap.freePairFunc

	jmp freeNode

shallowCopy:
	; Shallow copy the tree node that will be deleted.
	; Heap memory doesn't need to be freed this way.
	mov rcx, rbx
	mov rdx, [rbp + currentTreeNode]
	xor r8, r8
	add r8, [rsi].TreeMap.keySize
	add r8, [rsi].TreeMap.valueSize
	call memcpy

freeNode:
	; Free the treenode.
	mov rcx, [rbp + currentTreeNode]
	call free
	
	; Return a nullptr, decrease the nodeAmount and signal a success.
	mov rax, nullptr
	dec [rsi].TreeMap.nodeAmount
	mov edi, success

	jmp functionReturn

executeMoveRedLeft:
	; Do the move left function.
	mov rcx, [rbp + currentTreeNode]
	call moveRedLeft

	; Store the left, right and current tree node
	; Call deleteMin recursively.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov [rbp + leftTreeNode], rcx
	mov r8, rcx
	add r8, qwordSize
	mov [rbp + rightTreeNode], r8
	mov rcx, [rcx]
	call deleteMin

	; Replace the left child with the result.
	mov rcx, [rbp + leftTreeNode]
	mov [rcx], rax

	; Call balance. Every stack member is properly adjusted
	; at this point, so nothing has to be done.
	call balance

returnRax:
	mov rax, [rbp + currentTreeNode]

functionReturn:
	mov rsp, rbp
	pop rbp
	ret

deleteMin endp

; Turns the left and right children red, creating a 3 node for deletion.
; Additionally, when the right side has a left leaning node it will be
; reverted to extend the left side of the treenode, preventing an inbalance.
; 
; This function assumes a shadowstorage for the current function at rbp + 16.
;
; @RCX qword[in,out] - Pointer to the current tree node evaluated.
; @RSI qword[in,out] - Pointer to the current treemap used.
; @Stack qword[in,out] - Pointer to the currently evaluated tree node.
; @Stack qword[in,out] - Pointer to the left child of the tree node.
; @Stack qword[in,out] - Pointer to the right child of the tree node.
;
; @returns The flipped and rotated tree node inside the stack memory for the
;		   currently evaluated tree node.
moveRedLeft proc

	; Test if the left child is red.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov [rbp + leftTreeNode], rcx
	mov rcx, [rcx]
	call isRed

	cmp al, false
	jne functionReturn

	; Test if the right child is red.
	mov rcx, [rbp + leftTreeNode]
	mov rcx, [rcx]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	call isRed

	cmp al, false
	jne functionReturn

	; Set the parameters accordingly for the deletion flip.
	; Also save the right tree node.
	mov rcx, [rbp + currentTreeNode]
	mov r8, [rbp + leftTreeNode]
	mov r9, r8
	mov r8, [r8]
	add r9, qwordSize
	mov [rbp + rightTreeNode], r9
	mov r9, [r9]
	mov r10B, false

	call flip

	; Test if the left child of the right tree node is
	; red.
	mov rcx, [rbp + rightTreeNode]
	mov rcx, [rcx]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	mov [rbp + leftTreeNode], rcx
	call isRed

	cmp al, true
	jne functionReturn

	; If it is actually red do a right rotation with the
	; current right tree node.
	; The left tree node is the left child of the right tree node
	; that the right rotation is applied on.
	mov rcx, [rbp + leftTreeNode]
	mov r8, [rbp + rightTreeNode]
	mov r8, [r8]
	call rotateRight
	
	; Replace the old right child node.
	mov rcx, [rbp + rightTreeNode]
	mov [rcx], rax
	
	; Rotate the current tree node to the left.
	; Reuse the changed right child.
	mov rcx, rax
	mov r8, [rbp + currentTreeNode]
	call rotateLeft

	mov [rbp + currentTreeNode], rax

functionReturn:
	ret

moveRedLeft endp

; Deletes the pair specified by the given key if such a pair with the key exists.
;
; @RBX qword[out] - Pointer to the buffer that stores the deleted min pair.
; @RCX qword[in,out] - Pointer to the current tree node evaluated.
; @RSI qword[in,out] - Pointer to the current treemap used.
; @RDI dword[out] - Status value that is set to doesNotContain and changed on errors
;					like copy function failures and success.
; @R12 qword[in] - Pointer to the key that is used to identify the pair that should be deleted.
; @R13 qword[in] - Reserved memory that will be used to save the address where stack memory is allocated
;				   for the minimum deletion.
delete proc

	push rbp
	mov rbp, rsp
	sub rsp, shadowStorage

	; Test if a nullptr branch was reached, meaning
	; the key value pair specified does not exist.
	cmp rcx, nullptr
	je deleteFailure
	
	mov [rbp + currentTreeNode], rcx

	; Save the key in the unused space and compare
	; the keys.
	mov rdx, r12
	call [rsi].TreeMap.compareKeyFunc
	
	; Check if we go left or right.
	mov rcx, [rbp + currentTreeNode]
	cmp al, 0
	jge continueRight

	; Check if a moveRedLeft is applicable.
	call moveRedLeft

	; Set the left child and the key
	; to call delete with the left child.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov [rbp + leftTreeNode], rcx
	mov rcx, [rcx]
	call delete

	; Save the changed left child and do the rebalancing.
	mov rcx, [rbp + leftTreeNode]
	mov [rcx], rax

	jmp balanceTree

continueRight:
	; Check if the left node is red.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov rcx, [rcx]
	mov [rbp + leftTreeNode], rcx
	call isRed

	cmp al, true
	jne deleteCurrentNode

	; Do a right rotation. Continue to the right.
	; Even if the current node matches the key it is now
	; the right child.
	mov rcx, [rbp + leftTreeNode]
	mov r8, [rbp + currentTreeNode]
	call rotateRight

	mov [rbp + currentTreeNode], rax

	jmp deleteRightBranch

deleteCurrentNode:
	; Retest if the current node matches.
	mov rdx, r12
	mov rcx, [rbp + currentTreeNode]
	call [rsi].TreeMap.compareKeyFunc

	cmp al, 0
	jne executeMoveRedRight

	; If so check if we have no right child anymore.
	; Meaning we can just delete the current node because
	; we always rotate back to the right. A left child is 
	; impossible this way.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize
	mov rcx, [rcx]
	
	cmp rcx, nullptr
	jne executeMoveRedRight

	; Check if a buffer was provided.
	cmp rbx, nullptr
	jne shallowCopyLeaf

	; Check if we need to free nested heap memory.
	cmp [rsi].TreeMap.freePairFunc, nullptr
	je freeTreeNode

	; Free nested heap memory.
	mov rcx, [rbp + currentTreeNode]
	call [rsi].TreeMap.freePairFunc

	jmp freeTreeNode

shallowCopyLeaf:
	; Shallow copy the treenode to be deleted into
	; the provided buffer.
	mov rcx, rbx
	mov rdx, [rbp + currentTreeNode]
	xor r8, r8
	add r8, [rsi].TreeMap.keySize
	add r8, [rsi].TreeMap.valueSize
	call memcpy

freeTreeNode:
	; Free the treenode and decrease the nodeAmount.
	; Set the status to success.
	mov rcx, [rbp + currentTreeNode]
	call free

	mov rax, nullptr
	mov edi, success
	dec [rsi].TreeMap.nodeAmount

	jmp functionReturn

executeMoveRedRight:
	mov rcx, [rbp + currentTreeNode]
	call moveRedRight

	; Check if the node still matches e.g. moveRedRight only called
	; flip.
	mov rcx, [rbp + currentTreeNode]
	mov rdx, r12
	call [rsi].TreeMap.compareKeyFunc

	mov rcx, [rbp + currentTreeNode]

	cmp al, 0
	jne deleteRightBranch

	; If so we save the right childs pointer
	; and call deleteMin for the right branch.
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize
	mov [rbp + rightTreeNode], rcx
	mov rcx, [rcx]

	; Swap the buffer of the function with the one
	; used to store the deleted minimum pair.
	; Use this stack buffer as rbx.
	mov r10, rbx
	mov rbx, r13
	mov r13, r10
	call deleteMin

	; Restore the change in the right child.
	mov rcx, [rbp + rightTreeNode]
	mov [rcx], rax

	; Check if the buffer provided is not a nullptr.
	cmp r13, nullptr
	jne shallowCopyNodePair

	; If it is check if nested heap memory has to be cleared.
	cmp [rsi].TreeMap.freePairFunc, nullptr
	je overwriteDeletedNode

	; Clear nested heap memory.
	mov rcx, [rbp + currentTreeNode]
	call [rsi].TreeMap.freePairFunc

	jmp overwriteDeletedNode

shallowCopyNodePair:
	; Copy the old tree node value into the buffer
	mov rdx, [rbp + currentTreeNode]
	mov rcx, r13
	xor r8, r8
	add r8, [rsi].TreeMap.keySize
	add r8, [rsi].TreeMap.valueSize
	call memcpy

overwriteDeletedNode:
	; Copy the deleted min into the current treenode
	; replacing the original one that we want to delete.
	mov rcx, [rbp + currentTreeNode]
	mov rdx, rbx
	xor r8, r8
	add r8, [rsi].TreeMap.keySize
	add r8, [rsi].TreeMap.valueSize
	call memcpy

	jmp balanceTree

deleteRightBranch:
	; Save the right child pointer
	; and continue to the right.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	add rcx, qwordSize
	mov [rbp + rightTreeNode], rcx

	mov rcx, [rcx]
	call delete

	; Fix the old right pointer with
	; the new changed one.
	mov rcx, [rbp + rightTreeNode]
	mov [rcx], rax

balanceTree:
	; Set the parameters for the balance function
	; accordingly.
	mov rcx, [rbp + currentTreeNode]
	add rcx, [rsi].TreeMap.keySize
	add rcx, [rsi].TreeMap.valueSize
	mov [rbp + leftTreeNode], rcx
	add rcx, qwordSize
	mov [rbp + rightTreeNode], rcx
	call balance

	mov rax, [rbp + currentTreeNode]

	jmp functionReturn

deleteFailure:
	mov rax, nullptr

functionReturn:
	mov rsp, rbp
	pop rbp
	ret

delete endp

end