=================== IR ===================
(procBegin, main)
(addressOf, r1, i)
(constInt, r2, 0)
(storeWord, r1, r2)
(label, _GeneratedLabel_2)
(addressOf, r3, i)
(constInt, r4, 5)
(storeWord, r3, r4)
(addressOf, r5, i)
(constInt, r6, 5)
(loadWord, r7, r5)
(ltSignedWord, r8, r7, r6)
(gotoIfFalse, r8, _GeneratedLabel_3)
(goto, _GeneratedLabel_2)
(label, _GeneratedLabel_3)
(constInt, r9, 0)
(returnWord, r9)
(goto, _GeneratedLabel_1)
(label, _GeneratedLabel_1)
(procEnd, main)
