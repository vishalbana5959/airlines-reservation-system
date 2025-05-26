/*

Airlines Reservation System

A new airlines have only one plane of capacity 40. Your system will assign seats on each flight. 5%
seats are reserved for first class and rest for economic class. Passengers are allowed to book single
and group tickets. Passenger may be a Prime category passenger (first class) or normal category
(economic class). Generate a passenger list sorted on category if 60% tickets are sold. Senior citizens
will have preference over normal category. Passengers may enquire about seat through interactive
mode.

*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>

#define TOTAL_SEATS 40
#define FIRST_CLASS_SEATS TOTAL_SEATS*0.05
#define ECONOMIC_CLASS_SEATS TOTAL_SEATS*0.95
#define PASSENGER_LIST_THRESHOLD TOTAL_SEATS*0.6

////////////// BOOKING SYSTEM STRUCTURES /////////////

typedef enum {
    SINGLE,
    GROUP
} bookingType;

typedef enum {
    FIRST_CLASS,
    ECONOMIC_CLASS
} seatType;

typedef enum {
    SENIOR,
    YOUNGER
} ageType;

typedef enum {
    TOTAL_RECORDS,
    BOOKED_FIRST_CLASS,
    BOOKED_ECONOMIC_CLASS
} infoType;

typedef struct {
    char name[50];
    int age;
    char sex;
    int ticket_number;
    int seat_number;
    seatType seat_type;
    ageType age_type;
} Passenger;

////////////// BOOKING SYSTEM STRUCTURES /////////////

///////////////// PRIORITY QUEUE /////////////////////

typedef struct Node Node;
typedef struct PriorityQueue PriorityQueue;

struct Node {
    Passenger data;
    int priority;
    Node* next;
};

struct PriorityQueue {
    Node* front;
    Node* rear;
};

void init(PriorityQueue* pq) {
    pq->front = NULL;
    pq->rear = NULL;
}

Node* createNode(Passenger data, int priority) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;
    return newNode;
}

bool isEmpty(PriorityQueue* pq) {
    if(pq->front == NULL) return true;
    else return false;
}

Passenger dequeue(PriorityQueue* pq) {
    Node* temp = pq->front;
    pq->front = pq->front->next;
    Passenger p = temp->data;
    free(temp);
    return p;
}

void enqueue(PriorityQueue* pq, Passenger data, int priority) {
    Node* newNode = createNode(data, priority);
    if(isEmpty(pq)) {
        pq->front = pq->rear = newNode;
    } else {
        if(pq->front->priority > newNode->priority) {
            newNode->next = pq->front;
            pq->front = newNode;
        } else {
            Node* temp = pq->front;
            while(temp->next != NULL && temp->priority < newNode->priority) {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
    }
}

///////////////// PRIORITY QUEUE /////////////////////

void writeRecord(Passenger p) {
    FILE* fptr;
    fptr = fopen("passenger_details.csv", "a");
    if(fptr == NULL) {
        printf("Cannot open file.\n");
        exit(1);
    }
    fprintf(fptr, "%s,%d,%c,%d,%d,%d,%d\n", p.name, p.age, p.sex, p.ticket_number, p.seat_number, p.seat_type, p.age_type);
    printf("| [✓] Ticket booked for %s successfully! Ticket No. %d\n", p.name, p.ticket_number);
    fclose(fptr);
}

void clearCSV() {
    FILE* fptr;
    fptr = fopen("passenger_details.csv", "w");
    fclose(fptr);
}

int generateSeatNo(seatType seat_type) {
    int seat_number = 1;
    FILE* fptr;
	fptr = fopen("passenger_details.csv", "r");
	if(fptr!=NULL){
        while(!feof(fptr)) {
            Passenger p;
            fscanf(fptr, "%99[^,],%d,%c,%d,%d,%d,%d\n", p.name, &p.age, &p.sex, &p.ticket_number, &p.seat_number, (int*)&p.seat_type, (int*)&p.age_type);
            if(p.seat_type == seat_type) {
                seat_number = p.seat_number + 1;
            }
        }
    }
    if(seat_number == 1 && seat_type == ECONOMIC_CLASS) {
        seat_number += FIRST_CLASS_SEATS;
    }
    fclose(fptr);
    return seat_number;
}

void queueToCSV(PriorityQueue* pq) {
    while(!isEmpty(pq)) {
        Passenger p = dequeue(pq);
        p.seat_number = generateSeatNo(p.seat_type);
        writeRecord(p);
    }
    printf("+------------------------------------------------------------------------------+\n");
}

int getRecordCount(infoType info_type) {
    FILE * fptr;
	fptr = fopen("passenger_details.csv", "r");
	if (fptr == NULL) {
        clearCSV();
		return 0;
	}
    if(info_type == TOTAL_RECORDS) {
        int count = 0;
        for (char c = getc(fptr); c != EOF; c = getc(fptr)) {
            if (c == '\n') {
                count++;
            }
        }
        fclose(fptr);
        return count;
    } else if (info_type == BOOKED_FIRST_CLASS) {
        int count = 0;
        while(!feof(fptr)) {
            Passenger p;
            fscanf(fptr, "%99[^,],%d,%c,%d,%d,%d,%d\n", p.name, &p.age, &p.sex, &p.ticket_number, &p.seat_number, (int*)&p.seat_type, (int*)&p.age_type);
            if (p.seat_type == FIRST_CLASS) {
                count++;
            }
	    }
        fclose(fptr);
	    return count;
	} else if(info_type == ECONOMIC_CLASS) {
        return getRecordCount(TOTAL_RECORDS) - getRecordCount(BOOKED_FIRST_CLASS);
    }
}

PriorityQueue booking_queue;

void bookTicket(bookingType booking_type) {
    if(booking_type == SINGLE) {
        Passenger p;
        printf("+------------------------------------------------------------------------------+\n");
        printf("| + Enter name: ");
        scanf(" %99[^\n]", p.name);
        printf("| + Enter age: ");
        scanf(" %d", &p.age);
        if(p.age >= 60) {
            p.age_type = SENIOR;
        } else {
            p.age_type = YOUNGER;
        }
        printf("| + Enter sex: ");
        scanf(" %c", &p.sex);
        p.ticket_number = time(0) % 100000;
        if(getRecordCount(BOOKED_FIRST_CLASS) < FIRST_CLASS_SEATS) {
            printf("| [?] Do you want to book First Class seat? (Y/N): ");
            char c;
            scanf(" %c", &c);
            switch(c) {
                case 'Y': {
                    p.seat_type = FIRST_CLASS;
                    break;
                }
                case 'N': {
                    p.seat_type = ECONOMIC_CLASS;
                    break;
                }
            }
        } else {
            p.seat_type = ECONOMIC_CLASS;
        }
        enqueue(&booking_queue, p, p.age_type);
        printf("+------------------------------------------------------------------------------+\n");
    } else if(booking_type == GROUP) {
        printf("+------------------------------------------------------------------------------+\n");
        printf("| [?] Enter number of tickets you want to book: ");
        int n;
        scanf(" %d", &n);
        for(int i = 0; i < n; i++) {
            bookTicket(SINGLE);
        }
    }
}

void seePassengerList() {
    printf("+------------------------------------------------------------------------------+\n");
    printf("|                                PASSENGER LIST                                |\n");
    printf("+------------------------------------------------------------------------------+\n");
    int total_bookings = getRecordCount(TOTAL_RECORDS);
    if(total_bookings >= TOTAL_SEATS*PASSENGER_LIST_THRESHOLD) {
        FILE* fptr;
        PriorityQueue csv_queue;
        init(&csv_queue);
        fptr = fopen("passenger_details.csv", "r");
        while(!feof(fptr)) {
            Passenger p;
            fscanf(fptr, "%99[^,],%d,%c,%d,%d,%d,%d\n", p.name, &p.age, &p.sex, &p.ticket_number, &p.seat_number, (int*)&p.seat_type, (int*)&p.age_type);
            enqueue(&csv_queue, p, p.seat_type);
        }

        while(!isEmpty(&csv_queue)) {
            Passenger p = dequeue(&csv_queue);
            printf("| %-15s %-5d %-4c %-8d %-5d %-18s %-15s |\n", p.name, p.age, p.sex, p.ticket_number, p.seat_number, (p.seat_type == FIRST_CLASS) ? "First Class" : "Economic Class", (p.age_type == SENIOR) ? "Senior Citizen" : "Normal");
        }
        printf("+------------------------------------------------------------------------------+\n");
    } else {
        printf("|               [x] Bookings are ongoing. Please comeback later!               |\n");
        printf("+------------------------------------------------------------------------------+\n");
    }
}

void checkYourTicket(int ticketNumber) {
    FILE* fptr;
    fptr = fopen("passenger_details.csv", "r");
    bool found = false;
    while(!feof(fptr)) {
            Passenger p;
            fscanf(fptr, "%99[^,],%d,%c,%d,%d,%d,%d\n", p.name, &p.age, &p.sex, &p.ticket_number, &p.seat_number, (int*)&p.seat_type, (int*)&p.age_type);
            if(p.ticket_number == ticketNumber) {
                found = true;
                printf("| Name: %s, Age: %d, Sex: %c\n| Ticket No. %d\n| Seat No. %d\n| %s, %s\n", p.name, p.age, p.sex, p.ticket_number, p.seat_number, (p.seat_type == FIRST_CLASS) ? "First Class" : "Economic Class", (p.age_type == SENIOR) ? "Senior Citizen" : "Normal");
            }
	}
    if(found == false) {
        printf("| [x] You have entered wrong ticket number!                                    |\n");
        printf("+------------------------------------------------------------------------------+\n");
    }
}

int main() {
    printf("+------------------------------------------------------------------------------+\n");
	printf("|                           WELCOME TO KALYANI AIRLINES                        |\n");
	printf("+------------------------------------------------------------------------------+\n");
	printf("| 1: Book ticket                                                               |\n");
	printf("| 2: Check passenger list                                                      |\n");
	printf("| 3: Ticket enquiry                                                            |\n");
	printf("| 4: Exit                                                                      |\n");
	printf("+------------------------------------------------------------------------------+\n");
	printf("| Choose an option: ");
    int choice;
    scanf(" %d", &choice);
    switch(choice) {
        case 1: {
            init(&booking_queue);
            printf("+------------------------------------------------------------------------------+\n");
            printf("|                            TICKET BOOKING COUNTER                            |\n");
            printf("+------------------------------------------------------------------------------+\n");
            if(getRecordCount(TOTAL_RECORDS) >= TOTAL_SEATS) {
                printf("|                     [x] SEATS ARE FULL! NO MORE BOOKING!                     |\n");
                printf("+------------------------------------------------------------------------------+\n");
                break;
            }
            printf("| 1: Book a single ticket                                                      |\n");
            printf("| 2: Book tickets for a group                                                  |\n");
            printf("+------------------------------------------------------------------------------+\n");
            printf("| Choose an option: ");
            int choice1;
            scanf(" %d", &choice1);
            switch(choice1) {
                case 1: {
                    bookTicket(SINGLE);
                    break;
                }
                case 2: {
                    bookTicket(GROUP);
                    break;
                }
            }
            queueToCSV(&booking_queue);
            break;
        }
        case 2: {
            seePassengerList();
            break;
        }
        case 3: {
            printf("+------------------------------------------------------------------------------+\n");
            printf("|                                TICKET ENQUIRY                                |\n");
            printf("+------------------------------------------------------------------------------+\n");
            printf("| + Enter your ticket number: ");
            int ticket_number;
            scanf(" %d", &ticket_number);
            printf("+------------------------------------------------------------------------------+\n");
            checkYourTicket(ticket_number);
            printf("+------------------------------------------------------------------------------+\n");
            break;
        }
        case 4: {
            printf("+------------------------------------------------------------------------------+\n");
            printf("|                                    EXITED                                    |\n");
            printf("+------------------------------------------------------------------------------+\n");
            exit(0);
        }
    }
}