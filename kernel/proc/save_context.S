
.global save_context_1
save_context_1:
	push {r0-r12, r14}

	/* return address of process is passed in r0 */
	mov r14, r0

	/* load address of context array for current process into r0 */
	push {r1-r12, r14}
	bl get_current_context
	pop {r1-r12, r14}

	/* if no process is active, return */
	cmpeq r0, #0
	beq save_context_return

	/* store cpsr */
	push {r1}
	mrs r1, SPSR
	str r1, [r0]
	pop {r1}

	/* store return address */
	add r0, #4
	str r14, [r0]

	/* store the rest, except r0 */
	add r0, #8
	stmia r0, {r1-r14}^

	b save_context_return


.global save_context_2
save_context_2:
	push {r0-r12, r14}
	
	mov r1, r0

	/* load address of context array for current process into r0 */
	push {r1-r12, r14}
	bl get_current_context
	pop {r1-r12, r14}

	/* if no process is active, return */
	cmpeq r0, #0
	beq save_context_return

	/* store r0 */
	add r0, #8
	str r1, [r0]

save_context_return:
	pop {r0-r12, r14}
	mov pc, lr
	