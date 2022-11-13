#Return list at index
def getElement(index):
    data = [100,99,88,77,66,65,72,53,1,134,23,51,38,50,6]
    element = data[index]
    return element

#Increment val
def increment(val):
    val = val + 1
    return val

listA = [1,2,3]
idx = 0

a = getElement(idx)
listA[0] = listA[0] + a


idx = increment(idx)
a = getElement(idx)
listA[1] = listA[1] + a

idx = increment(idx)
a = getElement(idx)
listA[2] = listA[2] + a

print("A: ",listA)
