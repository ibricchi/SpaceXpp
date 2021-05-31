package server

import "errors"

/*
	A* algorithm for finding the shortest path from a start node to a destination node in a square tile map.
	See https://en.wikipedia.org/wiki/A*_search_algorithm for more information about the A* algorithm.

	gScore = distance from start node to current node
	h = heuristic function which estimates the shorted distance from the current node to the destination node
	fScore = gScore + h (estimated distance from start node to destination node)

	The A* algorithm is identical to Dijkstra's algorithm when h = 0.

	Time complexity = O(n*log(n))
	Space complexity = O(n)
	where n = number of nodes in graph (n = cols * rows)
*/
func getShortedPathFromStartToDestination(startRow int, startCol int, destinationRow int, destinationCol int, tileMap tileMap) ([][]int, error) {
	// Check if start node is equal to destination node
	if startRow == destinationRow && startCol == destinationCol {
		return [][]int{}, nil
	}

	nodes := initNodes(tileMap)
	startNode := nodes[startRow][startCol]
	destinationNode := nodes[destinationRow][destinationCol]

	// No distance to get to start node
	startNode.gScore = 0
	startNode.fScore = h(startNode, destinationNode)

	// Min heap for choosing next node based on Greedy property (priority element is element with lowest fScore)
	openSet := initMinHeap([]*node{startNode})

	// openSet contains nodes that could be visited next
	for !openSet.isEmpty() {
		// Get node with lowest fScore
		current := openSet.remove()

		// Found shortest path
		if current == destinationNode {
			break
		}

		// Look at neighbor nodes and check if we found a shorter path to them
		neighbors := getNeighborNodes(current, nodes)
		for _, neighbor := range neighbors {
			// Check for obstruction (0 = unknown, 1 = empty space, >1 = some kind of obstruction)
			if neighbor.val > 1 {
				continue
			}

			// Neighboring tiles are a distance of one from the current tile
			tentativeGScore := current.gScore + 1

			// New path not better than previous one
			if tentativeGScore >= neighbor.gScore {
				continue
			}

			// New path better than previous one => update
			neighbor.cameFrom = current
			neighbor.gScore = tentativeGScore
			neighbor.fScore = tentativeGScore + h(neighbor, destinationNode)

			if !openSet.containsNode(neighbor) {
				openSet.insert(neighbor)
			} else {
				// Change priority in min heap
				openSet.update(neighbor)
			}
		}
	}

	return reconstructPath(destinationNode)
}

/*
	Heuristic function.
	The Manhattan distance is used.
	It is both admissible and consistent as we are not allowed to move diagonally on the map (constraint to vertical/horizontal movements).
	Hence, this heuristic leads to an optimal solution.
*/
func h(currentNode *node, destinationNode *node) int {
	return abs(currentNode.col-destinationNode.col) + abs(currentNode.row-destinationNode.row)
}

func initNodes(tileMap tileMap) [][]*node {
	nodes := [][]*node{}
	for row := 0; row < tileMap.Rows; row++ {
		newRow := []*node{}
		for col := 0; col < tileMap.Cols; col++ {
			val := tileMap.getTile(row, col)
			newRow = append(newRow, newNode(row, col, val))
		}
		nodes = append(nodes, newRow)
	}
	return nodes
}

// Returns all neighboring nodes that are vertically or horizontally from the current node
func getNeighborNodes(currentNode *node, nodes [][]*node) []*node {
	neighbors := []*node{}
	row := currentNode.row
	col := currentNode.col

	if row > 0 {
		neighbors = append(neighbors, nodes[row-1][col])
	}
	if row < len(nodes)-1 {
		neighbors = append(neighbors, nodes[row+1][col])
	}
	if col > 0 {
		neighbors = append(neighbors, nodes[row][col-1])
	}
	if col < len(nodes[0])-1 {
		neighbors = append(neighbors, nodes[row][col+1])
	}

	return neighbors
}

func reconstructPath(destinationNode *node) ([][]int, error) {
	if destinationNode.cameFrom == nil {
		return [][]int{}, errors.New("server: map_paths: failed to reconstruct path: no possible path exists")
	}

	// Record each tile contained in the path from the destination node to the start node
	reversedPath := [][]int{}
	for destinationNode != nil {
		reversedPath = append(reversedPath, []int{destinationNode.row, destinationNode.col})
		destinationNode = destinationNode.cameFrom
	}

	// Reverse path for correct ordering (want path from start node to destinaton node)
	path := make([][]int, len(reversedPath))
	for i := range path {
		j := len(path) - i - 1
		path[i] = reversedPath[j]
	}

	return path, nil
}

func abs(x int) int {
	if x > 0 {
		return x
	}
	return -x
}
