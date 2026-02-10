#include "algo.hpp"

/**
 * @brief Goes through a linked list and deletes the memory allocated for
 * its creation.
 * 
 * @param linkedList The linked list which we want destroyed.
 */
void Algo::destroyLinkedList(Node* linkedList) {
    Node* llPtr = linkedList;
    
    while(llPtr != nullptr) {
        Node* temp = llPtr->node;

        // Delete the memory of the current pointer, and then set llPtr to the next node.
        delete llPtr;

        llPtr = temp;
    }
}

Node* Algo::generateLinkedList(const int numNodes) {
    if(numNodes < 0) {
        return nullptr;
    }
    
    Node* ll = new Node;
    Node* llPtr = ll;

    for(int i = 0; i < numNodes; i++) {
        llPtr->value = i;
        
        if(i == numNodes - 1) {
            llPtr->node = nullptr;
        } else {
            llPtr->node = new Node;
            llPtr = llPtr->node;
        }
    }
    
    return ll;
}

Node* Algo::reverseLinkedList(Node* linkedList) {
    Node* llPtr = linkedList;
    Node* prevPtr = nullptr;
    Node* nextPtr = llPtr;

    while(true) {
        // Set the nextPtr to the next node
        nextPtr = llPtr->node;

        // Set the current node to the previous node
        llPtr->node = prevPtr;

        // Set the previous pointer to the current node
        prevPtr = llPtr;

        // Now set the loop pointer to the next node in the linked list
        if(!nextPtr) {
            break;
        }

        llPtr = nextPtr;
    }

    return llPtr;
}

void Algo::printLinkedList(Node* linkedList) {
    Node* llPtr = linkedList;

    while(llPtr != nullptr) {
        std::println("Node value {}", llPtr->value);
        llPtr = llPtr->node;
    }
}

int main() {
    Node* ll = Algo::generateLinkedList(3);
    Algo::printLinkedList(ll);
    std::println("Reverting the linked list...");

    ll = Algo::reverseLinkedList(ll);
    Algo::printLinkedList(ll);

    Algo::destroyLinkedList(ll);

    {
        std::vector<int> test {
            10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        };

        std::println("Performing bubble sort...");
        Algo::bubbleSort(test);

        for(const auto& val : test) {
            std::println("{}", val);
        }
    }

    {
        std::vector<int> test2 {
            10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        };

        std::println("Performing selection sort...");
        Algo::selectionSort(test2);

        for(const auto& val : test2) {
            std::println("{}", val);
        }
    }

    {
        std::vector<int> test3 {
            10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        };

        std::println("Performing insertion sort...");
        Algo::insertionSort(test3);

        for(const auto& val : test3) {
            std::println("{}", val);
        }
    }

    {
        std::vector<int> test4 {
            10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        };

        std::println("Performing merge sort...");
        auto sortedArr = Algo::mergeSort(test4);

        for(const auto& val : sortedArr) {
            std::println("{}", val);
        }
    }
}
