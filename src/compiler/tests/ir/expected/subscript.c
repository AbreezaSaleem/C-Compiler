=================== IR ===================
(procBegin, main)
(addressOf, r1, i)
(addressOf, r2, a)
(addressOf, r3, j)
(loadWord, r4, r3)
(constInt, r5, 4)
(multSignedWord, r6, r5, r4)
(addSignedWord, r7, r2, r6)
(loadWord, r8, r7)
(storeWord, r1, r8)
(constInt, r9, 0)
(returnWord, r9)
(goto, _GeneratedLabel_1)
(label, _GeneratedLabel_1)
(procEnd, main)