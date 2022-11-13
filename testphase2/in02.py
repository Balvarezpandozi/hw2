def IncrementIterator(itr):
    if itr == None:
        return None
    itr = itr + 1
    return itr

#initialize
elements = [3,6,9]
i = 0

#increment
i = IncrementIterator(i)

#print
element = elements[i]
print(element)

