//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/07/27
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package cmdqueue

import "errors"
import "sync"


const (
	defaultQueueSize = 5
)

type MyQueue struct {
	mutex        sync.Mutex
	queueSize    int
	front        int
	rear         int
	currentCount int
	elements     []interface{}
}

/**
  指定大小的初始化
*/
func NewMyQueueBySize(size int) *MyQueue {
	return &MyQueue{queueSize: size + 1, front: 0, rear: size, currentCount: 0,
		elements: make([]interface{}, size+1)}
}

/**
  按默认大小进行初始化
*/
func NewMyQueueByDefault() *MyQueue {
	return NewMyQueueBySize(defaultQueueSize)
}

/**
  向下一个位置做探测
*/
func probeNext(i, size int) int {
	return (i + 1) % size
}

/**
创建队列
*/
func (queue *MyQueue) CreateQueue(size int) {
	queue.queueSize = size + 1
	queue.front = 0
	queue.rear = size
	queue.currentCount = 0
	queue.elements = make([]interface{}, size+1)
}

/**
  清空队列
*/
func (queue *MyQueue) ClearQueue() {
	queue.mutex.Lock()
	queue.front = 0
	queue.rear = queue.queueSize - 1
	queue.currentCount = 0
	queue.mutex.Unlock()
}

/**
  是否为空队列
*/
func (queue *MyQueue) IsEmpty() bool {
	if probeNext(queue.rear, queue.queueSize) == queue.front {
		return true
	}
	return false
}

/**
  队列是否满了
*/
func (queue *MyQueue) IsFull() bool {
	if probeNext(probeNext(queue.rear, queue.queueSize), queue.queueSize) == queue.front {
		return true
	}
	return false
}

/**
  入队
*/
func (queue *MyQueue) Insert(e interface{}) error {
	
	if queue.IsFull() == true {
		return errors.New("the queue is full.")
	}
	queue.mutex.Lock()
	queue.rear = probeNext(queue.rear, queue.queueSize)
	queue.elements[queue.rear] = e
	queue.currentCount = queue.queueSize + 1
	queue.mutex.Unlock()
	return nil
}

/**
  出队一个元素
*/
func (queue *MyQueue) Pickout() (interface{}, error) {
	if queue.IsEmpty() == true {
		return nil, errors.New("the queue is empty.")
	}
	queue.mutex.Lock()
	tmp := queue.front
	queue.front = probeNext(queue.front, queue.queueSize)
	queue.currentCount = queue.currentCount - 1
	queue.mutex.Unlock()
	return queue.elements[tmp], nil
}
