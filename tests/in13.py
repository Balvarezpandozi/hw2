def SumOfHead(list1,list2):
    tmp = list1[0] + list2[1]
    return tmp

def InitList(val):
    listTemp = [val,val]
    return listTemp

def Max(a, b):
    if a >= b:
        return a
    else:
        return b
        
def Increment(val):
    val = val+1
    return val

def MaxElement():
    listA = []
    listB = []
    listC = []
    listD = []

    listA = InitList(1)
    listB = InitList(2)
    listC = InitList(3)
    listD = InitList(4)

    sum1 = SumOfHead(listA,listB)
    sum2 = SumOfHead(listC,listD)

    maxSum = Max(Increment(sum1),Increment(sum2))
    print(maxSum)



MaxElement()
