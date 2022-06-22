# search.py
# ---------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html
# python pacman.py -l mapa_a_posar p SearchAgent -a fn=tipus_de_busqueda
# python pacman.py -l tinyMaze -p SearchAgent -a fn=depthFirstSearch  
'''
Alumnes:
	Carolina Romeu Farre
	Pol Llagostera Blasco
'''

"""
In search.py, you will implement generic search algorithms which are called 
by Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
  """
  This class outlines the structure of a search problem, but doesn't implement
  any of the methods (in object-oriented terminology: an abstract class).
  
  You do not need to change anything in this class, ever.
  """
  
  def getStartState(self):
     """
     Returns the start state for the search problem 
     """
     util.raiseNotDefined()
    
  def isGoalState(self, state):
     """
       state: Search state
    
     Returns True if and only if the state is a valid goal state
     """
     util.raiseNotDefined()

  def getSuccessors(self, state):
     """
       state: Search state
     
     For a given state, this should return a list of triples, 
     (successor, action, stepCost), where 'successor' is a 
     successor to the current state, 'action' is the action
     required to get there, and 'stepCost' is the incremental 
     cost of expanding to that successor
     """
     util.raiseNotDefined()

  def getCostOfActions(self, actions):
     """
      actions: A list of actions to take
 
     This method returns the total cost of a particular sequence of actions.  The sequence must
     be composed of legal moves
     """
     util.raiseNotDefined()
           

def tinyMazeSearch(problem):
  """
  Returns a sequence of moves that solves tinyMaze.  For any other
  maze, the sequence of moves will be incorrect, so only use this for tinyMaze
  """
  from game import Directions
  s = Directions.SOUTH
  w = Directions.WEST
  return  [s,s,w,s,w,w,s,w]


def depthFirstSearch(problem):
  """
  This algorithm returns the depth-first search with a stack.
  """

  return blind(problem, util.Stack())

  util.raiseNotDefined()


def depthFirstSearchTree(problem):
  """
  This algorithm returns the depth-first search tree with a stack.
  """

  return blindTree(problem, util.Stack())
  
  util.raiseNotDefined()


def depthLimitedSearch(problem, limit):
  """
  This algorithm is called from the IDS function, make a deep search. 
  The use of Boolean variable cut allows us to know if the depth limit 
  has prevented any node to expand. The limit is sent from the IDS function 
  which will increment with each turn. If the boundary is empty and cut is 
  false we can conclude that we have explored all the space
  states and that the problem has no solution.
  """
  
  import node
  import sys
  node.Node

  fringe = util.Stack()
  fringe.push(node.Node(problem.getStartState(), None, None, 0))
  expanded = {}
  cut = False

  while True:

     if fringe.isEmpty(): 
        if cut==True:
           return 'cutoff'
        else:
           return 'Failure: No Solution'
		
     n = fringe.pop()

     expanded[n.state] = ['E', n]

     if problem.isGoalState(n.state): 
        return n.path()

     if n.pathcost == limit: cut = True

     else:
        for state, action, cost in problem.getSuccessors(n.state):
	   ns = node.Node(state, n, action, n.pathcost+cost)

	   if ns.state in expanded.keys():
	      tipusIn, nodeIn = expanded[ns.state]

	      if nodeIn.pathcost > ns.pathcost:
	         expanded[ns.state] = ['F', ns]
	         fringe.push(ns)		
	   else:
	      expanded[ns.state] = ['F', ns]
	      fringe.push(ns)

  util.raiseNotDefined()


def depthLimitedSearchTree(problem, limit):
  """
  This algorithm is called from the IDS function, make a deep search. 
  The use of Boolean variable cut allows us to know if the depth limit 
  has prevented any node to expand. The limit is sent from the IDS function 
  which will increment with each turn. If the boundary is empty and cut is 
  false we can conclude that we have explored all the space
  states and that the problem has no solution.
  """
  import node
  import sys
  node.Node

  fringe = util.Stack()
  fringe.push(node.Node(problem.getStartState(), None, None, 0))
  cut = False

  while True:

     if fringe.isEmpty(): 
        if cut == True:
	   return 'cutoff'
	else:
	   return 'Failure: No Solution'
	
     n = fringe.pop()
	
     if n.pathcost == limit: cut = True

     else:

        for state, action, cost in problem.getSuccessors(n.state):
	   ns = node.Node(state, n, action, n.pathcost + cost)
	   
           if problem.isGoalState(ns.state): 
	      return ns.path()
	  
           fringe.push(ns)

  util.raiseNotDefined()


def breadthFirstSearch(problem):
  """
  This algorithm returns the breadth-first search with a queque.
  """
  return blind(problem, util.Queue())

  util.raiseNotDefined()


def breadthFirstSearchTree(problem):
  """
  This algorithm returns the breadth-first search tree with a queque.
  """

  return blindTree(problem, util.Queue())

  util.raiseNotDefined() 


def iterativeDeepingSearch(problem):
  """
  This algorithm has an infinite loop where you explore each level of graf, 
  each level makes a call to the DLS algorithm, which will search depth.
  """
  import node
  import sys
  node.Node

  cutoff = False
  depth = 0

  while True:

     result = depthLimitedSearch(problem, depth)
     depth = depth + 1

     if result is not 'cutoff': 
        return result

  util.raiseNotDefined()


def iterativeDeepingSearchTree(problem):
  """
  This algorithm has an infinite loop where you explore each level of the tree, 
  each level makes a call to the DLST algorithm, which will search depth.
  """
  import node
  import sys
  node.Node

  cutoff = False
  depth = 0

  while True:

     result = depthLimitedSearchTree(problem, depth)
     depth = depth + 1

     if result is not 'cutoff': 
        return result

  util.raiseNotDefined()

    
def uniformCostSearch(problem):
  """
  This algorithm choose the one with less cost from their respective initial state
  and replaces the cost of node in fringe within a lower cost.
  """
  import node
  import sys
  import searchAgents
  
  fringe = util.PriorityQueue()
  fringe.push(node.Node(problem.getStartState(), None, None, 0), 0)
  expanded = {}

  while True:

     if fringe.isEmpty(): return 'Failure: No Solution'
     
     n = fringe.pop()
	
     if problem.isGoalState(n.state): 
        return n.path()

     expanded[n.state] = ['E', n]

     for state, action, cost in problem.getSuccessors(n.state):

        ns = node.Node(state, n, action, n.pathcost + cost)

        if ns.state not in expanded.keys():
	   expanded[ns.state] = ['F', ns]
	   fringe.push(ns, ns.pathcost)

	else:
	   tipusIn, nodeIn = expanded[ns.state]

	   if tipusIn == 'F':
	      if nodeIn.pathcost > ns.pathcost:
	         fringe.push(ns, ns.pathcost)
		 expanded[ns.state] = ['F',ns]

  util.raiseNotDefined()


def blind(problem, queue):
  """
  DFS and BFS functions are called with this algorithm, equivalent to search, 
  since its function call with the queue and stack.
  """
  import node
  import sys
  node.Node

  fringe = queue
  fringe.push(node.Node(problem.getStartState(), None, None, 0))
  expanded = {}
  
  while True:

     if fringe.isEmpty(): 'Failure: No Solution'
     n = fringe.pop()
     expanded[n.state] = ['E', n]

     for state, action, cost in problem.getSuccessors(n.state):
        ns = node.Node(state, n, action, n.pathcost + cost)
	
        if ns.state not in expanded.keys():
	   if problem.isGoalState(ns.state): 
	      return ns.path()

	   expanded[ns.state] = ['F', ns]
	   fringe.push(ns)

  util.raiseNotDefined()


def blindTree(problem, queue):
  """
  DFS and BFS functions are called with this algorithm, equivalent to search, 
  since its function call with the queue and stack, the difference with the blind 
  function is, this algorithm is not expanded and not remember their sequence or path.
  """

  import node
  import sys
  node.Node

  fringe = queue
  fringe.push(node.Node(problem.getStartState(), None, None, 0))

  while True:

     if fringe.isEmpty(): return 'Failure: No Solution'
     
     n = fringe.pop()
	
     if problem.isGoalState(n.state): 
        return n.path()

     for state, action, cost in problem.getSuccessors(n.state):
        ns = node.Node(state, n, action, n.pathcost+cost)
	fringe.push(ns)

  util.raiseNotDefined()


def nullHeuristic(state, problem = None):
  """
  A heuristic function estimates the cost from the current state to the nearest
  goal in the provided SearchProblem.  This heuristic is trivial.
  """
  return 0


def greedyBestFirstSearch(problem, heuristic = nullHeuristic):
  """
  This algorithm, where the selected node is selected based on a cost function f(n). 
  Thus, that one node n with less f (n) is chosen to be expanded.
  Where f (n) would be h (n) = minimum cost estimate from n to a goal node.
  """
  import node
  import sys
  import searchAgents
  
  fringe = util.PriorityQueue()
  n = node.Node(problem.getStartState(), None, None, 0)
  fringe.push(n, heuristic(n.state, problem))
  expanded = {}	

  while True:

     if fringe.isEmpty(): 'Failure: No Solution'
     
     n = fringe.pop()
	
     expanded[n.state] = ['E', n]

     for state, action, cost in problem.getSuccessors(n.state):
        ns = node.Node(state, n, action, n.pathcost + cost)

        if ns.state not in expanded.keys():

	   if problem.isGoalState(ns.state): 
	      return ns.path()

	   fringe.push(ns, heuristic(ns.state, problem))
	   expanded[ns.state] = ['F', ns]

  util.raiseNotDefined()


def aStarSearch(problem, heuristic = nullHeuristic):
  """
  Search the node that has the lowest combined cost and heuristic first.
  This algorithm works as the UCS, unlike the priority queue but adding cost heuristics, 
  in addition to a check of visited nodes when a node is on the fringe 
  and has already been visited, it will not be checked again.
  """
  
  import node
  import sys
  import searchAgents

  fringe = util.PriorityQueue()
  n = node.Node(problem.getStartState(), None, None, 0)
  fringe.push(n, heuristic(n.state, problem))
  expanded = {}
  visited = {}

  while True:

	if fringe.isEmpty(): 'Failure: No Solution'
	n = fringe.pop()
        expanded[n.state] = ['E', n]
	
	if problem.isGoalState(n.state): 
	   return n.path()
        
        if n.state not in visited.keys():

 	   for state, action, cost in problem.getSuccessors(n.state):

	      ns = node.Node(state, n, action, n.pathcost + cost)


	      if ns.state not in expanded.keys():
		
	         expanded[ns.state] = ['F', ns]
	         fringe.push(ns, heuristic(ns.state, problem) + ns.pathcost)		

	      else:
	         tipusIn, nodeIn = expanded[ns.state]

	         if tipusIn == 'F' and nodeIn.pathcost > ns.pathcost:	
		    fringe.push(ns, heuristic(ns.state, problem) + ns.pathcost)
		    visited[nodeIn.state] = ['V', nodeIn]
		    expanded[ns.state] = ['F', ns]
        else:
	   del visited[n.state]

  util.raiseNotDefined()

# Abbreviations
b = blind
bt = blindTree
bfs = breadthFirstSearch
bfst = breadthFirstSearchTree
dfs = depthFirstSearch
dfst = depthFirstSearchTree
dls = depthLimitedSearch
dlst = depthLimitedSearchTree
ids = iterativeDeepingSearch
idst = iterativeDeepingSearchTree
gbfs = greedyBestFirstSearch #voraz
astar = aStarSearch
ucs = uniformCostSearch
