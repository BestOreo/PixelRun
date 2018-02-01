#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace glm;

typedef struct ListNode {
    vec3 content;
    struct ListNode *next;
} Node;


class List {
public:
    Node *front = NULL;
    Node *back = NULL;
    int count = 0;

    List()
    {

    }

    ~List() {
        Clear();
    }

    void Clear() {
        if (front != NULL) {
            Node *curNode = front;
            Node *next;
            do{
                next = curNode->next;
                delete curNode;
                curNode = next;
            } while (curNode != NULL);
            front = back = NULL;
            count = 0;
        }
    }

    void PushBack(vec3 content) {
        Node *p = CreateNode(content);
        count++;
        if (back == NULL) {
            back = front = p;
        }
        else {
            back->next = p;
            back = p;
        }
    }

    void PushFront(vec3 content) {
        Node *p = CreateNode(content);
        count++;
        if (front == NULL) {
            back = front = p;
        }
        else {
            p->next = front;
            front = p;
        }
    }

    void PopFront() {
        if (front == NULL) {
            return;
        }

        count--;
        Node *p = front;
        front = front->next;
        delete p;

        if (count == 0) {
            back = front = NULL;
        }
    }

private:
    Node *CreateNode(vec3 content) {
        Node *p = (Node *)malloc(sizeof(Node));
        p->content = content;
        p->next = NULL;
        return p;
    }
};