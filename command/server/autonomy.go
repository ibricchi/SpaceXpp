package server

import (
	"container/list"
	"fmt"
)

const tileMapUnknownVal = 1

/*
	Used for discovering a map without user input.
	A "worst-fit" approach is used to determine where the rover should move next.

	Return values: isFullyDiscovered bool, destinationRow int, destinationCol int
	isFullyDiscovered is true if there are no more "unknown" tiles on the map (tile value 1).
*/
// func getBestNextDestinationCoordinates(tileMap tileMap) (bool, int, int) {
// 	// Traverse graph to find size of biggest connected 1s. Store size of current biggest with one of its coordinates (update if find new biggest)
// 	// At end: Use saved coordinate to traverse biggest again and find its centre. This centre is the desired destination
// 	maxSize := 0
// 	currentRow := -1
// 	currentCol := -1

// 	visited := make([][]bool, tileMap.Rows)
// 	for i := range visited {
// 		visited[i] = make([]bool, tileMap.Cols)
// 	}

// 	for row := 0; row < tileMap.Rows; row++ {
// 		for col := 0; col < tileMap.Cols; col++ {
// 			if tileMap.getTile(row, col) != tileMapUnknownVal || visited[row][col] {
// 				continue
// 			}

// 			currentSize := getSizeOfUnknownField(row, col, tileMap, visited)
// 			if currentSize > maxSize {
// 				currentRow, currentCol = row, col
// 			}
// 		}
// 	}

// 	// The whole map is already discovered
// 	if maxSize == 0 {
// 		return true, -1, -1
// 	}

// 	destinationRow, destinationCol := getUnknownMapFieldCentreCoordinates(currentRow, currentCol, tileMap)
// 	return false, destinationRow, destinationCol
// }

// // Depth first traversal
// func getSizeOfUnknownField(row int, col int, tileMap tileMap, visited [][]bool) int {
// 	if tileMap.getTile(row, col) != tileMapUnknownVal || row < 0 || col < 0 || row >= tileMap.Rows || col >= tileMap.Cols || visited[row][col] {
// 		return 0
// 	}

// 	visited[row][col] = true

// 	return 1 +
// 		getSizeOfUnknownField(row+1, col, tileMap, visited) +
// 		getSizeOfUnknownField(row-1, col, tileMap, visited) +
// 		getSizeOfUnknownField(row, col+1, tileMap, visited) +
// 		getSizeOfUnknownField(row, col-1, tileMap, visited)
// }

// // Assumes that field has a rectangular shape and returns the centre coordinate of this rectangular shape
// func getUnknownMapFieldCentreCoordinates(row int, col int, tileMap tileMap) (int, int) {
// 	// Find edges of rectangle
// 	leftRow, leftCol, rightRow, rightCol := row, col, row, col

// 	visited := make([][]bool, tileMap.Rows)
// 	for i := range visited {
// 		visited[i] = make([]bool, tileMap.Cols)
// 	}

// }

// // Return values: leftRow, leftCol, rightRow, rightCol
// func getEdgeCoordinatesOfUnknownField(row int, col int, tileMap tileMap, visited [][]bool, leftRow int, leftCol int, rightRow int, rightCol int) (int, int, int, int) {

// }

type rectangle struct {
	left   int
	right  int
	top    int
	bottom int
}

/*
	Return values: isFullyDiscovered bool, destinationRow int, destinationCol int
	isFullyDiscovered is true if there are no more "unknown" tiles on the map (tile value 1).
*/
func getBestNextDestinationCoordinates(tileMap tileMap) (bool, int, int) {
	undiscoveredRectanglesInMap := getUndiscoveredRectangles(tileMap)

	var biggestRectangle rectangle
	biggestSize := 0

	for _, rectangle := range undiscoveredRectanglesInMap {
		currentSize := (rectangle.right - rectangle.left + 1) * (rectangle.bottom - rectangle.top + 1)
		if currentSize > biggestSize {
			biggestSize = currentSize
			biggestRectangle = rectangle
		}
	}

	// Map already fully discovered
	if biggestSize == 0 {
		return true, -1, -1
	}

	destinationRow, destinationCol := getRectangleCentreCoordinates(biggestRectangle)

	return false, destinationRow, destinationCol
}

