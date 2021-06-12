/*
   Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

package server

/*
	Min heap implementation for the A* algorithm, used for finding the shortest path on a square map.
	The priority node is the one with the smallest fScore.
*/

import (
	"fmt"
	"math"
)

type node struct {
	id       string
	row      int
	col      int
	val      int
	gScore   int
	fScore   int
	cameFrom *node
}

type minHeap struct {
	nodes         []*node
	heapPositions map[string]int // Positions of nodes in heap (key = node.id)
}

func newNode(row int, col int, val int) *node {
	return &node{
		id:       fmt.Sprintf("%d%d", row, col), // Unique id for each node
		row:      row,
		col:      col,
		val:      val,
		gScore:   math.MaxInt32,
		fScore:   math.MaxInt32,
		cameFrom: nil,
	}
}

func initMinHeap(nodes []*node) *minHeap {
	// Insert all nodes into heap using original slice ordering
	heapPositions := map[string]int{}
	for i, node := range nodes {
		heapPositions[node.id] = i
	}

	heap := &minHeap{
		nodes:         nodes,
		heapPositions: heapPositions,
	}

	// Build min heap (change node ordering according to priority)
	for i := (len(heap.nodes)-2)/2 + 1; i >= 0; i-- {
		heap.siftDown(i, len(heap.nodes)-1)
	}

	return heap
}

func (h *minHeap) isEmpty() bool {
	return len(h.nodes) == 0
}

func (h *minHeap) containsNode(node *node) bool {
	_, contains := h.heapPositions[node.id]
	return contains
}

func (h *minHeap) insert(node *node) {
	h.nodes = append(h.nodes, node)
	h.heapPositions[node.id] = len(h.nodes) - 1

	h.siftUp(len(h.nodes) - 1)
}

func (h *minHeap) update(node *node) {
	h.siftUp(h.heapPositions[node.id])
}

func (h *minHeap) remove() *node {
	if h.isEmpty() {
		return nil
	}

	// Move priority node to end of slice for easy removal
	h.swap(0, len(h.nodes)-1)

	// Pop priority node of heap
	node := h.nodes[len(h.nodes)-1]
	h.nodes = h.nodes[0 : len(h.nodes)-1]
	delete(h.heapPositions, node.id)

	h.siftDown(0, len(h.nodes)-1)

	return node
}

func (h *minHeap) swap(i int, j int) {
	h.heapPositions[h.nodes[i].id] = j
	h.heapPositions[h.nodes[j].id] = i

	h.nodes[i], h.nodes[j] = h.nodes[j], h.nodes[i]
}

func (h *minHeap) siftDown(currentIdx int, endIdx int) {
	firstChildIdx := currentIdx*2 + 1
	for firstChildIdx <= endIdx {
		secondChildIdx := -1
		if currentIdx*2+2 <= endIdx {
			secondChildIdx = currentIdx*2 + 2
		}
		swapIdx := firstChildIdx
		firstChildDistance := h.nodes[firstChildIdx].fScore
		if secondChildIdx > -1 && h.nodes[secondChildIdx].fScore < firstChildDistance {
			swapIdx = secondChildIdx
		}

		if h.nodes[swapIdx].fScore >= h.nodes[currentIdx].fScore {
			return
		}

		h.swap(currentIdx, swapIdx)

		currentIdx = swapIdx
		firstChildIdx = currentIdx*2 + 1
	}
}

func (h *minHeap) siftUp(currentIdx int) {
	parentIdx := (currentIdx - 1) / 2
	for currentIdx > 0 && h.nodes[currentIdx].fScore < h.nodes[parentIdx].fScore {
		h.swap(currentIdx, parentIdx)

		currentIdx = parentIdx
		parentIdx = (currentIdx - 1) / 2
	}
}
