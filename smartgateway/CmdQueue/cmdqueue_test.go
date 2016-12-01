package cmdqueue

import (
	"fmt"
	"testing"
)

type command struct {
	cmd string
}

func TestCmdQueue(t *testing.T) {
	fmt.Println("test begin ...")

	//create
	var queue = NewMyQueueBySize(2)

	//insert
	err1 := queue.Insert(command{cmd: "DiscoverGW"})
	if err1 != nil {
		fmt.Println(err1)
	}

	//insert
	err2 := queue.Insert(command{cmd: "AddNode"})
	if err2 != nil {
		fmt.Println(err2)
	}

	//pickout
	c3, err3 := queue.Pickout()
	if err3 != nil {
		fmt.Println(err3)
	} else {
		fmt.Println(c3.(command).cmd)
	}

	//pickout
	c4, err4 := queue.Pickout()
	if err4 != nil {
		fmt.Println(err4)
	} else {
		fmt.Println(c4)
	}

	//pickout
	c5, err5 := queue.Pickout()
	if err5 != nil {
		fmt.Println(err5)
	} else {
		fmt.Println(c5)
	}

	//////////////////////////////////////////////////////////
	var queue2 MyQueue
	queue2.CreateQueue(2)
	//insert
	err6 := queue2.Insert(command{cmd: "Remove"})
	if err6 != nil {
		fmt.Println(err6)
	}

	//pickout
	c7, err7 := queue2.Pickout()
	if err7 != nil {
		fmt.Println(err7)
	} else {
		fmt.Println(c7)
	}

	fmt.Println("test end ...")
}

func TestCmdQueue2(t *testing.T) {
	fmt.Println("test begin ...")

	//create
	var queue *MyQueue
	queue = new(MyQueue)
	queue.CreateQueue(2)

	//insert
	err1 := queue.Insert(1)
	if err1 != nil {
		fmt.Println(err1)
	}

	//insert
	err2 := queue.Insert(2)
	if err2 != nil {
		fmt.Println(err2)
	}

	//insert
	err0 := queue.Insert(3)
	if err0 != nil {
		fmt.Println(err0)
	}

	//pickout
	c3, err3 := queue.Pickout()
	if err3 != nil {
		fmt.Println(err3)
	} else {
		fmt.Println(c3)
	}

	//pickout
	c4, err4 := queue.Pickout()
	if err4 != nil {
		fmt.Println(err4)
	} else {
		fmt.Println(c4)
	}

	//pickout
	c5, err5 := queue.Pickout()
	if err5 != nil {
		fmt.Println(err5)
	} else {
		fmt.Println(c5)
	}
}