/*
	Expects no rectangles that are touching the map edges as all map edges should be filled by an obstruction.

	More work is needed for this to produce an exact solution when rectangles are touching. However, the current accuracy is probably enough to use it in automation.
*/
func getUndiscoveredRectangles(tileMap tileMap) []rectangle {
	finishedRectangles := make([]rectangle, 0)
	unfinishedRectangles := list.New() // linked list
	var currentListEntry *list.Element
	var currentRectangle *rectangle
	isNewRectangle := false

	for row := 0; row < tileMap.Rows; row++ {
		currentListEntry = unfinishedRectangles.Front()
		if currentListEntry != nil {
			currentRectangle = currentListEntry.Value.(*rectangle)
		}

		for col := 0; col < tileMap.Cols; col++ {
			if tileMap.getTile(row, col) == tileMapUnknownVal { // Part of some rectangle
				if currentListEntry != nil && col == currentRectangle.left { // Start col of current unfinished rectangle
					finishedRectangle := false

					for col <= currentRectangle.right {
						// Current rectangle finished
						if tileMap.getTile(row, col) != tileMapUnknownVal {
							// Reset col for finding new rectangles in this row
							col = currentRectangle.left

							currentRectangle.bottom = row - 1

							// Add current rectangle to finished
							finishedRectangles = append(finishedRectangles, *currentRectangle)

							fmt.Println(currentRectangle)

							// Remove current rectangle from unfinished
							nextListEntry := currentListEntry.Next()
							unfinishedRectangles.Remove(currentListEntry)
							currentListEntry = nextListEntry
							if currentListEntry != nil {
								currentRectangle = currentListEntry.Value.(*rectangle)
							}

							finishedRectangle = true

							break
						}

						col++
					}
					col--

					if !finishedRectangle {
						// Move to next unfinished rectangle
						currentListEntry = currentListEntry.Next()
						if currentListEntry != nil {
							currentRectangle = currentListEntry.Value.(*rectangle)
						}
					}
				} else if !isNewRectangle { // Found a new rectangle
					isNewRectangle = true

					newRectangle := rectangle{
						left:   col,
						right:  col,
						top:    row,
						bottom: row,
					}

					if currentListEntry != nil {
						currentListEntry = unfinishedRectangles.InsertBefore(&newRectangle, currentListEntry)
					} else { // Linked list currently empty
						currentListEntry = unfinishedRectangles.PushBack(&newRectangle)
					}

					currentRectangle = currentListEntry.Value.(*rectangle)
				}
			} else { // Not part of a rectangle
				if isNewRectangle { // Update right border of new rectangle
					isNewRectangle = false

					currentRectangle.right = col - 1

					// Move to next unfinished rectangle
					currentListEntry = currentListEntry.Next()
					if currentListEntry != nil {
						currentRectangle = currentListEntry.Value.(*rectangle)
					}
				} else if currentListEntry != nil && col == currentRectangle.left { // Found bottom of unfinished rectangle => finished
					currentRectangle.bottom = row - 1

					// Add current rectangle to finished
					finishedRectangles = append(finishedRectangles, *currentRectangle)

					// Remove current rectangle from unfinished
					nextListEntry := currentListEntry.Next()
					unfinishedRectangles.Remove(currentListEntry)
					currentListEntry = nextListEntry
					if currentListEntry != nil {
						currentRectangle = currentListEntry.Value.(*rectangle)
					}
				}
			}
		}
	}
	return finishedRectangles
}

// Return values: CentreRow, CentreCol
func getRectangleCentreCoordinates(rectangle rectangle) (int, int) {
	centreRow := (rectangle.top + rectangle.bottom) / 2
	centreCol := (rectangle.left + rectangle.right) / 2

	return centreRow, centreCol
}
