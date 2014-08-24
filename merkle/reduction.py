#!/usr/bin/env python3
# Dirty Prototyping module for whole Merkle tree

getParent = lambda : (i - 1)/2
leftChild = lambda i: (2 * i) + 1
rightChild = lambda i: (2 * i) + 2

class DiffieNode:
    def __init__(self, v=0, l=None, r=None):
        self.left = l
        self.right = r
        self.value = v
        self.setLeft = self.__setSide('left')
        self.setRight = self.__setSide('right')
        self.getLeft = self.__getSide('left')
        self.getRight = self.__getSide('right')

    def getValue(self):
        return self.value

    def __setSide(self, sideName):
        return lambda v: setattr(self, sideName.lower(), v)

    def __getSide(self, sideName):
        return lambda: getattr(self, sideName.lower(), None)
    
    def __str__(self):
        return 'v: {v} l:{l} r:{r}'.format(v=self.getValue(), l=self.getLeft(), r=self.getRight())

def retrDFNode(dfnMap, i, v):
    retr = dfnMap.get(i, None)
    if retr is None:
        retr = DiffieNode(v=v)
        dfnMap[i] = retr

    return retr

def main():
    level = [i for i in range(20)]
    finalLevel = []

    levelLen = len(level)
    mid = levelLen >> 1
    dfnMap = {}
    for i in range(mid):
        dfn = retrDFNode(dfnMap, i, level[i])

        l = leftChild(i)
        if l < levelLen:
            dfn.setLeft(retrDFNode(dfnMap, l, level[l]))

        r = rightChild(i)
        if r < levelLen:
            dfn.setRight(retrDFNode(dfnMap, r, level[r]))

        print('i', i, 'l', l, 'r', r)
        finalLevel.append(dfn)

    for dfn in finalLevel:
        print(dfn.value, dfn.getLeft(), dfn.getRight())

if __name__ == '__main__':
    main()
