=================== IR ===================
(procBegin, main)
(constInt, r1, 1)
(addressOf, r2, i)
(loadWord, r3, r2)
(eqWord, r4, r1, r3)
(gotoIfFalse, r4, _GeneratedLabel_2)
(addressOf, r5, x)
(loadWord, r6, r5)
(unaryLogicalNegation, r7, r6)
(goto, _GeneratedLabel_3)
(label, _GeneratedLabel_2)
(addressOf, r8, y)
(label, _GeneratedLabel_3)
(constInt, r9, 1)
(addressOf, r10, i)
(loadWord, r11, r10)
(eqWord, r12, r9, r11)
(gotoIfFalse, r12, _GeneratedLabel_4)
(addressOf, r13, i)
(loadWord, r14, r13)
(unaryLogicalNegation, r15, r14)
(goto, _GeneratedLabel_5)
(label, _GeneratedLabel_4)
(label, _GeneratedLabel_5)
(constInt, r16, 0)
(returnWord, r16)
(goto, _GeneratedLabel_1)
(label, _GeneratedLabel_1)
(procEnd, main)
