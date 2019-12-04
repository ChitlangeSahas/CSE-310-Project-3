class Node
{
public:
    Node* next;
    int data ;
};

using namespace std;

class LinkedList
{
public:
    int length;
    Node* head;

    LinkedList();
    ~LinkedList();
    void add(int data);
    void print();
    int at(unsigned int n);

};

LinkedList::LinkedList(){
    this->length = 0;
    this->head = NULL;
}

LinkedList::~LinkedList(){
    // std::cout << "LIST DELETED";
}

void LinkedList::add(int data){
    Node* node = new Node();
    node->data = data;
    node->next = this->head;
    this->head = node;
    this->length++;
}

void LinkedList::print(){
    Node* head = this->head;
    while(head){
        std::cout << head->data << "->";
        head = head->next;
    }
    if (length == 0)
    {
    	cout << "EMPTY LIST" ;
    }
    cout << endl;
}

int LinkedList::at(unsigned int index) {
    Node *ptr = this->head;
    for (int i = 0; i < index; ++i)
    {
        ptr = ptr->next;
    }
    return ptr->data;
}