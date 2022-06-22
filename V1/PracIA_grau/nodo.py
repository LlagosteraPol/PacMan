'''
Alumnes:
	Carolina Romeu Farre
	Pol Llagostera Blasco
'''

class Node:

	id=0
	#constructor
	def __init__(self, state, parent, action, pathcost):
		self.state=state
		self.parent=parent
		self.action=action
		self.pathcost=pathcost
		self.id=Node.id
		Node.id=Node.id + 1

	def __str__(self):
		return str(self.id)+': '+str(self.state)+' '+str(self.action)+' '+str(self.pathcost)

	def path(self):

		acciones=[]
		identifier=self.id
		nde=self
		while(identifier>0):
			print 'Identificador=' +str(identifier)
			acciones.append(nde.action)
			nde=nde.parent
			identifier=identifier-1

		acciones.reverse()
		return acciones


if __name__ == "__main__":

	n = Node((0,0),None,None, 0)
	n1 = Node((0, 1), n, 'south', 1)
	n2 = Node((0, 2), n1, 'west', 1)
	n3 = Node((0, 3), n2, 'east', 1)
	n4 = Node((0, 4), n3, 'nord', 1)
	print n1
	print n4.path()
